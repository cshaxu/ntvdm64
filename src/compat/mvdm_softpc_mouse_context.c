#include "include/host_context.h"
#include "include/mvdm_softpc_mouse_bridge.h"

/* The product binds this B-owned input object for exactly the endpoint's
 * lifetime. Original bound event/CPU threads use the existing host context. */
mvdm_mouse_bridge *mvdm_softpc_mouse_current(void)
{
    compat_host_context *context = compat_host_context_current();
    return context ? context->mouse : NULL;
}
