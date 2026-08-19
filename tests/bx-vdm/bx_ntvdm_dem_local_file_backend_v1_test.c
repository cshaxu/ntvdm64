#include "bop-v1/bx_ntvdm_dem_local_file_backend_v1.h"

#include <wchar.h>
#include <wctype.h>

static int profile_for(uint32_t mode, bx_ntvdm_mutation_profile_v1 *profile)
{
    bx_ntvdm_mutation_profile_v1_initialize(profile, mode);
    return bx_ntvdm_dem_profile_consumer_v1_register_class(profile,
        BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, 0x0fu) &&
        bx_ntvdm_dem_profile_consumer_v1_register_class(profile,
        BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT, 0x0fu);
}

static int open_for(uint32_t mode, const bx_ntvdm_host_namespace_v1 *space,
    const bx_ntvdm_dem_cwd_context_v1 *cwd, const char *path, uint32_t access,
    DWORD creation, uint32_t *token_out, bx_ntvdm_dem_file_session_v1 *session_out)
{
    bx_ntvdm_mutation_profile_v1 profile;
    bx_ntvdm_dem_local_file_backend_v1 backend;
    bx_ntvdm_dem_file_view_v1 view;
    if (!profile_for(mode, &profile) ||
        !bx_ntvdm_dem_file_session_v1_initialize(session_out, &profile) ||
        !bx_ntvdm_dem_file_view_v1_initialize(&view, &profile) ||
        !bx_ntvdm_dem_local_file_backend_v1_initialize(&backend, session_out,
            &view, space, cwd)) return BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_REJECTED;
    return bx_ntvdm_dem_local_file_backend_v1_open(&backend, path, access,
        creation, token_out);
}

int main(void)
{
    wchar_t temporary[MAX_PATH], short_name[MAX_PATH];
    char oem_path[MAX_PATH];
    bx_ntvdm_host_drive_snapshot_v1 snapshot = {0};
    bx_ntvdm_host_namespace_v1 space;
    bx_ntvdm_mutation_profile_v1 cwd_profile;
    bx_ntvdm_mutation_profile_v1 direct_profile;
    bx_ntvdm_dem_cwd_context_v1 cwd;
    bx_ntvdm_dem_local_file_backend_v1 backend;
    bx_ntvdm_dem_file_session_v1 session;
    bx_ntvdm_dem_file_view_v1 direct_view;
    bx_ntvdm_dem_file_view_v1 mismatched_view;
    HANDLE source = INVALID_HANDLE_VALUE, opened = INVALID_HANDLE_VALUE;
    uint8_t drive;
    uint32_t token = 0u, released = 0u;
    DWORD written = 0u, read = 0u;
    char output = 0;
    int failed = 0;

    if (GetTempPathW(MAX_PATH, temporary) == 0u ||
        GetTempFileNameW(temporary, L"n64", 0u, temporary) == 0u ||
        GetShortPathNameW(temporary, short_name, MAX_PATH) == 0u ||
        short_name[1] != L':') return 1;
    source = CreateFileW(temporary, GENERIC_WRITE, 0u, 0, CREATE_ALWAYS,
        FILE_ATTRIBUTE_TEMPORARY, 0);
    if (source == INVALID_HANDLE_VALUE || !WriteFile(source, "x", 1u,
        &written, 0) || written != 1u) {
        if (source != INVALID_HANDLE_VALUE) CloseHandle(source);
        DeleteFileW(temporary);
        return 2;
    }
    CloseHandle(source);
    drive = (uint8_t)(towupper((wint_t)short_name[0]) - L'A');
    snapshot.magic = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_MAGIC;
    snapshot.version = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_VERSION;
    snapshot.present_mask = UINT32_C(1) << drive;
    snapshot.admitted_mask = snapshot.present_mask;
    snapshot.types[drive] = DRIVE_FIXED;
    if (!bx_ntvdm_host_namespace_v1_initialize(&space, &snapshot) ||
        !profile_for(BX_NTVDM_MUTATION_MODE_V1_DIRECT, &cwd_profile) ||
        !bx_ntvdm_dem_cwd_context_v1_initialize(&cwd, &cwd_profile) ||
        WideCharToMultiByte(CP_OEMCP, 0, short_name, -1, oem_path,
            MAX_PATH, 0, 0) == 0) failed = 1;
    if (!failed) {
        if (!profile_for(BX_NTVDM_MUTATION_MODE_V1_DIRECT, &direct_profile) ||
            !bx_ntvdm_dem_file_session_v1_initialize(&session, &direct_profile) ||
            !bx_ntvdm_dem_file_view_v1_initialize(&direct_view, &direct_profile) ||
            !bx_ntvdm_dem_local_file_backend_v1_initialize(&backend, &session,
                &direct_view, &space, &cwd) ||
            bx_ntvdm_dem_local_file_backend_v1_open_ex_owned(&backend, oem_path,
                BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_READ, FILE_SHARE_READ,
                OPEN_EXISTING, 0u, &token, 0) !=
                BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_REJECTED || token != 0u ||
            bx_ntvdm_dem_local_file_backend_v1_open_ex_owned(&backend, oem_path,
                BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_READ, FILE_SHARE_READ,
                OPEN_EXISTING, 0x1234u, &token, 0) !=
                BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_OK || token == 0u ||
            !bx_ntvdm_dem_file_session_v1_release_owner(&session, 0x1234u,
                &released) || released != 1u ||
            bx_ntvdm_dem_file_session_v1_lookup(&session, token, &opened)) failed = 1;
        if (!failed) {
            bx_ntvdm_mutation_profile_v1 readonly_profile;
            if (!profile_for(BX_NTVDM_MUTATION_MODE_V1_READONLY, &readonly_profile) ||
                !bx_ntvdm_dem_file_view_v1_initialize(&mismatched_view, &readonly_profile) ||
                bx_ntvdm_dem_local_file_backend_v1_initialize(&backend, &session,
                    &mismatched_view, &space, &cwd)) failed = 1;
        }
        bx_ntvdm_dem_file_session_v1_teardown(&session);
        token = 0u;
        if (open_for(BX_NTVDM_MUTATION_MODE_V1_DIRECT, &space, &cwd,
                oem_path, BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_READ,
                OPEN_EXISTING, &token, &session) !=
                BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_OK || token == 0u ||
            !bx_ntvdm_dem_file_session_v1_lookup(&session, token, &opened) ||
            !ReadFile(opened, &output, 1u, &read, 0) || read != 1u || output != 'x' ||
            !bx_ntvdm_dem_file_session_v1_release(&session, token)) failed = 1;
        bx_ntvdm_dem_file_session_v1_teardown(&session);
        token = 0u;
        if (open_for(BX_NTVDM_MUTATION_MODE_V1_READONLY, &space, &cwd,
                oem_path, BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_READ,
                OPEN_EXISTING, &token, &session) !=
                BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_OK || token == 0u ||
            !bx_ntvdm_dem_file_session_v1_release(&session, token)) failed = 1;
        bx_ntvdm_dem_file_session_v1_teardown(&session);
        token = 0u;
        if (open_for(BX_NTVDM_MUTATION_MODE_V1_READONLY, &space, &cwd,
                oem_path, BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_WRITE,
                OPEN_EXISTING, &token, &session) !=
                BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_READONLY || token != 0u) failed = 1;
        bx_ntvdm_dem_file_session_v1_teardown(&session);
        token = 0u;
        if (open_for(BX_NTVDM_MUTATION_MODE_V1_OVERLAY, &space, &cwd,
                oem_path, BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_WRITE,
                OPEN_EXISTING, &token, &session) !=
                BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_OVERLAY || token != 0u) failed = 1;
        bx_ntvdm_dem_file_session_v1_teardown(&session);
        token = 0u;
        if (open_for(BX_NTVDM_MUTATION_MODE_V1_VIRTUAL, &space, &cwd,
                oem_path, BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_READ,
                OPEN_EXISTING, &token, &session) !=
                BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_VIRTUAL || token != 0u) failed = 1;
        bx_ntvdm_dem_file_session_v1_teardown(&session);
        token = 0u;
        if (open_for(BX_NTVDM_MUTATION_MODE_V1_DIRECT, &space, &cwd,
                "C:\\\\..\\\\X.TXT", BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_READ,
                OPEN_EXISTING, &token, &session) !=
                BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_REJECTED || token != 0u) failed = 1;
        bx_ntvdm_dem_file_session_v1_teardown(&session);
    }
    bx_ntvdm_host_namespace_v1_release(&space);
    DeleteFileW(temporary);
    return failed ? 3 : 0;
}
