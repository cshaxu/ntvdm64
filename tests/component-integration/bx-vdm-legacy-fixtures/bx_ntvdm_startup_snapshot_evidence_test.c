#include <windows.h>
#include <string.h>

#include "startup_snapshot_evidence.h"

int main(void)
{
    wchar_t root[MAX_PATH], path[MAX_PATH];
    HANDLE file;
    DWORD read = 0u;
    char text[1024] = { 0 };
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 state;
    bx_ntvdm_startup_snapshot_range_v1 ranges[2] = {
        { 1u, 0u, { 64u, 2u } }, { 2u, 0u, { 1812u, 1u } }
    };
    bx_ntvdm_startup_snapshot_transaction_v1 transaction;
    const uint8_t output[] = { 0x12u, 0x34u, 0x56u };
    memset(&event, 0, sizeof(event));
    event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event.struct_bytes = sizeof(event);
    event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event.vector = 6u; event.fault_rip = 0x7c00u;
    bx_ntvdm_cpu_state_v1_initialize(&state, BX_NTVDM_CPU_EXECUTION_REAL);
    state.eip = 0x7c00u; state.cs = 0u;
    bx_ntvdm_startup_snapshot_transaction_v1_initialize(&transaction, &event,
        &state, ranges, 2u);
    if (GetTempPathW(MAX_PATH, root) == 0 || GetTempFileNameW(root, L"n64", 0u, root) == 0 ||
        !DeleteFileW(root) || !CreateDirectoryW(root, 0) ||
        !bx_ntvdm_startup_snapshot_evidence_v1_write(root, L"evidence.json",
            &transaction, output, sizeof(output), UINT64_C(0x1122334455667788)) ||
        bx_ntvdm_startup_snapshot_evidence_v1_write(root, L"..\\bad.json",
            &transaction, output, sizeof(output), 0u)) return 1;
    swprintf(path, MAX_PATH, L"%ls\\evidence.json", root);
    file = CreateFileW(path, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    if (file == INVALID_HANDLE_VALUE || !ReadFile(file, text, sizeof(text) - 1u, &read, 0)) return 2;
    CloseHandle(file);
    DeleteFileW(path); RemoveDirectoryW(root);
    return strstr(text, "ntdos64-startup-snapshot-evidence-v1") == 0 ||
        strstr(text, "\"bytes_hex\":\"1234\"") == 0 ||
        strstr(text, "\"bytes_hex\":\"56\"") == 0 ||
        strstr(text, "1122334455667788") == 0;
}
