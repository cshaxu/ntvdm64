#include "command_v2_runtime_session.h"

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
