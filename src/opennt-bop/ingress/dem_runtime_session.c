#include "dem_runtime_session.h"
#include "opennt-bop/ingress/dem_direct_session.h"
#include "opennt-bop/dem/opennt_demdasd_ioctl_compat.h"
#include "opennt-bop/ingress/dem_native_session.h"
#include "opennt-bop/ingress/redir_native_session.h"

#include <string.h>

typedef struct runtime_dem_runtime_session {
    runtime_dem_direct_host_session host;
    runtime_dem_native_session native;
    runtime_redir_native_session redir;
    int bound;
} runtime_dem_runtime_session;

static runtime_dem_runtime_session runtime;

void runtime_dem_runtime_session_reset(void)
{
    if (runtime.bound) {
        runtime_redir_native_session_unbind(&runtime.redir);
        runtime_dem_native_session_unbind(&runtime.native);
    }
    runtime_dem_direct_host_session_reset(&runtime.host);
    memset(&runtime, 0, sizeof(runtime));
}

int runtime_dem_runtime_session_bind(void)
{
    runtime_dem_runtime_session_reset();
    if (!runtime_dem_direct_host_session_initialize(&runtime.host) ||
        !runtime_dem_native_session_initialize(&runtime.native,
            runtime_dem_direct_host_session_context(&runtime.host),
            &runtime.host, runtime_dem_direct_host_session_guest_read,
            runtime_dem_direct_host_session_guest_write) ||
        !runtime_dem_native_session_bind(&runtime.native) ||
        !runtime_redir_native_session_initialize(&runtime.redir,
            runtime_dem_direct_host_session_context(&runtime.host),
            &runtime.host, runtime_dem_direct_host_session_guest_read,
            runtime_dem_direct_host_session_guest_write) ||
        !runtime_redir_native_session_bind(&runtime.redir)) {
        runtime_dem_runtime_session_reset();
        return 0;
    }
    runtime.bound = 1;
    return 1;
}
