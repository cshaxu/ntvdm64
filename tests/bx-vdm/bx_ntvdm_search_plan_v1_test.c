#include <stdio.h>
#include <string.h>

#include "bop-v1/bx_ntvdm_search_plan_v1.h"

static int write_file(const wchar_t *path)
{
    HANDLE file = CreateFileW(path, GENERIC_WRITE, 0u, 0, CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL, 0);
    DWORD written;
    if (file == INVALID_HANDLE_VALUE) return 0;
    if (!WriteFile(file, "x", 1u, &written, 0) || written != 1u) {
        CloseHandle(file); return 0;
    }
    CloseHandle(file);
    return 1;
}

static uint32_t u32(const uint8_t *value)
{
    return (uint32_t)value[0] | ((uint32_t)value[1] << 8) |
        ((uint32_t)value[2] << 16) | ((uint32_t)value[3] << 24);
}

int main(void)
{
    wchar_t temporary[MAX_PATH], root[MAX_PATH], short_root[MAX_PATH];
    wchar_t alpha[MAX_PATH], zeta[MAX_PATH], hidden[MAX_PATH];
    bx_ntvdm_host_drive_snapshot_v1 snapshot = {0};
    bx_ntvdm_host_namespace_v1 space;
    bx_ntvdm_search_plan_v1 plan;
    bx_ntvdm_search_query_v1 query = {0};
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x7c00u };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_multi_write_transaction_v1 tx;
    bx_ntvdm_search_token_v1 token;
    uint8_t payload[51];
    uint32_t payload_bytes;
    uint8_t drive;
    int failed = 0;

    if (GetTempPathW(MAX_PATH, temporary) == 0u ||
        GetTempFileNameW(temporary, L"n64", 0u, root) == 0u ||
        !DeleteFileW(root) || !CreateDirectoryW(root, 0)) return 2;
    swprintf(alpha, MAX_PATH, L"%ls\\ALPHA.TXT", root);
    swprintf(zeta, MAX_PATH, L"%ls\\ZETA.TXT", root);
    swprintf(hidden, MAX_PATH, L"%ls\\HIDDEN.TXT", root);
    if (!write_file(alpha) || !write_file(zeta) || !write_file(hidden) ||
        !SetFileAttributesW(hidden, FILE_ATTRIBUTE_HIDDEN) ||
        GetShortPathNameW(root, short_root, MAX_PATH) == 0u || short_root[1] != L':')
        failed = 1;
    drive = (uint8_t)(towupper((wint_t)short_root[0]) - L'A');
    snapshot.magic = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_MAGIC;
    snapshot.version = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_VERSION;
    snapshot.present_mask = UINT32_C(1) << drive;
    snapshot.admitted_mask = snapshot.present_mask;
    snapshot.types[drive] = DRIVE_FIXED;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    bx_ntvdm_search_plan_v1_initialize(&plan);
    if (!failed && !bx_ntvdm_host_namespace_v1_initialize(&space, &snapshot)) failed = 1;
    if (!failed) {
        query.drive_index = drive;
        wcscpy(query.relative_directory, short_root + 3u);
        wcscpy(query.pattern, L"*.TXT");
        if (bx_ntvdm_search_plan_v1_first(&plan, &space, &event, &cpu, 0x1111u,
            BX_NTVDM_SEARCH_PLAN_V1_PATH, 0x900u, &query, &tx, payload,
            &payload_bytes) != BX_NTVDM_SEARCH_PLAN_V1_OK || payload_bytes != 30u ||
            memcmp(payload + 17u, "ALPHA.TXT", 9u) != 0 ||
            !bx_ntvdm_multi_write_transaction_v1_preflight(&tx, 0x100000u, payload_bytes)) failed = 1;
        token.cookie = u32(payload); token.id = u32(payload + 4u);
        if (token.cookie == 0u || token.id == 0u ||
            bx_ntvdm_search_plan_v1_next(&plan, &event, &cpu, 0x2222u,
                BX_NTVDM_SEARCH_PLAN_V1_PATH, 0x900u, &token, &tx, payload,
                &payload_bytes) != BX_NTVDM_SEARCH_PLAN_V1_NO_MATCH ||
            bx_ntvdm_search_plan_v1_next(&plan, &event, &cpu, 0x1111u,
                BX_NTVDM_SEARCH_PLAN_V1_PATH, 0x900u, &token, &tx, payload,
                &payload_bytes) != BX_NTVDM_SEARCH_PLAN_V1_OK || payload_bytes != 30u ||
            memcmp(payload + 17u, "ZETA.TXT", 8u) != 0 || u32(payload) != 0u ||
            u32(payload + 4u) != 0u) failed = 1;
        query.attributes = 0x02u;
        if (bx_ntvdm_search_plan_v1_first(&plan, &space, &event, &cpu, 0x3333u,
            BX_NTVDM_SEARCH_PLAN_V1_FCB, 0xa00u, &query, &tx, payload,
            &payload_bytes) != BX_NTVDM_SEARCH_PLAN_V1_OK || payload_bytes != 51u ||
            memcmp(payload, "ALPHA   TXT", 11u) != 0) failed = 1;
        query.attributes = 0x08u;
        if (bx_ntvdm_search_plan_v1_first(&plan, &space, &event, &cpu, 0x4444u,
            BX_NTVDM_SEARCH_PLAN_V1_PATH, 0x900u, &query, &tx, payload,
            &payload_bytes) != BX_NTVDM_SEARCH_PLAN_V1_REJECTED) failed = 1;
        query.attributes = 0u;
        wcscpy(query.pattern, L"NONE.*");
        if (bx_ntvdm_search_plan_v1_first(&plan, &space, &event, &cpu, 0x5555u,
            BX_NTVDM_SEARCH_PLAN_V1_PATH, 0x900u, &query, &tx, payload,
            &payload_bytes) != BX_NTVDM_SEARCH_PLAN_V1_NO_MATCH || payload_bytes != 0u)
            failed = 1;
        bx_ntvdm_host_namespace_v1_release(&space);
    }
    bx_ntvdm_search_plan_v1_release(&plan);
    SetFileAttributesW(hidden, FILE_ATTRIBUTE_NORMAL);
    DeleteFileW(hidden); DeleteFileW(alpha); DeleteFileW(zeta); RemoveDirectoryW(root);
    if (failed) return 1;
    puts("bx-ntvdm-search-plan-v1-test: paired namespace/session/output plan verified");
    return 0;
}
