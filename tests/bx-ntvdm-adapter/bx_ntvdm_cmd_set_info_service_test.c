#include <stdio.h>
#include "bx_ntvdm_cmd_set_info_service.h"
int main(void) {
 bx_ntvdm_exception_event_v1 e={BX_NTVDM_EXCEPTION_ABI_MAGIC,BX_NTVDM_EXCEPTION_ABI_VERSION,sizeof(e),BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION,0u,6u,0u,0u,0x749du};
 bx_ntvdm_cpu_state_v1 c; bx_ntvdm_instruction_window_v1 w; bx_ntvdm_cmd_set_info_registration_v1 r; bx_ntvdm_cpu_result_v2 x;
 const uint8_t b[4]={0xc4u,0xc4u,0x54u,0x05u}; bx_ntvdm_cpu_state_v1_initialize(&c,BX_NTVDM_CPU_EXECUTION_REAL);
 c.ds=0x00a7u;c.edx=0x12f9u;c.ebx=0x0048u;c.ecx=0x0325u;bx_ntvdm_instruction_window_v1_capture(&w,b,sizeof(b));
 if(!bx_ntvdm_cmd_set_info_service_v1_dispatch(&e,&c,&w,&r,&x)||x.resume_rip!=0x74a1u||r.scs_info!=0x1d69u||r.scs_to_sync!=0x1e37u||r.is_dos_binary!=0x0ab8u||r.fd_access!=0x0d95u)return 1;
 w.bytes[3]=6u;if(bx_ntvdm_cmd_set_info_service_v1_dispatch(&e,&c,&w,&r,&x))return 2;puts("bx-ntvdm CmdSetInfo: source-derived locator registration verified");return 0;
}
