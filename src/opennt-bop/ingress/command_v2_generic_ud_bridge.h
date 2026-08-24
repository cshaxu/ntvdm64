#ifndef BX_NTVDM_BOP_COMMAND_V2_GENERIC_UD_BRIDGE_H
#define BX_NTVDM_BOP_COMMAND_V2_GENERIC_UD_BRIDGE_H

#include "adapter-bop/bx_ntvdm_generic_ud_bridge.h"

int bx_ntvdm_command_v2_generic_ud_recognizes(
    const struct bx_ntvdm_generic_ud_event_v1 *event);
int bx_ntvdm_command_v2_generic_ud_dispatch(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome);

#endif
