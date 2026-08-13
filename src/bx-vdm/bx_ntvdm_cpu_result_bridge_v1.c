#include "bx_ntvdm_cpu_result_bridge_v1.h"

#include "bx_ntvdm_bios_memory_service.h"
#include "bx_ntvdm_dem_boot_drive_service.h"
#include "bx_ntvdm_cmd_set_info_service.h"
#include "bx_ntvdm_dem_debug_service.h"
#include "bx_ntvdm_dem_dta_service.h"
#include "bx_ntvdm_dem_drive_service.h"
#include "bx_ntvdm_dem_hard_error_service.h"
#include "bx_ntvdm_startup_transaction_bridge.h"

int bx_ntvdm_cpu_result_bridge_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    bx_ntvdm_exception_result_v1 legacy;
    bx_ntvdm_dem_hard_error_registration_v1 hard_error_registration;
    bx_ntvdm_cmd_set_info_registration_v1 cmd_set_info_registration;
    uint8_t inventory_types[26];

    if (result == 0 || !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_instruction_window_v1_valid(window)) return 0;
    bx_ntvdm_cpu_result_v2_pass_through(result);
    if (!bx_ntvdm_bios_memory_service_v1_dispatch(event, cpu_before, window,
            &legacy) && !bx_ntvdm_dem_boot_drive_service_v1_dispatch(event,
            cpu_before, window, &legacy) && !bx_ntvdm_dem_debug_service_v1_dispatch(
            event, cpu_before, window, &legacy)) {
        if (bx_ntvdm_startup_transaction_bridge_v1_copy_host_drive_inventory(
                inventory_types) && bx_ntvdm_dem_drive_service_v1_dispatch(
                inventory_types, event, cpu_before, window, result))
            return bx_ntvdm_cpu_result_v2_valid(result);
        if (bx_ntvdm_dem_hard_error_service_v1_dispatch(event, cpu_before,
                window, &hard_error_registration, result) &&
            bx_ntvdm_startup_transaction_bridge_v1_store_hard_error_registration(
                &hard_error_registration)) return bx_ntvdm_cpu_result_v2_valid(result);
        if (bx_ntvdm_cmd_set_info_service_v1_dispatch(event, cpu_before,
                window, &cmd_set_info_registration, result) &&
            bx_ntvdm_startup_transaction_bridge_v1_store_cmd_set_info_registration(
                &cmd_set_info_registration)) return bx_ntvdm_cpu_result_v2_valid(result);
        return 1;
    }
    if (legacy.disposition != BX_NTVDM_EXCEPTION_RESULT_RESUME ||
        !bx_ntvdm_cpu_delta_v1_valid(&legacy.cpu_delta) ||
        !bx_ntvdm_cpu_result_v2_resume(result, legacy.resume_rip)) {
        bx_ntvdm_cpu_result_v2_pass_through(result);
        return 0;
    }
    result->cpu_delta = legacy.cpu_delta;
    return bx_ntvdm_cpu_result_v2_valid(result);
}

int bx_ntvdm_cpu_result_bridge_v1_dispatch_guest_read(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_guest_read_action_v1 *action)
{
    if (action == 0 || !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_instruction_window_v1_valid(window)) return 0;
    bx_ntvdm_guest_read_action_v1_pass_through(action);
    if (bx_ntvdm_dem_dta_service_v1_dispatch(event, cpu_before, window,
            action)) return bx_ntvdm_guest_read_action_v1_valid(action);
    return 1;
}

int bx_ntvdm_cpu_result_bridge_v1_complete_guest_read(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_read_action_v1 *action,
    const uint8_t *bytes, uint64_t byte_count,
    bx_ntvdm_cpu_result_v2 *result)
{
    bx_ntvdm_dem_dta_registration_v1 registration;
    if (result == 0 || !bx_ntvdm_dem_dta_service_v1_complete(event,
            cpu_before, action, bytes, byte_count, &registration, result) ||
        !bx_ntvdm_startup_transaction_bridge_v1_store_dta_registration(
            &registration)) return 0;
    return bx_ntvdm_cpu_result_v2_valid(result);
}
