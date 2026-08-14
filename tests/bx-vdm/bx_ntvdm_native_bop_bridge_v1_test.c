#include "bx_ntvdm_vdm_generic_ud_bridge_v1.h"
#include "bx_ntvdm_native_bop_composition_v1.h"
#include "bx-mantle/bx_ntvdm_a20_capability_v1.h"
#include "bx-mantle/bx_ntvdm_extended_memory_v1.h"
#include <string.h>

static uint32_t a20=1u,handle=1u;static uint64_t address=UINT64_C(0x100000);
int bx_ntvdm_boot_namespace_composition_v1_handle(const struct bx_ntvdm_generic_ud_event_v1 *e,struct bx_ntvdm_generic_ud_outcome_v1 *o)
{ (void)e;(void)o;return 0; }
void bx_ntvdm_a20_capability_v1_set_lifecycle_active(uint32_t v){(void)v;}
void bx_ntvdm_a20_capability_v1_dispatch(const struct bx_ntvdm_a20_capability_request_v1 *q,struct bx_ntvdm_a20_capability_result_v1 *r)
{ r->status=BX_NTVDM_A20_CAPABILITY_OK;if(q->operation==BX_NTVDM_A20_CAPABILITY_SET)a20=q->requested_enabled;r->enabled=a20; }
void bx_ntvdm_extended_memory_v1_set_lifecycle_active(uint32_t v){(void)v;}
void bx_ntvdm_extended_memory_v1_dispatch(const struct bx_ntvdm_extended_memory_request_v1 *q,struct bx_ntvdm_extended_memory_result_v1 *r)
{ memset(r,0,sizeof(*r));r->status=BX_NTVDM_EXTMEM_OK;r->kib=3072u;if(q->operation==BX_NTVDM_EXTMEM_ALLOCATE){r->handle=handle++;r->physical_address=address;address+=(uint64_t)q->kib*1024u;} }
int main(void)
{ bx_ntvdm_native_bop_composition_v1 c;struct bx_ntvdm_generic_ud_event_v1 e;struct bx_ntvdm_generic_ud_outcome_v1 o;
  if(!bx_ntvdm_native_bop_composition_v1_initialize(&c)||!bx_ntvdm_native_bop_composition_v1_bind(&c))return 1;
  memset(&e,0,sizeof(e));e.magic=BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;e.abi_version=BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;e.struct_bytes=sizeof(e);e.vector=6u;e.execution_mode=BX_NTVDM_CPU_EXECUTION_REAL;e.fault_rip=0x100u;e.window_bytes=4u;e.window[0]=0xc4u;e.window[1]=0xc4u;e.window[2]=0x52u;e.window[3]=2u;e.edx=64u;
  if(!bx_ntvdm_mantle_generic_ud_bridge_v1(&e,&o)||o.disposition!=BX_NTVDM_GENERIC_UD_RESUME||o.gpr16_values[0]!=1088u)return 2;
  bx_ntvdm_native_bop_composition_v1_unbind(&c);return 0; }
