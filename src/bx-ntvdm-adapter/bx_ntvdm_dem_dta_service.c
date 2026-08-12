#include "bx_ntvdm_dem_dta_service.h"

#define BX_NTVDM_DEM_DTA_DOSWOW_SFT_OFFSET 32u
#define BX_NTVDM_DEM_DTA_READ_BYTES 2u
#define BX_NTVDM_DEM_DTA_REAL_APERTURE UINT64_C(0x100000)

static int bx_ntvdm_dem_dta_physical(uint16_t segment, uint16_t offset,
    uint64_t *physical)
{
    uint64_t value;
    if (physical == 0) return 0;
    value = ((uint64_t)segment << 4) + (uint64_t)offset;
    if (value >= BX_NTVDM_DEM_DTA_REAL_APERTURE) return 0;
    *physical = value;
    return 1;
}

static int bx_ntvdm_dem_dta_matches(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window)
{
    return event != 0 && cpu_before != 0 && window != 0 &&
        bx_ntvdm_exception_event_v1_valid(event) &&
        bx_ntvdm_cpu_state_v1_valid(cpu_before) &&
        bx_ntvdm_instruction_window_v1_valid(window) && event->vector == 6u &&
        cpu_before->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        window->valid_bytes >= 4u && window->bytes[0] == 0xc4u &&
        window->bytes[1] == 0xc4u && window->bytes[2] == 0x50u &&
        window->bytes[3] == 0x1bu;
}

int bx_ntvdm_dem_dta_service_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_guest_read_action_v1 *action)
{
    uint16_t si;
    uint64_t sft_offset_location;
    if (action == 0 || !bx_ntvdm_dem_dta_matches(event, cpu_before, window) ||
        (si = (uint16_t)cpu_before->esi) > UINT16_MAX - BX_NTVDM_DEM_DTA_DOSWOW_SFT_OFFSET ||
        !bx_ntvdm_dem_dta_physical(cpu_before->ds,
            (uint16_t)(si + BX_NTVDM_DEM_DTA_DOSWOW_SFT_OFFSET),
            &sft_offset_location)) return 0;
    return bx_ntvdm_guest_read_action_v1_need_read(action, sft_offset_location,
        BX_NTVDM_DEM_DTA_READ_BYTES);
}

int bx_ntvdm_dem_dta_service_v1_complete(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_read_action_v1 *action,
    const uint8_t *bytes, uint64_t byte_count,
    bx_ntvdm_dem_dta_registration_v1 *registration,
    bx_ntvdm_cpu_result_v2 *result)
{
    uint16_t si, sft_offset;
    uint64_t expected_read;
    if (event == 0 || cpu_before == 0 || action == 0 || bytes == 0 ||
        registration == 0 || result == 0 ||
        !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_guest_read_action_v1_valid(action) ||
        action->disposition != BX_NTVDM_GUEST_READ_ACTION_V1_NEED_READ ||
        byte_count != BX_NTVDM_DEM_DTA_READ_BYTES ||
        (si = (uint16_t)cpu_before->esi) > UINT16_MAX - BX_NTVDM_DEM_DTA_DOSWOW_SFT_OFFSET ||
        event->fault_rip > UINT64_MAX - 4u) return 0;
    if (!bx_ntvdm_dem_dta_physical(cpu_before->ds,
            (uint16_t)(si + BX_NTVDM_DEM_DTA_DOSWOW_SFT_OFFSET),
            &expected_read) || action->guest_read.address != expected_read ||
        action->guest_read.length != BX_NTVDM_DEM_DTA_READ_BYTES ||
        !bx_ntvdm_dem_dta_physical(cpu_before->ds, (uint16_t)cpu_before->eax,
            &registration->dta_location) ||
        !bx_ntvdm_dem_dta_physical(cpu_before->ds, (uint16_t)cpu_before->edx,
            &registration->current_pdb) ||
        !bx_ntvdm_dem_dta_physical(cpu_before->ds, (uint16_t)cpu_before->ecx,
            &registration->extended_error)) return 0;
    sft_offset = (uint16_t)((uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8));
    if (!bx_ntvdm_dem_dta_physical(cpu_before->ds, sft_offset,
            &registration->sft_head) ||
        !bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u)) return 0;
    return 1;
}
