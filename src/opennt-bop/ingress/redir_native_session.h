#ifndef RUNTIME_OPENNT_BOP_REDIR_NATIVE_SESSION_H
#define RUNTIME_OPENNT_BOP_REDIR_NATIVE_SESSION_H

/*
 * Session owner for the historical VDMREDIR composition boundary.
 *
 * OpenNT retained the DOS callers and VDMREDIR headers, but not the DLL
 * provider bodies.  This record is therefore deliberately a bx-vdm
 * source-derived seam: native HANDLEs stay private to the Direct session,
 * while selector 57 sees only copied CPU state and checked guest RAM.
 */

#include "opennt-bop/dem/opennt_dem_ccpu_sas_facade.h"
#include "adapter-bop/generic_ud_bridge.h"

#define RUNTIME_REDIR_NATIVE_SESSION_MAGIC 0x42585244u
#define RUNTIME_REDIR_NATIVE_SESSION_VERSION 1u

typedef struct runtime_redir_native_session {
    uint32_t magic, abi_version, struct_bytes, bound;
    runtime_dem_direct_context *direct;
    void *guest_state;
    runtime_demhndl_guest_read_fn guest_read;
    runtime_demhndl_guest_write_fn guest_write;
    uint32_t loaded;
    uint32_t mode;
} runtime_redir_native_session;

int runtime_redir_native_session_initialize(runtime_redir_native_session *session,
    runtime_dem_direct_context *direct, void *guest_state,
    runtime_demhndl_guest_read_fn guest_read,
    runtime_demhndl_guest_write_fn guest_write);
int runtime_redir_native_session_bind(runtime_redir_native_session *session);
void runtime_redir_native_session_unbind(runtime_redir_native_session *session);

int runtime_redir_native_session_dispatch(
    const struct runtime_generic_ud_event_v1 *event,
    struct runtime_generic_ud_outcome_v1 *outcome);

/* Invoke one imported VDMREDIR body through the one established CCPU/SAS
 * mechanical facade.  This is BOP composition only: service meaning remains
 * at the caller and the facade remains selector-blind. */
int runtime_redir_native_session_invoke_scoped_body(
    const struct runtime_generic_ud_event_v1 *event,
    struct runtime_generic_ud_outcome_v1 *outcome, void (*body)(void),
    uint32_t resume_bytes);

#endif
