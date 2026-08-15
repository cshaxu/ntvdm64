#include "bx_ntvdm_cmd_cli_profile_service.h"

static int selected(const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window)
{
    return event && cpu && window && bx_ntvdm_exception_event_v1_valid(event) &&
        bx_ntvdm_cpu_state_v1_valid(cpu) &&
        bx_ntvdm_instruction_window_v1_valid(window) &&
        event->vector == 6u && cpu->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        window->valid_bytes >= 4u && window->bytes[0] == 0xc4u &&
        window->bytes[1] == 0xc4u && window->bytes[2] == 0x54u &&
        event->fault_rip <= UINT64_MAX - 4u;
}

int bx_ntvdm_cmd_cli_profile_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    uint16_t ax;

    if (!result || !selected(event, cpu, window)) return 0;
    switch (window->bytes[3]) {
    case 0u:
        /* cmdExitVDM: TerminateVDM().  The engine owns external status
         * transport; the host-service result is only controlled stop. */
        return bx_ntvdm_cpu_result_v2_stop(result);
    case 3u:
        /* cmdSaveWorld has no active body in the original source. */
        return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u);
    case 6u: case 8u: case 10u:
        /* Standard-stream and non-DOS process capabilities are whole
         * COMMAND-provider slices.  Until their opaque stream/typed child
         * lifecycle owners are admitted, retain the explicit terminal
         * disposition here rather than bypassing this package session. */
        return bx_ntvdm_cpu_result_v2_stop(result);
    case 7u:
        /* cmdCheckBinary's historical DontCheckDosBinaryType branch: the
         * declared CLI profile admits only DOS guest executables. */
        return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
            bx_ntvdm_cpu_result_v2_set_cf(result, 0);
    case 9u:
        /* cmdInitConsole changes only historical VDD/event-thread state. */
        return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u);
    case 11u:
        /* cmdReturnExitCode with no queued command: CF clear and AL receives
         * the contained profile's fixed successful child result (zero). */
        ax = (uint16_t)(cpu->eax & 0xff00u);
        return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
            bx_ntvdm_cpu_result_v2_set_cf(result, 0) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, ax);
    case 16u:
        /* cmdGetStartInfo: no separately created console in this profile. */
        ax = (uint16_t)(cpu->eax & 0xff00u);
        return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, ax);
    default:
        return 0;
    }
}
