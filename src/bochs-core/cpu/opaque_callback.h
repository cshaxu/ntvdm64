/////////////////////////////////////////////////////////////////////////
// DIVERGENCE(BX-UD-002): selector-blind, fixed-width CPU callback slot.
/////////////////////////////////////////////////////////////////////////

#ifndef BOCHS_CORE_CPU_OPAQUE_CALLBACK_H
#define BOCHS_CORE_CPU_OPAQUE_CALLBACK_H

typedef int (*bochs_core_opaque_callback_v1)(void *context,
  const void *event, unsigned event_bytes, void *outcome, unsigned outcome_bytes);

int bochs_core_opaque_callback_v1_bind(bochs_core_opaque_callback_v1 callback,
  void *context);
void bochs_core_opaque_callback_v1_unbind(void);

#endif
