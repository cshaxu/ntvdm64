#include "bx_ntvdm_xms_package_session_v1.h"
#include "bx-mantle/bx_ntvdm_a20_capability_v1.h"
#include "bx-mantle/bx_ntvdm_extended_memory_v1.h"
#include "bx-mantle/bx_ntvdm_mechanical_action_v1.h"
#include <string.h>

static uint32_t a20_enabled = 1u;
static uint32_t next_handle = 1u;
static uint64_t next_address = UINT64_C(0x100000);
static uint8_t move_record[12];
static uint64_t moved_source, moved_destination, moved_bytes;

void bx_ntvdm_a20_capability_v1_set_lifecycle_active(uint32_t active) { (void)active; }
void bx_ntvdm_a20_capability_v1_dispatch(
    const struct bx_ntvdm_a20_capability_request_v1 *q,
    struct bx_ntvdm_a20_capability_result_v1 *r)
{ r->status=BX_NTVDM_A20_CAPABILITY_OK;r->enabled=a20_enabled;if(q->operation==BX_NTVDM_A20_CAPABILITY_SET)a20_enabled=q->requested_enabled;r->enabled=a20_enabled; }
void bx_ntvdm_extended_memory_v1_set_lifecycle_active(uint32_t active) { (void)active; }
void bx_ntvdm_extended_memory_v1_dispatch(
    const struct bx_ntvdm_extended_memory_request_v1 *q,
    struct bx_ntvdm_extended_memory_result_v1 *r)
{ memset(r,0,sizeof(*r));r->status=BX_NTVDM_EXTMEM_OK;r->kib=3072u;
  r->free_kib=2048u;r->largest_free_kib=1024u;
  if(q->operation==BX_NTVDM_EXTMEM_ALLOCATE){r->handle=next_handle++;r->kib=q->kib;r->physical_address=next_address;next_address+=(uint64_t)q->kib*1024u;}
  if(q->operation==BX_NTVDM_EXTMEM_MOVE_PHYSICAL){moved_source=q->source_address;moved_destination=q->destination_address;moved_bytes=q->byte_count;}
}
int bx_ntvdm_mantle_execute_mechanical_action_v1(struct bx_ntvdm_mechanical_action_v1 *action)
{ if(!action||action->kind!=BX_NTVDM_MECHANICAL_ACTION_V1_READ||action->payload_bytes!=12u)return 0;memcpy(action->payload,move_record,12u);return 1; }

static int dispatch(bx_ntvdm_xms_package_session_v1 *s,uint8_t service,
    uint16_t ax,uint16_t bx,uint16_t dx,bx_ntvdm_cpu_result_v2 *r)
{ uint8_t bytes[4]={0xc4u,0xc4u,0x52u,service};bx_ntvdm_instruction_window_v1 w;bx_ntvdm_bop_ingress_v1 i;bx_ntvdm_bop_provider_selection_v1 p;bx_ntvdm_exception_event_v1 e;bx_ntvdm_cpu_state_v1 c;
  bx_ntvdm_instruction_window_v1_capture(&w,bytes,4u);if(!bx_ntvdm_bop_ingress_v1_classify(&w,&i)||!bx_ntvdm_bop_provider_registry_v1_select(&i,&p))return 0;
  memset(&e,0,sizeof(e));e.magic=BX_NTVDM_EXCEPTION_ABI_MAGIC;e.abi_version=BX_NTVDM_EXCEPTION_ABI_VERSION;e.struct_bytes=sizeof(e);e.kind=BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;e.cpu_id=1u;e.vector=6u;e.fault_rip=0x100u;
  bx_ntvdm_cpu_state_v1_initialize(&c,BX_NTVDM_CPU_EXECUTION_REAL);c.eax=ax;c.ebx=bx;c.edx=dx;c.ss=0x100u;c.ebp=0x20u;
  return bx_ntvdm_xms_package_session_v1_dispatch(s,&i,&p,&e,&c,&w,r); }
int main(void)
{ bx_ntvdm_xms_package_session_v1 s;bx_ntvdm_cpu_result_v2 r;static const uint8_t deferred[]={6u,7u,8u,9u};uint32_t n;
  if(!bx_ntvdm_xms_package_session_v1_initialize(&s)||!dispatch(&s,0u,0u,0u,0u,&r)||r.disposition!=BX_NTVDM_CPU_RESULT_V2_RESUME||r.cpu_delta.gpr16_values[0]!=1u||a20_enabled!=0u)return 1;
  if(!dispatch(&s,0u,2u,0u,0u,&r)||r.cpu_delta.gpr16_values[0]!=0u||r.cpu_delta.gpr16_values[3]!=0u)return 2;
  if(!dispatch(&s,2u,0u,0u,64u,&r)||r.cpu_delta.gpr16_values[0]!=1088u)return 3;
  if(!dispatch(&s,3u,1088u,0u,64u,&r)||r.cpu_delta.gpr16_values[0]!=1u)return 4;
  if(!dispatch(&s,5u,0u,0u,0u,&r)||r.cpu_delta.gpr16_values[0]!=3072u)return 5;
  if(!dispatch(&s,4u,0u,0u,0u,&r)||r.cpu_delta.gpr16_values[0]==0u)return 6;
  if(!dispatch(&s,10u,0u,0u,0u,&r)||r.cpu_delta.gpr16_values[0]!=2048u||r.cpu_delta.gpr16_values[2]!=1024u)return 7;
  memcpy(move_record,(const uint8_t[]){2u,0u,0u,0u,0x00u,0x20u,0u,0u,0x00u,0x30u,0u,0u},12u);
  if(!dispatch(&s,1u,0u,0u,0u,&r)||r.cpu_delta.gpr16_values[0]!=1u||moved_source!=0x2000u||moved_destination!=0x3000u||moved_bytes!=4u)return 8;
  for(n=0;n<sizeof(deferred)/sizeof(deferred[0]);n++)if(!dispatch(&s,deferred[n],0u,0u,0u,&r)||r.disposition!=BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH)return 9;
  return 0; }
