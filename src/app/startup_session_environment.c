#include "startup_session_environment.h"

#include <windows.h>

#include "startup_session.h"

#define BX_NTVDM_STARTUP_SESSION_ENV_PROFILE L"NTDOS64_ADAPTER_PROFILE"
#define BX_NTVDM_STARTUP_SESSION_ENV_ROOT L"NTDOS64_ADAPTER_ROOT"

static bx_ntvdm_startup_session_v1 bx_ntvdm_startup_environment_session;
static int bx_ntvdm_startup_environment_attempted;
static int bx_ntvdm_startup_environment_installed;

int bx_ntvdm_startup_session_v1_install_from_environment(
    bx_ntvdm_startup_session_v1 *session)
{
    wchar_t profile[MAX_PATH];
    wchar_t root[MAX_PATH];
    DWORD profile_size;
    DWORD root_size;
    if (session == 0) return -1;
    profile_size = GetEnvironmentVariableW(BX_NTVDM_STARTUP_SESSION_ENV_PROFILE,
        profile, MAX_PATH);
    root_size = GetEnvironmentVariableW(BX_NTVDM_STARTUP_SESSION_ENV_ROOT,
        root, MAX_PATH);
    if (profile_size == 0u && root_size == 0u) return 0;
    if (profile_size == 0u || root_size == 0u || profile_size >= MAX_PATH ||
        root_size >= MAX_PATH) return -1;
    return bx_ntvdm_startup_session_v1_install(session, profile, root) ? 1 : -1;
}

int bx_ntvdm_startup_session_v1_prepare_from_environment(
    bx_ntvdm_startup_plan_v1 *plan, const uint8_t **payload,
    uint64_t *payload_bytes)
{
    int install_result;
    if (bx_ntvdm_startup_environment_installed) {
        return bx_ntvdm_startup_session_v1_prepare_ntio_plan(
            &bx_ntvdm_startup_environment_session, plan, payload, payload_bytes) ? 1 : -1;
    }
    if (bx_ntvdm_startup_environment_attempted) return 0;
    bx_ntvdm_startup_environment_attempted = 1;
    install_result = bx_ntvdm_startup_session_v1_install_from_environment(
        &bx_ntvdm_startup_environment_session);
    if (install_result < 0) return -1;
    if (install_result == 0) return 0;
    bx_ntvdm_startup_environment_installed = 1;
    return bx_ntvdm_startup_session_v1_prepare_ntio_plan(
        &bx_ntvdm_startup_environment_session, plan, payload, payload_bytes) ? 1 : -1;
}

int bx_ntvdm_startup_session_v1_copy_host_drive_inventory_from_environment(
    uint8_t out_types[26])
{
    if (!bx_ntvdm_startup_environment_installed) return 0;
    return bx_ntvdm_startup_session_v1_copy_host_drive_inventory(
        &bx_ntvdm_startup_environment_session, out_types);
}
