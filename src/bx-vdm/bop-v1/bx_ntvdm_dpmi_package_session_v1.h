#ifndef BX_NTVDM_DPMI_PACKAGE_SESSION_V1_H
#define BX_NTVDM_DPMI_PACKAGE_SESSION_V1_H

#include "bx_ntvdm_xms_dpmi_plane_v1.h"

#define BX_NTVDM_DPMI_PACKAGE_SESSION_V1_MAGIC 0x42584450u
#define BX_NTVDM_DPMI_PACKAGE_SESSION_V1_VERSION 1u

typedef struct bx_ntvdm_dpmi_package_session_v1 {
  uint32_t magic, abi_version, struct_bytes, initialized;
} bx_ntvdm_dpmi_package_session_v1;

#ifdef __cplusplus
extern "C" {
#endif
int bx_ntvdm_dpmi_package_session_v1_initialize(
  bx_ntvdm_dpmi_package_session_v1 *session);
int bx_ntvdm_dpmi_package_session_v1_valid(
  const bx_ntvdm_dpmi_package_session_v1 *session);
int bx_ntvdm_dpmi_package_session_v1_dispatch(
  bx_ntvdm_dpmi_package_session_v1 *session,
  const bx_ntvdm_bop_ingress_v1 *ingress,
  const bx_ntvdm_bop_provider_selection_v1 *selection,
  const bx_ntvdm_exception_event_v1 *event,
  const bx_ntvdm_cpu_state_v1 *cpu,
  const bx_ntvdm_instruction_window_v1 *window,
  bx_ntvdm_cpu_result_v2 *result);
#ifdef __cplusplus
}
#endif

#endif
