#include "bx_ntvdm_dem_file_session_v1.h"

#include <windows.h>

static int profile(bx_ntvdm_mutation_profile_v1 *value)
{
    bx_ntvdm_mutation_profile_v1_initialize(value, BX_NTVDM_MUTATION_MODE_V1_DIRECT);
    return bx_ntvdm_dem_profile_consumer_v1_register_class(value,
        BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT, 1u <<
        (BX_NTVDM_MUTATION_MODE_V1_DIRECT - 1u)) &&
        bx_ntvdm_dem_profile_consumer_v1_register_class(value,
            BX_NTVDM_MUTATION_CLASS_V1_FILE_METADATA, 1u <<
            (BX_NTVDM_MUTATION_MODE_V1_DIRECT - 1u));
}

int main(void)
{
    bx_ntvdm_mutation_profile_v1 mutation;
    bx_ntvdm_dem_file_session_v1 session;
    wchar_t path[MAX_PATH];
    HANDLE file, looked_up;
    uint32_t first, second, third, released;
    if (!profile(&mutation) || !bx_ntvdm_dem_file_session_v1_initialize(&session, &mutation) ||
        GetTempPathW(MAX_PATH, path) == 0u ||
        GetTempFileNameW(path, L"nd6", 0u, path) == 0u) return 1;
    file = CreateFileW(path, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING,
        FILE_ATTRIBUTE_TEMPORARY, 0);
    if (file == INVALID_HANDLE_VALUE ||
        !bx_ntvdm_dem_file_session_v1_adopt(&session, file, &first) ||
        first == 0u || first == (uint32_t)(uintptr_t)file ||
        !bx_ntvdm_dem_file_session_v1_lookup(&session, first, &looked_up) ||
        looked_up != file || !bx_ntvdm_dem_file_session_v1_release(&session, first) ||
        bx_ntvdm_dem_file_session_v1_lookup(&session, first, &looked_up)) {
        bx_ntvdm_dem_file_session_v1_teardown(&session);
        DeleteFileW(path);
        return 2;
    }
    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,
        FILE_ATTRIBUTE_TEMPORARY, 0);
    if (file == INVALID_HANDLE_VALUE ||
        !bx_ntvdm_dem_file_session_v1_adopt_owned(&session, file, 0x1234u,
            &second) ||
        second == first || !bx_ntvdm_dem_file_session_v1_valid(&session)) {
        bx_ntvdm_dem_file_session_v1_teardown(&session);
        DeleteFileW(path);
        return 3;
    }
    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,
        FILE_ATTRIBUTE_TEMPORARY, 0);
    if (file == INVALID_HANDLE_VALUE ||
        !bx_ntvdm_dem_file_session_v1_adopt_owned(&session, file, 0x4321u,
            &third) ||
        !bx_ntvdm_dem_file_session_v1_release_owner(&session, 0x1234u,
            &released) || released != 1u ||
        bx_ntvdm_dem_file_session_v1_lookup(&session, second, &looked_up) ||
        !bx_ntvdm_dem_file_session_v1_lookup(&session, third, &looked_up) ||
        !bx_ntvdm_dem_file_session_v1_release(&session, third) ||
        bx_ntvdm_dem_file_session_v1_release_owner(&session, 0u, &released)) {
        bx_ntvdm_dem_file_session_v1_teardown(&session);
        DeleteFileW(path);
        return 4;
    }
    bx_ntvdm_dem_file_session_v1_teardown(&session);
    DeleteFileW(path);
    return bx_ntvdm_dem_file_session_v1_valid(&session) ? 5 : 0;
}
