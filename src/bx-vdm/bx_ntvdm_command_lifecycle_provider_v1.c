#include "bx_ntvdm_command_lifecycle_provider_v1.h"
#include <string.h>

static int selected(const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window)
{
    return event && cpu && window && bx_ntvdm_exception_event_v1_valid(event) &&
        bx_ntvdm_cpu_state_v1_valid(cpu) && bx_ntvdm_instruction_window_v1_valid(window) &&
        event->vector==6u && cpu->execution_mode==BX_NTVDM_CPU_EXECUTION_REAL &&
        event->fault_rip<=UINT64_MAX-4u && window->valid_bytes>=4u &&
        window->bytes[0]==0xc4u && window->bytes[1]==0xc4u && window->bytes[2]==0x54u;
}
int bx_ntvdm_command_lifecycle_provider_v1_initialize(
    bx_ntvdm_command_lifecycle_provider_v1 *provider)
{
    if (!provider) return 0;
    memset(provider,0,sizeof(*provider));
    provider->magic=BX_NTVDM_COMMAND_LIFECYCLE_PROVIDER_V1_MAGIC;
    provider->abi_version=BX_NTVDM_COMMAND_LIFECYCLE_PROVIDER_V1_VERSION;
    provider->struct_bytes=(uint32_t)sizeof(*provider); provider->initialized=1u;
    return bx_ntvdm_command_lifecycle_provider_v1_valid(provider);
}
int bx_ntvdm_command_lifecycle_provider_v1_valid(
    const bx_ntvdm_command_lifecycle_provider_v1 *provider)
{
    return provider && provider->magic==BX_NTVDM_COMMAND_LIFECYCLE_PROVIDER_V1_MAGIC &&
        provider->abi_version==BX_NTVDM_COMMAND_LIFECYCLE_PROVIDER_V1_VERSION &&
        provider->struct_bytes==sizeof(*provider) && provider->initialized==1u;
}
int bx_ntvdm_command_lifecycle_provider_v1_owns_service(uint8_t service)
{ return service==0u || service==3u || service==11u; }
int bx_ntvdm_command_lifecycle_provider_v1_dispatch(
    bx_ntvdm_command_lifecycle_provider_v1 *provider,
    bx_ntvdm_cmd_get_next_state_v1 *get_next,
    const byob_launch_plan_v2 *launch_plan,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    if (!bx_ntvdm_command_lifecycle_provider_v1_valid(provider) || !result ||
        !selected(event,cpu,window) ||
        !bx_ntvdm_command_lifecycle_provider_v1_owns_service(window->bytes[3])) return 0;
    switch (window->bytes[3]) {
    case 0u:
        /* cmdExitVDM calls private TerminateVDM.  The current lifecycle
           boundary may only report an accepted typed stop; it never exits
           the host process or claims a CLI completion result. */
        return bx_ntvdm_cpu_result_v2_stop(result);
    case 3u:
        /* cmdSaveWorld's body is compiled out in the OpenNT source. */
        return bx_ntvdm_cpu_result_v2_resume(result,event->fault_rip+4u);
    case 11u:
        if (get_next && launch_plan && get_next->delivered!=0u)
            return bx_ntvdm_cmd_return_exit_code_v1_dispatch(get_next,launch_plan,
                event,cpu,window,result);
        /* Without an admitted original command broker, preserve the current
           source-derived no-reentry answer.  It is not a terminal record. */
        return bx_ntvdm_cpu_result_v2_resume(result,event->fault_rip+4u) &&
            bx_ntvdm_cpu_result_v2_set_cf(result,0) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta,0u,
                (uint16_t)(cpu->eax&0xff00u));
    default: return 0;
    }
}
