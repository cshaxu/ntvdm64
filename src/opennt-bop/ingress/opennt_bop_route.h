#ifndef OPENNT_BOP_ROUTE_H
#define OPENNT_BOP_ROUTE_H

#include "adapter-bop/bx_ntvdm_bop_ingress_v1.h"

#ifdef __cplusplus
extern "C" {
#endif

/* OpenNT-owned selector/service route.  The context is intentionally ignored:
 * session/provider state remains in the original owner-family composition. */
int bx_ntvdm_opennt_bop_route_dispatch_v1(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome, void *context);

#ifdef __cplusplus
}
#endif

#endif
