#ifndef RUNTIME_SOFTPC_MOUSE_VECTOR_GENERIC_UD_BRIDGE_H
#define RUNTIME_SOFTPC_MOUSE_VECTOR_GENERIC_UD_BRIDGE_H

#include "adapter-bop/generic_ud_bridge.h"

#ifdef __cplusplus
extern "C" {
#endif

int runtime_softpc_mouse_vector_generic_ud_recognizes(
    const struct runtime_generic_ud_event *event);
int runtime_softpc_mouse_vector_generic_ud_dispatch(
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome);

#ifdef __cplusplus
}
#endif

#endif
