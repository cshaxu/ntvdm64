#include "nt4_oracle_date_adapter.h"

#include <stdio.h>

static int expect_m1(owned_monitor_m1_result actual, owned_monitor_m1_result expected)
{
    return actual == expected ? 0 : 1;
}

static int initialize_machine(
    owned_monitor_m0 **out_monitor,
    owned_monitor_m1 **out_registry,
    nt4_oracle_date_adapter *adapter,
    const owned_clock_snapshot *snapshot)
{
    if (owned_monitor_m0_create(64u, out_monitor) != OWNED_MONITOR_M0_OK ||
        owned_monitor_m1_create(out_registry) != OWNED_MONITOR_M1_OK ||
        !nt4_oracle_date_adapter_initialize(adapter, snapshot) ||
        nt4_oracle_date_adapter_register(*out_registry, adapter) != OWNED_MONITOR_M1_OK ||
        owned_monitor_m1_freeze(*out_registry) != OWNED_MONITOR_M1_OK) {
        owned_monitor_m1_destroy(*out_registry);
        owned_monitor_m0_destroy(*out_monitor);
        *out_registry = NULL;
        *out_monitor = NULL;
        return 0;
    }
    return 1;
}

static int verify_invalid_snapshot(
    const owned_clock_snapshot *snapshot,
    const uint8_t *trace,
    size_t trace_byte_count)
{
    owned_monitor_m0 *monitor = NULL;
    owned_monitor_m1 *registry = NULL;
    owned_monitor_m0_state state;
    nt4_oracle_date_adapter adapter;
    uint8_t sentinel = 0xa5u;
    int failed = 0;

    adapter.snapshot = *snapshot;
    failed |= owned_monitor_m0_create(64u, &monitor) != OWNED_MONITOR_M0_OK;
    failed |= owned_monitor_m1_create(&registry) != OWNED_MONITOR_M1_OK;
    failed |= nt4_oracle_date_adapter_register(registry, &adapter) != OWNED_MONITOR_M1_OK;
    failed |= owned_monitor_m1_freeze(registry) != OWNED_MONITOR_M1_OK;
    state.ax = 0xaaaau; state.bx = 0xbbbbu; state.cx = 0xccccu; state.dx = 0xddddu;
    state.sp = 0u; state.bp = 0u; state.si = 0u; state.di = 0u;
    state.cs = 0u; state.ds = 0u; state.es = 0u; state.ss = 0u; state.ip = 0x0040u; state.flags = 0x0203u;
    failed |= owned_monitor_m0_set_state(monitor, &state) != OWNED_MONITOR_M0_OK;
    failed |= owned_monitor_m0_write(monitor, 8u, trace, trace_byte_count) != OWNED_MONITOR_M0_OK;
    failed |= owned_monitor_m0_write(monitor, 60u, &sentinel, sizeof(sentinel)) != OWNED_MONITOR_M0_OK;
    failed |= expect_m1(owned_monitor_m1_dispatch(registry, monitor, 8u), OWNED_MONITOR_M1_FAULT);
    failed |= owned_monitor_m0_get_state(monitor, &state) != OWNED_MONITOR_M0_OK ||
        state.ax != 0xaaaau || state.cx != 0xccccu || state.dx != 0xddddu || state.ip != 0x0040u || state.flags != 0x0203u;
    sentinel = 0u;
    failed |= owned_monitor_m0_read(monitor, 60u, &sentinel, sizeof(sentinel)) != OWNED_MONITOR_M0_OK || sentinel != 0xa5u;
    owned_monitor_m1_destroy(registry);
    owned_monitor_m0_destroy(monitor);
    return failed;
}

int main(void)
{
    owned_monitor_m0 *monitor = NULL;
    owned_monitor_m1 *registry = NULL;
    owned_monitor_m0_state state;
    nt4_oracle_date_adapter adapter;
    owned_clock_snapshot snapshot = { 1996u, 7u, 4u, 4u };
    const uint8_t trace[] = { 0xc4u, 0xc4u, 0x50u, 0x14u };
    const uint8_t nonmatching[] = { 0xc4u, 0xc4u, 0x50u, 0x15u };
    int failed = 0;

    if (!initialize_machine(&monitor, &registry, &adapter, &snapshot)) return 1;
    state.ax = 0xaa00u; state.bx = 0x1111u; state.cx = 0x2222u; state.dx = 0x3333u;
    state.sp = 0x4444u; state.bp = 0x5555u; state.si = 0x6666u; state.di = 0x7777u;
    state.cs = 0u; state.ds = 0u; state.es = 0u; state.ss = 0u; state.ip = 0xfffcu; state.flags = 0x0203u;
    failed |= owned_monitor_m0_set_state(monitor, &state) != OWNED_MONITOR_M0_OK;
    failed |= owned_monitor_m0_write(monitor, 8u, trace, sizeof(trace)) != OWNED_MONITOR_M0_OK;
    failed |= expect_m1(owned_monitor_m1_dispatch(registry, monitor, 8u), OWNED_MONITOR_M1_HANDLED_RESUME);
    failed |= owned_monitor_m0_get_state(monitor, &state) != OWNED_MONITOR_M0_OK;
    failed |= state.ax != 0xaa04u || state.bx != 0x1111u || state.cx != 1996u ||
        state.dx != 0x0704u || state.ip != 0u || state.flags != 0x0203u;
    failed |= owned_monitor_m0_write(monitor, 16u, nonmatching, sizeof(nonmatching)) != OWNED_MONITOR_M0_OK;
    failed |= expect_m1(owned_monitor_m1_dispatch(registry, monitor, 16u), OWNED_MONITOR_M1_UNHANDLED);
    owned_monitor_m1_destroy(registry);
    owned_monitor_m0_destroy(monitor);

    snapshot.month = 0u;
    failed |= nt4_oracle_date_adapter_initialize(&adapter, &snapshot) != 0;
    failed |= verify_invalid_snapshot(&snapshot, trace, sizeof(trace));
    snapshot.month = 7u; snapshot.day = 0u;
    failed |= nt4_oracle_date_adapter_initialize(&adapter, &snapshot) != 0;
    failed |= verify_invalid_snapshot(&snapshot, trace, sizeof(trace));
    snapshot.day = 4u; snapshot.weekday = 7u;
    failed |= nt4_oracle_date_adapter_initialize(&adapter, &snapshot) != 0;
    failed |= verify_invalid_snapshot(&snapshot, trace, sizeof(trace));
    snapshot.weekday = 4u; snapshot.year = 1979u;
    failed |= nt4_oracle_date_adapter_initialize(&adapter, &snapshot) != 0;
    failed |= verify_invalid_snapshot(&snapshot, trace, sizeof(trace));
    if (failed) return 1;
    puts("nt4-oracle-date-adapter-test: bounded source-derived date mapping verified");
    return 0;
}
