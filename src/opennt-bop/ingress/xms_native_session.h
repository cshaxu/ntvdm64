#ifndef RUNTIME_BOP_SHIM_XMS_NATIVE_SESSION_SHIM_H
#define RUNTIME_BOP_SHIM_XMS_NATIVE_SESSION_SHIM_H

#include "adapter-bop/generic_ud_bridge.h"

typedef struct runtime_xms_native_session {
    uint32_t magic, abi_version, struct_bytes, bound;
} runtime_xms_native_session;

#define RUNTIME_XMS_NATIVE_SESSION_MAGIC 0x42584d53u
#define RUNTIME_XMS_NATIVE_SESSION_VERSION 1u

int runtime_xms_native_session_initialize(
    runtime_xms_native_session *session, uint32_t total_memory_kib);
int runtime_xms_native_session_bind(runtime_xms_native_session *session);
void runtime_xms_native_session_unbind(runtime_xms_native_session *session);
int runtime_xms_native_session_dispatch(
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome);

#endif
