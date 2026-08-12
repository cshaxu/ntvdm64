#ifndef BX_NTVDM_STARTUP_TRANSACTION_BRIDGE_H
#define BX_NTVDM_STARTUP_TRANSACTION_BRIDGE_H

#include <stdint.h>

#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_host_transaction_abi.h"
#include "bx_ntvdm_instruction_window_abi.h"
#include "bx_ntvdm_startup_plan_abi.h"
#include "bx_ntvdm_dem_dta_service.h"
#include "bx_ntvdm_dem_hard_error_service.h"
#include "bx_ntvdm_cmd_set_info_service.h"
#include "bx_ntvdm_multi_write_transaction.h"

#ifdef __cplusplus
extern "C" {
#endif

int bx_ntvdm_startup_transaction_bridge_v1_prepare_from_environment(
    bx_ntvdm_startup_plan_v1 *plan, const uint8_t **payload,
    uint64_t *payload_bytes);
int bx_ntvdm_startup_transaction_bridge_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_exception_result_v1 *result);
int bx_ntvdm_startup_transaction_bridge_v1_take_pending_write(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_host_transaction_v1 *transaction,
    const uint8_t **payload, uint64_t *payload_bytes,
    uint64_t *resume_rip);
int bx_ntvdm_startup_transaction_bridge_v1_copy_host_drive_inventory(
    uint8_t out_types[26]);
int bx_ntvdm_startup_transaction_bridge_v1_store_dta_registration(
    const bx_ntvdm_dem_dta_registration_v1 *registration);
int bx_ntvdm_startup_transaction_bridge_v1_store_hard_error_registration(
    const bx_ntvdm_dem_hard_error_registration_v1 *registration);
int bx_ntvdm_startup_transaction_bridge_v1_store_cmd_set_info_registration(
    const bx_ntvdm_cmd_set_info_registration_v1 *registration);
int bx_ntvdm_startup_transaction_bridge_v1_prepare_multi_write(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window);
int bx_ntvdm_startup_transaction_bridge_v1_take_pending_multi_write(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    const uint8_t **payload, uint64_t *payload_bytes);

#ifdef __cplusplus
}
#endif

#endif
