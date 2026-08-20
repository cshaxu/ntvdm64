#include "command_v2_runtime_session.h"

#include "dem_v2_startup_composition.h"
#include "shim/command_native_session_shim.h"

#include <string.h>

static bx_ntvdm_command_native_session runtime;

void bx_ntvdm_command_v2_runtime_session_reset(void)
{
    if (runtime.bound != 0u) bx_ntvdm_command_native_session_unbind(&runtime);
    memset(&runtime, 0, sizeof(runtime));
}

int bx_ntvdm_command_v2_runtime_session_bind(void)
{
    bx_ntvdm_command_v2_runtime_session_reset();
    if (!bx_ntvdm_command_native_session_initialize(&runtime) ||
        !bx_ntvdm_command_native_session_bind(&runtime)) {
        bx_ntvdm_command_v2_runtime_session_reset();
        return 0;
    }
    return 1;
}

int bx_ntvdm_command_v2_runtime_session_bind_from_startup(void)
{
    char application[MAX_PATH + 1u];
    char tail[128u];
    uint16_t drive, code_page;
    bx_ntvdm_command_v2_runtime_session_reset();
    if (!bx_ntvdm_command_native_session_initialize(&runtime) ||
        !bx_ntvdm_dem_v2_startup_copy_command_source(application,
            (uint32_t)sizeof(application), tail, (uint32_t)sizeof(tail),
            &drive, &code_page) ||
        !bx_ntvdm_command_misc_session_set_command_source(&runtime.direct,
            application, tail, drive, code_page) ||
        !bx_ntvdm_command_native_session_bind(&runtime)) {
        bx_ntvdm_command_v2_runtime_session_reset();
        return 0;
    }
    return 1;
}
