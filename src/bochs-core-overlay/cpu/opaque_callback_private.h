/////////////////////////////////////////////////////////////////////////
// Private bochs-core-overlay implementation boundary.  No non-core
// component may include this header.
/////////////////////////////////////////////////////////////////////////

#ifndef BOCHS_CORE_OVERLAY_CPU_OPAQUE_CALLBACK_PRIVATE_H
#define BOCHS_CORE_OVERLAY_CPU_OPAQUE_CALLBACK_PRIVATE_H

#include "bochs-core/cpu/opaque_callback.h"

int bochs_core_overlay_opaque_callback_v1_bind(
  bochs_core_opaque_callback_v1 callback, void *context);
void bochs_core_overlay_opaque_callback_v1_unbind(void);
int bochs_core_overlay_opaque_callback_v1_invoke(
  const void *event, unsigned event_bytes, void *outcome,
  unsigned outcome_bytes);

#endif
