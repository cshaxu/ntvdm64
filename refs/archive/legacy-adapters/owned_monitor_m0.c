#include "owned_monitor_m0.h"

#include <stdlib.h>
#include <string.h>

struct owned_monitor_m0 {
    unsigned char *physical_ram;
    size_t physical_ram_bytes;
    owned_monitor_m0_state state;
};

static void reset_state(owned_monitor_m0_state *state)
{
    memset(state, 0, sizeof(*state));
    state->flags = 0x0002u;
}

static owned_monitor_m0_result checked_interval(
    const owned_monitor_m0 *monitor,
    uint64_t physical_address,
    size_t byte_count,
    size_t *out_offset)
{
    uint64_t ram_bytes;

    if (monitor == NULL || out_offset == NULL || byte_count == 0u) {
        return OWNED_MONITOR_M0_INVALID_ARGUMENT;
    }

    ram_bytes = (uint64_t)monitor->physical_ram_bytes;
    if (physical_address >= ram_bytes || (uint64_t)byte_count > ram_bytes - physical_address) {
        return OWNED_MONITOR_M0_INACCESSIBLE_RANGE;
    }

    *out_offset = (size_t)physical_address;
    return OWNED_MONITOR_M0_OK;
}

owned_monitor_m0_result owned_monitor_m0_create(
    size_t physical_ram_bytes,
    owned_monitor_m0 **out_monitor)
{
    owned_monitor_m0 *monitor;

    if (out_monitor == NULL || physical_ram_bytes == 0u) {
        return OWNED_MONITOR_M0_INVALID_ARGUMENT;
    }
    *out_monitor = NULL;

    monitor = (owned_monitor_m0 *)calloc(1u, sizeof(*monitor));
    if (monitor == NULL) {
        return OWNED_MONITOR_M0_ALLOCATION_FAILED;
    }
    monitor->physical_ram = (unsigned char *)calloc(physical_ram_bytes, 1u);
    if (monitor->physical_ram == NULL) {
        free(monitor);
        return OWNED_MONITOR_M0_ALLOCATION_FAILED;
    }

    monitor->physical_ram_bytes = physical_ram_bytes;
    reset_state(&monitor->state);
    *out_monitor = monitor;
    return OWNED_MONITOR_M0_OK;
}

void owned_monitor_m0_destroy(owned_monitor_m0 *monitor)
{
    if (monitor == NULL) {
        return;
    }
    free(monitor->physical_ram);
    free(monitor);
}

owned_monitor_m0_result owned_monitor_m0_reset(owned_monitor_m0 *monitor)
{
    if (monitor == NULL) {
        return OWNED_MONITOR_M0_INVALID_ARGUMENT;
    }
    memset(monitor->physical_ram, 0, monitor->physical_ram_bytes);
    reset_state(&monitor->state);
    return OWNED_MONITOR_M0_OK;
}

owned_monitor_m0_result owned_monitor_m0_get_state(
    const owned_monitor_m0 *monitor,
    owned_monitor_m0_state *out_state)
{
    if (monitor == NULL || out_state == NULL) {
        return OWNED_MONITOR_M0_INVALID_ARGUMENT;
    }
    *out_state = monitor->state;
    return OWNED_MONITOR_M0_OK;
}

owned_monitor_m0_result owned_monitor_m0_set_state(
    owned_monitor_m0 *monitor,
    const owned_monitor_m0_state *state)
{
    if (monitor == NULL || state == NULL) {
        return OWNED_MONITOR_M0_INVALID_ARGUMENT;
    }
    monitor->state = *state;
    return OWNED_MONITOR_M0_OK;
}

owned_monitor_m0_result owned_monitor_m0_read(
    const owned_monitor_m0 *monitor,
    uint64_t physical_address,
    void *out_bytes,
    size_t byte_count)
{
    size_t offset;
    owned_monitor_m0_result result;

    if (out_bytes == NULL) {
        return OWNED_MONITOR_M0_INVALID_ARGUMENT;
    }
    result = checked_interval(monitor, physical_address, byte_count, &offset);
    if (result != OWNED_MONITOR_M0_OK) {
        return result;
    }
    memcpy(out_bytes, monitor->physical_ram + offset, byte_count);
    return OWNED_MONITOR_M0_OK;
}

owned_monitor_m0_result owned_monitor_m0_write(
    owned_monitor_m0 *monitor,
    uint64_t physical_address,
    const void *bytes,
    size_t byte_count)
{
    size_t offset;
    owned_monitor_m0_result result;

    if (bytes == NULL) {
        return OWNED_MONITOR_M0_INVALID_ARGUMENT;
    }
    result = checked_interval(monitor, physical_address, byte_count, &offset);
    if (result != OWNED_MONITOR_M0_OK) {
        return result;
    }
    memcpy(monitor->physical_ram + offset, bytes, byte_count);
    return OWNED_MONITOR_M0_OK;
}

owned_monitor_m0_result owned_monitor_m0_fetch(
    const owned_monitor_m0 *monitor,
    uint64_t physical_address,
    void *out_bytes,
    size_t byte_count)
{
    return owned_monitor_m0_read(monitor, physical_address, out_bytes, byte_count);
}
