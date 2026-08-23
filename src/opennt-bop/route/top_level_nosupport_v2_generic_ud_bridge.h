#ifndef BX_NTVDM_BOP_TOP_LEVEL_NOSUPPORT_V2_GENERIC_UD_BRIDGE_H
#define BX_NTVDM_BOP_TOP_LEVEL_NOSUPPORT_V2_GENERIC_UD_BRIDGE_H

#include "adapter-softpc/bx_ntvdm_generic_ud_bridge.h"

/* OpenNT source: softpc.new/host/src/nt_bop.c:MS_bop_9.  This is a
 * top-level adapter route; the mantle sees only its opaque generic event. */
int bx_ntvdm_top_level_nosupport_v2_generic_ud_recognizes(
    const struct bx_ntvdm_generic_ud_event_v1 *event);
int bx_ntvdm_top_level_nosupport_v2_generic_ud_dispatch(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome);

#endif
