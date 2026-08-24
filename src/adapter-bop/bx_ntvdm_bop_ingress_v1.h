#ifndef BX_NTVDM_BOP_INGRESS_V1_H
#define BX_NTVDM_BOP_INGRESS_V1_H

/* Selector-blind, in-process composition seam.  Both arguments are copied
 * fixed-width records.  The callback is registered only by app inside the
 * single MSVC /MT process; it is never guest-visible or cross-toolchain. */

#include "adapter-bop/bx_ntvdm_generic_ud_bridge.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*bx_ntvdm_bop_route_v1)(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome, void *context);

int bx_ntvdm_bop_ingress_v1_bind(bx_ntvdm_bop_route_v1 route, void *context);
void bx_ntvdm_bop_ingress_v1_unbind(void);
int bx_ntvdm_bop_ingress_v1_bound(void);

#ifdef __cplusplus
}
#endif

#endif
