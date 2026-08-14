#include "bx_ntvdm_dpmi_package_session_v1.h"
#include <string.h>

int bx_ntvdm_dpmi_package_session_v1_valid(
  const bx_ntvdm_dpmi_package_session_v1 *s)
{ return s && s->magic==BX_NTVDM_DPMI_PACKAGE_SESSION_V1_MAGIC &&
  s->abi_version==BX_NTVDM_DPMI_PACKAGE_SESSION_V1_VERSION &&
  s->struct_bytes==sizeof(*s) && s->initialized==1u; }

int bx_ntvdm_dpmi_package_session_v1_initialize(
  bx_ntvdm_dpmi_package_session_v1 *s)
{ if(!s)return 0;memset(s,0,sizeof(*s));s->magic=BX_NTVDM_DPMI_PACKAGE_SESSION_V1_MAGIC;
  s->abi_version=BX_NTVDM_DPMI_PACKAGE_SESSION_V1_VERSION;s->struct_bytes=(uint32_t)sizeof(*s);
  s->initialized=1u;return bx_ntvdm_dpmi_package_session_v1_valid(s); }

int bx_ntvdm_dpmi_package_session_v1_dispatch(
  bx_ntvdm_dpmi_package_session_v1 *s,const bx_ntvdm_bop_ingress_v1 *i,
  const bx_ntvdm_bop_provider_selection_v1 *p,const bx_ntvdm_exception_event_v1 *e,
  const bx_ntvdm_cpu_state_v1 *c,const bx_ntvdm_instruction_window_v1 *w,
  bx_ntvdm_cpu_result_v2 *r)
{ bx_ntvdm_xms_dpmi_plane_record_v1 plane;
  if(!bx_ntvdm_dpmi_package_session_v1_valid(s)||!i||!p||!e||!c||!w||!r||
     !bx_ntvdm_xms_dpmi_plane_v1_classify(i,p,&plane)||
     plane.provider_family!=BX_NTVDM_BOP_PROVIDER_DPMI||w->valid_bytes<4u||
     w->bytes[0]!=0xc4u||w->bytes[1]!=0xc4u||w->bytes[2]!=0x53u||
     w->bytes[3]!=plane.service)return 0;
  if(plane.disposition==BX_NTVDM_XMS_DPMI_EXPLICIT_UNAVAILABLE)
    return bx_ntvdm_cpu_result_v2_stop(r);
  bx_ntvdm_cpu_result_v2_pass_through(r);return 1; }
