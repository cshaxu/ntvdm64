#ifndef BX_NTVDM_OPENNT_BOP_REDIR_NATIVE_SESSION_H
#define BX_NTVDM_OPENNT_BOP_REDIR_NATIVE_SESSION_H

/*
 * Session owner for the historical VDMREDIR composition boundary.
 *
 * OpenNT retained the DOS callers and VDMREDIR headers, but not the DLL
 * provider bodies.  This record is therefore deliberately a bx-vdm
 * source-derived seam: native HANDLEs stay private to the Direct session,
 * while selector 57 sees only copied CPU state and checked guest RAM.
 */

#include "adapter-softpc/opennt_dem_ccpu_sas_facade.h"
#include "adapter-bop/bx_ntvdm_generic_ud_bridge.h"

#define BX_NTVDM_REDIR_NATIVE_SESSION_MAGIC 0x42585244u
#define BX_NTVDM_REDIR_NATIVE_SESSION_VERSION 1u

typedef struct bx_ntvdm_redir_native_session {
    uint32_t magic, abi_version, struct_bytes, bound;
    bx_ntvdm_dem_direct_context *direct;
    void *guest_state;
    bx_ntvdm_demhndl_guest_read_fn guest_read;
    bx_ntvdm_demhndl_guest_write_fn guest_write;
    uint32_t loaded;
    uint32_t mode;
} bx_ntvdm_redir_native_session;

int bx_ntvdm_redir_native_session_initialize(bx_ntvdm_redir_native_session *session,
    bx_ntvdm_dem_direct_context *direct, void *guest_state,
    bx_ntvdm_demhndl_guest_read_fn guest_read,
    bx_ntvdm_demhndl_guest_write_fn guest_write);
int bx_ntvdm_redir_native_session_bind(bx_ntvdm_redir_native_session *session);
void bx_ntvdm_redir_native_session_unbind(bx_ntvdm_redir_native_session *session);

int bx_ntvdm_redir_native_session_dispatch(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome);

/* Invoke one imported VDMREDIR body through the one established CCPU/SAS
 * mechanical facade.  This is BOP composition only: service meaning remains
 * at the caller and the facade remains selector-blind. */
int bx_ntvdm_redir_native_session_invoke_scoped_body(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome, void (*body)(void),
    uint32_t resume_bytes);

/* Invoke one imported VDMREDIR body through the one established CCPU/SAS
 * mechanical facade.  This is BOP composition only: service meaning remains
 * at the caller and the facade remains selector-blind. */
int bx_ntvdm_redir_native_session_invoke_scoped_body(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome, void (*body)(void),
    uint32_t resume_bytes);

#endif
