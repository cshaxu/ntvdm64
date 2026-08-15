#include "bx_ntvdm_dem_cwd_context_v1.h"

#include <stdio.h>
#include <string.h>

static int profile_for(uint32_t mode, bx_ntvdm_mutation_profile_v1 *profile)
{
    bx_ntvdm_mutation_profile_v1_initialize(profile, mode);
    return bx_ntvdm_dem_profile_consumer_v1_register_class(profile,
        BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, 0x0fu);
}

static int non_direct(uint32_t mode, int expected)
{
    bx_ntvdm_mutation_profile_v1 profile;
    bx_ntvdm_dem_cwd_context_v1 context;
    wchar_t out[16];
    if (!profile_for(mode, &profile) ||
        !bx_ntvdm_dem_cwd_context_v1_initialize(&context, &profile) ||
        bx_ntvdm_dem_cwd_context_v1_set(&context, 0, 2u, L"SAMPLE") != expected)
        return 0;
    if (expected == BX_NTVDM_DEM_CWD_V1_OK &&
        (bx_ntvdm_dem_cwd_context_v1_query(&context, 2u, out, 16u) !=
            BX_NTVDM_DEM_CWD_V1_OK || wcscmp(out, L"SAMPLE") != 0)) return 0;
    if (expected == BX_NTVDM_DEM_CWD_V1_OK && mode ==
            BX_NTVDM_MUTATION_MODE_V1_OVERLAY &&
        (bx_ntvdm_dem_cwd_context_v1_set(&context, 0, 2u, L"SECOND") !=
            BX_NTVDM_DEM_CWD_V1_OK ||
         bx_ntvdm_dem_cwd_context_v1_query(&context, 2u, out, 16u) !=
            BX_NTVDM_DEM_CWD_V1_OK || wcscmp(out, L"SECOND") != 0)) return 0;
    return bx_ntvdm_dem_cwd_context_v1_set(&context, 0, 2u, L"..")==
        BX_NTVDM_DEM_CWD_V1_REJECTED;
}

int main(void)
{
    bx_ntvdm_mutation_profile_v1 profile;
    bx_ntvdm_dem_cwd_context_v1 context;
    bx_ntvdm_host_drive_snapshot_v1 snapshot = {0};
    bx_ntvdm_host_namespace_v1 space;
    wchar_t temporary[MAX_PATH], root[MAX_PATH], short_root[MAX_PATH];
    wchar_t child[MAX_PATH], relative[MAX_PATH], out[128];
    uint8_t drive;
    int failed = 0;
    if (GetTempPathW(MAX_PATH, temporary) == 0u ||
        GetTempFileNameW(temporary, L"n64", 0u, root) == 0u ||
        !DeleteFileW(root) || !CreateDirectoryW(root, 0) ||
        GetShortPathNameW(root, short_root, MAX_PATH) == 0u || short_root[1] != L':')
        return 2;
    swprintf(child, MAX_PATH, L"%ls\\SUBDIR", root);
    if (!CreateDirectoryW(child, 0)) failed = 1;
    drive = (uint8_t)(towupper((wint_t)short_root[0]) - L'A');
    snapshot.magic = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_MAGIC;
    snapshot.version = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_VERSION;
    snapshot.present_mask = UINT32_C(1) << drive;
    snapshot.admitted_mask = snapshot.present_mask;
    snapshot.types[drive] = DRIVE_FIXED;
    if (!failed && (!bx_ntvdm_host_namespace_v1_initialize(&space, &snapshot) ||
        !profile_for(BX_NTVDM_MUTATION_MODE_V1_DIRECT, &profile) ||
        !bx_ntvdm_dem_cwd_context_v1_initialize(&context, &profile))) failed = 1;
    if (!failed) {
        const wchar_t *suffix = short_root + 3u;
        swprintf(relative, MAX_PATH, L"%ls\\SUBDIR", suffix);
        failed |= bx_ntvdm_dem_cwd_context_v1_set(&context, &space, drive,
            relative) != BX_NTVDM_DEM_CWD_V1_OK;
        failed |= bx_ntvdm_dem_cwd_context_v1_query(&context, drive, out, 128u) !=
            BX_NTVDM_DEM_CWD_V1_OK || wcscmp(out, relative) != 0;
        failed |= bx_ntvdm_dem_cwd_context_v1_set_default_drive(&context, &space,
            drive) != BX_NTVDM_DEM_CWD_V1_OK;
        failed |= bx_ntvdm_dem_cwd_context_v1_set(&context, &space, drive,
            L"MISSING") != BX_NTVDM_DEM_CWD_V1_NOT_FOUND;
        bx_ntvdm_host_namespace_v1_release(&space);
    }
    RemoveDirectoryW(child); RemoveDirectoryW(root);
    failed |= !non_direct(BX_NTVDM_MUTATION_MODE_V1_READONLY,
        BX_NTVDM_DEM_CWD_V1_READONLY);
    failed |= !non_direct(BX_NTVDM_MUTATION_MODE_V1_OVERLAY,
        BX_NTVDM_DEM_CWD_V1_OK);
    failed |= !non_direct(BX_NTVDM_MUTATION_MODE_V1_VIRTUAL,
        BX_NTVDM_DEM_CWD_V1_OK);
    if (failed) return 1;
    puts("bx-ntvdm-dem-cwd-context-test: shared profile CWD outcomes verified");
    return 0;
}
