#ifndef BX_NTVDM_DEM_READONLY_FILE_SERVICE_H
#define BX_NTVDM_DEM_READONLY_FILE_SERVICE_H

#include "bx_ntvdm_bulk_result_transaction.h"
#include "bx_ntvdm_guest_read_action_v1.h"
#include "bx_ntvdm_readonly_namespace.h"

/* Source-shaped completion for the read-only subset of DEM file services.
 * It is adapter-local: callers supply copied CPU/guest data and receive only
 * generic CPU/bulk records, never a host handle or guest pointer. */
int bx_ntvdm_dem_readonly_file_v1_prepare_open(
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, bx_ntvdm_guest_read_action_v1 *action);
int bx_ntvdm_dem_readonly_file_v1_complete_open(
    bx_ntvdm_readonly_namespace_v1 *space, const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu, const bx_ntvdm_guest_read_action_v1 *action,
    const uint8_t *path_bytes, uint64_t path_byte_count, bx_ntvdm_cpu_result_v2 *result);
int bx_ntvdm_dem_readonly_file_v1_seek(
    bx_ntvdm_readonly_namespace_v1 *space, const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu, const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);
int bx_ntvdm_dem_readonly_file_v1_read(
    bx_ntvdm_readonly_namespace_v1 *space, const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu, const bx_ntvdm_instruction_window_v1 *window,
    uint8_t *payload, uint32_t payload_capacity,
    bx_ntvdm_bulk_result_transaction_v1 *transaction,
    bx_ntvdm_cpu_result_v2 *direct_result);
int bx_ntvdm_dem_readonly_file_v1_fast_read(
    bx_ntvdm_readonly_namespace_v1 *space,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, uint8_t *payload,
    uint32_t payload_capacity, bx_ntvdm_bulk_result_transaction_v1 *transaction,
    bx_ntvdm_cpu_result_v2 *direct_result);
int bx_ntvdm_dem_readonly_file_v1_close(
    bx_ntvdm_readonly_namespace_v1 *space, const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu, const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);

#endif
