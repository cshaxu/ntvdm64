#include "xms_v2_runtime_session.h"

#include "opennt-host/xms/xms_shim.h"
#include "opennt-host/xms/xms_native_session_shim.h"

#include <string.h>

static bx_ntvdm_xms_native_session runtime;

void bx_ntvdm_xms_v2_runtime_session_reset(void)
{
    if (runtime.bound != 0u) bx_ntvdm_xms_native_session_unbind(&runtime);
    bx_ntvdm_xms_clear_himem_a20_state();
    memset(&runtime, 0, sizeof(runtime));
}

int bx_ntvdm_xms_v2_runtime_session_bind(uint32_t xms_memory_kib)
{
    bx_ntvdm_xms_v2_runtime_session_reset();
    if (!bx_ntvdm_xms_native_session_initialize(&runtime, xms_memory_kib) ||
        !bx_ntvdm_xms_native_session_bind(&runtime)) {
        bx_ntvdm_xms_v2_runtime_session_reset();
        return 0;
    }
    return 1;
}
