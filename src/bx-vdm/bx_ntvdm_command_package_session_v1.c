#include "bx_ntvdm_command_package_session_v1.h"
#include "bx_ntvdm_cmd_boot_file_service.h"
#include "bx_ntvdm_cmd_current_dir_service.h"
#include "bx_ntvdm_cmd_keyboard_layout_service.h"
#include "bx_ntvdm_cmd_cli_profile_service.h"
#include "bx-mantle/bx_ntvdm_mechanical_action_v1.h"
#include <string.h>

static int next_id(bx_ntvdm_command_package_session_v1 *s, uint32_t *id)
{ if(!s||!id||!s->namespace_plane||!s->namespace_plane->next_action_id)return 0;
  *id=s->namespace_plane->next_action_id++; if(!s->namespace_plane->next_action_id)s->namespace_plane->next_action_id=1u; return 1; }
static int write_tx(bx_ntvdm_command_package_session_v1 *s,const bx_ntvdm_multi_write_transaction_v1 *t,const uint8_t *p)
{ struct bx_ntvdm_mechanical_action_v1 a;uint32_t i,id;
  if(!s||!t||!p||!bx_ntvdm_multi_write_transaction_v1_preflight(t,UINT64_C(0x100000),t->writes.payload_bytes))return 0;
  if(!t->writes.write_count)return 1;
  if(t->writes.write_count>BX_NTVDM_MECHANICAL_ACTION_V1_MAX_RANGES||t->writes.payload_bytes>BX_NTVDM_MECHANICAL_ACTION_V1_MAX_BYTES||!next_id(s,&id))return 0;
  bx_ntvdm_mechanical_action_v1_clear(&a);a.action_id=id;a.kind=BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;a.range_count=t->writes.write_count;a.payload_bytes=(uint32_t)t->writes.payload_bytes;
  for(i=0;i<a.range_count;i++){a.ranges[i].physical_address=t->writes.writes[i].guest_physical_address;a.ranges[i].byte_count=(uint32_t)t->writes.writes[i].byte_count;a.ranges[i].payload_offset=(uint32_t)t->writes.writes[i].payload_offset;}
  memcpy(a.payload,p,a.payload_bytes);return bx_ntvdm_mechanical_action_v1_valid(&a)&&bx_ntvdm_mantle_execute_mechanical_action_v1(&a); }
static int read_one(bx_ntvdm_command_package_session_v1 *s,uint64_t address,uint32_t bytes,uint8_t *payload)
{ struct bx_ntvdm_mechanical_action_v1 a;uint32_t id;
  if(!s||!payload||!bytes||bytes>BX_NTVDM_MECHANICAL_ACTION_V1_MAX_BYTES||address>UINT64_C(0x100000)-bytes||!next_id(s,&id))return 0;
  bx_ntvdm_mechanical_action_v1_clear(&a);a.action_id=id;a.kind=BX_NTVDM_MECHANICAL_ACTION_V1_READ;a.range_count=1u;a.payload_bytes=bytes;a.ranges[0].physical_address=address;a.ranges[0].byte_count=bytes;
  if(!bx_ntvdm_mechanical_action_v1_valid(&a)||!bx_ntvdm_mantle_execute_mechanical_action_v1(&a))return 0;memcpy(payload,a.payload,bytes);return 1; }
static int validate_exec_streams(bx_ntvdm_command_package_session_v1 *s,
    const bx_ntvdm_cpu_state_v1 *c, const bx_ntvdm_instruction_window_v1 *w,
    int direct)
{ uint8_t payload[12];uint64_t address;
  if(!s||!c||!w||!direct||w->bytes[3]!=8u)return 0;
  address=((uint64_t)c->ss<<4u)+(c->ebp&0xffffu);
  if(address>UINT64_C(0x100000)-sizeof(payload)||!read_one(s,address,sizeof(payload),payload)) {
    if(s->stream_child.rejected_record_count!=UINT32_MAX)++s->stream_child.rejected_record_count;
    return 0;
  }
  return bx_ntvdm_command_stream_child_v1_validate_std_handles(&s->stream_child,payload,sizeof(payload)); }
static int read_exec_tail(bx_ntvdm_command_package_session_v1 *s,const bx_ntvdm_cpu_state_v1 *c,uint8_t payload[125],uint32_t *bytes)
{ uint64_t address;uint32_t i;
  if(!s||!c||!payload||!bytes)return 0;address=((uint64_t)c->ds<<4u)+(c->esi&0xffffu);
  if(address>UINT64_C(0x100000)-124u||!read_one(s,address,124u,payload))return 0;
  for(i=0u;i<124u;i++)if(payload[i]==0x0du){payload[i]=0u;*bytes=i+1u;return i!=0u;}return 0; }
static int read_exec_environment(bx_ntvdm_command_package_session_v1 *s,const bx_ntvdm_cpu_state_v1 *c,uint8_t payload[BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES],uint32_t *bytes)
{ uint64_t address;uint32_t i;
  if(!s||!c||!payload||!bytes)return 0;address=(uint64_t)c->es<<4u;
  if(address>UINT64_C(0x100000)-BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES||!read_one(s,address,BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES,payload))return 0;
  for(i=1u;i<BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES;i++)if(!payload[i-1u]&&!payload[i]){*bytes=i+1u;return 1;}return 0; }
static int dispatch_child(bx_ntvdm_command_package_session_v1 *s,const bx_ntvdm_exception_event_v1 *e,const bx_ntvdm_cpu_state_v1 *c,const bx_ntvdm_instruction_window_v1 *w,int direct,bx_ntvdm_cpu_result_v2 *r)
{ uint8_t command[125],environment[BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES];uint32_t command_bytes,environment_bytes;
  if(!s||!e||!c||!w||!r||!direct||!s->has_host_context||(w->bytes[3]!=8u&&w->bytes[3]!=10u))return 0;
  if(w->bytes[3]==8u){if(!validate_exec_streams(s,c,w,direct)||!read_exec_tail(s,c,command,&command_bytes))return 0;}
  else {if(!s->host_context.processor_bytes)return 0;memcpy(command,s->host_context.processor,s->host_context.processor_bytes);command_bytes=s->host_context.processor_bytes;}
  if(!read_exec_environment(s,c,environment,&environment_bytes))return 0;
  return bx_ntvdm_command_stream_child_v1_launch(&s->stream_child,command,command_bytes,environment,environment_bytes,&s->host_context,e,c,w,r); }
int bx_ntvdm_command_package_session_v1_valid(const bx_ntvdm_command_package_session_v1 *s)
{ return s&&s->magic==BX_NTVDM_COMMAND_PACKAGE_SESSION_V1_MAGIC&&s->abi_version==BX_NTVDM_COMMAND_PACKAGE_SESSION_V1_VERSION&&s->struct_bytes==sizeof(*s)&&s->initialized==1u&&s->namespace_plane&&s->gset&&s->has_mutation_profile<=1u&&s->has_host_context<=1u&&s->has_session_host_context<=1u&&bx_ntvdm_command_bootstrap_provider_v1_valid(&s->bootstrap_provider)&&bx_ntvdm_command_console_capability_v1_valid(&s->console)&&bx_ntvdm_command_execution_lifecycle_v1_valid(&s->execution_lifecycle)&&bx_ntvdm_command_stream_child_v1_valid(&s->stream_child)&&(!s->has_mutation_profile||bx_ntvdm_command_profile_consumer_v1_valid(&s->mutation_profile))&&(!s->has_host_context||bx_ntvdm_command_host_context_v1_valid(&s->host_context))&&(!s->has_session_host_context||bx_ntvdm_session_host_context_v1_valid(s->session_host_context)); }
int bx_ntvdm_command_package_session_v1_initialize(bx_ntvdm_command_package_session_v1 *s,bx_ntvdm_boot_namespace_plane_v1 *n,bx_ntvdm_dem_gset_plane_v1 *g)
{ if(!s||!n||!g)return 0;memset(s,0,sizeof(*s));s->magic=BX_NTVDM_COMMAND_PACKAGE_SESSION_V1_MAGIC;s->abi_version=BX_NTVDM_COMMAND_PACKAGE_SESSION_V1_VERSION;s->struct_bytes=(uint32_t)sizeof(*s);s->namespace_plane=n;s->gset=g;if(!bx_ntvdm_command_bootstrap_provider_v1_initialize(&s->bootstrap_provider,&n->provider.readonly_namespace))return 0;bx_ntvdm_command_console_capability_v1_initialize(&s->console,0u);bx_ntvdm_command_execution_lifecycle_v1_initialize(&s->execution_lifecycle);bx_ntvdm_command_stream_child_v1_initialize(&s->stream_child);s->initialized=1u;return bx_ntvdm_command_package_session_v1_valid(s); }
int bx_ntvdm_command_package_session_v1_set_launch_plan(bx_ntvdm_command_package_session_v1 *s,const byob_launch_plan_v2 *p)
{ wchar_t encoded[BYOB_LAUNCH_PLAN_V2_ENV_CHARS];byob_launch_plan_v2 checked;if(!bx_ntvdm_command_package_session_v1_valid(s)||!p||!byob_launch_plan_v2_to_environment(p,encoded)||!byob_launch_plan_v2_from_environment(&checked,encoded))return 0;s->launch_plan=checked;s->has_launch_plan=1u;return 1; }
int bx_ntvdm_command_package_session_v1_set_mutation_profile(bx_ntvdm_command_package_session_v1 *s,const bx_ntvdm_mutation_profile_v1 *p)
{ if(!bx_ntvdm_command_package_session_v1_valid(s)||!p||s->has_mutation_profile||!bx_ntvdm_command_profile_consumer_v1_initialize(&s->mutation_profile,p))return 0;s->has_mutation_profile=1u;return bx_ntvdm_command_package_session_v1_valid(s); }
int bx_ntvdm_command_package_session_v1_set_host_context(bx_ntvdm_command_package_session_v1 *s,const bx_ntvdm_command_host_context_v1 *c)
{ if(!bx_ntvdm_command_package_session_v1_valid(s)||!c||s->has_host_context||!bx_ntvdm_command_host_context_v1_valid(c))return 0;s->host_context=*c;s->has_host_context=1u;return bx_ntvdm_command_package_session_v1_valid(s); }
int bx_ntvdm_command_package_session_v1_set_session_host_context(bx_ntvdm_command_package_session_v1 *s,const bx_ntvdm_session_host_context_v1 *c)
{ if(!bx_ntvdm_command_package_session_v1_valid(s)||!c||s->has_session_host_context||!bx_ntvdm_session_host_context_v1_valid(c))return 0;
  s->session_host_context=c;s->has_session_host_context=1u;
  return bx_ntvdm_command_package_session_v1_valid(s); }
int bx_ntvdm_command_package_session_v1_set_boot_input(bx_ntvdm_command_package_session_v1 *s,const bx_ntvdm_command_boot_input_v1 *b)
{ if(!bx_ntvdm_command_package_session_v1_valid(s)||!b||!bx_ntvdm_command_boot_input_v1_valid(b))return 0;s->bootstrap_provider.boot_input=*b;return bx_ntvdm_command_package_session_v1_valid(s); }
int bx_ntvdm_command_package_session_v1_resolve_mutation_class(const bx_ntvdm_command_package_session_v1 *s,uint32_t c,uint32_t *r)
{ return bx_ntvdm_command_package_session_v1_valid(s)&&s->has_mutation_profile&&bx_ntvdm_command_profile_consumer_v1_resolve(&s->mutation_profile,c,r); }
static int bootstrap(bx_ntvdm_command_package_session_v1 *s,const bx_ntvdm_exception_event_v1 *e,const bx_ntvdm_cpu_state_v1 *c,const bx_ntvdm_instruction_window_v1 *w,bx_ntvdm_cpu_result_v2 *r)
{ bx_ntvdm_guest_gather_read_action_v1 read;bx_ntvdm_multi_write_transaction_v1 write;uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD];
  if(w->bytes[3]==2u){if(bx_ntvdm_cmd_comspec_bootstrap_v1_prepare_comspec(e,c,w,&s->bootstrap_provider.comspec,&read)){if(read.range_count!=1u||read.total_bytes!=BX_NTVDM_CMD_COMSPEC_TEXT_MAX_BYTES||!read_one(s,read.ranges[0].address,(uint32_t)read.ranges[0].length,payload)||!bx_ntvdm_cmd_comspec_bootstrap_v1_complete_comspec(e,c,&read,payload,read.total_bytes,s->has_host_context?&s->host_context:0,&s->bootstrap_provider.comspec,r))return 0;return bx_ntvdm_cpu_result_v2_valid(r);}return bx_ntvdm_cmd_comspec_bootstrap_v1_repeat_comspec(e,c,w,&s->bootstrap_provider.comspec,r)&&bx_ntvdm_cpu_result_v2_valid(r);}
  if(w->bytes[3]!=15u||!bx_ntvdm_cmd_comspec_bootstrap_v1_prepare_environment(e,c,w,&s->bootstrap_provider.comspec,&write,payload)||!bx_ntvdm_cpu_result_v2_valid(&write.result)||(write.writes.write_count&&!write_tx(s,&write,payload))||(write.writes.write_count&&!bx_ntvdm_cmd_comspec_bootstrap_v1_complete_environment(&s->bootstrap_provider.comspec,&write)))return 0;*r=write.result;return bx_ntvdm_cpu_result_v2_valid(r); }
static int get_next(bx_ntvdm_command_package_session_v1 *s,const bx_ntvdm_exception_event_v1 *e,const bx_ntvdm_cpu_state_v1 *c,const bx_ntvdm_instruction_window_v1 *w,bx_ntvdm_cpu_result_v2 *r)
{ bx_ntvdm_guest_gather_read_action_v1 read;bx_ntvdm_multi_write_transaction_v1 write;uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD];uint32_t env=0u;
  if(s&&s->bootstrap_provider.comspec.stage==BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_ENVIRONMENT_CONSUMED)env=s->bootstrap_provider.comspec.environment_bytes;
  /* OpenNT cmdGetNextCmd terminates the VDM when GetNextVDMCommand has no
     command.  A declared CLI plan has no ambient queue, so its exhausted
     state is the contained equivalent: typed stop, not raw #UD or a fake
     command.  This branch is reached only after COMMAND package routing. */
  if(s&&s->has_launch_plan&&s->bootstrap_provider.get_next.delivered>=s->launch_plan.slot_count)
    return bx_ntvdm_cpu_result_v2_stop(r);
  if(!s->has_launch_plan||!s->gset->has_drive_snapshot||!bx_ntvdm_cmd_get_next_v1_prepare(&s->bootstrap_provider.get_next,&s->launch_plan,e,c,w,&read)||read.range_count!=1u||read.total_bytes!=BX_NTVDM_CMDINFO_V1_BYTES||!read_one(s,read.ranges[0].address,(uint32_t)read.ranges[0].length,payload)||!bx_ntvdm_cmd_get_next_v1_complete(&s->namespace_plane->provider.readonly_namespace,&s->launch_plan,&s->gset->drive_snapshot,env,s->bootstrap_provider.set_info.valid?&s->bootstrap_provider.set_info.registration:0,&s->bootstrap_provider.get_next,e,c,&read,payload,read.total_bytes,&write,payload)||!write_tx(s,&write,payload))return 0;bx_ntvdm_cmd_get_next_state_v1_commit(&s->bootstrap_provider.get_next);*r=write.result;return bx_ntvdm_cpu_result_v2_valid(r); }
int bx_ntvdm_command_package_session_v1_dispatch(bx_ntvdm_command_package_session_v1 *s,const bx_ntvdm_bop_ingress_v1 *i,const bx_ntvdm_bop_provider_selection_v1 *p,const bx_ntvdm_exception_event_v1 *e,const bx_ntvdm_cpu_state_v1 *c,const bx_ntvdm_instruction_window_v1 *w,bx_ntvdm_cpu_result_v2 *r)
{ bx_ntvdm_command_package_route_v1 route;bx_ntvdm_multi_write_transaction_v1 tx;bx_ntvdm_command_host_context_v1 projected;const bx_ntvdm_command_host_context_v1 *context;uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD];uint32_t policy=0u;
  if(!bx_ntvdm_command_package_session_v1_valid(s)||!i||!p||!e||!c||!w||!r||!bx_ntvdm_command_package_facade_v1_classify(i,p,&route))return 0;
  if(bx_ntvdm_command_package_facade_v1_dispatch(i,p,&route,e,c,r))return 1;
  if(route.disposition!=BX_NTVDM_COMMAND_PACKAGE_EXISTING_PROVIDER)return 0;
  if(bx_ntvdm_cmd_cli_profile_v1_dispatch(e,c,w,r))return 1;
  if(bx_ntvdm_command_console_capability_v1_dispatch(&s->console,e,c,w,r))return 1;
  if(bx_ntvdm_command_stream_child_v1_dispatch_stream(&s->stream_child,
      s->has_mutation_profile&&bx_ntvdm_command_package_session_v1_resolve_mutation_class(s,BX_NTVDM_MUTATION_CLASS_V1_HOST_GLOBAL,&policy)&&policy==BX_NTVDM_MUTATION_POLICY_V1_DIRECT_HOST,e,c,w,r))return 1;
  if(dispatch_child(s,e,c,w,s->has_mutation_profile&&bx_ntvdm_command_package_session_v1_resolve_mutation_class(s,BX_NTVDM_MUTATION_CLASS_V1_HOST_GLOBAL,&policy)&&policy==BX_NTVDM_MUTATION_POLICY_V1_DIRECT_HOST,r))return 1;
  if(bx_ntvdm_command_execution_lifecycle_v1_dispatch(&s->execution_lifecycle,&s->bootstrap_provider.get_next,&s->launch_plan,e,c,w,r))return 1;
  if (bx_ntvdm_command_bootstrap_provider_v1_owns_service((uint8_t)route.plane.service)) switch(route.plane.service){
  case 1u:return get_next(s,e,c,w,r);case 2u:case 15u:return bootstrap(s,e,c,w,r);
  case 4u:context=s->has_host_context?&s->host_context:0;if(s->has_session_host_context){if(!bx_ntvdm_session_host_context_v1_project_command(s->session_host_context,&projected))return 0;context=&projected;}if(!s->gset->has_drive_snapshot||!bx_ntvdm_cmd_current_dir_service_v1_prepare(s->gset->drive_snapshot.admitted_mask,context,e,c,w,&tx,payload)||(tx.writes.write_count&&!write_tx(s,&tx,payload)))return 0;*r=tx.result;return bx_ntvdm_cpu_result_v2_valid(r);
  case 5u:return bx_ntvdm_command_launch_plane_v1_dispatch(&s->bootstrap_provider.set_info,i,p,e,c,w,r);
  case 12u:case 13u:if(!bx_ntvdm_cmd_boot_file_service_v1_prepare(&s->bootstrap_provider.boot_input,e,c,w,&tx,payload)||(tx.writes.write_count&&!write_tx(s,&tx,payload)))return 0;*r=tx.result;return bx_ntvdm_cpu_result_v2_valid(r);
  default:return 0; }
  return 0;
}
