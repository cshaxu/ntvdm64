#ifndef OWNED_MONITOR_M1_H
#define OWNED_MONITOR_M1_H

#include "owned_monitor_m0.h"

#include <stddef.h>
#include <stdint.h>

#define OWNED_MONITOR_M1_MAX_PATTERN_BYTES 15u
#define OWNED_MONITOR_M1_MAX_ENTRIES 32u

#define OWNED_MONITOR_M1_PATCH_AX 0x0001u
#define OWNED_MONITOR_M1_PATCH_BX 0x0002u
#define OWNED_MONITOR_M1_PATCH_CX 0x0004u
#define OWNED_MONITOR_M1_PATCH_DX 0x0008u
#define OWNED_MONITOR_M1_PATCH_SP 0x0010u
#define OWNED_MONITOR_M1_PATCH_BP 0x0020u
#define OWNED_MONITOR_M1_PATCH_SI 0x0040u
#define OWNED_MONITOR_M1_PATCH_DI 0x0080u
#define OWNED_MONITOR_M1_PATCH_DS 0x0100u
#define OWNED_MONITOR_M1_PATCH_ES 0x0200u
#define OWNED_MONITOR_M1_PATCH_ALL 0x03ffu
#define OWNED_MONITOR_M1_FLAGS_WRITABLE 0x0fd5u

typedef enum owned_monitor_m1_result {
    OWNED_MONITOR_M1_OK = 0,
    OWNED_MONITOR_M1_INVALID_ARGUMENT,
    OWNED_MONITOR_M1_ALLOCATION_FAILED,
    OWNED_MONITOR_M1_REGISTRY_FROZEN,
    OWNED_MONITOR_M1_PATTERN_CONFLICT,
    OWNED_MONITOR_M1_UNHANDLED,
    OWNED_MONITOR_M1_HANDLED_RESUME,
    OWNED_MONITOR_M1_STOP,
    OWNED_MONITOR_M1_FAULT,
    OWNED_MONITOR_M1_INVALID_HANDLER_OUTPUT,
    OWNED_MONITOR_M1_FETCH_UNAVAILABLE
} owned_monitor_m1_result;

typedef enum owned_monitor_m1_disposition {
    OWNED_MONITOR_M1_DISPOSITION_UNHANDLED = 0,
    OWNED_MONITOR_M1_DISPOSITION_HANDLED_RESUME,
    OWNED_MONITOR_M1_DISPOSITION_STOP,
    OWNED_MONITOR_M1_DISPOSITION_FAULT
} owned_monitor_m1_disposition;

typedef struct owned_monitor_m1_patch {
    uint16_t register_write_mask;
    uint16_t ax;
    uint16_t bx;
    uint16_t cx;
    uint16_t dx;
    uint16_t sp;
    uint16_t bp;
    uint16_t si;
    uint16_t di;
    uint16_t ds;
    uint16_t es;
    uint16_t flags_write_mask;
    uint16_t flags_value;
    uint8_t consumed_byte_count;
} owned_monitor_m1_patch;

typedef struct owned_monitor_m1_handler_output {
    owned_monitor_m1_disposition disposition;
    owned_monitor_m1_patch patch;
} owned_monitor_m1_handler_output;

typedef void (*owned_monitor_m1_handler)(
    const owned_monitor_m0_state *state,
    uint64_t physical_fetch_address,
    const uint8_t *matched_bytes,
    size_t matched_byte_count,
    uintptr_t handler_id,
    void *context,
    owned_monitor_m1_handler_output *out_output);

typedef struct owned_monitor_m1 owned_monitor_m1;

owned_monitor_m1_result owned_monitor_m1_create(owned_monitor_m1 **out_registry);
void owned_monitor_m1_destroy(owned_monitor_m1 *registry);
owned_monitor_m1_result owned_monitor_m1_register(
    owned_monitor_m1 *registry,
    const uint8_t *pattern,
    size_t pattern_byte_count,
    uintptr_t handler_id,
    owned_monitor_m1_handler handler,
    void *context);
owned_monitor_m1_result owned_monitor_m1_freeze(owned_monitor_m1 *registry);
owned_monitor_m1_result owned_monitor_m1_dispatch(
    const owned_monitor_m1 *registry,
    owned_monitor_m0 *monitor,
    uint64_t physical_fetch_address);

#endif
