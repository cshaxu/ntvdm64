#include "bx_ntvdm_host_drive_policy.h"

#include <string.h>
#include <windows.h>

#define BX_NTVDM_ADAPTER_ENV_INCLUDE_DRIVES L"NTDOS64_HOST_INCLUDE_DRIVES"
#define BX_NTVDM_ADAPTER_ENV_EXCLUDE_DRIVES L"NTDOS64_HOST_EXCLUDE_DRIVES"

static uint32_t bx_ntvdm_host_drive_bit(unsigned index)
{
    return UINT32_C(1) << index;
}

int bx_ntvdm_host_drive_policy_v1_parse(const wchar_t *text,
    uint32_t *out_mask)
{
    uint32_t mask = 0u;
    size_t index = 0u;

    if (text == 0 || out_mask == 0) return 0;
    if (text[0] == L'\0') { *out_mask = 0u; return 1; }
    for (;;) {
        wchar_t letter = text[index++];
        unsigned drive_index;
        uint32_t bit;
        if (letter >= L'a' && letter <= L'z') letter = (wchar_t)(letter - L'a' + L'A');
        if (letter < L'A' || letter > L'Z') return 0;
        drive_index = (unsigned)(letter - L'A');
        bit = bx_ntvdm_host_drive_bit(drive_index);
        if ((mask & bit) != 0u) return 0;
        mask |= bit;
        if (text[index] == L'\0') break;
        if (text[index++] != L',' || text[index] == L'\0') return 0;
    }
    *out_mask = mask;
    return 1;
}

int bx_ntvdm_host_drive_snapshot_v1_valid(
    const bx_ntvdm_host_drive_snapshot_v1 *snapshot)
{
    uint32_t index;
    if (snapshot == 0 || snapshot->magic != BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_MAGIC ||
        snapshot->version != BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_VERSION ||
        (snapshot->admitted_mask & ~snapshot->present_mask) != 0u ||
        (snapshot->admitted_mask & snapshot->exclude_mask) != 0u) return 0;
    for (index = 0u; index < 26u; ++index) {
        uint32_t bit = bx_ntvdm_host_drive_bit(index);
        if ((snapshot->admitted_mask & bit) == 0u && snapshot->types[index] != 0u)
            return 0;
        if ((snapshot->admitted_mask & bit) != 0u && snapshot->types[index] == 0u)
            return 0;
    }
    return 1;
}

int bx_ntvdm_host_drive_snapshot_v1_apply(uint32_t present_mask,
    const uint8_t types[26], uint32_t include_mask, uint32_t exclude_mask,
    bx_ntvdm_host_drive_snapshot_v1 *out)
{
    uint32_t index;
    uint32_t eligible_mask;
    if (types == 0 || out == 0) return 0;
    eligible_mask = include_mask == 0u ? present_mask : present_mask & include_mask;
    memset(out, 0, sizeof(*out));
    out->magic = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_MAGIC;
    out->version = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_VERSION;
    out->present_mask = present_mask;
    out->include_mask = include_mask;
    out->exclude_mask = exclude_mask;
    out->admitted_mask = eligible_mask & ~exclude_mask;
    for (index = 0u; index < 26u; ++index) {
        uint32_t bit = bx_ntvdm_host_drive_bit(index);
        if ((out->admitted_mask & bit) == 0u) continue;
        if (types[index] == 0u) return 0;
        out->types[index] = types[index];
    }
    return bx_ntvdm_host_drive_snapshot_v1_valid(out);
}

int bx_ntvdm_host_drive_snapshot_v1_capture_from_environment(
    bx_ntvdm_host_drive_snapshot_v1 *out)
{
    wchar_t include_text[52] = {0};
    wchar_t exclude_text[52] = {0};
    uint8_t types[26] = {0};
    uint32_t include_mask, exclude_mask, present_mask;
    DWORD returned;
    uint32_t index;

    if (out == 0) return 0;
    returned = GetEnvironmentVariableW(BX_NTVDM_ADAPTER_ENV_INCLUDE_DRIVES,
        include_text, (DWORD)(sizeof(include_text) / sizeof(include_text[0])));
    if (returned >= sizeof(include_text) / sizeof(include_text[0])) return 0;
    returned = GetEnvironmentVariableW(BX_NTVDM_ADAPTER_ENV_EXCLUDE_DRIVES,
        exclude_text, (DWORD)(sizeof(exclude_text) / sizeof(exclude_text[0])));
    if (returned >= sizeof(exclude_text) / sizeof(exclude_text[0]) ||
        !bx_ntvdm_host_drive_policy_v1_parse(include_text, &include_mask) ||
        !bx_ntvdm_host_drive_policy_v1_parse(exclude_text, &exclude_mask)) return 0;
    present_mask = GetLogicalDrives();
    if (present_mask == 0u) return 0;
    for (index = 0u; index < 26u; ++index) {
        wchar_t root[] = { (wchar_t)(L'A' + index), L':', L'\\', L'\0' };
        uint32_t bit = bx_ntvdm_host_drive_bit(index);
        UINT type;
        if ((present_mask & bit) == 0u) continue;
        type = GetDriveTypeW(root);
        if (type == DRIVE_UNKNOWN || type == DRIVE_NO_ROOT_DIR || type > 6u) {
            present_mask &= ~bit;
            continue;
        }
        types[index] = (uint8_t)type;
    }
    return bx_ntvdm_host_drive_snapshot_v1_apply(present_mask, types,
        include_mask, exclude_mask, out);
}
