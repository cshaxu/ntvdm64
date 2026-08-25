#ifndef RUNTIME_BOP_INGRESS_V1_H
#define RUNTIME_BOP_INGRESS_V1_H

/* Selector-blind, in-process composition seam.  Both arguments are copied
 * fixed-width records.  The callback is registered only by app inside the
 * single MSVC /MT process; it is never guest-visible or cross-toolchain. */

#include "adapter-bop/generic_ud_bridge.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*runtime_bop_route_v1)(
    const struct runtime_generic_ud_event_v1 *event,
    struct runtime_generic_ud_outcome_v1 *outcome, void *context);

int runtime_bop_ingress_v1_bind(runtime_bop_route_v1 route, void *context);
void runtime_bop_ingress_v1_unbind(void);
int runtime_bop_ingress_v1_bound(void);
int runtime_bop_ingress_v1_opaque_callback(void *context,
    const void *event, unsigned event_bytes, void *outcome,
    unsigned outcome_bytes);

#ifdef __cplusplus
}
#endif

#endif
