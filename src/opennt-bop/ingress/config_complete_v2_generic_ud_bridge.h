#ifndef RUNTIME_BOP_CONFIG_COMPLETE_V2_GENERIC_UD_BRIDGE_H
#define RUNTIME_BOP_CONFIG_COMPLETE_V2_GENERIC_UD_BRIDGE_H

#include "adapter-bop/generic_ud_bridge.h"

int runtime_config_complete_v2_generic_ud_recognizes(
    const struct runtime_generic_ud_event_v1 *event);
int runtime_config_complete_v2_generic_ud_dispatch(
    const struct runtime_generic_ud_event_v1 *event,
    struct runtime_generic_ud_outcome_v1 *outcome);

#endif
