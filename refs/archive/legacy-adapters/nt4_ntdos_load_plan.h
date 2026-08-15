#ifndef NT4_NTDOS_LOAD_PLAN_H
#define NT4_NTDOS_LOAD_PLAN_H

#include <stddef.h>
#include <stdint.h>

typedef enum nt4_ntdos_load_plan_result {
    NT4_NTDOS_LOAD_PLAN_OK = 0,
    NT4_NTDOS_LOAD_PLAN_INVALID_ARGUMENT,
    NT4_NTDOS_LOAD_PLAN_RAM_RANGE_REJECTED,
    NT4_NTDOS_LOAD_PLAN_NO_MEMORY
} nt4_ntdos_load_plan_result;

typedef struct nt4_ntdos_load_plan {
    uint8_t *preload_bytes;
    size_t preload_byte_count;
    uint32_t preload_physical;
    uint16_t load_segment;
} nt4_ntdos_load_plan;

/* Trace-only range-planning evidence. This is not the runtime NTDOS loader:
 * original demLoadDos owns the live SVC_DEMLOADDOS transfer. out_plan must be
 * zero initialized or released before reuse. On failure it remains unchanged. */
nt4_ntdos_load_plan_result nt4_ntdos_load_plan_build(
    const uint8_t *ntdos_bytes,
    size_t ntdos_byte_count,
    uint16_t load_segment,
    size_t ordinary_ram_byte_count,
    nt4_ntdos_load_plan *out_plan);
void nt4_ntdos_load_plan_release(nt4_ntdos_load_plan *plan);

#endif
