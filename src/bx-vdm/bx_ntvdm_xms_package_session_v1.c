#include "bx_ntvdm_xms_package_session_v1.h"
#include "bx-mantle/bx_ntvdm_a20_capability_v1.h"
#include "bx-mantle/bx_ntvdm_extended_memory_v1.h"
#include <string.h>

#define BX_NTVDM_XMS_GPR_AX 0u
#define BX_NTVDM_XMS_GPR_BX 3u
#define BX_NTVDM_XMS_GPR_CX 1u
#define BX_NTVDM_XMS_GPR_DX 2u

static int set16(bx_ntvdm_cpu_result_v2 *r,uint32_t n,uint16_t v)
{ return bx_ntvdm_cpu_delta_v1_set_gpr16(&r->cpu_delta,n,v); }
static int resume(const bx_ntvdm_exception_event_v1 *e,bx_ntvdm_cpu_result_v2 *r)
{ return e->fault_rip<=UINT64_MAX-4u&&bx_ntvdm_cpu_result_v2_resume(r,e->fault_rip+4u); }
static bx_ntvdm_xms_package_allocation_v1 *find(bx_ntvdm_xms_package_session_v1 *s,uint16_t base,uint16_t kib)
{ uint32_t n;for(n=0;n<BX_NTVDM_XMS_PACKAGE_SESSION_V1_SLOTS;n++)if(s->allocations[n].mantle_handle&&s->allocations[n].base_kib==base&&s->allocations[n].kib==kib)return &s->allocations[n];return 0; }
static bx_ntvdm_xms_package_allocation_v1 *free_slot(bx_ntvdm_xms_package_session_v1 *s)
{ uint32_t n;for(n=0;n<BX_NTVDM_XMS_PACKAGE_SESSION_V1_SLOTS;n++)if(!s->allocations[n].mantle_handle)return &s->allocations[n];return 0; }
static int reserve_xms_base(bx_ntvdm_xms_package_session_v1 *s)
{ struct bx_ntvdm_extended_memory_request_v1 q;struct bx_ntvdm_extended_memory_result_v1 v;
  if(s->reserved_mantle_handle)return 1;memset(&q,0,sizeof(q));q.version=BX_NTVDM_EXTENDED_MEMORY_V1_VERSION;q.operation=BX_NTVDM_EXTMEM_ALLOCATE;q.kib=64u;bx_ntvdm_extended_memory_v1_dispatch(&q,&v);if(v.status!=BX_NTVDM_EXTMEM_OK)return 0;s->reserved_mantle_handle=v.handle;return 1; }
int bx_ntvdm_xms_package_session_v1_valid(const bx_ntvdm_xms_package_session_v1 *s)
{ uint32_t n;if(!s||s->magic!=BX_NTVDM_XMS_PACKAGE_SESSION_V1_MAGIC||s->abi_version!=BX_NTVDM_XMS_PACKAGE_SESSION_V1_VERSION||s->struct_bytes!=sizeof(*s)||s->initialized!=1u)return 0;for(n=0;n<BX_NTVDM_XMS_PACKAGE_SESSION_V1_SLOTS;n++)if((s->allocations[n].mantle_handle==0u)!=(s->allocations[n].kib==0u))return 0;return 1; }
int bx_ntvdm_xms_package_session_v1_initialize(bx_ntvdm_xms_package_session_v1 *s)
{ if(!s)return 0;memset(s,0,sizeof(*s));s->magic=BX_NTVDM_XMS_PACKAGE_SESSION_V1_MAGIC;s->abi_version=BX_NTVDM_XMS_PACKAGE_SESSION_V1_VERSION;s->struct_bytes=(uint32_t)sizeof(*s);s->initialized=1u;return bx_ntvdm_xms_package_session_v1_valid(s); }
static int a20(const bx_ntvdm_cpu_state_v1 *c,const bx_ntvdm_exception_event_v1 *e,bx_ntvdm_cpu_result_v2 *r)
{ struct bx_ntvdm_a20_capability_request_v1 q;struct bx_ntvdm_a20_capability_result_v1 v;uint16_t reason=(uint16_t)c->eax;
  q.version=BX_NTVDM_A20_CAPABILITY_V1_VERSION;q.requested_enabled=0u;
  if(reason==0u){q.operation=BX_NTVDM_A20_CAPABILITY_SET;q.requested_enabled=0u;}else if(reason==1u){q.operation=BX_NTVDM_A20_CAPABILITY_SET;q.requested_enabled=1u;}else if(reason==2u)q.operation=BX_NTVDM_A20_CAPABILITY_QUERY;else return resume(e,r)&&set16(r,BX_NTVDM_XMS_GPR_AX,0u);
  bx_ntvdm_a20_capability_v1_dispatch(&q,&v);if(!resume(e,r))return 0;
  if(v.status!=BX_NTVDM_A20_CAPABILITY_OK)return set16(r,BX_NTVDM_XMS_GPR_AX,0u);
  if(reason==2u)return set16(r,BX_NTVDM_XMS_GPR_AX,(uint16_t)(v.enabled?1u:0u))&&set16(r,BX_NTVDM_XMS_GPR_BX,0u);
  return set16(r,BX_NTVDM_XMS_GPR_AX,1u); }
static int extmem(bx_ntvdm_xms_package_session_v1 *s,uint8_t service,const bx_ntvdm_cpu_state_v1 *c,const bx_ntvdm_exception_event_v1 *e,bx_ntvdm_cpu_result_v2 *r)
{ struct bx_ntvdm_extended_memory_request_v1 q;struct bx_ntvdm_extended_memory_result_v1 v;bx_ntvdm_xms_package_allocation_v1 *slot;uint16_t base=(uint16_t)c->eax,kib=(uint16_t)c->edx;
  memset(&q,0,sizeof(q));q.version=BX_NTVDM_EXTENDED_MEMORY_V1_VERSION;
  if(service==5u){q.operation=BX_NTVDM_EXTMEM_QUERY;bx_ntvdm_extended_memory_v1_dispatch(&q,&v);return resume(e,r)&&set16(r,BX_NTVDM_XMS_GPR_AX,v.status==BX_NTVDM_EXTMEM_OK?(uint16_t)v.kib:0u);}
  if(service==2u){q.operation=BX_NTVDM_EXTMEM_ALLOCATE;q.kib=kib;slot=free_slot(s);if(!slot||!kib||!reserve_xms_base(s)){if(!resume(e,r))return 0;return set16(r,BX_NTVDM_XMS_GPR_AX,0u);}bx_ntvdm_extended_memory_v1_dispatch(&q,&v);if(!resume(e,r))return 0;if(v.status!=BX_NTVDM_EXTMEM_OK||v.physical_address/1024u>0xffffu)return set16(r,BX_NTVDM_XMS_GPR_AX,0u);slot->mantle_handle=v.handle;slot->kib=kib;slot->base_kib=(uint16_t)(v.physical_address/1024u);return set16(r,BX_NTVDM_XMS_GPR_AX,slot->base_kib);}
  if(service==3u){slot=find(s,base,kib);if(!resume(e,r))return 0;if(!slot)return set16(r,BX_NTVDM_XMS_GPR_AX,0u);q.operation=BX_NTVDM_EXTMEM_FREE;q.handle=slot->mantle_handle;bx_ntvdm_extended_memory_v1_dispatch(&q,&v);if(v.status!=BX_NTVDM_EXTMEM_OK)return set16(r,BX_NTVDM_XMS_GPR_AX,0u);memset(slot,0,sizeof(*slot));return set16(r,BX_NTVDM_XMS_GPR_AX,1u);}
  if(service==11u){uint16_t new_kib=(uint16_t)c->ebx;slot=find(s,base,kib);if(!resume(e,r))return 0;if(!slot||!new_kib)return set16(r,BX_NTVDM_XMS_GPR_CX,0u);q.operation=BX_NTVDM_EXTMEM_REALLOCATE;q.handle=slot->mantle_handle;q.kib=new_kib;bx_ntvdm_extended_memory_v1_dispatch(&q,&v);if(v.status!=BX_NTVDM_EXTMEM_OK||v.physical_address/1024u>0xffffu)return set16(r,BX_NTVDM_XMS_GPR_CX,0u);slot->kib=new_kib;slot->base_kib=(uint16_t)(v.physical_address/1024u);return set16(r,BX_NTVDM_XMS_GPR_CX,slot->base_kib);}
  return 0; }
int bx_ntvdm_xms_package_session_v1_dispatch(bx_ntvdm_xms_package_session_v1 *s,const bx_ntvdm_bop_ingress_v1 *i,const bx_ntvdm_bop_provider_selection_v1 *p,const bx_ntvdm_exception_event_v1 *e,const bx_ntvdm_cpu_state_v1 *c,const bx_ntvdm_instruction_window_v1 *w,bx_ntvdm_cpu_result_v2 *r)
{ bx_ntvdm_xms_dpmi_plane_record_v1 plane;if(!bx_ntvdm_xms_package_session_v1_valid(s)||!i||!p||!e||!c||!w||!r||!bx_ntvdm_xms_dpmi_plane_v1_classify(i,p,&plane)||plane.provider_family!=BX_NTVDM_BOP_PROVIDER_XMS||c->execution_mode!=BX_NTVDM_CPU_EXECUTION_REAL||w->valid_bytes<4u||w->bytes[0]!=0xc4u||w->bytes[1]!=0xc4u||w->bytes[2]!=0x52u||w->bytes[3]!=plane.service)return 0;bx_ntvdm_cpu_result_v2_pass_through(r);if(plane.service==0u)return a20(c,e,r);if(plane.service==2u||plane.service==3u||plane.service==5u||plane.service==11u)return extmem(s,(uint8_t)plane.service,c,e,r);return 1; }
