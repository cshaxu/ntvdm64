#include "bop-v1/bx_ntvdm_top_level_package_facade_v1.h"
#include "bop-v1/bx_ntvdm_machine_bop_facade_v1.h"
#include <string.h>

static int invoke(uint8_t selector, uint32_t route, uint32_t disposition, int machine)
{
    uint8_t bytes[3] = { 0xc4u, 0xc4u, selector }; bx_ntvdm_instruction_window_v1 w;
    bx_ntvdm_bop_ingress_v1 i; bx_ntvdm_bop_provider_selection_v1 p;
    bx_ntvdm_exception_event_v1 e; bx_ntvdm_cpu_state_v1 c; bx_ntvdm_cpu_result_v2 r; uint32_t got;
    memset(&e, 0, sizeof(e)); e.magic=BX_NTVDM_EXCEPTION_ABI_MAGIC; e.abi_version=BX_NTVDM_EXCEPTION_ABI_VERSION;
    e.struct_bytes=sizeof(e); e.kind=BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION; e.vector=6u; e.fault_rip=0x100u;
    bx_ntvdm_cpu_state_v1_initialize(&c, BX_NTVDM_CPU_EXECUTION_REAL); if(selector==0x5eu)c.eax=1u;
    bx_ntvdm_instruction_window_v1_capture(&w,bytes,sizeof(bytes));
    if(!bx_ntvdm_bop_ingress_v1_classify(&w,&i)||!bx_ntvdm_bop_provider_registry_v1_select(&i,&p))return 0;
    if(machine) { if(!bx_ntvdm_machine_bop_facade_v1_classify(&i,&p,&got)||got!=route)return 0;
        if(route==BX_NTVDM_MACHINE_BOP_HANDOFF)return !bx_ntvdm_machine_bop_facade_v1_dispatch(got,&e,&c,&r);
        return bx_ntvdm_machine_bop_facade_v1_dispatch(got,&e,&c,&r)&&r.disposition==disposition; }
    return bx_ntvdm_top_level_package_facade_v1_classify(&i,&p,&got)&&got==route&&
        bx_ntvdm_top_level_package_facade_v1_dispatch(got,&e,&c,&r)&&r.disposition==disposition&&
        (disposition!=BX_NTVDM_CPU_RESULT_V2_RESUME || r.resume_rip==0x103u);
}
int main(void)
{
    return invoke(0x59u,BX_NTVDM_TOP_LEVEL_PACKAGE_TERMINAL,BX_NTVDM_CPU_RESULT_V2_STOP,0)&&
      invoke(0x5au,BX_NTVDM_TOP_LEVEL_PACKAGE_IDLE,BX_NTVDM_CPU_RESULT_V2_RESUME,0)&&
      invoke(0x5bu,BX_NTVDM_TOP_LEVEL_PACKAGE_TERMINAL,BX_NTVDM_CPU_RESULT_V2_STOP,0)&&
      invoke(0x5eu,BX_NTVDM_TOP_LEVEL_PACKAGE_CONFIG,BX_NTVDM_CPU_RESULT_V2_RESUME,0)&&
      invoke(0xfeu,BX_NTVDM_TOP_LEVEL_PACKAGE_TERMINAL,BX_NTVDM_CPU_RESULT_V2_STOP,0)&&
      invoke(0x5cu,BX_NTVDM_MACHINE_BOP_DEFERRED_STOP,BX_NTVDM_CPU_RESULT_V2_STOP,1)&&
      invoke(0x5du,BX_NTVDM_MACHINE_BOP_DEFERRED_STOP,BX_NTVDM_CPU_RESULT_V2_STOP,1)&&
      invoke(0x5fu,BX_NTVDM_MACHINE_BOP_HANDOFF,BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH,1)&&
      invoke(0xfdu,BX_NTVDM_MACHINE_BOP_DEFERRED_STOP,BX_NTVDM_CPU_RESULT_V2_STOP,1) ? 0 : 1;
}