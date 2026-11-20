#include "include/wow_worker_state.h"
#include "include/host_context.h"

BOOL WINAPI wow_user_worker_active(void)
{
    const compat_host_context *owner = compat_host_context_current();
    return owner != NULL && owner->state == COMPAT_HOST_CONTEXT_ACTIVE;
}
