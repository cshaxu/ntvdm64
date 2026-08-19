#include "../../src/bx-vdm/bop-v1/bx_ntvdm_dem_virtual_namespace_backend_v1.h"

#include <string.h>

#define CHECK(value, code) do { if (!(value)) return (code); } while (0)

int main(void)
{
    bx_ntvdm_mutation_profile_v1 profile;
    bx_ntvdm_dem_file_session_v1 session;
    bx_ntvdm_dem_overlay_store_v1 store;
    bx_ntvdm_dem_overlay_file_v1 files;
    uint32_t token = 0u, backend = 0u, size = 0u, written = 0u, count = 0u, kind = 0u;
    uint8_t bytes[4]; DWORD error = ERROR_GEN_FAILURE;
    memset(&session, 0, sizeof(session)); memset(&store, 0, sizeof(store)); memset(&files, 0, sizeof(files));
    bx_ntvdm_mutation_profile_v1_initialize(&profile, BX_NTVDM_MUTATION_MODE_V1_VIRTUAL);
    CHECK(bx_ntvdm_dem_file_session_v1_initialize(&session, &profile), 2);
    CHECK(bx_ntvdm_dem_overlay_store_v1_initialize(&store), 3);
    CHECK(bx_ntvdm_dem_overlay_file_v1_initialize(&files, &store), 4);
    CHECK(bx_ntvdm_dem_virtual_namespace_backend_v1_open(&session, &files, 2u,
        L"HELLO.TXT", BX_NTVDM_DEM_OVERLAY_FILE_V1_READ | BX_NTVDM_DEM_OVERLAY_FILE_V1_WRITE,
        3u, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0x1234u, &token, &size, &error), 5);
    CHECK(token != 0u && size == 0u && error == ERROR_SUCCESS, 6);
    CHECK(bx_ntvdm_dem_file_session_v1_token_kind(&session, token, &kind) &&
        kind == BX_NTVDM_DEM_FILE_TOKEN_KIND_V1_VIRTUAL_FILE, 7);
    CHECK(bx_ntvdm_dem_file_session_v1_lookup_backend(&session, token,
        BX_NTVDM_DEM_FILE_TOKEN_KIND_V1_VIRTUAL_FILE, &backend), 8);
    CHECK(bx_ntvdm_dem_overlay_file_v1_write(&files, backend, (const uint8_t *)"ok", 2u, &written) &&
        written == 2u, 9);
    CHECK(bx_ntvdm_dem_overlay_file_v1_seek(&files, backend, 0,
        BX_NTVDM_DEM_OVERLAY_FILE_V1_SEEK_BEGIN, &size), 10);
    CHECK(bx_ntvdm_dem_overlay_file_v1_read(&files, backend, bytes, sizeof(bytes), &count) &&
        count == 2u && memcmp(bytes, "ok", 2u) == 0, 11);
    CHECK(bx_ntvdm_dem_overlay_file_v1_close(&files, backend), 12);
    CHECK(bx_ntvdm_dem_file_session_v1_release_backend(&session, token,
        BX_NTVDM_DEM_FILE_TOKEN_KIND_V1_VIRTUAL_FILE), 13);
    CHECK(!bx_ntvdm_dem_file_session_v1_lookup_backend(&session, token,
        BX_NTVDM_DEM_FILE_TOKEN_KIND_V1_VIRTUAL_FILE, &backend), 14);
    CHECK(bx_ntvdm_dem_virtual_namespace_backend_v1_open(&session, &files, 2u,
        L"HELLO.TXT", BX_NTVDM_DEM_OVERLAY_FILE_V1_READ, 1u, OPEN_EXISTING,
        0u, 0u, &token, &size, &error) && size == 2u, 15);
    CHECK(bx_ntvdm_dem_virtual_namespace_backend_v1_open(&session, &files, 2u,
        L"MISSING.TXT", BX_NTVDM_DEM_OVERLAY_FILE_V1_READ, 1u, OPEN_EXISTING,
        0u, 0u, &backend, &size, &error) && error == ERROR_FILE_NOT_FOUND, 16);
    bx_ntvdm_dem_overlay_file_v1_teardown(&files); bx_ntvdm_dem_overlay_store_v1_teardown(&store);
    bx_ntvdm_dem_file_session_v1_teardown(&session); return 0;
}
