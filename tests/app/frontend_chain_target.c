/* Twelve-target chain witness. Build as both GUI and CUI; DOS target stages
 * use immutable COMMAND.COM and invoke the CUI identity probe as auxiliary
 * observation only. No fixture process substitutes for DOS execution. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <tlhelp32.h>
#include <rpc.h>
#include "service.h"
#include "ntsrv-exe/transport/rpc_security.h"
#include "common/protocol/version.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"

PVOID CsrPortHeap;

static BOOL dos_records(PCWSTR report, unsigned stage, PCWSTR phase)
{
    broker_rpc_scope scope = {0}; RPC_WSTR text = NULL;
    RPC_BINDING_HANDLE binding = NULL; HANDLE self = NULL;
    WCHAR endpoint[128], path[MAX_PATH]; FILE *file = NULL;
    DTASKMGR_WORKER *rows = NULL; ULONG count = 0, i;
    DWORD error; BOOL ok = FALSE;
    unsigned char version[APP_VERSION_BYTES] = APP_VERSION;
    if (!broker_rpc_capture_scope(&scope)) return FALSE;
    swprintf_s(endpoint, ARRAYSIZE(endpoint), L"ntvdm-basesrv-%lu-%08lx-%08lx",
        scope.session, (ULONG)scope.logon.HighPart, (ULONG)scope.logon.LowPart);
    error = RpcStringBindingComposeW(NULL, (RPC_WSTR)L"ncalrpc", NULL,
        (RPC_WSTR)endpoint, NULL, &text);
    if (!error) error = RpcBindingFromStringBindingW(text, &binding);
    if (text) RpcStringFreeW(&text);
    if (!error) error = RpcBindingSetAuthInfoW(binding, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
        RPC_C_AUTHN_WINNT, NULL, RPC_C_AUTHZ_NONE);
    if (error) goto done;
    self = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE | PROCESS_DUP_HANDLE,
        FALSE, GetCurrentProcessId());
    if (!self) goto done;
    RpcTryExcept {
        error = Client_TaskSnapshot(binding, self, APP_PROTOCOL_VERSION, version, &count, &rows);
    } RpcExcept(1) { error = RpcExceptionCode(); } RpcEndExcept
    /* Stage 1 observes final completion; an empty snapshot is valid there. */
    if (error || (!count && stage != 1)) goto done;
    swprintf_s(path, ARRAYSIZE(path), L"%ls.records-%u-%ls", report, stage, phase);
    if (_wfopen_s(&file, path, L"w")) goto done;
    for (i = 0; i < count; ++i) {
        fprintf(file, "RECORD %lu %lu %lu %lu %lu %ls\n", rows[i].process_id,
            rows[i].task, rows[i].kind, rows[i].state, rows[i].stack_depth, rows[i].image);
    }
    ok = !ferror(file);
done:
    if (file && fclose(file)) ok = FALSE;
    if (rows) MIDL_user_free(rows);
    if (self) CloseHandle(self);
    if (binding) RpcBindingFree(&binding);
    return ok;
}

static HANDLE start_input(PCWSTR report, unsigned stage, PCWSTR phase, WCHAR kind, DWORD owner)
{
    WCHAR image[MAX_PATH], command[2048], output[MAX_PATH], *slash;
    STARTUPINFOW startup = {sizeof(startup)}; PROCESS_INFORMATION child = {0};
    if (!GetModuleFileNameW(NULL, image, ARRAYSIZE(image))) return NULL;
    slash = wcsrchr(image, L'\\'); if (!slash) return NULL;
    wcscpy_s(slash+1, ARRAYSIZE(image)-(size_t)(slash+1-image), L"CINPUT.EXE");
    swprintf_s(output, ARRAYSIZE(output), L"%ls.io-%u-%ls", report, stage, phase);
    swprintf_s(command, ARRAYSIZE(command),
        L"\"%ls\" %lu READY-%u-%ls ACK-%u-%ls %lc \"%ls\" \"chain\r\"",
        image, owner, stage, phase, stage, phase, kind, output);
    if (!CreateProcessW(image, command, NULL, NULL, FALSE, CREATE_NO_WINDOW,
        NULL, NULL, &startup, &child)) return NULL;
    CloseHandle(child.hThread); return child.hProcess;
}

static BOOL native_input(PCWSTR report, unsigned stage, PCWSTR phase, DWORD owner)
{
    char line[64] = {0}; DWORD read, result; HANDLE driver = start_input(report, stage, phase, L'W', owner);
    BOOL success = FALSE;
    if (!driver) return FALSE;
    printf("READY-%u-%ls\n", stage, phase); fflush(stdout);
    if (ReadFile(GetStdHandle(STD_INPUT_HANDLE), line, sizeof(line)-1, &read, NULL) &&
        read >= 6 && !memcmp(line, "chain\r", 6)) {
        printf("ACK-%u-%ls\n", stage, phase); fflush(stdout);
        success = WaitForSingleObject(driver, 12000) == WAIT_OBJECT_0 &&
            GetExitCodeProcess(driver, &result) && !result;
    }
    CloseHandle(driver); return success;
}

/* Observation only: match the exact candidate file identity, not an arbitrary
 * process basename. These handles never authorize a production association. */
static BOOL same_image(HANDLE process, const BY_HANDLE_FILE_INFORMATION *wanted)
{
    WCHAR image[MAX_PATH]; DWORD size = ARRAYSIZE(image);
    BY_HANDLE_FILE_INFORMATION found; HANDLE file; BOOL same;
    if (!QueryFullProcessImageNameW(process, 0, image, &size)) return FALSE;
    file = CreateFileW(image, 0, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL, OPEN_EXISTING, 0, NULL);
    if (file == INVALID_HANDLE_VALUE) return FALSE;
    same = GetFileInformationByHandle(file, &found) &&
        found.dwVolumeSerialNumber == wanted->dwVolumeSerialNumber &&
        found.nFileIndexHigh == wanted->nFileIndexHigh && found.nFileIndexLow == wanted->nFileIndexLow;
    CloseHandle(file); return same;
}

static BOOL topology(PCWSTR report, unsigned stage, BOOL continuous)
{
    WCHAR image[MAX_PATH], output[MAX_PATH], *slash;
    BY_HANDLE_FILE_INFORMATION wanted; HANDLE file, snapshot, first = NULL;
    PROCESSENTRY32W entry = {sizeof(entry)};
    DWORD count = 0, first_pid = 0; FILE *events = NULL, *log = NULL;
    char phase[32], kind; unsigned index; unsigned long pid, owner, gen, child, result;
    if (!GetModuleFileNameW(NULL, image, ARRAYSIZE(image))) return FALSE;
    slash = wcsrchr(image, L'\\'); if (!slash) return FALSE; *slash = 0;
    slash = wcsrchr(image, L'\\'); if (!slash) return FALSE;
    wcscpy_s(slash+1, ARRAYSIZE(image)-(size_t)(slash+1-image), L"ntcon.exe");
    file = CreateFileW(image, 0, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL, OPEN_EXISTING, 0, NULL);
    if (file == INVALID_HANDLE_VALUE) return FALSE;
    if (!GetFileInformationByHandle(file, &wanted)) { CloseHandle(file); return FALSE; }
    CloseHandle(file);
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return FALSE;
    if (Process32FirstW(snapshot, &entry)) do {
        if (!_wcsicmp(entry.szExeFile, L"ntcon.exe")) {
            HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE,
                FALSE, entry.th32ProcessID);
            if (process) {
                if (same_image(process, &wanted) && WaitForSingleObject(process, 0) == WAIT_TIMEOUT) ++count;
                CloseHandle(process);
            }
        }
    } while (Process32NextW(snapshot, &entry));
    CloseHandle(snapshot);
    if (continuous && count != 1) return FALSE;
    if (!continuous && stage <= 3 && count) return FALSE;
    if (!continuous && stage >= 7) {
        if (_wfopen_s(&events, report, L"r")) return FALSE;
        while (fscanf_s(events, "%31s %u %c %lu %lu %lu %lu %lu", phase, (unsigned)sizeof(phase),
                &index, &kind, 1u, &pid, &owner, &gen, &child, &result) == 8) {
            if (index == 4 && !strcmp(phase, "ENTER")) first_pid = owner;
        }
        fclose(events);
        if (!first_pid) return FALSE;
        first = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE, FALSE, first_pid);
        if (!first) return FALSE;
        if (!same_image(first, &wanted) || WaitForSingleObject(first, 0) != WAIT_TIMEOUT) {
            CloseHandle(first); return FALSE;
        }
        CloseHandle(first);
    }
    swprintf_s(output, ARRAYSIZE(output), L"%ls.topology", report);
    if (_wfopen_s(&log, output, L"a")) return FALSE;
    fprintf(log, "TOPOLOGY %u %lu %lu\n", stage, count, first_pid);
    return fclose(log) == 0;
}

static DWORD identity(WCHAR kind, DWORD *pid, DWORD *generation)
{
    WCHAR value[64], *end;
    HANDLE capability, owner = NULL;
    DWORD error;
    *pid = *generation = 0;
    if (kind == L'G') {
        const WCHAR *names[] = {L"NTVDM_FRONTEND_CAPABILITY", L"NTVDM_EXECUTION_CONSOLE"};
        unsigned i;
        for (i = 0; i < ARRAYSIZE(names); ++i) {
            SetLastError(ERROR_SUCCESS);
            if (GetEnvironmentVariableW(names[i], value, ARRAYSIZE(value)) ||
                GetLastError() != ERROR_ENVVAR_NOT_FOUND) return ERROR_INVALID_DATA;
        }
        return 0;
    }
    if (!GetEnvironmentVariableW(L"NTVDM_FRONTEND_CAPABILITY", value, ARRAYSIZE(value)))
        return ERROR_INVALID_HANDLE;
    capability = (HANDLE)(UINT_PTR)wcstoul(value, &end, 16);
    if (!capability || *end) return ERROR_INVALID_DATA;
    CsrPortHeap = HeapCreate(0, 0, 0);
    if (!CsrPortHeap) return GetLastError();
    error = OpenNtBaseClientConnectCurrent();
    if (!error) error = OpenNtBaseClientRetainFrontendRoot(capability, &owner, generation);
    if (!error) {
        *pid = GetProcessId(owner);
        if (!*pid || WaitForSingleObject(owner, 0) != WAIT_TIMEOUT) error = ERROR_INVALID_DATA;
    }
    if (owner) CloseHandle(owner);
    OpenNtBaseClientDisconnectCurrent();
    HeapDestroy(CsrPortHeap); CsrPortHeap = NULL;
    return error;
}

static BOOL record(PCWSTR path, PCWSTR phase, unsigned stage, WCHAR kind,
    DWORD owner, DWORD generation, DWORD child, DWORD result)
{
    char line[256]; DWORD written; int length;
    HANDLE file = CreateFileW(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return FALSE;
    length = sprintf_s(line, sizeof(line), "%ls %u %lc %lu %lu %lu %lu %lu\r\n",
        phase, stage, kind, GetCurrentProcessId(), owner, generation, child, result);
    if (length < 0 || !WriteFile(file, line, (DWORD)length, &written, NULL) || written != (DWORD)length) {
        CloseHandle(file); return FALSE;
    }
    FlushFileBuffers(file); CloseHandle(file);
    return TRUE;
}

int wmain(int argc, WCHAR **argv)
{
    WCHAR kinds[20], report[MAX_PATH], section[16], command[2048];
    WCHAR title[80]; unsigned stage, stages; DWORD owner, generation, error, result = 37;
    DWORD child_pid = 0, returned_owner, returned_generation;
    STARTUPINFOW startup = {sizeof(startup)};
    PROCESS_INFORMATION child = {0}; HWND window = NULL;
    BOOL probe;
    if (argc == 3 && !wcscmp(argv[1], L"--snapshot"))
        return dos_records(argv[2], 1, L"RETURN") ? 0 : 99;
    if (argc != 3 && argc != 4) return 80;
    stage = (unsigned)_wtoi(argv[2]);
    if (stage < 1 || stage > 12) return 81;
    stages = GetPrivateProfileStringW(L"chain", L"kinds", L"", kinds, ARRAYSIZE(kinds), argv[1]);
    if ((stages != 12 && wcscmp(kinds, L"DDWWDDWW")) || stage > stages ||
        !GetPrivateProfileStringW(L"chain", L"report", L"", report, ARRAYSIZE(report), argv[1])) return 82;
    probe = kinds[stage-1] == L'D';
    if (probe != (argc == 4) || (probe && wcscmp(argv[3], L"ENTER") && wcscmp(argv[3], L"RETURN"))) return 83;
    error = identity(kinds[stage-1], &owner, &generation);
    if (error) { record(report, L"IDENTITY-FAIL", stage, kinds[stage-1], 0, 0, 0, error); return 84; }
    if (!topology(report, stage, stages == 8)) return 95;
    if (probe) {
        if (!dos_records(report, stage, argv[3])) return 99;
        HANDLE driver = start_input(report, stage, argv[3], L'D', owner);
        if (!driver) return 96;
        CloseHandle(driver); /* Batch PAUSE/ACK owns the subsequent guest input. */
        printf("DOS-STAGE-%02u-%ls\n", stage, argv[3]); fflush(stdout);
        return record(report, argv[3], stage, L'D', owner, generation, 0, 0) ? 0 : 85;
    }
    if (kinds[stage-1] == L'G') {
        swprintf_s(title, ARRAYSIZE(title), L"CHAIN-GUI-%02u", stage);
        window = CreateWindowExW(0, L"STATIC", title, WS_OVERLAPPEDWINDOW,
            0, 0, 240, 120, NULL, NULL, GetModuleHandleW(NULL), NULL);
        if (!window) return 86;
        ShowWindow(window, SW_SHOWNOACTIVATE);
        if (!IsWindowVisible(window)) { DestroyWindow(window); return 87; }
    } else {
        if (!native_input(report, stage, L"ENTER", owner)) return 97;
        printf("NATIVE-STAGE-%02u-ENTER\n", stage); fflush(stdout);
    }
    if (!record(report, L"ENTER", stage, kinds[stage-1], owner, generation, 0, 0)) return 88;
    if (stage < stages) {
        swprintf_s(section, ARRAYSIZE(section), L"%u", stage + 1);
        if (!GetPrivateProfileStringW(section, L"command", L"", command, ARRAYSIZE(command), argv[1])) return 89;
        if (!CreateProcessW(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &startup, &child)) {
            record(report, L"LAUNCH-FAIL", stage, kinds[stage-1], owner, generation, 0, GetLastError()); return 90;
        }
        child_pid = child.dwProcessId; CloseHandle(child.hThread);
        /* Keep all GUI parents responsive while their direct launcher waits. */
        for (;;) {
            DWORD wait = MsgWaitForMultipleObjects(1, &child.hProcess, FALSE, 45000, QS_ALLINPUT);
            MSG message;
            if (wait == WAIT_OBJECT_0) break;
            if (wait != WAIT_OBJECT_0 + 1) { CloseHandle(child.hProcess); return 91; }
            while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage(&message); DispatchMessageW(&message);
            }
        }
        if (!GetExitCodeProcess(child.hProcess, &result)) { CloseHandle(child.hProcess); return 92; }
        CloseHandle(child.hProcess);
    }
    error = identity(kinds[stage-1], &returned_owner, &returned_generation);
    if (error || owner != returned_owner || generation != returned_generation) return 93;
    if (window) DestroyWindow(window);
    else {
        if (!native_input(report, stage, L"RETURN", owner)) return 98;
        printf("NATIVE-STAGE-%02u-RETURN-%lu\n", stage, result); fflush(stdout);
    }
    if (stage == 1 && !dos_records(report, stage, L"RETURN")) return 99;
    if (!record(report, L"RETURN", stage, kinds[stage-1], owner, generation, child_pid, result)) return 94;
    return (int)result;
}
