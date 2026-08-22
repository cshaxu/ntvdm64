#ifndef BX_NTVDM_BOP_SHIM_XMS_NATIVE_SESSION_SHIM_H
#define BX_NTVDM_BOP_SHIM_XMS_NATIVE_SESSION_SHIM_H

#include "bx-mantle/bx_ntvdm_generic_ud_bridge.h"

typedef struct bx_ntvdm_xms_native_session {
    uint32_t magic, abi_version, struct_bytes, bound;
} bx_ntvdm_xms_native_session;

#define BX_NTVDM_XMS_NATIVE_SESSION_MAGIC 0x42584d53u
#define BX_NTVDM_XMS_NATIVE_SESSION_VERSION 1u

int bx_ntvdm_xms_native_session_initialize(
    bx_ntvdm_xms_native_session *session, uint32_t total_memory_kib);
int bx_ntvdm_xms_native_session_bind(bx_ntvdm_xms_native_session *session);
void bx_ntvdm_xms_native_session_unbind(bx_ntvdm_xms_native_session *session);
int bx_ntvdm_xms_native_session_dispatch(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome);

#endif
