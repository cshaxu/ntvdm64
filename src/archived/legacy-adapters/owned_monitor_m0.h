#ifndef OWNED_MONITOR_M0_H
#define OWNED_MONITOR_M0_H

#include <stddef.h>
#include <stdint.h>

typedef enum owned_monitor_m0_result {
    OWNED_MONITOR_M0_OK = 0,
    OWNED_MONITOR_M0_INVALID_ARGUMENT,
    OWNED_MONITOR_M0_ALLOCATION_FAILED,
    OWNED_MONITOR_M0_INACCESSIBLE_RANGE,
    OWNED_MONITOR_M0_UNAVAILABLE
} owned_monitor_m0_result;

typedef struct owned_monitor_m0_state {
    uint16_t ax;
    uint16_t bx;
    uint16_t cx;
    uint16_t dx;
    uint16_t sp;
    uint16_t bp;
    uint16_t si;
    uint16_t di;
    uint16_t cs;
    uint16_t ds;
    uint16_t es;
    uint16_t ss;
    uint16_t ip;
    uint16_t flags;
} owned_monitor_m0_state;

typedef struct owned_monitor_m0 owned_monitor_m0;

owned_monitor_m0_result owned_monitor_m0_create(
    size_t physical_ram_bytes,
    owned_monitor_m0 **out_monitor);
void owned_monitor_m0_destroy(owned_monitor_m0 *monitor);

owned_monitor_m0_result owned_monitor_m0_reset(owned_monitor_m0 *monitor);
owned_monitor_m0_result owned_monitor_m0_get_state(
    const owned_monitor_m0 *monitor,
    owned_monitor_m0_state *out_state);
owned_monitor_m0_result owned_monitor_m0_set_state(
    owned_monitor_m0 *monitor,
    const owned_monitor_m0_state *state);

owned_monitor_m0_result owned_monitor_m0_read(
    const owned_monitor_m0 *monitor,
    uint64_t physical_address,
    void *out_bytes,
    size_t byte_count);
owned_monitor_m0_result owned_monitor_m0_write(
    owned_monitor_m0 *monitor,
    uint64_t physical_address,
    const void *bytes,
    size_t byte_count);
owned_monitor_m0_result owned_monitor_m0_fetch(
    const owned_monitor_m0 *monitor,
    uint64_t physical_address,
    void *out_bytes,
    size_t byte_count);

#endif
