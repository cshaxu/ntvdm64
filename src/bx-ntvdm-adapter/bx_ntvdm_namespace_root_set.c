#include "bx_ntvdm_namespace_root_set.h"

#include <string.h>

static uint32_t bx_ntvdm_namespace_root_set_bit(uint32_t index)
{
    return UINT32_C(1) << index;
}

void bx_ntvdm_namespace_root_set_v1_release(bx_ntvdm_namespace_root_set_v1 *set)
{
    uint32_t index;
    if (set == 0) return;
    for (index = 0u; index < 26u; ++index) {
        if (set->roots[index] != 0 && set->roots[index] != INVALID_HANDLE_VALUE)
            CloseHandle(set->roots[index]);
    }
    memset(set, 0, sizeof(*set));
}

int bx_ntvdm_namespace_root_set_v1_valid(
    const bx_ntvdm_namespace_root_set_v1 *set)
{
    uint32_t index;
    if (set == 0 || set->magic != BX_NTVDM_NAMESPACE_ROOT_SET_V1_MAGIC ||
        set->abi_version != BX_NTVDM_NAMESPACE_ROOT_SET_V1_VERSION ||
        !bx_ntvdm_host_drive_snapshot_v1_valid(&set->snapshot) ||
        (set->available_mask & ~set->snapshot.admitted_mask) != 0u) return 0;
    for (index = 0u; index < 26u; ++index) {
        uint32_t bit = bx_ntvdm_namespace_root_set_bit(index);
        if ((set->available_mask & bit) != 0u &&
            (set->roots[index] == 0 || set->roots[index] == INVALID_HANDLE_VALUE))
            return 0;
    }
    return 1;
}

int bx_ntvdm_namespace_root_set_v1_install(bx_ntvdm_namespace_root_set_v1 *set,
    const bx_ntvdm_host_drive_snapshot_v1 *snapshot)
{
    uint32_t index;
    if (set == 0 || !bx_ntvdm_host_drive_snapshot_v1_valid(snapshot)) return 0;
    memset(set, 0, sizeof(*set));
    for (index = 0u; index < 26u; ++index) set->roots[index] = INVALID_HANDLE_VALUE;
    set->magic = BX_NTVDM_NAMESPACE_ROOT_SET_V1_MAGIC;
    set->abi_version = BX_NTVDM_NAMESPACE_ROOT_SET_V1_VERSION;
    memcpy(&set->snapshot, snapshot, sizeof(*snapshot));
    for (index = 0u; index < 26u; ++index) {
        wchar_t root[] = { (wchar_t)(L'A' + index), L':', L'\\', L'\0' };
        HANDLE handle;
        if ((snapshot->admitted_mask & bx_ntvdm_namespace_root_set_bit(index)) == 0u)
            continue;
        handle = CreateFileW(root, FILE_LIST_DIRECTORY | FILE_READ_ATTRIBUTES,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, 0);
        if (handle == INVALID_HANDLE_VALUE) continue;
        set->roots[index] = handle;
        set->available_mask |= bx_ntvdm_namespace_root_set_bit(index);
    }
    if (!bx_ntvdm_namespace_root_set_v1_valid(set)) {
        bx_ntvdm_namespace_root_set_v1_release(set);
        return 0;
    }
    return 1;
}
