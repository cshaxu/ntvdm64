#ifndef RUNTIME_BOP_SPCKBD_HANDOFF_GENERIC_UD_BRIDGE_H
#define RUNTIME_BOP_SPCKBD_HANDOFF_GENERIC_UD_BRIDGE_H

#include "adapter-bop/generic_ud_bridge.h"

int runtime_spckbd_handoff_generic_ud_recognizes(
    const struct runtime_generic_ud_event *event);
int runtime_spckbd_handoff_generic_ud_dispatch(
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome);
void runtime_spckbd_handoff_display_state_set(uint8_t state);
void runtime_spckbd_handoff_display_state_reset(void);

#endif
