#include "dem_v2_runtime_session.h"
#include "dem_v2_startup_composition.h"

#include "shim/dem_direct_host_session.h"
#include "shim/demdasd_ioctl_shim.h"
#include "shim/dem_native_session_shim.h"
#include "shim/redir_session_shim.h"

#include <string.h>

typedef struct bx_ntvdm_dem_v2_runtime_session {
    bx_ntvdm_dem_direct_host_session host;
    bx_ntvdm_dem_native_session native;
    bx_ntvdm_redir_native_session redir;
    int bound;
} bx_ntvdm_dem_v2_runtime_session;

static bx_ntvdm_dem_v2_runtime_session runtime;

void bx_ntvdm_dem_v2_runtime_session_reset(void)
{
    if (runtime.bound) {
        bx_ntvdm_redir_native_session_unbind(&runtime.redir);
        bx_ntvdm_dem_native_session_unbind(&runtime.native);
    }
    bx_ntvdm_dem_direct_host_session_reset(&runtime.host);
    bx_ntvdm_demdasd_drive_policy_reset();
    memset(&runtime, 0, sizeof(runtime));
}

int bx_ntvdm_dem_v2_runtime_session_bind(void)
{
    const bx_ntvdm_host_drive_snapshot_v1 *drive_snapshot =
        bx_ntvdm_dem_v2_startup_drive_snapshot();
    bx_ntvdm_dem_v2_runtime_session_reset();
    if (drive_snapshot == NULL ||
        !bx_ntvdm_demdasd_drive_policy_bind(drive_snapshot) ||
        !bx_ntvdm_dem_direct_host_session_initialize(&runtime.host) ||
        !bx_ntvdm_dem_native_session_initialize(&runtime.native,
            bx_ntvdm_dem_direct_host_session_context(&runtime.host),
            &runtime.host, bx_ntvdm_dem_direct_host_session_guest_read,
            bx_ntvdm_dem_direct_host_session_guest_write) ||
        !bx_ntvdm_dem_native_session_bind(&runtime.native) ||
        !bx_ntvdm_redir_native_session_initialize(&runtime.redir,
            bx_ntvdm_dem_direct_host_session_context(&runtime.host),
            &runtime.host, bx_ntvdm_dem_direct_host_session_guest_read,
            bx_ntvdm_dem_direct_host_session_guest_write) ||
        !bx_ntvdm_redir_native_session_bind(&runtime.redir)) {
        bx_ntvdm_dem_v2_runtime_session_reset();
        return 0;
    }
    runtime.bound = 1;
    return 1;
}
