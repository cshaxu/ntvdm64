#include "transition_registry.h"

#include <string.h>

static int is_prefix(const uint8_t *left, size_t left_length,
    const uint8_t *right, size_t right_length)
{
    return left_length <= right_length &&
        memcmp(left, right, left_length) == 0;
}

void transition_registry_init(transition_registry *registry)
{
    if (registry != NULL) memset(registry, 0, sizeof(*registry));
}

int transition_registry_register(transition_registry *registry,
    const uint8_t *bytes, size_t length, uint8_t trap_port)
{
    size_t index;
    transition_pattern_registration *target;

    if (registry == NULL || bytes == NULL || registry->frozen ||
        length < 3u || length > TRANSITION_PATTERN_MAX_BYTES ||
        registry->count == TRANSITION_REGISTRY_MAX_PATTERNS) return 0;
    for (index = 0u; index < registry->count; ++index) {
        const transition_pattern_registration *existing = &registry->patterns[index];
        if (is_prefix(bytes, length, existing->bytes, existing->length) ||
            is_prefix(existing->bytes, existing->length, bytes, length)) return 0;
    }
    target = &registry->patterns[registry->count++];
    memcpy(target->bytes, bytes, length);
    target->length = length;
    target->trap_port = trap_port;
    return 1;
}

int transition_registry_freeze(transition_registry *registry)
{
    if (registry == NULL || registry->frozen) return 0;
    registry->frozen = 1;
    return 1;
}

int transition_registry_is_frozen(const transition_registry *registry)
{
    return registry != NULL && registry->frozen;
}

int transition_registry_apply(const transition_registry *registry,
    uint8_t *memory, size_t bytes, bop_rewrite_entry *entries,
    size_t entry_capacity, size_t *entry_count)
{
    bop_pattern patterns[TRANSITION_REGISTRY_MAX_PATTERNS];
    size_t index;

    if (registry == NULL || !registry->frozen || registry->count == 0u) return 0;
    for (index = 0u; index < registry->count; ++index) {
        patterns[index].bytes = registry->patterns[index].bytes;
        patterns[index].length = registry->patterns[index].length;
        patterns[index].trap_port = registry->patterns[index].trap_port;
    }
    return bop_rewrite_apply(memory, bytes, patterns, registry->count,
        entries, entry_capacity, entry_count);
}

int transition_disposition_is_valid(transition_disposition disposition)
{
    return disposition == TRANSITION_UNHANDLED ||
        disposition == TRANSITION_HANDLED_RESUME ||
        disposition == TRANSITION_STOP || disposition == TRANSITION_FAULT;
}
