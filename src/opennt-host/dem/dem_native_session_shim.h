#ifndef BX_NTVDM_BOP_SHIM_DEM_NATIVE_SESSION_SHIM_H
#define BX_NTVDM_BOP_SHIM_DEM_NATIVE_SESSION_SHIM_H

/* Adapter-owned session bridge for the direct OpenNT DEM dispatcher.  It
 * consumes only the mantle's copied generic-UD record; native memory access
 * is supplied as checked callbacks by the composition owner. */

#include "opennt-bop/ingress/dem_ingress_shim.h"
#include "adapter-softpc/bx_ntvdm_generic_ud_bridge.h"

#define BX_NTVDM_DEM_NATIVE_SESSION_MAGIC 0x4258444eu
#define BX_NTVDM_DEM_NATIVE_SESSION_VERSION 1u

typedef struct bx_ntvdm_dem_native_session {
    uint32_t magic, abi_version, struct_bytes, bound;
    bx_ntvdm_dem_direct_context *direct;
    void *guest_state;
    bx_ntvdm_demhndl_guest_read_fn guest_read;
    bx_ntvdm_demhndl_guest_write_fn guest_write;
} bx_ntvdm_dem_native_session;

int bx_ntvdm_dem_native_session_initialize(bx_ntvdm_dem_native_session *session,
    bx_ntvdm_dem_direct_context *direct, void *guest_state,
    bx_ntvdm_demhndl_guest_read_fn guest_read,
    bx_ntvdm_demhndl_guest_write_fn guest_write);
int bx_ntvdm_dem_native_session_bind(bx_ntvdm_dem_native_session *session);
void bx_ntvdm_dem_native_session_unbind(bx_ntvdm_dem_native_session *session);

/* Converts a copied generic #UD event into the direct DEM call ABI.  It
 * declines every non-DEM BOP and never installs a global mantle bridge. */
int bx_ntvdm_dem_native_session_dispatch(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome);

/* A source-owner bridge may use this only after it has recognized its own
 * non-DEM BOP.  This helper contributes no selector or service meaning: it
 * scopes a copied CPU state around one imported source body and returns its
 * typed three- or four-byte resume result. */
int bx_ntvdm_dem_native_session_invoke_scoped_body(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome, void (*body)(void),
    uint32_t resume_bytes);

#endif
