#include "bx_ntvdm_command_lifecycle_provider_v1.h"

static void event_initialize(bx_ntvdm_exception_event_v1 *event)
{ *event=(bx_ntvdm_exception_event_v1){BX_NTVDM_EXCEPTION_ABI_MAGIC,BX_NTVDM_EXCEPTION_ABI_VERSION,sizeof(*event),BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION,0u,6u,0u,0u,0x200u}; }
static void window_initialize(bx_ntvdm_instruction_window_v1 *window,uint8_t service)
{ const uint8_t bytes[4]={0xc4u,0xc4u,0x54u,service}; bx_ntvdm_instruction_window_v1_capture(window,bytes,sizeof(bytes)); }
int main(void)
{
    bx_ntvdm_command_lifecycle_provider_v1 provider; bx_ntvdm_cmd_get_next_state_v1 state;
    bx_ntvdm_exception_event_v1 event; bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window; bx_ntvdm_cpu_result_v2 result;
    bx_ntvdm_command_terminal_v1 terminal;
    byob_launch_plan_v2 plan={2u,1u,{1u,BYOB_LAUNCH_TARGET_KIND_V1_COM,0u,{0}}};
    if(!bx_ntvdm_command_lifecycle_provider_v1_initialize(&provider)||
       !bx_ntvdm_command_lifecycle_provider_v1_valid(&provider)||
       !bx_ntvdm_command_lifecycle_provider_v1_owns_service(0u)||
       !bx_ntvdm_command_lifecycle_provider_v1_owns_service(3u)||
       !bx_ntvdm_command_lifecycle_provider_v1_owns_service(11u)||
       bx_ntvdm_command_lifecycle_provider_v1_owns_service(10u)) return 1;
    event_initialize(&event); bx_ntvdm_cpu_state_v1_initialize(&cpu,BX_NTVDM_CPU_EXECUTION_REAL);
    window_initialize(&window,0u);
    if(!bx_ntvdm_command_lifecycle_provider_v1_dispatch(&provider,0,0,&event,&cpu,&window,&result)||result.disposition!=BX_NTVDM_CPU_RESULT_V2_STOP) return 2;
    if(!bx_ntvdm_command_lifecycle_provider_v1_copy_terminal(&provider,&terminal)||
       terminal.present!=1u||terminal.terminal_kind!=BX_NTVDM_COMMAND_TERMINAL_V1_TOP_LEVEL_EXIT||
       terminal.has_dos_exit_code!=0u||terminal.dos_exit_code!=0u) return 7;
    window_initialize(&window,3u);
    if(!bx_ntvdm_command_lifecycle_provider_v1_dispatch(&provider,0,0,&event,&cpu,&window,&result)||result.disposition!=BX_NTVDM_CPU_RESULT_V2_RESUME||result.resume_rip!=0x204u||result.cpu_delta.gpr16_write_mask!=0u) return 3;
    window_initialize(&window,11u); cpu.eax=0xa500u;
    if(!bx_ntvdm_command_lifecycle_provider_v1_dispatch(&provider,0,0,&event,&cpu,&window,&result)||result.disposition!=BX_NTVDM_CPU_RESULT_V2_RESUME||result.eflags_values!=0u||result.cpu_delta.gpr16_values[0]!=0xa500u) return 4;
    bx_ntvdm_cmd_get_next_state_v1_initialize(&state); state.delivered=1u; cpu.edx=0x1234u;
    if(!bx_ntvdm_command_lifecycle_provider_v1_dispatch(&provider,&state,&plan,&event,&cpu,&window,&result)||result.disposition!=BX_NTVDM_CPU_RESULT_V2_RESUME||result.eflags_values!=0u||result.cpu_delta.gpr16_values[0]!=0x0034u||state.terminal.present!=1u||state.terminal.dos_exit_code!=0x34u) return 5;
    cpu.execution_mode=BX_NTVDM_CPU_EXECUTION_PROTECTED;
    if(bx_ntvdm_command_lifecycle_provider_v1_dispatch(&provider,&state,&plan,&event,&cpu,&window,&result)) return 6;
    return 0;
}
