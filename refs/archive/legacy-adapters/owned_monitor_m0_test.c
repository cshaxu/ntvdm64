#include "owned_monitor_m0.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int expect_result(owned_monitor_m0_result actual, owned_monitor_m0_result expected)
{
    return actual == expected ? 0 : 1;
}

int main(void)
{
    owned_monitor_m0 *monitor = NULL;
    owned_monitor_m0_state state;
    unsigned char write_bytes[] = { 0x12u, 0x34u, 0x56u };
    unsigned char read_bytes[3] = { 0xffu, 0xffu, 0xffu };
    unsigned char untouched = 0xa5u;
    int failed = 0;

    failed |= expect_result(
        owned_monitor_m0_create(0u, &monitor),
        OWNED_MONITOR_M0_INVALID_ARGUMENT);
    failed |= monitor != NULL;
    failed |= expect_result(
        owned_monitor_m0_create(64u, &monitor),
        OWNED_MONITOR_M0_OK);
    if (failed) {
        owned_monitor_m0_destroy(monitor);
        return 1;
    }

    failed |= expect_result(owned_monitor_m0_read(monitor, 0u, read_bytes, sizeof(read_bytes)), OWNED_MONITOR_M0_OK);
    failed |= read_bytes[0] != 0u || read_bytes[1] != 0u || read_bytes[2] != 0u;
    failed |= expect_result(owned_monitor_m0_write(monitor, 8u, write_bytes, sizeof(write_bytes)), OWNED_MONITOR_M0_OK);
    failed |= expect_result(owned_monitor_m0_fetch(monitor, 8u, read_bytes, sizeof(read_bytes)), OWNED_MONITOR_M0_OK);
    failed |= memcmp(write_bytes, read_bytes, sizeof(write_bytes)) != 0;

    failed |= expect_result(
        owned_monitor_m0_write(monitor, 8u, write_bytes, 0u),
        OWNED_MONITOR_M0_INVALID_ARGUMENT);
    failed |= expect_result(
        owned_monitor_m0_read(monitor, 64u, &untouched, 1u),
        OWNED_MONITOR_M0_INACCESSIBLE_RANGE);
    failed |= untouched != 0xa5u;
    failed |= expect_result(
        owned_monitor_m0_fetch(monitor, UINT64_MAX, &untouched, 1u),
        OWNED_MONITOR_M0_INACCESSIBLE_RANGE);

    memset(&state, 0xff, sizeof(state));
    failed |= expect_result(owned_monitor_m0_set_state(monitor, &state), OWNED_MONITOR_M0_OK);
    failed |= expect_result(owned_monitor_m0_reset(monitor), OWNED_MONITOR_M0_OK);
    failed |= expect_result(owned_monitor_m0_get_state(monitor, &state), OWNED_MONITOR_M0_OK);
    failed |= state.ax != 0u || state.cs != 0u || state.ip != 0u || state.flags != 0x0002u;
    memset(read_bytes, 0xff, sizeof(read_bytes));
    failed |= expect_result(owned_monitor_m0_read(monitor, 8u, read_bytes, sizeof(read_bytes)), OWNED_MONITOR_M0_OK);
    failed |= read_bytes[0] != 0u || read_bytes[1] != 0u || read_bytes[2] != 0u;

    owned_monitor_m0_destroy(monitor);
    if (failed) {
        return 1;
    }
    puts("owned-monitor-m0-test: checked mapping and reset invariants verified");
    return 0;
}
