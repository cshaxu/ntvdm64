/* Real Console control events, not keyboard injection. All processes run on
 * a private desktop; no SwitchDesktop, global input, or product Job object. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <wchar.h>

static HANDLE received;
static DWORD expected_event;
static BOOL WINAPI child_control(DWORD event)
{
    if (event != expected_event) return FALSE;
    SetEvent(received);
    return TRUE;
}
static BOOL WINAPI supervisor_control(DWORD event)
{
    return event == CTRL_C_EVENT || event == CTRL_BREAK_EVENT;
}

static HANDLE find_native_child(DWORD launcher)
{
    HANDLE result = NULL;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32W entry = { sizeof(entry) };
    if (snapshot == INVALID_HANDLE_VALUE) return NULL;
    if (Process32FirstW(snapshot, &entry)) do {
        if (entry.th32ParentProcessID == launcher) {
            result = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION |
                PROCESS_TERMINATE, FALSE, entry.th32ProcessID);
            break;
        }
    } while (Process32NextW(snapshot, &entry));
    CloseHandle(snapshot);
    return result;
}

int wmain(int argc, WCHAR **argv)
{
    WCHAR self[MAX_PATH], command[4096], name[96];
    STARTUPINFOW startup = { sizeof(startup) };
    PROCESS_INFORMATION process = { 0 };
    DWORD code = 69, native_code = STILL_ACTIVE, native_wait = WAIT_FAILED;
    HANDLE ready = NULL, native = NULL;
    FILE *report;
    BOOL generated = FALSE, passed = FALSE;
    if (argc != 5 || (wcscmp(argv[4], L"c") && wcscmp(argv[4], L"break"))) return 64;
    expected_event = !wcscmp(argv[4], L"c") ? CTRL_C_EVENT : CTRL_BREAK_EVENT;
    if (!wcscmp(argv[1], L"--child")) {
        ready = OpenEventW(EVENT_MODIFY_STATE, FALSE, argv[2]);
        received = CreateEventW(NULL, TRUE, FALSE, NULL);
        if (!ready || !received || !SetConsoleCtrlHandler(child_control, TRUE)) return 65;
        SetEvent(ready);
        CloseHandle(ready);
        code = WaitForSingleObject(received, 10000);
        CloseHandle(received);
        /* The launcher must still wait while the child finishes cleanup. */
        Sleep(200);
        return code == WAIT_OBJECT_0 ? 37 : 66;
    }
    if (!GetModuleFileNameW(NULL, self, ARRAYSIZE(self))) return 65;
    if (wcscmp(argv[1], L"--supervisor")) {
        HDESK desktop;
        swprintf_s(name, ARRAYSIZE(name), L"NTVDMControl-%lu", GetCurrentProcessId());
        desktop = CreateDesktopW(name, NULL, NULL, 0, GENERIC_ALL, NULL);
        if (!desktop) return 65;
        startup.lpDesktop = name;
        startup.dwFlags = STARTF_USESHOWWINDOW;
        startup.wShowWindow = SW_HIDE;
        swprintf_s(command, ARRAYSIZE(command), L"\"%ls\" --supervisor \"%ls\" \"%ls\" %ls",
            self, argv[2], argv[3], argv[4]);
        if (CreateProcessW(NULL, command, NULL, NULL, FALSE, CREATE_NEW_CONSOLE,
                NULL, NULL, &startup, &process)) {
            if (WaitForSingleObject(process.hProcess, 30000) == WAIT_OBJECT_0)
                GetExitCodeProcess(process.hProcess, &code);
            else TerminateProcess(process.hProcess, 69);
            CloseHandle(process.hThread);
            CloseHandle(process.hProcess);
        }
        CloseDesktop(desktop);
        return (int)code;
    }
    if (_wfopen_s(&report, argv[3], L"w")) return 65;
    swprintf_s(name, ARRAYSIZE(name), L"Local\\NTVDMControlReady-%lu", GetCurrentProcessId());
    ready = CreateEventW(NULL, TRUE, FALSE, name);
    if (!ready || !SetConsoleCtrlHandler(supervisor_control, TRUE)) goto done;
    swprintf_s(command, ARRAYSIZE(command), L"\"%ls\" \"%ls\" --child %ls unused %ls",
        argv[2], self, name, argv[4]);
    if (!CreateProcessW(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &startup, &process)) goto done;
    if (WaitForSingleObject(ready, 10000) != WAIT_OBJECT_0) goto done;
    native = find_native_child(process.dwProcessId);
    if (!native) goto done;
    generated = GenerateConsoleCtrlEvent(expected_event, 0);
    if (!generated) goto done;
    native_wait = WaitForSingleObject(native, 10000);
    GetExitCodeProcess(native, &native_code);
    if (WaitForSingleObject(process.hProcess, 5000) == WAIT_OBJECT_0)
        GetExitCodeProcess(process.hProcess, &code);
    passed = native_wait == WAIT_OBJECT_0 && native_code == 37 && code == native_code;
done:
    fprintf(report, "event=%lu generated=%u native-wait=%lu native-exit=%lu launcher-exit=%lu pass=%u\n",
        expected_event, generated, native_wait, native_code, code, passed);
    fclose(report);
    /* Cleanup after verdict only, through exact retained test handles. */
    if (native) {
        if (WaitForSingleObject(native, 0) == WAIT_TIMEOUT) TerminateProcess(native, 69);
        CloseHandle(native);
    }
    if (process.hProcess) {
        if (WaitForSingleObject(process.hProcess, 0) == WAIT_TIMEOUT) TerminateProcess(process.hProcess, 69);
        CloseHandle(process.hThread);
        CloseHandle(process.hProcess);
    }
    if (ready) CloseHandle(ready);
    return passed ? 0 : 1;
}
