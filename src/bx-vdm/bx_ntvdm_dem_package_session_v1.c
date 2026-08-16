#include "bx_ntvdm_dem_package_session_v1.h"
#include "bx_ntvdm_dem_boot_drive_service.h"
#include "bx_ntvdm_dem_cli_unavailable_provider_v1.h"
#include "bx_ntvdm_dem_readonly_namespace_failure_provider_v1.h"
#include "bx_ntvdm_dem_fcb_provider_v1.h"
#include "bx_ntvdm_dem_search_partition_v1.h"
#include "bx_ntvdm_dem_handle_route_partition_v1.h"
#include "bx_ntvdm_dem_fcb_io_route_partition_v1.h"
#include "bx_ntvdm_dem_namespace_route_partition_v1.h"
#include "bx_ntvdm_dem_fcb_path_route_partition_v1.h"
#include "bx_ntvdm_dem_fcb_handle_partition_v1.h"
#include "bx_ntvdm_dem_dpb_service.h"
#include "bx_ntvdm_dem_full_dpb_service_v1.h"
#include "bx_ntvdm_dem_computer_name_service_v1.h"
#include "bx_ntvdm_dem_ioctl_metadata_provider_v1.h"
#include "bx_ntvdm_dem_raw_media_provider_v1.h"
#include "bx_ntvdm_dem_media_id_service_v1.h"
#include "bx_ntvdm_dem_misc_plane_v1.h"
#include "bx_ntvdm_dem_session_lifecycle_provider_v1.h"
#include "bx_ntvdm_vdd_create_user_notify_service.h"
#include "bx_ntvdm_dem_process_owner_v1.h"
#include "bx-mantle/bx_ntvdm_mechanical_action_v1.h"
#include <string.h>

static int action(struct bx_ntvdm_mechanical_action_v1 *a)
{ return a->kind==BX_NTVDM_MECHANICAL_ACTION_V1_NONE||
  (a->kind==BX_NTVDM_MECHANICAL_ACTION_V1_WRITE&&!a->range_count&&!a->payload_bytes)||
  bx_ntvdm_mantle_execute_mechanical_action_v1(a); }
static int memory_result(const bx_ntvdm_exception_result_v1 *m,bx_ntvdm_cpu_result_v2 *r)
{ if(!m||!r||m->disposition!=BX_NTVDM_EXCEPTION_RESULT_RESUME||!bx_ntvdm_cpu_result_v2_resume(r,m->resume_rip))return 0;r->cpu_delta=m->cpu_delta;return 1; }
static int terminal_or_complete(const bx_ntvdm_bop_ingress_v1 *i,
 const bx_ntvdm_bop_provider_selection_v1 *p,const bx_ntvdm_dem_package_route_v1 *route,
 const bx_ntvdm_exception_event_v1 *e,const bx_ntvdm_cpu_state_v1 *c,
 const bx_ntvdm_instruction_window_v1 *w,bx_ntvdm_cpu_result_v2 *r)
{ return bx_ntvdm_cpu_result_v2_valid(r)&&r->disposition!=BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH ? 1 :
  bx_ntvdm_dem_cli_unavailable_provider_v1_dispatch(i,p,route,e,c,w,r); }
static int write_tx(bx_ntvdm_dem_package_session_v1 *s,const bx_ntvdm_multi_write_transaction_v1 *t,const uint8_t *payload)
{ struct bx_ntvdm_mechanical_action_v1 a;uint32_t i,id;
  if(!s||!t||!payload||!bx_ntvdm_multi_write_transaction_v1_preflight(t,UINT64_C(0x100000),t->writes.payload_bytes))return 0;
  if(!t->writes.write_count)return 1;
  if(t->writes.write_count>BX_NTVDM_MECHANICAL_ACTION_V1_MAX_RANGES||t->writes.payload_bytes>BX_NTVDM_MECHANICAL_ACTION_V1_MAX_BYTES||!s->namespace_plane->next_action_id)return 0;
  id=s->namespace_plane->next_action_id++;if(!s->namespace_plane->next_action_id)s->namespace_plane->next_action_id=1u;
  bx_ntvdm_mechanical_action_v1_clear(&a);a.action_id=id;a.kind=BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;a.range_count=t->writes.write_count;a.payload_bytes=(uint32_t)t->writes.payload_bytes;
  for(i=0;i<a.range_count;i++){a.ranges[i].physical_address=t->writes.writes[i].guest_physical_address;a.ranges[i].byte_count=(uint32_t)t->writes.writes[i].byte_count;a.ranges[i].payload_offset=(uint32_t)t->writes.writes[i].payload_offset;}
  memcpy(a.payload,payload,a.payload_bytes);return bx_ntvdm_mechanical_action_v1_valid(&a)&&bx_ntvdm_mantle_execute_mechanical_action_v1(&a); }
static int gather_read(bx_ntvdm_dem_package_session_v1 *s,
 const bx_ntvdm_guest_gather_read_action_v1 *g,struct bx_ntvdm_mechanical_action_v1 *a)
{ uint32_t i,id,offset=0u;
  if(!s||!g||!a||g->disposition!=BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_NEED_READ||
    !bx_ntvdm_guest_gather_read_action_v1_valid(g)||g->range_count>BX_NTVDM_MECHANICAL_ACTION_V1_MAX_RANGES||
    g->total_bytes>BX_NTVDM_MECHANICAL_ACTION_V1_MAX_BYTES||!s->namespace_plane->next_action_id)return 0;
  id=s->namespace_plane->next_action_id++;if(!s->namespace_plane->next_action_id)s->namespace_plane->next_action_id=1u;
  bx_ntvdm_mechanical_action_v1_clear(a);a->action_id=id;a->kind=BX_NTVDM_MECHANICAL_ACTION_V1_READ;a->range_count=g->range_count;a->payload_bytes=g->total_bytes;
  for(i=0;i<g->range_count;i++){if(g->ranges[i].length>UINT32_MAX||offset>a->payload_bytes-(uint32_t)g->ranges[i].length)return 0;
    a->ranges[i].physical_address=g->ranges[i].address;a->ranges[i].byte_count=(uint32_t)g->ranges[i].length;a->ranges[i].payload_offset=offset;offset+=a->ranges[i].byte_count;}
  return offset==a->payload_bytes&&bx_ntvdm_mechanical_action_v1_valid(a); }
static int direct_namespace_owner(bx_ntvdm_dem_package_session_v1 *s,
 bx_ntvdm_dem_process_owner_v1 *owner)
{ bx_ntvdm_guest_read_action_v1 read;struct bx_ntvdm_mechanical_action_v1 a;uint32_t id;
  if(!s||!owner||!s->namespace_plane||!s->namespace_plane->has_dta||
    !bx_ntvdm_dem_process_owner_v1_prepare(&s->namespace_plane->dta,owner,&read)||
    owner->status!=BX_NTVDM_DEM_PROCESS_OWNER_V1_READ_REQUIRED||
    read.disposition!=BX_NTVDM_GUEST_READ_ACTION_V1_NEED_READ||
    read.guest_read.length!=2u||!s->namespace_plane->next_action_id)return 0;
  id=s->namespace_plane->next_action_id++;if(!s->namespace_plane->next_action_id)s->namespace_plane->next_action_id=1u;
  bx_ntvdm_mechanical_action_v1_clear(&a);a.action_id=id;a.kind=BX_NTVDM_MECHANICAL_ACTION_V1_READ;
  a.range_count=1u;a.payload_bytes=2u;a.ranges[0].physical_address=read.guest_read.address;
  a.ranges[0].byte_count=2u;
  return bx_ntvdm_mechanical_action_v1_valid(&a)&&action(&a)&&
    bx_ntvdm_dem_process_owner_v1_complete(&s->namespace_plane->dta,&read,
      a.payload,a.payload_bytes,owner)&&owner->status==BX_NTVDM_DEM_PROCESS_OWNER_V1_VALID; }
static int search_dispatch(bx_ntvdm_dem_package_session_v1 *s,uint8_t service,
 const bx_ntvdm_exception_event_v1 *e,const bx_ntvdm_cpu_state_v1 *c,
 const bx_ntvdm_instruction_window_v1 *w,bx_ntvdm_cpu_result_v2 *r)
{ bx_ntvdm_guest_gather_read_action_v1 gather;struct bx_ntvdm_mechanical_action_v1 read;
  bx_ntvdm_multi_write_transaction_v1 tx;uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD];uint32_t used=0u;
  if(!s||!e||!c||!w||!r||!s->has_whole_provider||!s->namespace_plane->has_dta||
    !bx_ntvdm_dem_search_partition_v1_prepare(&s->whole_provider,&s->namespace_plane->dta,
      service,e,c,w,&gather))return 0;
  if(!gather_read(s,&gather,&read)||!action(&read)){bx_ntvdm_dem_whole_provider_v1_cancel_gather(
      &s->whole_provider,service,e,c,&gather);return 0;}
  if(!bx_ntvdm_dem_search_partition_v1_complete(&s->whole_provider,service,e,c,&gather,
      read.payload,read.payload_bytes,&tx,payload,&used)||!write_tx(s,&tx,payload))return 0;
  *r=tx.result;return bx_ntvdm_cpu_result_v2_valid(r); }
static int handle_dispatch(bx_ntvdm_dem_package_session_v1 *s,uint8_t service,
 const bx_ntvdm_exception_event_v1 *e,const bx_ntvdm_cpu_state_v1 *c,
 const bx_ntvdm_instruction_window_v1 *w,bx_ntvdm_cpu_result_v2 *r)
{ struct bx_ntvdm_mechanical_action_v1 a;
  if(!s||!e||!c||!w||!r||!s->has_whole_provider||
    !bx_ntvdm_dem_handle_route_partition_v1_dispatch(&s->whole_provider,service,e,c,w,&a,r))return 0;
  if(!action(&a)){if(a.kind==BX_NTVDM_MECHANICAL_ACTION_V1_READ)
      bx_ntvdm_dem_whole_provider_v1_cancel_gather(&s->whole_provider,service,e,c,
        &s->whole_provider.pending_gather);return 0;}
  if(a.kind==BX_NTVDM_MECHANICAL_ACTION_V1_READ&&!bx_ntvdm_dem_handle_route_partition_v1_complete_read(
      &s->whole_provider,service,e,c,&a,r))return 0;
  return bx_ntvdm_cpu_result_v2_valid(r); }
static int fcb_io_dispatch(bx_ntvdm_dem_package_session_v1 *s,uint8_t service,
 const bx_ntvdm_exception_event_v1 *e,const bx_ntvdm_cpu_state_v1 *c,
 const bx_ntvdm_instruction_window_v1 *w,bx_ntvdm_cpu_result_v2 *r)
{ struct bx_ntvdm_mechanical_action_v1 a;
  if(!s||!e||!c||!w||!r||!s->has_whole_provider||!s->namespace_plane->has_dta||
    !bx_ntvdm_dem_fcb_io_route_partition_v1_dispatch(&s->whole_provider,service,e,c,w,
      s->namespace_plane->dta.dta_location,&a,r))return 0;
  if(!action(&a)){if(a.kind==BX_NTVDM_MECHANICAL_ACTION_V1_READ)
      bx_ntvdm_dem_whole_provider_v1_cancel_gather(&s->whole_provider,service,e,c,
        &s->whole_provider.pending_gather);return 0;}
  if(a.kind==BX_NTVDM_MECHANICAL_ACTION_V1_READ&&
    !bx_ntvdm_dem_fcb_io_route_partition_v1_complete_write(&s->whole_provider,
      service,e,c,&a,r))return 0;
  return bx_ntvdm_cpu_result_v2_valid(r); }
static int namespace_dispatch(bx_ntvdm_dem_package_session_v1 *s,uint8_t service,
 const bx_ntvdm_exception_event_v1 *e,const bx_ntvdm_cpu_state_v1 *c,
 const bx_ntvdm_instruction_window_v1 *w,bx_ntvdm_cpu_result_v2 *r)
{ struct bx_ntvdm_mechanical_action_v1 a;bx_ntvdm_dem_process_owner_v1 owner;int needs_owner,completed;
  bx_ntvdm_dem_process_owner_v1_clear(&owner);
  if(!s||!e||!c||!w||!r||!s->has_whole_provider||
    !bx_ntvdm_dem_namespace_route_partition_v1_prepare(&s->whole_provider,service,e,c,w,&a))return 0;
  if(!action(&a)){bx_ntvdm_dem_whole_provider_v1_cancel_gather(&s->whole_provider,service,e,c,
      &s->whole_provider.pending_gather);return 0;}
  needs_owner=service==0x03u||service==0x12u||service==0x22u;
  if(needs_owner&&!direct_namespace_owner(s,&owner))return 0;
  if(needs_owner)s->whole_provider.direct_namespace_owner=owner.pdb_segment;
  completed=bx_ntvdm_dem_namespace_route_partition_v1_complete(&s->whole_provider,service,e,c,&a,r)&&
    bx_ntvdm_cpu_result_v2_valid(r);
  s->whole_provider.direct_namespace_owner=0u;
  return completed; }
static int fcb_path_dispatch(bx_ntvdm_dem_package_session_v1 *s,uint8_t service,
 const bx_ntvdm_exception_event_v1 *e,const bx_ntvdm_cpu_state_v1 *c,
 const bx_ntvdm_instruction_window_v1 *w,bx_ntvdm_cpu_result_v2 *r)
{ struct bx_ntvdm_mechanical_action_v1 a;
  if(!s||!e||!c||!w||!r||!s->has_whole_provider||
    !bx_ntvdm_dem_fcb_path_route_partition_v1_prepare(&s->whole_provider,service,e,c,w,&a))return 0;
  if(!action(&a)){bx_ntvdm_dem_whole_provider_v1_cancel_gather(&s->whole_provider,service,e,c,
      &s->whole_provider.pending_gather);return 0;}
  return bx_ntvdm_dem_fcb_path_route_partition_v1_complete(&s->whole_provider,service,e,c,&a,r)&&
    bx_ntvdm_cpu_result_v2_valid(r); }
static int fcb_direct_dispatch(bx_ntvdm_dem_package_session_v1 *s,uint8_t service,
 const bx_ntvdm_exception_event_v1 *e,const bx_ntvdm_cpu_state_v1 *c,
 bx_ntvdm_cpu_result_v2 *r)
{ uint32_t unused=0u;
  return s&&e&&c&&r&&s->has_whole_provider&&
    (service==0x2eu||service==0x30u)&&
    bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(&s->whole_provider,service,e,c,0,0,0u,
      &unused,r)&&bx_ntvdm_cpu_result_v2_valid(r); }
/* `demPipeFileDataEOF` and `demPipeFileEOF` are Redirector/COMMAND pipe
 * services, not a readonly namespace capability.  A CLI session which has
 * not admitted that complete provider must expose the original absent-SFT
 * outcome at the package boundary: invalid handle, carry set.  Keep this
 * disposition explicit here so T202's local DEM switch cannot accidentally
 * inherit a detached readonly fallback leaf. */
static int redirector_deferred(const bx_ntvdm_bop_ingress_v1 *i,
 const bx_ntvdm_bop_provider_selection_v1 *p,const bx_ntvdm_exception_event_v1 *e,
 const bx_ntvdm_cpu_state_v1 *c,const bx_ntvdm_instruction_window_v1 *w,
 bx_ntvdm_cpu_result_v2 *r)
{ return i&&p&&e&&c&&w&&r&&i->family==BX_NTVDM_BOP_FAMILY_DEM&&
  (i->service==0x47u||i->service==0x48u)&&
  p->provider_family==BX_NTVDM_BOP_PROVIDER_DEM&&
  p->disposition==BX_NTVDM_BOP_PROVIDER_DEFERRED&&e->vector==6u&&
  c->execution_mode==BX_NTVDM_CPU_EXECUTION_REAL&&w->valid_bytes>=4u&&
  w->bytes[0]==0xc4u&&w->bytes[1]==0xc4u&&w->bytes[2]==0x50u&&
  w->bytes[3]==i->service&&e->fault_rip<=UINT64_MAX-4u&&
  bx_ntvdm_cpu_result_v2_resume(r,e->fault_rip+4u)&&
  bx_ntvdm_cpu_delta_v1_set_gpr16(&r->cpu_delta,0u,6u)&&
  bx_ntvdm_cpu_result_v2_set_cf(r,1); }
int bx_ntvdm_dem_package_session_v1_valid(const bx_ntvdm_dem_package_session_v1 *s)
{ return s&&s->magic==BX_NTVDM_DEM_PACKAGE_SESSION_V1_MAGIC&&s->abi_version==BX_NTVDM_DEM_PACKAGE_SESSION_V1_VERSION&&s->struct_bytes==sizeof(*s)&&s->initialized==1u&&s->namespace_plane&&s->has_whole_provider<=1u&&bx_ntvdm_dem_drive_view_provider_v1_valid(&s->drive_view)&&(!s->has_whole_provider||bx_ntvdm_dem_whole_provider_v1_valid(&s->whole_provider)); }
int bx_ntvdm_dem_package_session_v1_initialize(bx_ntvdm_dem_package_session_v1 *s,bx_ntvdm_boot_namespace_plane_v1 *p)
{ if(!s||!p)return 0;memset(s,0,sizeof(*s));s->magic=BX_NTVDM_DEM_PACKAGE_SESSION_V1_MAGIC;s->abi_version=BX_NTVDM_DEM_PACKAGE_SESSION_V1_VERSION;s->struct_bytes=(uint32_t)sizeof(*s);s->namespace_plane=p;bx_ntvdm_dem_error_lock_plane_v1_clear(&s->error_lock);bx_ntvdm_dem_drive_view_provider_v1_initialize(&s->drive_view);s->initialized=1u;return bx_ntvdm_dem_package_session_v1_valid(s); }
void bx_ntvdm_dem_package_session_v1_teardown(bx_ntvdm_dem_package_session_v1 *s)
{ if(!s)return;if(s->has_whole_provider)bx_ntvdm_dem_whole_provider_v1_teardown(&s->whole_provider);memset(s,0,sizeof(*s)); }
int bx_ntvdm_dem_package_session_v1_set_drive_snapshot(bx_ntvdm_dem_package_session_v1 *s,const bx_ntvdm_host_drive_snapshot_v1 *v)
{ return bx_ntvdm_dem_package_session_v1_valid(s)&&bx_ntvdm_dem_drive_view_provider_v1_set_drive_snapshot(&s->drive_view,v)&&bx_ntvdm_boot_namespace_plane_v1_set_drive_snapshot(s->namespace_plane,v); }
int bx_ntvdm_dem_package_session_v1_set_volume_snapshot(bx_ntvdm_dem_package_session_v1 *s,const bx_ntvdm_host_volume_snapshot_v1 *v)
{ return bx_ntvdm_dem_package_session_v1_valid(s)&&bx_ntvdm_dem_drive_view_provider_v1_set_volume_snapshot(&s->drive_view,v); }
int bx_ntvdm_dem_package_session_v1_set_mutation_profile(bx_ntvdm_dem_package_session_v1 *s,const bx_ntvdm_mutation_profile_v1 *p)
{ if(!bx_ntvdm_dem_package_session_v1_valid(s)||!p||s->drive_view.has_mutation_profile)return 0;if(!bx_ntvdm_dem_drive_view_provider_v1_set_mutation_profile(&s->drive_view,p)||!bx_ntvdm_boot_namespace_plane_v1_set_dem_cwd_context(s->namespace_plane,&s->drive_view.cwd,0))return 0;s->drive_view.has_mutation_profile=1u;return bx_ntvdm_dem_package_session_v1_valid(s); }
int bx_ntvdm_dem_package_session_v1_set_host_namespace(
    bx_ntvdm_dem_package_session_v1 *s,
    const bx_ntvdm_host_namespace_v1 *host_namespace)
{ if(!bx_ntvdm_dem_package_session_v1_valid(s)||!s->drive_view.has_mutation_profile||
    s->has_whole_provider||!host_namespace||!bx_ntvdm_host_namespace_v1_valid(host_namespace)||
    !bx_ntvdm_dem_whole_provider_v1_initialize(&s->whole_provider,
      &s->drive_view.mutation_profile.profile,host_namespace,&s->drive_view.cwd))return 0;
  if(!bx_ntvdm_boot_namespace_plane_v1_set_dem_cwd_context(s->namespace_plane,
      &s->drive_view.cwd,host_namespace)||!bx_ntvdm_dem_whole_provider_v1_set_startup_namespace(
      &s->whole_provider,&s->namespace_plane->provider.readonly_namespace)||
      !bx_ntvdm_dem_whole_provider_v1_set_declared_search_snapshot(
      &s->whole_provider,&s->namespace_plane->provider.search_snapshot)){
    bx_ntvdm_dem_whole_provider_v1_teardown(&s->whole_provider);return 0;}
  s->has_whole_provider=1u;return bx_ntvdm_dem_package_session_v1_valid(s); }
int bx_ntvdm_dem_package_session_v1_set_boot_drive(
    bx_ntvdm_dem_package_session_v1 *s, uint32_t drive)
{ return bx_ntvdm_dem_package_session_v1_valid(s) &&
    bx_ntvdm_dem_drive_view_provider_v1_set_boot_drive(&s->drive_view, drive); }int bx_ntvdm_dem_package_session_v1_resolve_mutation_class(const bx_ntvdm_dem_package_session_v1 *s,uint32_t c,uint32_t *r)
{ return bx_ntvdm_dem_package_session_v1_valid(s)&&s->drive_view.has_mutation_profile&&bx_ntvdm_dem_profile_consumer_v1_resolve(&s->drive_view.mutation_profile,c,r); }
int bx_ntvdm_dem_package_session_v1_dispatch(bx_ntvdm_dem_package_session_v1 *s,const bx_ntvdm_bop_ingress_v1 *i,const bx_ntvdm_bop_provider_selection_v1 *p,const bx_ntvdm_exception_event_v1 *e,const bx_ntvdm_cpu_state_v1 *c,const bx_ntvdm_instruction_window_v1 *w,bx_ntvdm_cpu_result_v2 *r)
{ bx_ntvdm_dem_package_route_v1 route;bx_ntvdm_dem_plane_record_v1 plane;bx_ntvdm_exception_result_v1 mem;struct bx_ntvdm_mechanical_action_v1 a,next;bx_ntvdm_multi_write_transaction_v1 tx;uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD];uint32_t drive_view_write;
  if(!bx_ntvdm_dem_package_session_v1_valid(s)||!i||!p||!e||!c||!w||!r||!bx_ntvdm_dem_package_facade_v1_classify(i,p,&route))return 0;
  bx_ntvdm_cpu_result_v2_pass_through(r);
  if(bx_ntvdm_dem_package_facade_v1_dispatch(i,p,&route,e,c,r))return terminal_or_complete(i,p,&route,e,c,w,r);plane=route.plane;
  if(bx_ntvdm_vdd_create_user_notify_service_v1_dispatch(e,c,w,r))return terminal_or_complete(i,p,&route,e,c,w,r);
  if(bx_ntvdm_dem_misc_plane_v1_dispatch(i,p,e,c,w,&mem))return memory_result(&mem,r);
  if(bx_ntvdm_dem_session_lifecycle_provider_v1_dispatch(&s->namespace_plane->provider,i,p,&plane,e,c,r))return terminal_or_complete(i,p,&route,e,c,w,r);
  if(bx_ntvdm_dem_error_lock_plane_v1_dispatch(&s->error_lock,i,p,e,c,w,r))return terminal_or_complete(i,p,&route,e,c,w,r);
  if(bx_ntvdm_dem_drive_view_provider_v1_dispatch_observation(&s->drive_view,
      i,p,e,c,w,&tx,payload,&drive_view_write,r)){
    if(drive_view_write!=0u){if(!write_tx(s,&tx,payload))return 0;*r=tx.result;}
    return bx_ntvdm_cpu_result_v2_valid(r);
  }
  if(s->drive_view.gset.has_drive_snapshot&&bx_ntvdm_dem_ioctl_metadata_provider_v1_dispatch(i,p,&plane,&s->drive_view.gset.drive_snapshot,e,c,r))return terminal_or_complete(i,p,&route,e,c,w,r);
  if(bx_ntvdm_dem_raw_media_provider_v1_dispatch(i,p,&route,e,c,w,r))return 1;
  if(s->drive_view.gset.has_drive_snapshot&&bx_ntvdm_dem_boot_drive_service_v2_dispatch(&s->drive_view.gset.drive_snapshot,s->drive_view.has_boot_drive?s->drive_view.boot_drive_index:UINT32_MAX,e,c,w,&mem))return memory_result(&mem,r);

  /* T202/S4's atomic local-file package switch.  Once this provider is
   * installed, no identity in its 27-service work set falls through to a
   * boot/readonly fixture leaf.  Token validation happens inside the new
   * owner, so legacy tokens become source-shaped invalid handles. */
  if(s->has_whole_provider&&bx_ntvdm_dem_handle_route_partition_v1_owns_service(i->service))
    return handle_dispatch(s,i->service,e,c,w,r) ? 1 :
      bx_ntvdm_dem_cli_unavailable_provider_v1_dispatch(i,p,&route,e,c,w,r);
  if(s->has_whole_provider&&bx_ntvdm_dem_namespace_route_partition_v1_owns_service(i->service))
    return namespace_dispatch(s,i->service,e,c,w,r) ? 1 :
      bx_ntvdm_dem_cli_unavailable_provider_v1_dispatch(i,p,&route,e,c,w,r);
  if(s->has_whole_provider&&bx_ntvdm_dem_fcb_path_route_partition_v1_owns_service(i->service))
    return fcb_path_dispatch(s,i->service,e,c,w,r) ? 1 :
      bx_ntvdm_dem_cli_unavailable_provider_v1_dispatch(i,p,&route,e,c,w,r);
  if(s->has_whole_provider&&(i->service==0x2eu||i->service==0x30u))
    return fcb_direct_dispatch(s,i->service,e,c,r) ? 1 :
      bx_ntvdm_dem_cli_unavailable_provider_v1_dispatch(i,p,&route,e,c,w,r);
  if(s->has_whole_provider&&s->namespace_plane->has_dta&&i->service==0x2fu)
    return fcb_io_dispatch(s,i->service,e,c,w,r) ? 1 :
      bx_ntvdm_dem_cli_unavailable_provider_v1_dispatch(i,p,&route,e,c,w,r);
  /* Search is part of the same atomic provider. */
  if(bx_ntvdm_dem_search_partition_v1_owns_service(i->service)&&s->has_whole_provider)
    return search_dispatch(s,i->service,e,c,w,r) ? 1 :
      bx_ntvdm_dem_cli_unavailable_provider_v1_dispatch(i,p,&route,e,c,w,r);
  if(redirector_deferred(i,p,e,c,w,r))return 1;
  if(bx_ntvdm_dem_readonly_namespace_failure_provider_v1_dispatch(i,p,&route,e,c,w,r))return 1;
  /* A selected DEM provider may reject a mode/precondition.  The package
   * owns that refusal: preserve a typed pass-through instead of leaking the
   * request to a second detached recognizer. */
  if(!bx_ntvdm_boot_namespace_plane_v1_dispatch(s->namespace_plane,i,p,e,c,w,&a,r)) {
    if(bx_ntvdm_dem_fcb_provider_v1_dispatch(i,p,&route,e,c,w,r))return 1;
    return bx_ntvdm_dem_cli_unavailable_provider_v1_dispatch(i,p,&route,e,c,w,r);
  }
  if(!action(&a))return 0;
  if(a.kind==BX_NTVDM_MECHANICAL_ACTION_V1_READ){
    /* A completed copied read can still expose a guest-side precondition
     * which the selected historical provider rejects (for example the DTA
     * registration's zero physical addresses).  That is a provider outcome,
     * not a broken mantle transport: contain it in DEM's source-derived
     * failure path.  A subsequent mechanical write failure remains a real
     * transport failure and must not be disguised. */
    if(!bx_ntvdm_boot_namespace_plane_v1_complete_read(s->namespace_plane,&a,&next,r))
      return bx_ntvdm_dem_cli_unavailable_provider_v1_dispatch(i,p,&route,e,c,w,r);
    if(!action(&next))return 0;
  }
  /* A legacy-shaped leaf may report that it handled an invalid precondition
   * without producing a typed result.  That is not a legal bx-vdm outcome:
   * convert it to the package's source-derived CLI failure rather than
   * allowing an uninitialised CPU result to reach the mechanical bridge. */
  return bx_ntvdm_cpu_result_v2_valid(r) ? 1 :
    bx_ntvdm_dem_cli_unavailable_provider_v1_dispatch(i,p,&route,e,c,w,r);
}
