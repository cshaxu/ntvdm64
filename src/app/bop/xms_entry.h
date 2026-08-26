#ifndef RUNTIME_BOP_XMS_GENERIC_UD_BRIDGE_H
#define RUNTIME_BOP_XMS_GENERIC_UD_BRIDGE_H

/* XMS selector recognition belongs to bx-vdm.  The machine calls only the
 * opaque generic-UD entry and neither identifies BOP bytes nor learns XMS. */

#include "adapter-bop/generic_ud_bridge.h"

int runtime_xms_generic_ud_recognizes(
    const struct runtime_generic_ud_event *event);

int runtime_xms_generic_ud_dispatch(
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome);

#endif
