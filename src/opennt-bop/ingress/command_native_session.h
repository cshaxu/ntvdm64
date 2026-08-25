#ifndef RUNTIME_BOP_SHIM_COMMAND_NATIVE_SESSION_SHIM_H
#define RUNTIME_BOP_SHIM_COMMAND_NATIVE_SESSION_SHIM_H

/* Adapter-owned bridge from the machine's copied generic-UD record to the
 * directly imported OpenNT COMMAND bodies.  This is deliberately parallel to
 * dem_native_session: the machine never receives a selector, a service,
 * or an OpenNT COMMAND type. */

#include "opennt-bop/command/opennt_command_composition.h"
#include "adapter-bop/generic_ud_bridge.h"

#define RUNTIME_COMMAND_NATIVE_SESSION_MAGIC 0x4258434eu
#define RUNTIME_COMMAND_NATIVE_SESSION_VERSION 1u

typedef struct runtime_command_native_session {
    uint32_t magic, abi_version, struct_bytes, bound;
    runtime_command_misc_session direct;
} runtime_command_native_session;

int runtime_command_native_session_initialize(
    runtime_command_native_session *session);
int runtime_command_native_session_bind(
    runtime_command_native_session *session);
void runtime_command_native_session_unbind(
    runtime_command_native_session *session);

int runtime_command_native_session_dispatch(
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome);

#endif
