#ifndef RUNTIME_BOP_SHIM_DEM_NATIVE_SESSION_SHIM_H
#define RUNTIME_BOP_SHIM_DEM_NATIVE_SESSION_SHIM_H

/* Adapter-owned session bridge for the direct OpenNT DEM dispatcher.  It
 * consumes only the machine's copied generic-UD record; native memory access
 * is supplied as checked callbacks by the composition owner. */

#include "adapter-softpc/dem/ccpu_sas_facade.h"
#include "adapter-bop/generic_ud_bridge.h"

#define RUNTIME_DEM_NATIVE_SESSION_MAGIC 0x4258444eu
#define RUNTIME_DEM_NATIVE_SESSION_VERSION 1u

typedef struct runtime_dem_native_session {
    uint32_t magic, abi_version, struct_bytes, bound;
    runtime_dem_direct_context *direct;
    void *guest_state;
    runtime_demhndl_guest_read_fn guest_read;
    runtime_demhndl_guest_write_fn guest_write;
} runtime_dem_native_session;

int runtime_dem_native_session_initialize(runtime_dem_native_session *session,
    runtime_dem_direct_context *direct, void *guest_state,
    runtime_demhndl_guest_read_fn guest_read,
    runtime_demhndl_guest_write_fn guest_write);
int runtime_dem_native_session_bind(runtime_dem_native_session *session);
void runtime_dem_native_session_unbind(runtime_dem_native_session *session);

/* Converts a copied generic #UD event into the direct DEM call ABI.  It
 * declines every non-DEM BOP and never installs a global machine bridge. */
int runtime_dem_native_session_dispatch(
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome);

/* A source-owner bridge may use this only after it has recognized its own
 * non-DEM BOP.  This helper contributes no selector or service meaning: it
 * scopes a copied CPU state around one imported source body and returns its
 * typed three- or four-byte resume result. */
int runtime_dem_native_session_invoke_scoped_body(
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome, void (*body)(void),
    uint32_t resume_bytes);

#endif
