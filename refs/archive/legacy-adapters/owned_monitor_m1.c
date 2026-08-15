#include "owned_monitor_m1.h"

#include <stdlib.h>
#include <string.h>

typedef struct owned_monitor_m1_entry {
    uint8_t pattern[OWNED_MONITOR_M1_MAX_PATTERN_BYTES];
    size_t pattern_byte_count;
    uintptr_t handler_id;
    owned_monitor_m1_handler handler;
    void *context;
} owned_monitor_m1_entry;

struct owned_monitor_m1 {
    owned_monitor_m1_entry entries[OWNED_MONITOR_M1_MAX_ENTRIES];
    size_t entry_count;
    int frozen;
};

static int patterns_overlap_by_prefix(
    const uint8_t *first,
    size_t first_byte_count,
    const uint8_t *second,
    size_t second_byte_count)
{
    size_t common_byte_count = first_byte_count < second_byte_count ? first_byte_count : second_byte_count;
    return memcmp(first, second, common_byte_count) == 0;
}

static void apply_register_patch(
    owned_monitor_m0_state *state,
    const owned_monitor_m1_patch *patch)
{
    if ((patch->register_write_mask & OWNED_MONITOR_M1_PATCH_AX) != 0u) state->ax = patch->ax;
    if ((patch->register_write_mask & OWNED_MONITOR_M1_PATCH_BX) != 0u) state->bx = patch->bx;
    if ((patch->register_write_mask & OWNED_MONITOR_M1_PATCH_CX) != 0u) state->cx = patch->cx;
    if ((patch->register_write_mask & OWNED_MONITOR_M1_PATCH_DX) != 0u) state->dx = patch->dx;
    if ((patch->register_write_mask & OWNED_MONITOR_M1_PATCH_SP) != 0u) state->sp = patch->sp;
    if ((patch->register_write_mask & OWNED_MONITOR_M1_PATCH_BP) != 0u) state->bp = patch->bp;
    if ((patch->register_write_mask & OWNED_MONITOR_M1_PATCH_SI) != 0u) state->si = patch->si;
    if ((patch->register_write_mask & OWNED_MONITOR_M1_PATCH_DI) != 0u) state->di = patch->di;
    if ((patch->register_write_mask & OWNED_MONITOR_M1_PATCH_DS) != 0u) state->ds = patch->ds;
    if ((patch->register_write_mask & OWNED_MONITOR_M1_PATCH_ES) != 0u) state->es = patch->es;
    state->flags = (uint16_t)((state->flags & (uint16_t)~patch->flags_write_mask) |
        (patch->flags_value & patch->flags_write_mask));
}

owned_monitor_m1_result owned_monitor_m1_create(owned_monitor_m1 **out_registry)
{
    owned_monitor_m1 *registry;

    if (out_registry == NULL) {
        return OWNED_MONITOR_M1_INVALID_ARGUMENT;
    }
    *out_registry = NULL;
    registry = (owned_monitor_m1 *)calloc(1u, sizeof(*registry));
    if (registry == NULL) {
        return OWNED_MONITOR_M1_ALLOCATION_FAILED;
    }
    *out_registry = registry;
    return OWNED_MONITOR_M1_OK;
}

void owned_monitor_m1_destroy(owned_monitor_m1 *registry)
{
    free(registry);
}

owned_monitor_m1_result owned_monitor_m1_register(
    owned_monitor_m1 *registry,
    const uint8_t *pattern,
    size_t pattern_byte_count,
    uintptr_t handler_id,
    owned_monitor_m1_handler handler,
    void *context)
{
    size_t index;
    owned_monitor_m1_entry *entry;

    if (registry == NULL || pattern == NULL || handler == NULL || pattern_byte_count == 0u ||
        pattern_byte_count > OWNED_MONITOR_M1_MAX_PATTERN_BYTES) {
        return OWNED_MONITOR_M1_INVALID_ARGUMENT;
    }
    if (registry->frozen) {
        return OWNED_MONITOR_M1_REGISTRY_FROZEN;
    }
    if (registry->entry_count == OWNED_MONITOR_M1_MAX_ENTRIES) {
        return OWNED_MONITOR_M1_ALLOCATION_FAILED;
    }
    for (index = 0u; index < registry->entry_count; ++index) {
        if (patterns_overlap_by_prefix(pattern, pattern_byte_count,
                registry->entries[index].pattern, registry->entries[index].pattern_byte_count)) {
            return OWNED_MONITOR_M1_PATTERN_CONFLICT;
        }
    }

    entry = &registry->entries[registry->entry_count++];
    memcpy(entry->pattern, pattern, pattern_byte_count);
    entry->pattern_byte_count = pattern_byte_count;
    entry->handler_id = handler_id;
    entry->handler = handler;
    entry->context = context;
    return OWNED_MONITOR_M1_OK;
}

owned_monitor_m1_result owned_monitor_m1_freeze(owned_monitor_m1 *registry)
{
    if (registry == NULL) {
        return OWNED_MONITOR_M1_INVALID_ARGUMENT;
    }
    registry->frozen = 1;
    return OWNED_MONITOR_M1_OK;
}

owned_monitor_m1_result owned_monitor_m1_dispatch(
    const owned_monitor_m1 *registry,
    owned_monitor_m0 *monitor,
    uint64_t physical_fetch_address)
{
    size_t index;
    uint8_t fetched[OWNED_MONITOR_M1_MAX_PATTERN_BYTES];
    owned_monitor_m0_state snapshot;
    owned_monitor_m0_state proposed;
    owned_monitor_m1_handler_output output;

    if (registry == NULL || monitor == NULL) {
        return OWNED_MONITOR_M1_INVALID_ARGUMENT;
    }
    if (!registry->frozen) {
        return OWNED_MONITOR_M1_UNHANDLED;
    }
    for (index = 0u; index < registry->entry_count; ++index) {
        const owned_monitor_m1_entry *entry = &registry->entries[index];
        if (owned_monitor_m0_fetch(monitor, physical_fetch_address, fetched, entry->pattern_byte_count) !=
            OWNED_MONITOR_M0_OK) {
            continue;
        }
        if (memcmp(fetched, entry->pattern, entry->pattern_byte_count) != 0) {
            continue;
        }
        if (owned_monitor_m0_get_state(monitor, &snapshot) != OWNED_MONITOR_M0_OK) {
            return OWNED_MONITOR_M1_FETCH_UNAVAILABLE;
        }
        memset(&output, 0, sizeof(output));
        output.disposition = OWNED_MONITOR_M1_DISPOSITION_UNHANDLED;
        entry->handler(&snapshot, physical_fetch_address, fetched, entry->pattern_byte_count,
            entry->handler_id, entry->context, &output);

        if (output.disposition == OWNED_MONITOR_M1_DISPOSITION_UNHANDLED) {
            return OWNED_MONITOR_M1_UNHANDLED;
        }
        if (output.disposition == OWNED_MONITOR_M1_DISPOSITION_STOP) {
            return OWNED_MONITOR_M1_STOP;
        }
        if (output.disposition == OWNED_MONITOR_M1_DISPOSITION_FAULT) {
            return OWNED_MONITOR_M1_FAULT;
        }
        if (output.disposition != OWNED_MONITOR_M1_DISPOSITION_HANDLED_RESUME ||
            (output.patch.register_write_mask & (uint16_t)~OWNED_MONITOR_M1_PATCH_ALL) != 0u ||
            (output.patch.flags_write_mask & (uint16_t)~OWNED_MONITOR_M1_FLAGS_WRITABLE) != 0u ||
            output.patch.consumed_byte_count != entry->pattern_byte_count) {
            return OWNED_MONITOR_M1_INVALID_HANDLER_OUTPUT;
        }

        proposed = snapshot;
        apply_register_patch(&proposed, &output.patch);
        proposed.ip = (uint16_t)(snapshot.ip + output.patch.consumed_byte_count);
        if (owned_monitor_m0_set_state(monitor, &proposed) != OWNED_MONITOR_M0_OK) {
            return OWNED_MONITOR_M1_FETCH_UNAVAILABLE;
        }
        return OWNED_MONITOR_M1_HANDLED_RESUME;
    }
    return OWNED_MONITOR_M1_UNHANDLED;
}
