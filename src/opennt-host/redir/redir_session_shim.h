#ifndef BX_NTVDM_BOP_SHIM_REDIR_SESSION_SHIM_H
#define BX_NTVDM_BOP_SHIM_REDIR_SESSION_SHIM_H

/*
 * Session owner for the historical VDMREDIR composition boundary.
 *
 * OpenNT retained the DOS callers and VDMREDIR headers, but not the DLL
 * provider bodies.  This record is therefore deliberately a bx-vdm
 * source-derived seam: native HANDLEs stay private to the Direct session,
 * while selector 57 sees only copied CPU state and checked guest RAM.
 */

#include "opennt-host/dem/demhndl_shim.h"
#include "adapter-softpc/bx_ntvdm_generic_ud_bridge.h"

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

/* Imported DEM files retain these historical helper names.  They route to
 * this provider; they never establish per-DEM Redirector state. */
BOOL bx_ntvdm_redir_load(void);
LPSTR bx_ntvdm_redir_convert_local_nt_pipe_name(LPSTR existing, LPSTR name);
BOOL bx_ntvdm_redir_add_open_named_pipe_info(HANDLE file, LPSTR name);
BOOL bx_ntvdm_redir_remove_open_named_pipe_info(HANDLE file);
int bx_ntvdm_redir_is_named_pipe_handle(HANDLE file);
int bx_ntvdm_redir_read_named_pipe(HANDLE file, LPVOID buffer, DWORD count,
    DWORD *read_out, DWORD *error_out);
int bx_ntvdm_redir_write_named_pipe(HANDLE file, LPVOID buffer, DWORD count,
    DWORD *written_out);
int bx_ntvdm_redir_loaded(void);

#endif
