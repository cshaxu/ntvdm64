#include "command_v2_runtime_session.h"

#include "dem_v2_startup_composition.h"
#include "opennt-bop/command/opennt_pif_composition.h"
#include "opennt-bop/ingress/command_native_session.h"

#include <string.h>

static bx_ntvdm_command_native_session runtime;

static int configure_startup_config_inputs(const char *application)
{
    char config[MAX_PATH + 13u] = {0};
    char autoexec[MAX_PATH + 13u] = {0};
    char pif_path[MAX_PATH + 1u];
    const char *extension;
    DWORD config_bytes, autoexec_bytes, root_bytes;

    config_bytes = GetEnvironmentVariableA("NTVDM_CONFIG_SOURCE", config,
        (DWORD)sizeof(config));
    autoexec_bytes = GetEnvironmentVariableA("NTVDM_AUTOEXEC_SOURCE", autoexec,
        (DWORD)sizeof(autoexec));
    if (config_bytes >= sizeof(config) || autoexec_bytes >= sizeof(autoexec)) return 0;
    extension = application == NULL ? NULL : strrchr(application, '.');
    if (extension == NULL || _stricmp(extension, ".PIF") != 0) {
        bx_ntvdm_command_config_set_inputs(&runtime.direct, config, autoexec);
        return 1;
    }
    root_bytes = GetEnvironmentVariableA("NTVDM_TARGET_PATH", pif_path, (DWORD)sizeof(pif_path));
    if (root_bytes == 0u || root_bytes >= sizeof(pif_path)) return 0;
    if (!bx_ntvdm_command_pif_select_config_files(pif_path, config, autoexec)) return 0;
    bx_ntvdm_command_config_set_inputs(&runtime.direct, config, autoexec);
    return 1;
}

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
    char bootstrap_command[64u];
    uint16_t drive, code_page;
    bx_ntvdm_command_v2_runtime_session_reset();
    if (!bx_ntvdm_command_native_session_initialize(&runtime) ||
        !bx_ntvdm_dem_v2_startup_copy_command_source(application,
            (uint32_t)sizeof(application), tail, (uint32_t)sizeof(tail),
            &drive, &code_page) ||
        !bx_ntvdm_command_misc_session_set_command_source(&runtime.direct,
            application, tail, drive, code_page) ||
        !bx_ntvdm_dem_v2_startup_copy_bootstrap_command(bootstrap_command,
            (uint32_t)sizeof(bootstrap_command)) ||
        !bx_ntvdm_command_config_set_bootstrap_command(&runtime.direct,
            bootstrap_command) ||
        !configure_startup_config_inputs(application) ||
        !bx_ntvdm_command_native_session_bind(&runtime)) {
        bx_ntvdm_command_v2_runtime_session_reset();
        return 0;
    }
    return 1;
}
