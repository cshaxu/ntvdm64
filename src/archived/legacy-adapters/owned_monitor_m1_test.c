#include "owned_monitor_m1.h"

#include <stdio.h>
#include <string.h>

static void resume_handler(
    const owned_monitor_m0_state *state,
    uint64_t physical_fetch_address,
    const uint8_t *matched_bytes,
    size_t matched_byte_count,
    uintptr_t handler_id,
    void *context,
    owned_monitor_m1_handler_output *out_output)
{
    (void)state;
    (void)physical_fetch_address;
    (void)matched_bytes;
    (void)matched_byte_count;
    (void)handler_id;
    (void)context;
    out_output->disposition = OWNED_MONITOR_M1_DISPOSITION_HANDLED_RESUME;
    out_output->patch.register_write_mask = OWNED_MONITOR_M1_PATCH_AX;
    out_output->patch.ax = 0xbeefu;
    out_output->patch.flags_write_mask = 0x0001u;
    out_output->patch.flags_value = 0x0001u;
    out_output->patch.consumed_byte_count = 2u;
}

static void stop_handler(
    const owned_monitor_m0_state *state,
    uint64_t physical_fetch_address,
    const uint8_t *matched_bytes,
    size_t matched_byte_count,
    uintptr_t handler_id,
    void *context,
    owned_monitor_m1_handler_output *out_output)
{
    (void)state; (void)physical_fetch_address; (void)matched_bytes;
    (void)matched_byte_count; (void)handler_id; (void)context;
    out_output->disposition = OWNED_MONITOR_M1_DISPOSITION_STOP;
}

static void invalid_patch_handler(
    const owned_monitor_m0_state *state,
    uint64_t physical_fetch_address,
    const uint8_t *matched_bytes,
    size_t matched_byte_count,
    uintptr_t handler_id,
    void *context,
    owned_monitor_m1_handler_output *out_output)
{
    (void)state; (void)physical_fetch_address; (void)matched_bytes;
    (void)matched_byte_count; (void)handler_id; (void)context;
    out_output->disposition = OWNED_MONITOR_M1_DISPOSITION_HANDLED_RESUME;
    out_output->patch.register_write_mask = 0x8000u;
    out_output->patch.consumed_byte_count = 2u;
}

static void invalid_flags_handler(
    const owned_monitor_m0_state *state,
    uint64_t physical_fetch_address,
    const uint8_t *matched_bytes,
    size_t matched_byte_count,
    uintptr_t handler_id,
    void *context,
    owned_monitor_m1_handler_output *out_output)
{
    (void)state; (void)physical_fetch_address; (void)matched_bytes;
    (void)matched_byte_count; (void)handler_id; (void)context;
    out_output->disposition = OWNED_MONITOR_M1_DISPOSITION_HANDLED_RESUME;
    out_output->patch.flags_write_mask = 0x0002u;
    out_output->patch.consumed_byte_count = 2u;
}

static void invalid_ip_handler(
    const owned_monitor_m0_state *state,
    uint64_t physical_fetch_address,
    const uint8_t *matched_bytes,
    size_t matched_byte_count,
    uintptr_t handler_id,
    void *context,
    owned_monitor_m1_handler_output *out_output)
{
    (void)state; (void)physical_fetch_address; (void)matched_bytes;
    (void)matched_byte_count; (void)handler_id; (void)context;
    out_output->disposition = OWNED_MONITOR_M1_DISPOSITION_HANDLED_RESUME;
    out_output->patch.consumed_byte_count = 1u;
}

static void fault_handler(
    const owned_monitor_m0_state *state,
    uint64_t physical_fetch_address,
    const uint8_t *matched_bytes,
    size_t matched_byte_count,
    uintptr_t handler_id,
    void *context,
    owned_monitor_m1_handler_output *out_output)
{
    (void)state; (void)physical_fetch_address; (void)matched_bytes;
    (void)matched_byte_count; (void)handler_id; (void)context;
    out_output->disposition = OWNED_MONITOR_M1_DISPOSITION_FAULT;
}

static int expect(owned_monitor_m1_result actual, owned_monitor_m1_result expected)
{
    return actual == expected ? 0 : 1;
}

int main(void)
{
    owned_monitor_m0 *monitor = NULL;
    owned_monitor_m1 *registry = NULL;
    owned_monitor_m1 *conflict_registry = NULL;
    owned_monitor_m0_state state;
    const uint8_t pattern[] = { 0x31u, 0xa7u };
    const uint8_t prefix[] = { 0x31u };
    const uint8_t stop_pattern[] = { 0x5au };
    const uint8_t invalid_pattern[] = { 0x7eu, 0x7fu };
    const uint8_t invalid_flags_pattern[] = { 0x62u, 0x63u };
    const uint8_t invalid_ip_pattern[] = { 0x64u, 0x65u };
    const uint8_t fault_pattern[] = { 0x66u };
    const uint8_t too_long_pattern[OWNED_MONITOR_M1_MAX_PATTERN_BYTES + 1u] = { 0u };
    int failed = 0;

    failed |= expect(owned_monitor_m0_create(64u, &monitor), OWNED_MONITOR_M0_OK);
    failed |= expect(owned_monitor_m1_create(&registry), OWNED_MONITOR_M1_OK);
    failed |= expect(owned_monitor_m1_create(&conflict_registry), OWNED_MONITOR_M1_OK);
    failed |= expect(owned_monitor_m1_register(registry, pattern, 0u, 0u, resume_handler, NULL), OWNED_MONITOR_M1_INVALID_ARGUMENT);
    failed |= expect(owned_monitor_m1_register(registry, too_long_pattern, sizeof(too_long_pattern), 0u, resume_handler, NULL), OWNED_MONITOR_M1_INVALID_ARGUMENT);
    failed |= expect(owned_monitor_m1_register(registry, pattern, sizeof(pattern), 1u, resume_handler, NULL), OWNED_MONITOR_M1_OK);
    failed |= expect(owned_monitor_m1_register(registry, pattern, sizeof(pattern), 2u, resume_handler, NULL), OWNED_MONITOR_M1_PATTERN_CONFLICT);
    failed |= expect(owned_monitor_m1_register(registry, prefix, sizeof(prefix), 3u, resume_handler, NULL), OWNED_MONITOR_M1_PATTERN_CONFLICT);
    failed |= expect(owned_monitor_m1_register(registry, stop_pattern, sizeof(stop_pattern), 4u, stop_handler, NULL), OWNED_MONITOR_M1_OK);
    failed |= expect(owned_monitor_m1_register(registry, invalid_pattern, sizeof(invalid_pattern), 5u, invalid_patch_handler, NULL), OWNED_MONITOR_M1_OK);
    failed |= expect(owned_monitor_m1_register(registry, invalid_flags_pattern, sizeof(invalid_flags_pattern), 6u, invalid_flags_handler, NULL), OWNED_MONITOR_M1_OK);
    failed |= expect(owned_monitor_m1_register(registry, invalid_ip_pattern, sizeof(invalid_ip_pattern), 7u, invalid_ip_handler, NULL), OWNED_MONITOR_M1_OK);
    failed |= expect(owned_monitor_m1_register(registry, fault_pattern, sizeof(fault_pattern), 8u, fault_handler, NULL), OWNED_MONITOR_M1_OK);
    failed |= expect(owned_monitor_m1_register(conflict_registry, prefix, sizeof(prefix), 1u, resume_handler, NULL), OWNED_MONITOR_M1_OK);
    failed |= expect(owned_monitor_m1_register(conflict_registry, pattern, sizeof(pattern), 2u, resume_handler, NULL), OWNED_MONITOR_M1_PATTERN_CONFLICT);
    failed |= expect(owned_monitor_m1_dispatch(registry, monitor, 8u), OWNED_MONITOR_M1_UNHANDLED);
    failed |= expect(owned_monitor_m1_freeze(registry), OWNED_MONITOR_M1_OK);
    failed |= expect(owned_monitor_m1_register(registry, prefix, sizeof(prefix), 6u, resume_handler, NULL), OWNED_MONITOR_M1_REGISTRY_FROZEN);
    failed |= owned_monitor_m0_reset(monitor) != OWNED_MONITOR_M0_OK;
    failed |= expect(owned_monitor_m1_register(registry, prefix, sizeof(prefix), 6u, resume_handler, NULL), OWNED_MONITOR_M1_REGISTRY_FROZEN);

    state.ip = 0xfffeu;
    state.ax = 0u; state.bx = 0u; state.cx = 0u; state.dx = 0u;
    state.sp = 0u; state.bp = 0u; state.si = 0u; state.di = 0u;
    state.cs = 0u; state.ds = 0u; state.es = 0u; state.ss = 0u; state.flags = 0x0002u;
    failed |= owned_monitor_m0_set_state(monitor, &state) != OWNED_MONITOR_M0_OK;
    failed |= owned_monitor_m0_write(monitor, 8u, pattern, sizeof(pattern)) != OWNED_MONITOR_M0_OK;
    failed |= expect(owned_monitor_m1_dispatch(registry, monitor, 8u), OWNED_MONITOR_M1_HANDLED_RESUME);
    failed |= owned_monitor_m0_get_state(monitor, &state) != OWNED_MONITOR_M0_OK;
    failed |= state.ax != 0xbeefu || state.ip != 0u || state.flags != 0x0003u;
    failed |= expect(owned_monitor_m1_dispatch(registry, monitor, 63u), OWNED_MONITOR_M1_UNHANDLED);

    failed |= owned_monitor_m0_write(monitor, 20u, stop_pattern, sizeof(stop_pattern)) != OWNED_MONITOR_M0_OK;
    state.ax = 0x1234u;
    failed |= owned_monitor_m0_set_state(monitor, &state) != OWNED_MONITOR_M0_OK;
    failed |= expect(owned_monitor_m1_dispatch(registry, monitor, 20u), OWNED_MONITOR_M1_STOP);
    failed |= owned_monitor_m0_get_state(monitor, &state) != OWNED_MONITOR_M0_OK || state.ax != 0x1234u;

    failed |= owned_monitor_m0_write(monitor, 30u, invalid_pattern, sizeof(invalid_pattern)) != OWNED_MONITOR_M0_OK;
    failed |= expect(owned_monitor_m1_dispatch(registry, monitor, 30u), OWNED_MONITOR_M1_INVALID_HANDLER_OUTPUT);
    failed |= owned_monitor_m0_get_state(monitor, &state) != OWNED_MONITOR_M0_OK || state.ax != 0x1234u;

    failed |= owned_monitor_m0_write(monitor, 40u, invalid_flags_pattern, sizeof(invalid_flags_pattern)) != OWNED_MONITOR_M0_OK;
    failed |= expect(owned_monitor_m1_dispatch(registry, monitor, 40u), OWNED_MONITOR_M1_INVALID_HANDLER_OUTPUT);
    failed |= owned_monitor_m0_get_state(monitor, &state) != OWNED_MONITOR_M0_OK || state.ax != 0x1234u;

    failed |= owned_monitor_m0_write(monitor, 44u, invalid_ip_pattern, sizeof(invalid_ip_pattern)) != OWNED_MONITOR_M0_OK;
    failed |= expect(owned_monitor_m1_dispatch(registry, monitor, 44u), OWNED_MONITOR_M1_INVALID_HANDLER_OUTPUT);
    failed |= owned_monitor_m0_get_state(monitor, &state) != OWNED_MONITOR_M0_OK || state.ax != 0x1234u;

    failed |= owned_monitor_m0_write(monitor, 48u, fault_pattern, sizeof(fault_pattern)) != OWNED_MONITOR_M0_OK;
    failed |= expect(owned_monitor_m1_dispatch(registry, monitor, 48u), OWNED_MONITOR_M1_FAULT);
    failed |= owned_monitor_m0_get_state(monitor, &state) != OWNED_MONITOR_M0_OK || state.ax != 0x1234u;

    owned_monitor_m1_destroy(conflict_registry);
    owned_monitor_m1_destroy(registry);
    owned_monitor_m0_destroy(monitor);
    if (failed) return 1;
    puts("owned-monitor-m1-test: frozen generic transition invariants verified");
    return 0;
}
