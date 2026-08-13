#ifndef BX_NTVDM_VDM_GENERIC_UD_BRIDGE_V1_H
#define BX_NTVDM_VDM_GENERIC_UD_BRIDGE_V1_H

#include "bx_ntvdm_bop_provider_registry_v1.h"
#include "bx_ntvdm_generic_ud_bridge.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Convert one copied generic machine event into the existing global ingress
 * and provider-selection records.  This is classification only: it neither
 * invokes a provider nor requests guest memory. */
int bx_ntvdm_vdm_generic_ud_classify_v1(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    bx_ntvdm_bop_ingress_v1 *ingress,
    bx_ntvdm_bop_provider_selection_v1 *selection);

#ifdef __cplusplus
}
#endif

#endif
