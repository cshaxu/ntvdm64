#include "command_v2_runtime_session.h"

#include "dem_v2_startup_composition.h"
#include "shim/command_pif_shim.h"
#include "shim/command_native_session_shim.h"

#include <string.h>

static bx_ntvdm_command_native_session runtime;

static int configure_startup_config_inputs(const char *application)
{
    char config[MAX_PATH + 13u] = {0};
    char autoexec[MAX_PATH + 13u] = {0};
    char root[MAX_PATH] = {0};
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
    root_bytes = GetEnvironmentVariableA("NTVDM_CONFIG_ROOT", root, (DWORD)sizeof(root));
    if (root_bytes == 0u || root_bytes >= sizeof(root) ||
        (size_t)root_bytes + strlen(application) + 2u > sizeof(pif_path)) return 0;
    strcpy_s(pif_path, sizeof(pif_path), root);
    if (pif_path[strlen(pif_path) - 1u] != '\\') strcat_s(pif_path, sizeof(pif_path), "\\");
    strcat_s(pif_path, sizeof(pif_path), application);
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
    uint16_t drive, code_page;
    bx_ntvdm_command_v2_runtime_session_reset();
    if (!bx_ntvdm_command_native_session_initialize(&runtime) ||
        !bx_ntvdm_dem_v2_startup_copy_command_source(application,
            (uint32_t)sizeof(application), tail, (uint32_t)sizeof(tail),
            &drive, &code_page) ||
        !bx_ntvdm_command_misc_session_set_command_source(&runtime.direct,
            application, tail, drive, code_page) ||
        !configure_startup_config_inputs(application) ||
        !bx_ntvdm_command_native_session_bind(&runtime)) {
        bx_ntvdm_command_v2_runtime_session_reset();
        return 0;
    }
    return 1;
}
