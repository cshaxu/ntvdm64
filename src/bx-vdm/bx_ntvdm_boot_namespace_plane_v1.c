#include "bx_ntvdm_boot_namespace_plane_v1.h"
#include "bx_ntvdm_dem_fastio_provider_v1.h"
#include "bx_ntvdm_dem_fcb_search_service_v1.h"
#include "bx_ntvdm_dem_cwd_service_v2.h"
#include "bx_ntvdm_dem_check_path_service_v1.h"
#include "bx_ntvdm_dem_readonly_file_service.h"
#include <string.h>

#define BX_NTVDM_BOOT_NAMESPACE_APERTURE UINT64_C(0x100000)

static int valid(const bx_ntvdm_boot_namespace_plane_v1 *p)
{
    return p && p->magic == BX_NTVDM_BOOT_NAMESPACE_PLANE_V1_MAGIC &&
        p->abi_version == BX_NTVDM_BOOT_NAMESPACE_PLANE_V1_VERSION &&
        p->struct_bytes == sizeof(*p) && p->next_action_id &&
        bx_ntvdm_boot_namespace_provider_v1_valid(&p->provider) &&
        p->has_dta <= 1u && p->has_drive_snapshot <= 1u &&
        p->pending_kind <= BX_NTVDM_BOOT_NAMESPACE_PENDING_V1_CHECK_PATH &&
        (!p->dem_cwd_context || bx_ntvdm_dem_cwd_context_v1_valid(p->dem_cwd_context));
}
static int take_id(bx_ntvdm_boot_namespace_plane_v1 *p, uint32_t *id)
{ if (!p || !id || !p->next_action_id) return 0; *id=p->next_action_id++; if (!p->next_action_id) p->next_action_id=1; return 1; }
static int put_write(bx_ntvdm_boot_namespace_plane_v1 *p,
    const bx_ntvdm_multi_write_transaction_v1 *t, const uint8_t *bytes,
    struct bx_ntvdm_mechanical_action_v1 *a)
{
    uint32_t i,id;
    if (!p || !t || !bytes || !a || !bx_ntvdm_multi_write_transaction_v1_preflight(t,BX_NTVDM_BOOT_NAMESPACE_APERTURE,t->writes.payload_bytes) ||
        t->writes.write_count > BX_NTVDM_MECHANICAL_ACTION_V1_MAX_RANGES || !take_id(p,&id)) return 0;
    bx_ntvdm_mechanical_action_v1_clear(a); a->action_id=id;
    a->kind=BX_NTVDM_MECHANICAL_ACTION_V1_WRITE; a->range_count=t->writes.write_count;
    a->payload_bytes=(uint32_t)t->writes.payload_bytes;
    for(i=0;i<a->range_count;i++){ a->ranges[i].physical_address=t->writes.writes[i].guest_physical_address;
      a->ranges[i].byte_count=(uint32_t)t->writes.writes[i].byte_count;
      a->ranges[i].payload_offset=(uint32_t)t->writes.writes[i].payload_offset; }
    memcpy(a->payload,bytes,a->payload_bytes); return bx_ntvdm_mechanical_action_v1_valid(a);
}
static int put_read(bx_ntvdm_boot_namespace_plane_v1 *p, uint32_t kind,
    const bx_ntvdm_exception_event_v1 *e, const bx_ntvdm_cpu_state_v1 *c,
    const bx_ntvdm_guest_range *ranges, uint32_t count,
    struct bx_ntvdm_mechanical_action_v1 *a)
{
    uint32_t i,id,total=0;
    if(!p||!e||!c||!ranges||!a||!count||count>4u||p->pending_kind||!take_id(p,&id)) return 0;
    bx_ntvdm_mechanical_action_v1_clear(a); a->action_id=id; a->kind=BX_NTVDM_MECHANICAL_ACTION_V1_READ; a->range_count=count;
    for(i=0;i<count;i++){if(ranges[i].length==0||ranges[i].length>UINT32_MAX||total>BX_NTVDM_MECHANICAL_ACTION_V1_MAX_BYTES-ranges[i].length)return 0;
      a->ranges[i].physical_address=ranges[i].address; a->ranges[i].byte_count=(uint32_t)ranges[i].length; a->ranges[i].payload_offset=total; total+=(uint32_t)ranges[i].length;}
    a->payload_bytes=total; if(!bx_ntvdm_mechanical_action_v1_valid(a))return 0;
    p->pending_kind=kind;p->pending_event=*e;p->pending_cpu=*c;
    p->pending_action_id=id; return 1;
}
static int put_bulk(bx_ntvdm_boot_namespace_plane_v1 *p,
    const bx_ntvdm_bulk_result_transaction_v1 *t, const uint8_t *bytes,
    struct bx_ntvdm_mechanical_action_v1 *a)
{ uint32_t id; if(!p||!t||!bytes||!a||!bx_ntvdm_bulk_result_transaction_v1_preflight(t,BX_NTVDM_BOOT_NAMESPACE_APERTURE,t->payload_bytes)||!take_id(p,&id))return 0;
  bx_ntvdm_mechanical_action_v1_clear(a);a->action_id=id;a->kind=BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;a->range_count=t->payload_bytes?1u:0u;a->payload_bytes=t->payload_bytes;
  if(t->payload_bytes){a->ranges[0].physical_address=t->guest_physical_address;a->ranges[0].byte_count=t->payload_bytes;memcpy(a->payload,bytes,t->payload_bytes);}return t->payload_bytes?bx_ntvdm_mechanical_action_v1_valid(a):1; }

int bx_ntvdm_boot_namespace_plane_v1_initialize(bx_ntvdm_boot_namespace_plane_v1 *p,
    const byob_image *ntdos,const byob_image *command,const byob_image *target,const byob_image *quit,
    const byob_profile_selection *selection)
{ if(!p||!selection||!bx_ntvdm_boot_namespace_provider_v1_initialize(&p->provider,command,target,quit,selection))return 0;
  if(ntdos)p->ntdos=*ntdos;else memset(&p->ntdos,0,sizeof(p->ntdos));p->ntdos_identity=selection->ntdos;
  p->magic=BX_NTVDM_BOOT_NAMESPACE_PLANE_V1_MAGIC;p->abi_version=1u;p->struct_bytes=sizeof(*p);p->next_action_id=1u;p->has_dta=0;p->has_drive_snapshot=0;p->pending_kind=0;p->pending_action_id=0;p->pending_service=0;return valid(p); }
int bx_ntvdm_boot_namespace_plane_v1_set_dta(bx_ntvdm_boot_namespace_plane_v1 *p,const bx_ntvdm_dem_dta_registration_v1 *d)
{if(!valid(p)||!d||!d->dta_location||!d->current_pdb)return 0;p->dta=*d;p->has_dta=1;return 1;}
int bx_ntvdm_boot_namespace_plane_v1_set_drive_snapshot(bx_ntvdm_boot_namespace_plane_v1 *p,const bx_ntvdm_host_drive_snapshot_v1 *d)
{if(!valid(p)||!d||!bx_ntvdm_host_drive_snapshot_v1_valid(d))return 0;p->drive_snapshot=*d;p->has_drive_snapshot=1;return 1;}
int bx_ntvdm_boot_namespace_plane_v1_set_dem_cwd_context(
    bx_ntvdm_boot_namespace_plane_v1 *p,
    bx_ntvdm_dem_cwd_context_v1 *context,
    const bx_ntvdm_host_namespace_v1 *host_namespace)
{ if(!valid(p)||!context||!bx_ntvdm_dem_cwd_context_v1_valid(context))return 0;
  p->dem_cwd_context=context;p->dem_host_namespace=host_namespace;return valid(p); }

int bx_ntvdm_boot_namespace_plane_v1_dispatch(bx_ntvdm_boot_namespace_plane_v1 *p,
 const bx_ntvdm_bop_ingress_v1 *i,const bx_ntvdm_bop_provider_selection_v1 *s,
 const bx_ntvdm_exception_event_v1 *e,const bx_ntvdm_cpu_state_v1 *c,const bx_ntvdm_instruction_window_v1 *w,
 struct bx_ntvdm_mechanical_action_v1 *a,bx_ntvdm_cpu_result_v2 *r)
{ bx_ntvdm_dem_plane_record_v1 dem; bx_ntvdm_bulk_result_transaction_v1 bulk; uint8_t bytes[BX_NTVDM_MECHANICAL_ACTION_V1_MAX_BYTES]; bx_ntvdm_guest_read_action_v1 read; bx_ntvdm_guest_gather_read_action_v1 gather;
 if(!valid(p)||!i||!s||!e||!c||!w||!a||!r||p->pending_kind)return 0;
 bx_ntvdm_cpu_result_v2_pass_through(r);
 bx_ntvdm_mechanical_action_v1_clear(a);
 if(i->family!=BX_NTVDM_BOP_FAMILY_DEM||!bx_ntvdm_dem_plane_v1_classify(i,s,&dem)||dem.disposition!=BX_NTVDM_DEM_PLANE_DEFERRED)return 1;
 /* The original DEM dispatcher assigns SVC_DEMSETDTALOCATION (1Bh) to
  * demgset.c, not to the file namespace.  Its registration is retained here
  * only as the prerequisite consumed by the selected namespace provider. */
 if(i->service==0x1bu&&dem.component==BX_NTVDM_DEM_COMPONENT_GSET){
   if(bx_ntvdm_dem_dta_service_v1_dispatch(e,c,w,&read)){
     if(!put_read(p,BX_NTVDM_BOOT_NAMESPACE_PENDING_V1_DTA_REGISTRATION,e,c,&read.guest_read,1u,a))return 0;
     p->pending_read=read;return 1;
   }
   /* demSetDTALocation has no historical error return, but a copied address
    * rejected by this non-invasive composition must not escape as #UD. */
   return bx_ntvdm_cpu_result_v2_resume(r,e->fault_rip+4u)&&
     bx_ntvdm_cpu_delta_v1_set_gpr16(&r->cpu_delta,0u,5u)&&
     bx_ntvdm_cpu_result_v2_set_cf(r,1);
 }
 if((i->service==0x13u||i->service==0x18u||i->service==0x1au)&&p->dem_cwd_context){
   if(bx_ntvdm_dem_cwd_service_v2_prepare(i->service,e,c,&gather)){
     if(!put_read(p,BX_NTVDM_BOOT_NAMESPACE_PENDING_V1_CURRENT_DIR,e,c,gather.ranges,gather.range_count,a))return 0;
     p->pending_gather=gather;p->pending_service=i->service;return 1;
   }
 }
 if((i->service==0x0au||i->service==0x0cu)&&dem.component==BX_NTVDM_DEM_COMPONENT_FCB&&p->has_dta){
   int ok=i->service==0x0au?
     bx_ntvdm_dem_fcb_search_service_v1_prepare_first(&p->dta,e,c,w,&gather):
     bx_ntvdm_dem_fcb_search_service_v1_prepare_next(&p->dta,e,c,w,&gather);
   if(ok){if(!put_read(p,i->service==0x0au?BX_NTVDM_BOOT_NAMESPACE_PENDING_V1_FCB_FIRST:BX_NTVDM_BOOT_NAMESPACE_PENDING_V1_FCB_NEXT,e,c,gather.ranges,gather.range_count,a))return 0;p->pending_gather=gather;return 1;}
 }
 /* demLoadDos is owned by demmisc.c, not by the namespace or raw-media
  * package.  Its only admitted CLI capability is the already-copied NTDOS
  * image; the plane performs no host filesystem or device operation. */
 if(i->service==0x11u&&dem.component==BX_NTVDM_DEM_COMPONENT_MISC){uint32_t id=0;if(!take_id(p,&id)||!bx_ntvdm_dem_load_dos_service_v1_prepare(&p->ntdos,&p->ntdos_identity,e,c,w,id,a,r)){bx_ntvdm_mechanical_action_v1_clear(a);return bx_ntvdm_cpu_result_v2_stop(r);}return 1;}
 if(i->service==0x44u&&dem.component==BX_NTVDM_DEM_COMPONENT_NAMESPACE&&bx_ntvdm_dem_check_path_service_v1_prepare(e,c,w,&read)){
   if(!put_read(p,BX_NTVDM_BOOT_NAMESPACE_PENDING_V1_CHECK_PATH,e,c,&read.guest_read,1u,a))return 0;
   p->pending_read=read;return 1;
 }
 if(dem.component!=BX_NTVDM_DEM_COMPONENT_NAMESPACE)return 0;
  if(i->service==0u)return bx_ntvdm_boot_namespace_provider_v1_seek(&p->provider,e,c,w,r)&&bx_ntvdm_cpu_result_v2_valid(r);
  if(i->service==2u)return bx_ntvdm_boot_namespace_provider_v1_close(&p->provider,e,c,w,r)&&bx_ntvdm_cpu_result_v2_valid(r);
  if(i->service==8u)return bx_ntvdm_dem_readonly_file_v1_file_times(&p->provider.readonly_namespace,e,c,w,r)&&bx_ntvdm_cpu_result_v2_valid(r);
  if(i->service==0x16u&&bx_ntvdm_boot_namespace_provider_v1_read(&p->provider,e,c,w,bytes,sizeof(bytes),&bulk,r)){if(!bulk.magic)return bx_ntvdm_cpu_result_v2_valid(r);return put_bulk(p,&bulk,bytes,a)&&bx_ntvdm_cpu_result_v2_valid(r);}
  if(i->service==0x42u&&bx_ntvdm_dem_fastio_provider_v1_dispatch(&p->provider,e,c,w,bytes,sizeof(bytes),&bulk,r)){if(!bulk.magic)return bx_ntvdm_cpu_result_v2_valid(r);return put_bulk(p,&bulk,bytes,a)&&bx_ntvdm_cpu_result_v2_valid(r);}
  if(i->service==18u&&bx_ntvdm_boot_namespace_provider_v1_prepare_open(&p->provider,e,c,w,&read)){ if(!put_read(p,BX_NTVDM_BOOT_NAMESPACE_PENDING_V1_OPEN,e,c,&read.guest_read,1u,a))return 0;p->pending_read=read;return 1; }
 if((i->service==9u||i->service==11u)&&p->has_dta){int ok=i->service==9u?bx_ntvdm_dem_path_search_v1_prepare_first(&p->dta,e,c,w,&gather):bx_ntvdm_dem_path_search_v1_prepare_next(&p->dta,e,c,w,&gather);if(ok){if(!put_read(p,i->service==9u?BX_NTVDM_BOOT_NAMESPACE_PENDING_V1_PATH_FIRST:BX_NTVDM_BOOT_NAMESPACE_PENDING_V1_PATH_NEXT,e,c,gather.ranges,gather.range_count,a))return 0;p->pending_gather=gather;return 1;}}
 return 0; }

int bx_ntvdm_boot_namespace_plane_v1_complete_read(bx_ntvdm_boot_namespace_plane_v1 *p,
 const struct bx_ntvdm_mechanical_action_v1 *a,struct bx_ntvdm_mechanical_action_v1 *next,bx_ntvdm_cpu_result_v2 *r)
{ bx_ntvdm_multi_write_transaction_v1 tx;bx_ntvdm_dem_dta_registration_v1 dta;uint8_t bytes[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD];uint32_t used=0;int ok=0;
 if(!valid(p)||!a||!next||!r||!p->pending_kind||a->action_id!=p->pending_action_id||a->kind!=BX_NTVDM_MECHANICAL_ACTION_V1_READ||!bx_ntvdm_mechanical_action_v1_valid(a))return 0;
 memset(&tx,0,sizeof(tx));bx_ntvdm_cpu_result_v2_pass_through(r);bx_ntvdm_mechanical_action_v1_clear(next);
 if(p->pending_kind==BX_NTVDM_BOOT_NAMESPACE_PENDING_V1_OPEN)ok=bx_ntvdm_boot_namespace_provider_v1_complete_open(&p->provider,&p->pending_event,&p->pending_cpu,&p->pending_read,a->payload,a->payload_bytes,r);
 else if(p->pending_kind==BX_NTVDM_BOOT_NAMESPACE_PENDING_V1_DTA_REGISTRATION){ok=bx_ntvdm_dem_dta_service_v1_complete(&p->pending_event,&p->pending_cpu,&p->pending_read,a->payload,a->payload_bytes,&dta,r);if(ok)ok=bx_ntvdm_boot_namespace_plane_v1_set_dta(p,&dta);}
 else if(p->pending_kind==BX_NTVDM_BOOT_NAMESPACE_PENDING_V1_CURRENT_DIR)ok=bx_ntvdm_dem_cwd_service_v2_complete(p->dem_cwd_context,p->dem_host_namespace,(uint8_t)p->pending_service,&p->pending_event,&p->pending_cpu,&p->pending_gather,a->payload,a->payload_bytes,&tx,bytes);
 else if(p->pending_kind==BX_NTVDM_BOOT_NAMESPACE_PENDING_V1_CHECK_PATH)ok=bx_ntvdm_dem_check_path_service_v1_complete(&p->pending_event,&p->pending_cpu,&p->pending_read,a->payload,a->payload_bytes,r);
 else if(p->pending_kind==BX_NTVDM_BOOT_NAMESPACE_PENDING_V1_PATH_FIRST)ok=bx_ntvdm_dem_path_search_v1_complete_first(&p->provider.search_transaction,&p->provider.search_snapshot,&p->pending_event,&p->pending_cpu,&p->pending_gather,a->payload,a->payload_bytes,&tx,bytes,&used)>=0;
 else if(p->pending_kind==BX_NTVDM_BOOT_NAMESPACE_PENDING_V1_PATH_NEXT)ok=bx_ntvdm_dem_path_search_v1_complete_next(&p->provider.search_transaction,&p->pending_event,&p->pending_cpu,&p->pending_gather,a->payload,a->payload_bytes,&tx,bytes,&used)>=0;
 else if(p->pending_kind==BX_NTVDM_BOOT_NAMESPACE_PENDING_V1_FCB_FIRST)ok=bx_ntvdm_dem_fcb_search_service_v1_complete_first(&p->provider.search_transaction,&p->provider.search_snapshot,&p->pending_event,&p->pending_cpu,&p->pending_gather,a->payload,a->payload_bytes,&tx,bytes,&used)>=0;
 else if(p->pending_kind==BX_NTVDM_BOOT_NAMESPACE_PENDING_V1_FCB_NEXT)ok=bx_ntvdm_dem_fcb_search_service_v1_complete_next(&p->provider.search_transaction,&p->pending_event,&p->pending_cpu,&p->pending_gather,a->payload,a->payload_bytes,&tx,bytes,&used)>=0;
 p->pending_kind=0;p->pending_action_id=0;p->pending_service=0;if(!ok)return 0;
 if(tx.magic){if(tx.writes.write_count==0u){*r=tx.result;return bx_ntvdm_cpu_result_v2_valid(r);}if(!put_write(p,&tx,bytes,next))return 0;*r=tx.result;} return bx_ntvdm_cpu_result_v2_valid(r); }
