#ifndef BX_NTVDM_BOP_SHIM_COMMAND_NATIVE_SESSION_SHIM_H
#define BX_NTVDM_BOP_SHIM_COMMAND_NATIVE_SESSION_SHIM_H

/* Adapter-owned bridge from the mantle's copied generic-UD record to the
 * directly imported OpenNT COMMAND bodies.  This is deliberately parallel to
 * dem_native_session_shim: the mantle never receives a selector, a service,
 * or an OpenNT COMMAND type. */

#include "opennt-host/command/command_misc_shim.h"
#include "adapter-softpc/bx_ntvdm_generic_ud_bridge.h"

#define BX_NTVDM_COMMAND_NATIVE_SESSION_MAGIC 0x4258434eu
#define BX_NTVDM_COMMAND_NATIVE_SESSION_VERSION 1u

typedef struct bx_ntvdm_command_native_session {
    uint32_t magic, abi_version, struct_bytes, bound;
    bx_ntvdm_command_misc_session direct;
} bx_ntvdm_command_native_session;

int bx_ntvdm_command_native_session_initialize(
    bx_ntvdm_command_native_session *session);
int bx_ntvdm_command_native_session_bind(
    bx_ntvdm_command_native_session *session);
void bx_ntvdm_command_native_session_unbind(
    bx_ntvdm_command_native_session *session);

int bx_ntvdm_command_native_session_dispatch(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome);

#endif
