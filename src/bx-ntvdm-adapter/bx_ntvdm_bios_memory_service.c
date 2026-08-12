#include "bx_ntvdm_bios_memory_service.h"

/* r20 CPU5 stock-firmware snapshot: BDA 0040:0013 = 7f 02 (little endian).
 * This is adapter profile data, never a Bochs default or a direct memory read. */
#define BX_NTVDM_R20_CONVENTIONAL_MEMORY_KIB 0x027fu

/* The accepted bare-machine profile fixes `megs: 4`; AH=88h reports the
 * contiguous memory above the first MiB, in KiB.  Like the conventional
 * value above, this is profile data, not a query into Bochs or CMOS. */
#define BX_NTVDM_R25_EXTENDED_MEMORY_KIB 0x0c00u

int bx_ntvdm_bios_memory_service_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_exception_result_v1 *result)
{
    uint16_t result_ax;
    if (event == 0 || cpu_before == 0 || window == 0 || result == 0 ||
        !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_instruction_window_v1_valid(window) ||
        event->vector != 6u || cpu_before->execution_mode !=
            BX_NTVDM_CPU_EXECUTION_REAL || window->valid_bytes < 3u ||
        window->bytes[0] != 0xc4u || window->bytes[1] != 0xc4u ||
        event->fault_rip > UINT64_MAX - 3u)
        return 0;
    if (window->bytes[2] == 0x12u) {
        result_ax = BX_NTVDM_R20_CONVENTIONAL_MEMORY_KIB;
    } else if (window->bytes[2] == 0x15u &&
        ((cpu_before->eax >> 8) & 0xffu) == 0x88u) {
        result_ax = BX_NTVDM_R25_EXTENDED_MEMORY_KIB;
    } else {
        return 0;
    }
    if (!bx_ntvdm_exception_result_v1_resume(result, event->fault_rip + 3u) ||
        !bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u,
            result_ax)) {
        bx_ntvdm_exception_result_v1_pass_through(result);
        return 0;
    }
    return 1;
}
