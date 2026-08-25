/////////////////////////////////////////////////////////////////////////
// Private bochs-core-overlay implementation boundary.  No non-core
// component may include this header.
/////////////////////////////////////////////////////////////////////////

#ifndef BOCHS_CORE_OVERLAY_CPU_OPAQUE_CALLBACK_PRIVATE_H
#define BOCHS_CORE_OVERLAY_CPU_OPAQUE_CALLBACK_PRIVATE_H

typedef int (*bochs_core_overlay_opaque_callback_t)(void *context,
  const void *event, unsigned event_bytes, void *outcome, unsigned outcome_bytes);


int bochs_core_overlay_opaque_callback_v1_bind(
  bochs_core_overlay_opaque_callback_t callback, void *context);
void bochs_core_overlay_opaque_callback_v1_unbind(void);
int bochs_core_overlay_opaque_callback_v1_invoke(
  const void *event, unsigned event_bytes, void *outcome,
  unsigned outcome_bytes);

#endif
