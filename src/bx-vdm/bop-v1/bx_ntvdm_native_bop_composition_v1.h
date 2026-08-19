#ifndef BX_NTVDM_NATIVE_BOP_COMPOSITION_V1_H
#define BX_NTVDM_NATIVE_BOP_COMPOSITION_V1_H

#include "bx_ntvdm_dpmi_package_session_v1.h"
#include "bx_ntvdm_xms_package_session_v1.h"
#include "bx_ntvdm_generic_ud_bridge.h"

#define BX_NTVDM_NATIVE_BOP_COMPOSITION_V1_MAGIC 0x42584e42u
#define BX_NTVDM_NATIVE_BOP_COMPOSITION_V1_VERSION 1u

/* bx-vdm owns only BOP routing/session state. bx-mantle owns the native
 * machine lifetime and exposes C1/C2 through their fixed C records. */
typedef struct bx_ntvdm_native_bop_composition_v1 {
  uint32_t magic, abi_version, struct_bytes, bound;
  bx_ntvdm_xms_package_session_v1 xms;
  bx_ntvdm_dpmi_package_session_v1 dpmi;
} bx_ntvdm_native_bop_composition_v1;

#ifdef __cplusplus
extern "C" {
#endif
int bx_ntvdm_native_bop_composition_v1_initialize(
  bx_ntvdm_native_bop_composition_v1 *composition);
int bx_ntvdm_native_bop_composition_v1_bind(
  bx_ntvdm_native_bop_composition_v1 *composition);
void bx_ntvdm_native_bop_composition_v1_unbind(
  bx_ntvdm_native_bop_composition_v1 *composition);
int bx_ntvdm_native_bop_composition_v1_handle(
  const struct bx_ntvdm_generic_ud_event_v1 *event,
  struct bx_ntvdm_generic_ud_outcome_v1 *outcome);
#ifdef __cplusplus
}
#endif

#endif
