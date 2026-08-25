/////////////////////////////////////////////////////////////////////////
// DIVERGENCE(BX-UD-002): two-call boundary to the private overlay.
/////////////////////////////////////////////////////////////////////////

#include "bochs-core/cpu/opaque_callback.h"
#include "bochs-core-overlay/cpu/opaque_callback_private.h"

int bochs_core_opaque_callback_v1_bind(bochs_core_opaque_callback_v1 callback,
  void *context)
{
  return bochs_core_overlay_opaque_callback_v1_bind(callback, context);
}

void bochs_core_opaque_callback_v1_unbind(void)
{
  bochs_core_overlay_opaque_callback_v1_unbind();
}
