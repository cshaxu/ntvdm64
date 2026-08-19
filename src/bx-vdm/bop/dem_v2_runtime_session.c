#include "dem_v2_runtime_session.h"

#include "shim/dem_direct_host_session.h"
#include "shim/dem_native_session_shim.h"

#include <string.h>

typedef struct bx_ntvdm_dem_v2_runtime_session {
    bx_ntvdm_dem_direct_host_session host;
    bx_ntvdm_dem_native_session native;
    int bound;
} bx_ntvdm_dem_v2_runtime_session;

static bx_ntvdm_dem_v2_runtime_session runtime;

void bx_ntvdm_dem_v2_runtime_session_reset(void)
{
    if (runtime.bound) bx_ntvdm_dem_native_session_unbind(&runtime.native);
    bx_ntvdm_dem_direct_host_session_reset(&runtime.host);
    memset(&runtime, 0, sizeof(runtime));
}

int bx_ntvdm_dem_v2_runtime_session_bind(void)
{
    bx_ntvdm_dem_v2_runtime_session_reset();
    if (!bx_ntvdm_dem_direct_host_session_initialize(&runtime.host) ||
        !bx_ntvdm_dem_native_session_initialize(&runtime.native,
            bx_ntvdm_dem_direct_host_session_context(&runtime.host),
            &runtime.host, bx_ntvdm_dem_direct_host_session_guest_read,
            bx_ntvdm_dem_direct_host_session_guest_write) ||
        !bx_ntvdm_dem_native_session_bind(&runtime.native)) {
        bx_ntvdm_dem_v2_runtime_session_reset();
        return 0;
    }
    runtime.bound = 1;
    return 1;
}
