#ifndef BOP_ROUTE_H
#define BOP_ROUTE_H

#include "adapter-bop/bop_ingress.h"

#ifdef __cplusplus
extern "C" {
#endif

/* OpenNT-owned selector/service route.  The context is intentionally ignored:
 * session/provider state remains in the original owner-family composition. */
int runtime_opennt_bop_route_dispatch_v1(
    const struct runtime_generic_ud_event_v1 *event,
    struct runtime_generic_ud_outcome_v1 *outcome, void *context);

#ifdef __cplusplus
}
#endif

#endif
