/////////////////////////////////////////////////////////////////////////
// DIVERGENCE(BX-UD-002): private state for the selector-blind callback
// registration.  bochs-core retains only two forwarding calls.
/////////////////////////////////////////////////////////////////////////

#include "bochs-core-overlay/cpu/opaque_callback_private.h"

static bochs_core_overlay_opaque_callback_t callback_slot;
static void *callback_context;

int bochs_core_overlay_opaque_callback_bind(
  bochs_core_overlay_opaque_callback_t callback, void *context)
{
  if (callback == 0 || callback_slot != 0) return 0;
  callback_slot = callback;
  callback_context = context;
  return 1;
}

void bochs_core_overlay_opaque_callback_unbind(void)
{
  callback_slot = 0;
  callback_context = 0;
}

int bochs_core_overlay_opaque_callback_invoke(
  const void *event, unsigned event_bytes, void *outcome,
  unsigned outcome_bytes)
{
  if (callback_slot == 0) return 0;
  return callback_slot(callback_context, event, event_bytes, outcome,
    outcome_bytes);
}
