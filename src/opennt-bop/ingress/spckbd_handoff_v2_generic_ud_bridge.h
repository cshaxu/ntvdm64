#ifndef RUNTIME_BOP_SPCKBD_HANDOFF_V2_GENERIC_UD_BRIDGE_H
#define RUNTIME_BOP_SPCKBD_HANDOFF_V2_GENERIC_UD_BRIDGE_H

#include "adapter-bop/generic_ud_bridge.h"

int runtime_spckbd_handoff_v2_generic_ud_recognizes(
    const struct runtime_generic_ud_event_v1 *event);
int runtime_spckbd_handoff_v2_generic_ud_dispatch(
    const struct runtime_generic_ud_event_v1 *event,
    struct runtime_generic_ud_outcome_v1 *outcome);
void runtime_spckbd_handoff_v2_display_state_set(uint8_t state);
void runtime_spckbd_handoff_v2_display_state_reset(void);

#endif
