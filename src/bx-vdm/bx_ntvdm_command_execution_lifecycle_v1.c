#include "bx_ntvdm_command_execution_lifecycle_v1.h"

#include <string.h>

/* Win32 ERROR_NOT_SUPPORTED.  The provider intentionally keeps this fixed
 * copied error value rather than importing a host handle or ambient error. */
#define BX_NTVDM_COMMAND_EXECUTION_ERROR_NOT_SUPPORTED 50u

static int selected(const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window)
{
    return event != 0 && cpu != 0 && window != 0 &&
        bx_ntvdm_exception_event_v1_valid(event) &&
        bx_ntvdm_cpu_state_v1_valid(cpu) &&
        bx_ntvdm_instruction_window_v1_valid(window) && event->vector == 6u &&
        cpu->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        event->fault_rip <= UINT64_MAX - 4u && window->valid_bytes >= 4u &&
        window->bytes[0] == 0xc4u && window->bytes[1] == 0xc4u &&
        window->bytes[2] == 0x54u;
}

void bx_ntvdm_command_execution_lifecycle_v1_initialize(
    bx_ntvdm_command_execution_lifecycle_v1 *value)
{
    if (value == 0) return;
    memset(value, 0, sizeof(*value));
    value->magic = BX_NTVDM_COMMAND_EXECUTION_LIFECYCLE_V1_MAGIC;
    value->abi_version = BX_NTVDM_COMMAND_EXECUTION_LIFECYCLE_V1_VERSION;
    value->struct_bytes = sizeof(*value);
}

int bx_ntvdm_command_execution_lifecycle_v1_valid(
    const bx_ntvdm_command_execution_lifecycle_v1 *value)
{
    return value != 0 &&
        value->magic == BX_NTVDM_COMMAND_EXECUTION_LIFECYCLE_V1_MAGIC &&
        value->abi_version == BX_NTVDM_COMMAND_EXECUTION_LIFECYCLE_V1_VERSION &&
        value->struct_bytes == sizeof(*value) && value->reserved0 == 0u;
}

int bx_ntvdm_command_execution_lifecycle_v1_dispatch(
    bx_ntvdm_command_execution_lifecycle_v1 *value,
    bx_ntvdm_cmd_get_next_state_v1 *get_next,
    const byob_launch_plan_v2 *launch_plan,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    uint16_t ax;
    (void)get_next;
    (void)launch_plan;
    if (!bx_ntvdm_command_execution_lifecycle_v1_valid(value) || result == 0 ||
        !selected(event, cpu, window)) return 0;
    switch (window->bytes[3]) {
    case 7u:
        /* The declared profile has a DOS-only executable domain.  This is
         * cmdCheckBinary's DontCheckDosBinaryType success branch, not a host
         * pathname probe or a synthetic binary classification. */
        return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
            bx_ntvdm_cpu_result_v2_set_cf(result, 0);
    case 8u:
    case 10u:
        /* cmdExec/cmdExecComspec32 report synchronous launch failure with
         * CF clear and AL=GetLastError().  Until their complete typed child
         * plus opaque-stream owner is admitted, return the explicit modern
         * equivalent.  Do not stop the machine or expose a HANDLE. */
        if (value->deferred_execution_count == UINT32_MAX) return 0;
        ++value->deferred_execution_count;
        ax = (uint16_t)((cpu->eax & 0xff00u) |
            BX_NTVDM_COMMAND_EXECUTION_ERROR_NOT_SUPPORTED);
        return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
            bx_ntvdm_cpu_result_v2_set_cf(result, 0) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, ax);
    default:
        return 0;
    }
}
