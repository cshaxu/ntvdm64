#include "bx_ntvdm_cmd_comspec_bootstrap_service.h"
#include <stdio.h>
#include <string.h>

static void event(bx_ntvdm_exception_event_v1 *e, uint64_t rip)
{
    *e = (bx_ntvdm_exception_event_v1){ BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(*e),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0, 6, 0, 0, rip };
}
static void window(bx_ntvdm_instruction_window_v1 *w, unsigned char svc)
{ const unsigned char v[] = { 0xc4u, 0xc4u, 0x54u, svc }; bx_ntvdm_instruction_window_v1_capture(w,v,4u); }
int main(void)
{
    bx_ntvdm_cmd_comspec_bootstrap_v1 state;
    bx_ntvdm_exception_event_v1 e;
    bx_ntvdm_cpu_state_v1 c;
    bx_ntvdm_instruction_window_v1 w;
    bx_ntvdm_guest_gather_read_action_v1 action;
    bx_ntvdm_cpu_result_v2 result;
    bx_ntvdm_multi_write_transaction_v1 tx;
    unsigned char payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD];
    unsigned char path[BX_NTVDM_CMD_COMSPEC_TEXT_MAX_BYTES] = "C:\\ALT.COM";
    uint32_t need;
    event(&e,0x200u); bx_ntvdm_cpu_state_v1_initialize(&c,BX_NTVDM_CPU_EXECUTION_REAL);
    c.ds=0x100u; c.edx=0x20u; window(&w,2u);
    bx_ntvdm_cmd_comspec_bootstrap_v1_initialize(&state);
    if(!bx_ntvdm_cmd_comspec_bootstrap_v1_prepare_comspec(&e,&c,&w,&state,&action)||
       action.range_count!=1u||action.ranges[0].address!=0x1020u||
       action.total_bytes!=BX_NTVDM_CMD_COMSPEC_TEXT_MAX_BYTES)return 1;
    memset(path,'A',sizeof(path));
    if(bx_ntvdm_cmd_comspec_bootstrap_v1_complete_comspec(&e,&c,&action,path,sizeof(path),0,&state,&result)||
       state.stage!=BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_WAIT_COMSPEC)return 6;
    memcpy(path, "C:\\ALT.COM", sizeof("C:\\ALT.COM"));
    if(!bx_ntvdm_cmd_comspec_bootstrap_v1_complete_comspec(&e,&c,&action,path,sizeof(path),0,&state,&result)||
       result.resume_rip!=0x204u||result.cpu_delta.gpr16_values[0]!=1u||
       state.stage!=BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_ENVIRONMENT_READY||
       memcmp(state.environment,"COMSPEC=C:\\ALT.COM",19u))return 2;
    if(bx_ntvdm_cmd_comspec_bootstrap_v1_prepare_comspec(&e,&c,&w,&state,&action))return 5;
    if(!bx_ntvdm_cmd_comspec_bootstrap_v1_repeat_comspec(&e,&c,&w,&state,&result)||
       result.resume_rip!=0x204u||result.cpu_delta.gpr16_write_mask!=0u)return 7;
    event(&e,0x300u); c.es=0x200u; c.ebx=0u; window(&w,15u);
    if(!bx_ntvdm_cmd_comspec_bootstrap_v1_prepare_environment(&e,&c,&w,&state,&tx,payload)||
       tx.writes.write_count!=0u||tx.result.cpu_delta.gpr16_values[3]==0u)return 3;
    need=tx.result.cpu_delta.gpr16_values[3]; c.ebx=need;
    if(!bx_ntvdm_cmd_comspec_bootstrap_v1_prepare_environment(&e,&c,&w,&state,&tx,payload)||
       tx.writes.write_count!=1u||tx.writes.writes[0].guest_physical_address!=0x2000u||
       tx.writes.writes[0].byte_count!=state.environment_bytes||tx.result.cpu_delta.gpr16_values[3]!=need||
       memcmp(payload,state.environment,state.environment_bytes))return 4;
    if(!bx_ntvdm_cmd_comspec_bootstrap_v1_complete_environment(&state,&tx)||
       state.stage!=BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_ENVIRONMENT_DELIVERED)return 8;
    c.ebx=0xffffu;
    if(!bx_ntvdm_cmd_comspec_bootstrap_v1_prepare_environment(&e,&c,&w,&state,&tx,payload)||
       tx.writes.write_count!=1u||tx.result.cpu_delta.gpr16_values[3]!=need||
       memcmp(payload,state.environment,state.environment_bytes)||
       !bx_ntvdm_cmd_comspec_bootstrap_v1_complete_environment(&state,&tx))return 9;
    if(!bx_ntvdm_cmd_comspec_bootstrap_v1_close_initial_environment(&state)||
       state.stage!=BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_ENVIRONMENT_CLOSED)return 10;
    if(!bx_ntvdm_cmd_comspec_bootstrap_v1_prepare_environment(&e,&c,&w,&state,&tx,payload)||
       tx.writes.write_count!=0u||tx.result.cpu_delta.gpr16_write_mask!=(1u<<3)||
       tx.result.cpu_delta.gpr16_values[3]!=0u)return 11;
    puts("bx-ntvdm COMMAND COMSPEC bootstrap: gather, retry, and environment write verified");
    return 0;
}
