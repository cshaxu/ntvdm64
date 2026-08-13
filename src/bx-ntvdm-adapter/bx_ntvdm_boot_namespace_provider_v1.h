#ifndef BX_NTVDM_BOOT_NAMESPACE_PROVIDER_V1_H
#define BX_NTVDM_BOOT_NAMESPACE_PROVIDER_V1_H

#include "bx_ntvdm_cmd_boot_file_service.h"
#include "bx_ntvdm_dem_path_search_service_v1.h"
#include "bx_ntvdm_dem_readonly_file_service.h"

#define BX_NTVDM_BOOT_NAMESPACE_PROVIDER_V1_MAGIC 0x42584e50u
#define BX_NTVDM_BOOT_NAMESPACE_PROVIDER_V1_VERSION 1u

/* One adapter-local owner for the profile-declared boot namespace.  This
 * combines COMMAND config-path production with DEM normal read-only file and
 * pathname-search consumption.  It owns no host path, handle or guest pointer. */
typedef struct bx_ntvdm_boot_namespace_provider_v1 {
    uint32_t magic;
    uint32_t version;
    bx_ntvdm_readonly_namespace_v1 readonly_namespace;
    bx_ntvdm_profile_search_snapshot_v1 search_snapshot;
    bx_ntvdm_search_transaction_v1 search_transaction;
} bx_ntvdm_boot_namespace_provider_v1;

int bx_ntvdm_boot_namespace_provider_v1_initialize(
    bx_ntvdm_boot_namespace_provider_v1 *value, const byob_image *command,
    const byob_image *target, const byob_image *terminal_quit,
    const byob_profile_selection *selection);
int bx_ntvdm_boot_namespace_provider_v1_valid(
    const bx_ntvdm_boot_namespace_provider_v1 *value);

/* Preserves the provider boundary while exposing the subordinate source
 * condition group to same-island diagnostics. */
int bx_ntvdm_boot_namespace_provider_v1_prepare_boot_file_diagnostic(
    const bx_ntvdm_boot_namespace_provider_v1 *value,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD],
    bx_ntvdm_cmd_boot_file_prepare_diagnostic_v1 *diagnostic);

int bx_ntvdm_boot_namespace_provider_v1_prepare_boot_file(
    const bx_ntvdm_boot_namespace_provider_v1 *value,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD]);
int bx_ntvdm_boot_namespace_provider_v1_prepare_open(
    const bx_ntvdm_boot_namespace_provider_v1 *value,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, bx_ntvdm_guest_read_action_v1 *action);
int bx_ntvdm_boot_namespace_provider_v1_complete_open(
    bx_ntvdm_boot_namespace_provider_v1 *value,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_guest_read_action_v1 *action, const uint8_t *path_bytes,
    uint64_t path_byte_count, bx_ntvdm_cpu_result_v2 *result);
int bx_ntvdm_boot_namespace_provider_v1_seek(
    bx_ntvdm_boot_namespace_provider_v1 *value,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, bx_ntvdm_cpu_result_v2 *result);
int bx_ntvdm_boot_namespace_provider_v1_read(
    bx_ntvdm_boot_namespace_provider_v1 *value,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, uint8_t *payload,
    uint32_t payload_capacity, bx_ntvdm_bulk_result_transaction_v1 *transaction,
    bx_ntvdm_cpu_result_v2 *result);
int bx_ntvdm_boot_namespace_provider_v1_fast_read(
    bx_ntvdm_boot_namespace_provider_v1 *value,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, uint8_t *payload,
    uint32_t payload_capacity, bx_ntvdm_bulk_result_transaction_v1 *transaction,
    bx_ntvdm_cpu_result_v2 *result);
int bx_ntvdm_boot_namespace_provider_v1_close(
    bx_ntvdm_boot_namespace_provider_v1 *value,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, bx_ntvdm_cpu_result_v2 *result);
int bx_ntvdm_boot_namespace_provider_v1_terminate_pdb(
    bx_ntvdm_boot_namespace_provider_v1 *value, uint16_t pdb,
    uint32_t *released_slots);

#endif
