#include "dem_runtime.h"
#include "app/bop/dem_host_session.h"
#include "app/bop/dem_session.h"
#include "app/bop/redir_native_session.h"

#include <string.h>

/* Original OpenNT DEM initialization entry. The imported source body keeps
 * directory allocation and command-line ordering; its system-directory call
 * is bound through adapter-win32 before this session is installed. */
BOOL DemInit(int argc, char *argv[]);

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
    if (!DemInit(0, NULL) ||
        !runtime_dem_direct_host_session_initialize(&runtime.host) ||
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
