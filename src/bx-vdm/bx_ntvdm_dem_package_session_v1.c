#include "bx_ntvdm_dem_package_session_v1.h"
#include "bx_ntvdm_dem_boot_drive_service.h"
#include "bx_ntvdm_dem_cli_unavailable_provider_v1.h"
#include "bx_ntvdm_dem_readonly_namespace_failure_provider_v1.h"
#include "bx_ntvdm_dem_fcb_provider_v1.h"
#include "bx_ntvdm_dem_dpb_service.h"
#include "bx_ntvdm_dem_full_dpb_service_v1.h"
#include "bx_ntvdm_dem_computer_name_service_v1.h"
#include "bx_ntvdm_dem_ioctl_metadata_provider_v1.h"
#include "bx_ntvdm_dem_raw_media_provider_v1.h"
#include "bx_ntvdm_dem_media_id_service_v1.h"
#include "bx_ntvdm_dem_misc_plane_v1.h"
#include "bx_ntvdm_dem_session_lifecycle_provider_v1.h"
#include "bx_ntvdm_vdd_create_user_notify_service.h"
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
int bx_ntvdm_dem_package_session_v1_valid(const bx_ntvdm_dem_package_session_v1 *s)
{ return s&&s->magic==BX_NTVDM_DEM_PACKAGE_SESSION_V1_MAGIC&&s->abi_version==BX_NTVDM_DEM_PACKAGE_SESSION_V1_VERSION&&s->struct_bytes==sizeof(*s)&&s->initialized==1u&&s->namespace_plane&&s->has_mutation_profile<=1u&&(!s->has_mutation_profile||bx_ntvdm_dem_profile_consumer_v1_valid(&s->mutation_profile)); }
int bx_ntvdm_dem_package_session_v1_initialize(bx_ntvdm_dem_package_session_v1 *s,bx_ntvdm_boot_namespace_plane_v1 *p)
{ if(!s||!p)return 0;memset(s,0,sizeof(*s));s->magic=BX_NTVDM_DEM_PACKAGE_SESSION_V1_MAGIC;s->abi_version=BX_NTVDM_DEM_PACKAGE_SESSION_V1_VERSION;s->struct_bytes=(uint32_t)sizeof(*s);s->namespace_plane=p;bx_ntvdm_dem_error_lock_plane_v1_clear(&s->error_lock);bx_ntvdm_dem_gset_plane_v1_clear(&s->gset);s->initialized=1u;return bx_ntvdm_dem_package_session_v1_valid(s); }
int bx_ntvdm_dem_package_session_v1_set_drive_snapshot(bx_ntvdm_dem_package_session_v1 *s,const bx_ntvdm_host_drive_snapshot_v1 *v)
{ return bx_ntvdm_dem_package_session_v1_valid(s)&&bx_ntvdm_dem_gset_plane_v1_set_drive_snapshot(&s->gset,v)&&bx_ntvdm_boot_namespace_plane_v1_set_drive_snapshot(s->namespace_plane,v); }
int bx_ntvdm_dem_package_session_v1_set_volume_snapshot(bx_ntvdm_dem_package_session_v1 *s,const bx_ntvdm_host_volume_snapshot_v1 *v)
{ return bx_ntvdm_dem_package_session_v1_valid(s)&&bx_ntvdm_dem_gset_plane_v1_set_volume_snapshot(&s->gset,v); }
int bx_ntvdm_dem_package_session_v1_set_mutation_profile(bx_ntvdm_dem_package_session_v1 *s,const bx_ntvdm_mutation_profile_v1 *p)
{ if(!bx_ntvdm_dem_package_session_v1_valid(s)||!p||s->has_mutation_profile)return 0;if(!bx_ntvdm_dem_profile_consumer_v1_initialize(&s->mutation_profile,p))return 0;s->has_mutation_profile=1u;return bx_ntvdm_dem_package_session_v1_valid(s); }
int bx_ntvdm_dem_package_session_v1_resolve_mutation_class(const bx_ntvdm_dem_package_session_v1 *s,uint32_t c,uint32_t *r)
{ return bx_ntvdm_dem_package_session_v1_valid(s)&&s->has_mutation_profile&&bx_ntvdm_dem_profile_consumer_v1_resolve(&s->mutation_profile,c,r); }
int bx_ntvdm_dem_package_session_v1_dispatch(bx_ntvdm_dem_package_session_v1 *s,const bx_ntvdm_bop_ingress_v1 *i,const bx_ntvdm_bop_provider_selection_v1 *p,const bx_ntvdm_exception_event_v1 *e,const bx_ntvdm_cpu_state_v1 *c,const bx_ntvdm_instruction_window_v1 *w,bx_ntvdm_cpu_result_v2 *r)
{ bx_ntvdm_dem_package_route_v1 route;bx_ntvdm_dem_plane_record_v1 plane;bx_ntvdm_exception_result_v1 mem;struct bx_ntvdm_mechanical_action_v1 a,next;bx_ntvdm_multi_write_transaction_v1 tx;uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD];
  if(!bx_ntvdm_dem_package_session_v1_valid(s)||!i||!p||!e||!c||!w||!r||!bx_ntvdm_dem_package_facade_v1_classify(i,p,&route))return 0;
  bx_ntvdm_cpu_result_v2_pass_through(r);
  if(bx_ntvdm_dem_package_facade_v1_dispatch(i,p,&route,e,c,r))return terminal_or_complete(i,p,&route,e,c,w,r);plane=route.plane;
  if(bx_ntvdm_vdd_create_user_notify_service_v1_dispatch(e,c,w,r))return terminal_or_complete(i,p,&route,e,c,w,r);
  if(bx_ntvdm_dem_misc_plane_v1_dispatch(i,p,e,c,w,&mem))return memory_result(&mem,r);
  if(bx_ntvdm_dem_session_lifecycle_provider_v1_dispatch(&s->namespace_plane->provider,i,p,&plane,e,c,r))return terminal_or_complete(i,p,&route,e,c,w,r);
  if(bx_ntvdm_dem_error_lock_plane_v1_dispatch(&s->error_lock,i,p,e,c,w,r))return terminal_or_complete(i,p,&route,e,c,w,r);
  if(bx_ntvdm_dem_gset_plane_v1_dispatch(&s->gset,i,p,e,c,w,r))return terminal_or_complete(i,p,&route,e,c,w,r);
  if(s->gset.has_volume_snapshot&&bx_ntvdm_dem_media_id_service_v1_prepare(&s->gset.volume_snapshot,e,c,w,&tx,payload)){if(!write_tx(s,&tx,payload))return 0;*r=tx.result;return 1;}
  if(bx_ntvdm_dem_media_id_service_v1_set_failure(e,c,w,r))return 1;
  if(bx_ntvdm_dem_computer_name_service_v1_prepare(e,c,w,&tx,payload)){if(!write_tx(s,&tx,payload))return 0;*r=tx.result;return 1;}
  if(s->gset.has_volume_snapshot&&bx_ntvdm_dem_full_dpb_service_v1_prepare(&s->gset.volume_snapshot,e,c,w,&tx,payload)){if(!write_tx(s,&tx,payload))return 0;*r=tx.result;return 1;}
  if(s->gset.has_drive_snapshot&&bx_ntvdm_dem_ioctl_metadata_provider_v1_dispatch(i,p,&plane,&s->gset.drive_snapshot,e,c,r))return terminal_or_complete(i,p,&route,e,c,w,r);
  if(bx_ntvdm_dem_raw_media_provider_v1_dispatch(i,p,&route,e,c,w,r))return 1;
  if(bx_ntvdm_dem_boot_drive_service_v1_dispatch(e,c,w,&mem))return memory_result(&mem,r);
  if(s->gset.has_drive_snapshot&&bx_ntvdm_dem_dpb_service_v1_prepare(s->gset.drive_snapshot.types,e,c,w,&tx,payload)){if(!write_tx(s,&tx,payload))return 0;*r=tx.result;return terminal_or_complete(i,p,&route,e,c,w,r);}
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
