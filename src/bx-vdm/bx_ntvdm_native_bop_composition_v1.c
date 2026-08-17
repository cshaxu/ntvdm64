#include "bx_ntvdm_native_bop_composition_v1.h"
#include <string.h>
#include "bx-mantle/bx_ntvdm_mechanical_action_v1.h"
#include "bx-mantle/bx_ntvdm_port_action_v1.h"
#include "opennt_int06_provider_v1.h"

static bx_ntvdm_native_bop_composition_v1 *active;

static int valid(const bx_ntvdm_native_bop_composition_v1 *c)
{ return c && c->magic==BX_NTVDM_NATIVE_BOP_COMPOSITION_V1_MAGIC &&
  c->abi_version==BX_NTVDM_NATIVE_BOP_COMPOSITION_V1_VERSION &&
  c->struct_bytes==sizeof(*c) && c->bound<=1u &&
  bx_ntvdm_xms_package_session_v1_valid(&c->xms) &&
  bx_ntvdm_dpmi_package_session_v1_valid(&c->dpmi); }

int bx_ntvdm_native_bop_composition_v1_initialize(
  bx_ntvdm_native_bop_composition_v1 *c)
{ if(!c)return 0;memset(c,0,sizeof(*c));c->magic=BX_NTVDM_NATIVE_BOP_COMPOSITION_V1_MAGIC;
  c->abi_version=BX_NTVDM_NATIVE_BOP_COMPOSITION_V1_VERSION;c->struct_bytes=(uint32_t)sizeof(*c);
  return bx_ntvdm_xms_package_session_v1_initialize(&c->xms) &&
    bx_ntvdm_dpmi_package_session_v1_initialize(&c->dpmi) && valid(c); }

int bx_ntvdm_native_bop_composition_v1_bind(
  bx_ntvdm_native_bop_composition_v1 *c)
{ if(!valid(c)||active||c->bound)return 0;active=c;c->bound=1u;return 1; }

void bx_ntvdm_native_bop_composition_v1_unbind(
  bx_ntvdm_native_bop_composition_v1 *c)
{ if(c&&active==c)active=0;if(c&&valid(c))c->bound=0u; }

static int unpack(const struct bx_ntvdm_generic_ud_event_v1 *e,
  bx_ntvdm_exception_event_v1 *b,bx_ntvdm_cpu_state_v1 *c,
  bx_ntvdm_instruction_window_v1 *w)
{ if(!e||!b||!c||!w||e->magic!=BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC||
  e->abi_version!=BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION||e->struct_bytes!=sizeof(*e)||
  e->vector!=6u||e->window_bytes>BX_NTVDM_GENERIC_UD_WINDOW_BYTES)return 0;
  memset(b,0,sizeof(*b));b->magic=BX_NTVDM_EXCEPTION_ABI_MAGIC;b->abi_version=BX_NTVDM_EXCEPTION_ABI_VERSION;
  b->struct_bytes=sizeof(*b);b->kind=BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;b->cpu_id=e->cpu_id;b->vector=e->vector;b->error_code=e->error_code;b->fault_rip=e->fault_rip;
  bx_ntvdm_cpu_state_v1_initialize(c,e->execution_mode);c->eax=e->eax;c->ebx=e->ebx;c->ecx=e->ecx;c->edx=e->edx;c->esi=e->esi;c->edi=e->edi;c->ebp=e->ebp;c->esp=e->esp;c->eip=e->eip;c->eflags=e->eflags;c->cs=e->cs;c->ds=e->ds;c->es=e->es;c->ss=e->ss;c->fs=e->fs;c->gs=e->gs;
  bx_ntvdm_instruction_window_v1_capture(w,e->window,e->window_bytes);
  return bx_ntvdm_exception_event_v1_valid(b)&&bx_ntvdm_cpu_state_v1_valid(c)&&bx_ntvdm_instruction_window_v1_valid(w); }

static int int06(const struct bx_ntvdm_generic_ud_event_v1 *e, bx_ntvdm_cpu_result_v2 *r)
{ struct opennt_int06_provider_v1_state s; struct opennt_int06_provider_v1_operation o; struct bx_ntvdm_mechanical_action_v1 m; struct bx_ntvdm_port_action_v1 p; uint32_t n=0; uint8_t v=0;
  if(e->execution_mode!=BX_NTVDM_CPU_EXECUTION_REAL||e->window_bytes<3u||e->window[0]!=0xc4u||e->window[1]!=0xc4u||e->window[2]!=0x06u||e->fault_rip>UINT64_MAX-3u)return 0;
  bx_ntvdm_mechanical_action_v1_clear(&m);m.action_id=0x6001u;m.kind=BX_NTVDM_MECHANICAL_ACTION_V1_PREFLIGHT_WRITE;m.range_count=2u;m.payload_bytes=5u;m.ranges[0].physical_address=((uint64_t)e->ss<<4)+e->esp;m.ranges[0].byte_count=4u;m.ranges[1].physical_address=OPENNT_INT06_BDA_INTERRUPT_CAUSE;m.ranges[1].payload_offset=4u;m.ranges[1].byte_count=1u;if(!bx_ntvdm_mechanical_action_v1_valid(&m)||!bx_ntvdm_mantle_execute_mechanical_action_v1(&m))return bx_ntvdm_cpu_result_v2_stop(r);
  opennt_int06_provider_v1_initialize(&s);if(!opennt_int06_provider_v1_begin(&s,e->ss,e->esp))return bx_ntvdm_cpu_result_v2_stop(r);
  while(opennt_int06_provider_v1_next(&s,&o)){if(++n>24u)return bx_ntvdm_cpu_result_v2_stop(r);if(o.kind==OPENNT_INT06_PROVIDER_V1_OPERATION_RAM_READ8||o.kind==OPENNT_INT06_PROVIDER_V1_OPERATION_RAM_WRITE8){bx_ntvdm_mechanical_action_v1_clear(&m);m.action_id=0x6100u+n;m.kind=o.kind==OPENNT_INT06_PROVIDER_V1_OPERATION_RAM_READ8?BX_NTVDM_MECHANICAL_ACTION_V1_READ:BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;m.range_count=1;m.payload_bytes=1;m.ranges[0].physical_address=o.address;m.ranges[0].byte_count=1;m.payload[0]=o.value;if(!bx_ntvdm_mantle_execute_mechanical_action_v1(&m))return bx_ntvdm_cpu_result_v2_stop(r);v=m.payload[0];}else{bx_ntvdm_port_action_v1_initialize(&p);p.kind=o.kind==OPENNT_INT06_PROVIDER_V1_OPERATION_PORT_READ8?BX_NTVDM_PORT_ACTION_V1_READ8:BX_NTVDM_PORT_ACTION_V1_WRITE8;p.port=(uint16_t)o.address;p.value=o.value;if(!bx_ntvdm_mantle_execute_port_action_v1(&p))return bx_ntvdm_cpu_result_v2_stop(r);v=p.value;}if(!opennt_int06_provider_v1_accept(&s,&o,v))return bx_ntvdm_cpu_result_v2_stop(r);}return opennt_int06_provider_v1_complete(&s)?bx_ntvdm_cpu_result_v2_resume(r,e->fault_rip+3u):bx_ntvdm_cpu_result_v2_stop(r); }
static int publish(const bx_ntvdm_cpu_result_v2 *r,struct bx_ntvdm_generic_ud_outcome_v1 *o)
{ if(!r||!o||!bx_ntvdm_cpu_result_v2_valid(r)||r->disposition==BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH)return 0;
  memset(o,0,sizeof(*o));o->abi_version=BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;o->disposition=r->disposition;o->resume_rip=r->resume_rip;o->gpr16_write_mask=r->cpu_delta.gpr16_write_mask;memcpy(o->gpr16_values,r->cpu_delta.gpr16_values,sizeof(o->gpr16_values));o->eflags_write_mask=r->eflags_write_mask;o->eflags_values=r->eflags_values;return 1; }

int bx_ntvdm_native_bop_composition_v1_handle(
  const struct bx_ntvdm_generic_ud_event_v1 *e,struct bx_ntvdm_generic_ud_outcome_v1 *o)
{ bx_ntvdm_exception_event_v1 b;bx_ntvdm_cpu_state_v1 c;bx_ntvdm_instruction_window_v1 w;bx_ntvdm_bop_ingress_v1 i;bx_ntvdm_bop_provider_selection_v1 p;bx_ntvdm_cpu_result_v2 r;
  if(!valid(active)||!active->bound||!o||!unpack(e,&b,&c,&w))return 0; if(int06(e,&r))return publish(&r,o); if(!bx_ntvdm_bop_ingress_v1_dispatch(&b,&c,&w,&i,&r)||r.disposition!=BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH||!bx_ntvdm_bop_provider_registry_v1_select(&i,&p))return 0;
  if(bx_ntvdm_xms_package_session_v1_dispatch(&active->xms,&i,&p,&b,&c,&w,&r))return publish(&r,o);
  if(bx_ntvdm_dpmi_package_session_v1_dispatch(&active->dpmi,&i,&p,&b,&c,&w,&r))return publish(&r,o);
  return 0; }
