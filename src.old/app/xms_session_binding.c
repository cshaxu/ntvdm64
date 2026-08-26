#include "xms_session_binding.h"

#include "opennt-bop/xms/opennt_xms_compat.h"
#include "app/bop/xms_session.h"

#include <string.h>

static runtime_xms_native_session runtime;

void app_xms_session_reset(void)
{
    if (runtime.bound != 0u) runtime_xms_native_session_unbind(&runtime);
    runtime_xms_clear_himem_a20_state();
    memset(&runtime, 0, sizeof(runtime));
}

int app_xms_session_bind(uint32_t xms_memory_kib)
{
    app_xms_session_reset();
    if (!runtime_xms_native_session_initialize(&runtime, xms_memory_kib) ||
        !runtime_xms_native_session_bind(&runtime)) {
        app_xms_session_reset();
        return 0;
    }
    return 1;
}
