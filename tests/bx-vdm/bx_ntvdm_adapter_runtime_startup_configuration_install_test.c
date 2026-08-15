#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "bx_ntvdm_adapter_runtime.h"

static const char sha256_abc[] =
    "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";

static int write_bytes(const wchar_t *path, const char *bytes)
{
    HANDLE file;
    DWORD written = 0u;
    size_t length = strlen(bytes);
    file = CreateFileW(path, GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, 0);
    if (file == INVALID_HANDLE_VALUE) return 0;
    if (!WriteFile(file, bytes, (DWORD)length, &written, 0) || written != length) {
        CloseHandle(file);
        return 0;
    }
    CloseHandle(file);
    return 1;
}

int main(void)
{
    wchar_t root[MAX_PATH], ntio[MAX_PATH], ntdos[MAX_PATH], command[MAX_PATH];
    wchar_t config[MAX_PATH], autoexec[MAX_PATH], profile[MAX_PATH];
    char json[4096];
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x700u };
    bx_ntvdm_cpu_state_v1 state;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_cpu_result_v2 result;
    bx_ntvdm_multi_write_transaction_v1 transaction;
    const uint8_t *payload = 0;
    uint64_t payload_bytes = 0u;
    int failed = 0;

    if (GetTempPathW(MAX_PATH, root) == 0 ||
        GetTempFileNameW(root, L"n64", 0u, root) == 0 || !DeleteFileW(root) ||
        !CreateDirectoryW(root, 0)) return 1;
    swprintf(ntio, MAX_PATH, L"%ls\\NTIO.SYS", root);
    swprintf(ntdos, MAX_PATH, L"%ls\\NTDOS.SYS", root);
    swprintf(command, MAX_PATH, L"%ls\\COMMAND.COM", root);
    swprintf(config, MAX_PATH, L"%ls\\input-config.sys", root);
    swprintf(autoexec, MAX_PATH, L"%ls\\input-autoexec.bat", root);
    swprintf(profile, MAX_PATH, L"%ls\\profile.json", root);
    snprintf(json, sizeof(json),
        "{\"schema\":\"ntdos64-byob-profile-v2\",\"profile\":\"t204-install\",\"architecture\":\"x86\",\"locale\":\"en-US\",\"compatibility_group\":\"t204-install\",\"components\":[{\"role\":\"ntio\",\"file_name\":\"NTIO.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},{\"role\":\"ntdos\",\"file_name\":\"NTDOS.SYS\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null},{\"role\":\"command\",\"file_name\":\"COMMAND.COM\",\"required\":true,\"bytes\":3,\"sha256\":\"%s\",\"version\":null}],\"features\":[],\"owner_note\":null,\"guest_command_placement\":{\"path\":\"\\\\COMMAND.COM\",\"drive_index\":2},\"guest_boot_files\":{\"config\":{\"path\":\"\\\\CONFIG.SYS\",\"materialization\":\"minimal-comment-v1\"},\"autoexec\":{\"path\":\"\\\\AUTOEXEC.BAT\",\"materialization\":\"empty-v1\"}},\"guest_search_metadata\":{\"command\":{\"attributes\":32,\"dos_time\":1,\"dos_date\":1},\"config\":{\"attributes\":32,\"dos_time\":1,\"dos_date\":1},\"autoexec\":{\"attributes\":32,\"dos_time\":1,\"dos_date\":1}}}",
        sha256_abc, sha256_abc, sha256_abc);
    failed |= !write_bytes(ntio, "abc") || !write_bytes(ntdos, "abc") ||
        !write_bytes(command, "abc") || !write_bytes(config, "FILES=20\r\n") ||
        !write_bytes(autoexec, "") || !write_bytes(profile, json) ||
        !SetEnvironmentVariableW(L"NTDOS64_STARTUP_CONFIG_SOURCE", config) ||
        !SetEnvironmentVariableW(L"NTDOS64_STARTUP_AUTOEXEC_SOURCE", autoexec) ||
        !SetEnvironmentVariableW(L"NTDOS64_ADAPTER_LAUNCH_KIND", L"com") ||
        !SetEnvironmentVariableW(L"NTDOS64_ADAPTER_LAUNCH_TAIL", L"");
    bx_ntvdm_adapter_runtime_v1_reset();
    failed |= !bx_ntvdm_adapter_runtime_v1_install(profile, root) ||
        bx_ntvdm_adapter_runtime_v1_install_diagnostic() !=
            BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_NONE ||
        !DeleteFileW(config) || !DeleteFileW(autoexec);
    bx_ntvdm_cpu_state_v1_initialize(&state, BX_NTVDM_CPU_EXECUTION_REAL);
    state.ds = 0x100u;
    state.edx = 0x60u;
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){ 0xc4u, 0xc4u, 0x54u, 0x0cu }, 4u);
    failed |= !bx_ntvdm_adapter_runtime_v2_dispatch(&event, &state, &window, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        !bx_ntvdm_adapter_runtime_v1_take_pending_multi_write(&event, &state,
            &transaction, &payload, &payload_bytes) || payload_bytes != 14u ||
        memcmp(payload, "C:\\CONFIG.SYS", 14u) != 0;
    bx_ntvdm_adapter_runtime_v1_reset();
    SetEnvironmentVariableW(L"NTDOS64_STARTUP_CONFIG_SOURCE", 0);
    SetEnvironmentVariableW(L"NTDOS64_STARTUP_AUTOEXEC_SOURCE", 0);
    SetEnvironmentVariableW(L"NTDOS64_ADAPTER_LAUNCH_KIND", 0);
    SetEnvironmentVariableW(L"NTDOS64_ADAPTER_LAUNCH_TAIL", 0);
    DeleteFileW(profile);
    DeleteFileW(command);
    DeleteFileW(ntdos);
    DeleteFileW(ntio);
    RemoveDirectoryW(root);
    return failed ? 1 : 0;
}
