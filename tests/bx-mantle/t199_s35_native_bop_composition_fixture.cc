#include "bochs.h"
#include "bx-mantle/bx_ntvdm_minimal_machine.h"
#include "bx-vdm/bx_ntvdm_native_bop_composition_v1.h"

static int call(uint8_t selector,uint8_t service,uint32_t mode,uint32_t ax,
  uint32_t bx,uint32_t dx,bx_ntvdm_generic_ud_outcome_v1 *out)
{ bx_ntvdm_generic_ud_event_v1 e = {};
  e.magic=BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;e.abi_version=BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
  e.struct_bytes=sizeof(e);e.vector=6u;e.execution_mode=mode;e.fault_rip=0x100u;
  e.eax=ax;e.ebx=bx;e.edx=dx;e.window_bytes=4u;e.window[0]=0xc4u;e.window[1]=0xc4u;e.window[2]=selector;e.window[3]=service;
  return bx_ntvdm_native_bop_composition_v1_handle(&e,out); }

int main()
{ bx_ntvdm_minimal_machine_c machine;bx_ntvdm_native_bop_composition_v1 c;bx_ntvdm_generic_ud_outcome_v1 out;
  if(machine.initialize(0x400000,0x400000)!=BX_NTVDM_MINIMAL_MACHINE_OK)return 1;
  if(!bx_ntvdm_native_bop_composition_v1_initialize(&c)||!bx_ntvdm_native_bop_composition_v1_bind(&c))return 2;
  if(!call(0x52u,0u,BX_NTVDM_CPU_EXECUTION_REAL,0u,0u,0u,&out)||out.disposition!=BX_NTVDM_GENERIC_UD_RESUME||out.gpr16_values[0]!=1u)return 3;
  if(!call(0x52u,2u,BX_NTVDM_CPU_EXECUTION_REAL,0u,0u,64u,&out)||out.disposition!=BX_NTVDM_GENERIC_UD_RESUME||out.gpr16_values[0]!=1088u)return 4;
  if(!call(0x53u,18u,BX_NTVDM_CPU_EXECUTION_PROTECTED,0u,0u,0u,&out)||out.disposition!=BX_NTVDM_GENERIC_UD_STOP)return 5;
  if(call(0x53u,0u,BX_NTVDM_CPU_EXECUTION_PROTECTED,0u,0u,0u,&out))return 6;
  bx_ntvdm_native_bop_composition_v1_unbind(&c);
  return machine.cleanup()==BX_NTVDM_MINIMAL_MACHINE_OK?0:7; }
