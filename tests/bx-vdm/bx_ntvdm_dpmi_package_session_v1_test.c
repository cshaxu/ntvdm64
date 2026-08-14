#include "bx_ntvdm_dpmi_package_session_v1.h"
#include <string.h>

static int dispatch(bx_ntvdm_dpmi_package_session_v1 *s,uint8_t service,
  bx_ntvdm_cpu_result_v2 *r)
{ uint8_t bytes[4]={0xc4u,0xc4u,0x53u,service};bx_ntvdm_instruction_window_v1 w;
  bx_ntvdm_bop_ingress_v1 i;bx_ntvdm_bop_provider_selection_v1 p;
  bx_ntvdm_exception_event_v1 e;bx_ntvdm_cpu_state_v1 c;
  bx_ntvdm_instruction_window_v1_capture(&w,bytes,4u);
  if(!bx_ntvdm_bop_ingress_v1_classify(&w,&i)||!bx_ntvdm_bop_provider_registry_v1_select(&i,&p))return 0;
  memset(&e,0,sizeof(e));e.magic=BX_NTVDM_EXCEPTION_ABI_MAGIC;e.abi_version=BX_NTVDM_EXCEPTION_ABI_VERSION;
  e.struct_bytes=sizeof(e);e.kind=BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;e.vector=6u;
  bx_ntvdm_cpu_state_v1_initialize(&c,BX_NTVDM_CPU_EXECUTION_PROTECTED);
  return bx_ntvdm_dpmi_package_session_v1_dispatch(s,&i,&p,&e,&c,&w,r); }

int main(void)
{ bx_ntvdm_dpmi_package_session_v1 s;bx_ntvdm_cpu_result_v2 r;uint8_t n;
  if(!bx_ntvdm_dpmi_package_session_v1_initialize(&s))return 1;
  for(n=0;n<25u;n++){
    if(!dispatch(&s,n,&r))return 2;
    if(r.disposition!=BX_NTVDM_CPU_RESULT_V2_STOP)return 3;
  }
  return 0; }
