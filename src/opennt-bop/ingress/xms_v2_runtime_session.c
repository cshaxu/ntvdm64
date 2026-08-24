#include "xms_v2_runtime_session.h"

#include "opennt-bop/xms/opennt_xms_compat.h"
#include "opennt-bop/ingress/xms_native_session.h"

#include <string.h>

static runtime_xms_native_session runtime;

void runtime_xms_v2_runtime_session_reset(void)
{
    if (runtime.bound != 0u) runtime_xms_native_session_unbind(&runtime);
    runtime_xms_clear_himem_a20_state();
    memset(&runtime, 0, sizeof(runtime));
}

int runtime_xms_v2_runtime_session_bind(uint32_t xms_memory_kib)
{
    runtime_xms_v2_runtime_session_reset();
    if (!runtime_xms_native_session_initialize(&runtime, xms_memory_kib) ||
        !runtime_xms_native_session_bind(&runtime)) {
        runtime_xms_v2_runtime_session_reset();
        return 0;
    }
    return 1;
}
