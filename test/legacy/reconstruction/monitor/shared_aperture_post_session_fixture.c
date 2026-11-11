#include <stdint.h>
#include <wchar.h>
#include <windows.h>

#include "reconstructed_monitor.h"
#include "shared_aperture_v1.h"

static uint32_t read_u32(const uint8_t *bytes, size_t offset)
{
    return (uint32_t)bytes[offset] | ((uint32_t)bytes[offset + 1u] << 8) |
        ((uint32_t)bytes[offset + 2u] << 16) | ((uint32_t)bytes[offset + 3u] << 24);
}

static void write_u32(uint8_t *bytes, size_t offset, uint32_t value)
{
    bytes[offset] = (uint8_t)value;
    bytes[offset + 1u] = (uint8_t)(value >> 8);
    bytes[offset + 2u] = (uint8_t)(value >> 16);
    bytes[offset + 3u] = (uint8_t)(value >> 24);
}

int wmain(int argc, wchar_t **argv)
{
    const DWORD aperture_bytes = 2u * 1024u * 1024u;
    wchar_t mapping_name[128];
    wchar_t command_line[1024];
    HANDLE mapping = NULL;
    uint8_t *view = NULL;
    STARTUPINFOW startup = {0};
    PROCESS_INFORMATION process = {0};
    reconstructed_monitor_monitor monitor;
    reconstructed_monitor_state state;
    DWORD exit_code;
    int result = 1;

    if (argc != 2) return 2;
    if (swprintf(mapping_name, sizeof(mapping_name) / sizeof(mapping_name[0]),
        L"Local\\runner-s4-sas-%lu-%lu", (unsigned long)GetCurrentProcessId(),
        (unsigned long)GetTickCount()) < 0) return 3;
    mapping = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0u,
        aperture_bytes, mapping_name);
    if (mapping == NULL || GetLastError() == ERROR_ALREADY_EXISTS) goto cleanup;
    view = MapViewOfFile(mapping, FILE_MAP_READ | FILE_MAP_WRITE, 0u, 0u, aperture_bytes);
    if (view == NULL) goto cleanup;
    write_u32(view, APP_SHARED_APERTURE_V1_OFFSET_MAGIC,
        APP_SHARED_APERTURE_V1_MAGIC);
    write_u32(view, APP_SHARED_APERTURE_V1_OFFSET_VERSION,
        APP_SHARED_APERTURE_V1_VERSION);
    write_u32(view, APP_SHARED_APERTURE_V1_OFFSET_BYTES, aperture_bytes);
    write_u32(view, APP_SHARED_APERTURE_V1_OFFSET_HOST_READY, 1u);
    view[0x1000u] = 0x90u;

    if (swprintf(command_line, sizeof(command_line) / sizeof(command_line[0]),
        L"\"%ls\" \"%ls\" %lu", argv[1], mapping_name, (unsigned long)aperture_bytes) < 0) {
        goto cleanup;
    }
    startup.cb = sizeof(startup);
    if (!CreateProcessW(NULL, command_line, NULL, NULL, FALSE, 0u, NULL, NULL, &startup, &process)) {
        goto cleanup;
    }
    if (WaitForSingleObject(process.hProcess, 15000u) != WAIT_OBJECT_0 ||
        !GetExitCodeProcess(process.hProcess, &exit_code) || exit_code != 0u) {
        CloseHandle(process.hThread);
        CloseHandle(process.hProcess);
        goto cleanup;
    }
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
    if (read_u32(view, APP_SHARED_APERTURE_V1_OFFSET_SAS_READY) != 1u ||
        view[APP_SHARED_APERTURE_V1_OFFSET_SAS_PROBE] != 0x5au || view[0x1000u] != 0x90u) {
        goto cleanup;
    }

    if (reconstructed_monitor_monitor_initialize(&monitor, view, aperture_bytes, NULL, NULL) !=
        RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) goto cleanup;
    if (reconstructed_monitor_monitor_get_state(&monitor, &state) != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) goto cleanup;
    state.cs = 0x0100u;
    state.ip = 0u;
    if (reconstructed_monitor_monitor_set_state(&monitor, &state) != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED ||
        reconstructed_monitor_monitor_run(&monitor, 1u) != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED ||
        reconstructed_monitor_monitor_get_state(&monitor, &state) != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED ||
        state.cs != 0x0100u || state.ip != 1u) goto cleanup;
    result = 0;

cleanup:
    if (view != NULL) UnmapViewOfFile(view);
    if (mapping != NULL) CloseHandle(mapping);
    return result;
}
