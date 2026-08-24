#ifndef RUNTIME_SOFTPC_PRINTER_OPENCLOSE_V2_GENERIC_UD_BRIDGE_H
#define RUNTIME_SOFTPC_PRINTER_OPENCLOSE_V2_GENERIC_UD_BRIDGE_H

#include "adapter-bop/generic_ud_bridge.h"

#ifdef __cplusplus
extern "C" {
#endif

int runtime_softpc_printer_openclose_v2_generic_ud_recognizes(
    const struct runtime_generic_ud_event_v1 *event);
int runtime_softpc_printer_openclose_v2_generic_ud_dispatch(
    const struct runtime_generic_ud_event_v1 *event,
    struct runtime_generic_ud_outcome_v1 *outcome);

#ifdef __cplusplus
}
#endif

#endif
