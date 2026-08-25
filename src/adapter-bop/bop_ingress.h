#ifndef RUNTIME_BOP_INGRESS_H
#define RUNTIME_BOP_INGRESS_H

/* Selector-blind, in-process composition seam.  Both arguments are copied
 * fixed-width records.  The callback is registered only by app inside the
 * single MSVC /MT process; it is never guest-visible or cross-toolchain. */

#include "adapter-bop/generic_ud_bridge.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*runtime_bop_route)(
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome, void *context);

int runtime_bop_ingress_bind(runtime_bop_route route, void *context);
void runtime_bop_ingress_unbind(void);
int runtime_bop_ingress_bound(void);
int runtime_bop_ingress_opaque_callback(void *context,
    const void *event, unsigned event_bytes, void *outcome,
    unsigned outcome_bytes);

#ifdef __cplusplus
}
#endif

#endif
