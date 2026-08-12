#include "bx_ntvdm_cmd_set_info_service.h"

#define BX_NTVDM_CMD_SCS_TO_SYNC_OFFSET 206u
#define BX_NTVDM_CMD_REAL_APERTURE UINT64_C(0x100000)

static int physical(uint16_t seg, uint16_t off, uint64_t *out)
{
    uint64_t value = ((uint64_t)seg << 4) + off;
    if (out == 0 || value >= BX_NTVDM_CMD_REAL_APERTURE) return 0;
    *out = value; return 1;
}

int bx_ntvdm_cmd_set_info_service_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cmd_set_info_registration_v1 *registration,
    bx_ntvdm_cpu_result_v2 *result)
{
    uint16_t dx;
    if (event == 0 || cpu_before == 0 || window == 0 || registration == 0 ||
        result == 0 || !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_instruction_window_v1_valid(window) || event->vector != 6u ||
        cpu_before->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        window->valid_bytes < 4u || window->bytes[0] != 0xc4u ||
        window->bytes[1] != 0xc4u || window->bytes[2] != 0x54u ||
        window->bytes[3] != 0x05u || event->fault_rip > UINT64_MAX - 4u ||
        (dx = (uint16_t)cpu_before->edx) > UINT16_MAX - BX_NTVDM_CMD_SCS_TO_SYNC_OFFSET ||
        !physical(cpu_before->ds, dx, &registration->scs_info) ||
        !physical(cpu_before->ds, (uint16_t)(dx + BX_NTVDM_CMD_SCS_TO_SYNC_OFFSET),
            &registration->scs_to_sync) ||
        !physical(cpu_before->ds, (uint16_t)cpu_before->ebx,
            &registration->is_dos_binary) ||
        !physical(cpu_before->ds, (uint16_t)cpu_before->ecx,
            &registration->fd_access)) return 0;
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u);
}
