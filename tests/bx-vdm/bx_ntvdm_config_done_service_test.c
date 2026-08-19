#include "bop-v1/bx_ntvdm_config_done_service.h"
#include <stdint.h>
#include <stdio.h>
int main(void){bx_ntvdm_exception_event_v1 e={BX_NTVDM_EXCEPTION_ABI_MAGIC,BX_NTVDM_EXCEPTION_ABI_VERSION,sizeof(e),BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION,0,6,0,0,0x1234};bx_ntvdm_cpu_state_v1 c;bx_ntvdm_instruction_window_v1 w;bx_ntvdm_cpu_result_v2 r;bx_ntvdm_cpu_state_v1_initialize(&c,BX_NTVDM_CPU_EXECUTION_REAL);bx_ntvdm_instruction_window_v1_capture(&w,(const uint8_t[]){0xc4,0xc4,0x5e},3);if(!bx_ntvdm_config_done_service_v1_dispatch(&e,&c,&w,&r)||r.resume_rip!=0x1237||r.cpu_delta.gpr16_write_mask||r.eflags_write_mask)return 1;c.eax=1;if(bx_ntvdm_config_done_service_v1_dispatch(&e,&c,&w,&r))return 2;puts("bx-ntvdm config done: exact BOP 5E AL=0 empty return verified");return 0;}
