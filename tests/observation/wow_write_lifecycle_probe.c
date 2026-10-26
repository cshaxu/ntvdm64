/* Private-desktop WRITE lifecycle witness.  It sends WM_CLOSE only to the
 * test-owned visible MSWRITE_MENU window and observes its disappearance. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <wchar.h>

typedef struct write_window {
    DWORD process_id;
    HWND window;
} write_window;

static BOOL CALLBACK find_write_window(HWND window, LPARAM parameter)
{
    write_window *found = (write_window *)parameter;
    WCHAR class_name[64];
    DWORD process_id = 0;
    if (!IsWindowVisible(window) || !GetClassNameW(window, class_name,
            (int)(sizeof(class_name) / sizeof(class_name[0])))) return TRUE;
    (void)GetWindowThreadProcessId(window, &process_id);
    if (process_id == found->process_id && !wcscmp(class_name, L"MSWRITE_MENU")) {
        found->window = window;
        return FALSE;
    }
    return TRUE;
}

static BOOL find_write_window_on(HDESK desktop, DWORD process_id, HWND *window)
{
    write_window found;
    ZeroMemory(&found, sizeof(found));
    found.process_id = process_id;
    SetLastError(ERROR_SUCCESS);
    if (!EnumDesktopWindows(desktop, find_write_window, (LPARAM)&found) &&
            GetLastError() != ERROR_SUCCESS) return FALSE;
    *window = found.window;
    return TRUE;
}

static DWORD find_owned_worker(LPCWSTR runtime)
{
    HANDLE snapshot;
    PROCESSENTRY32W entry;
    WCHAR expected[MAX_PATH];
    DWORD result = 0;
    if (swprintf_s(expected, ARRAYSIZE(expected), L"%ls\\system32\\ntvdm.exe", runtime) < 0)
        return 0;
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return 0;
    ZeroMemory(&entry, sizeof(entry)); entry.dwSize = sizeof(entry);
    if (Process32FirstW(snapshot, &entry)) do {
        HANDLE process;
        WCHAR image[MAX_PATH]; DWORD length = ARRAYSIZE(image);
        if (_wcsicmp(entry.szExeFile, L"ntvdm.exe")) continue;
        process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, entry.th32ProcessID);
        if (!process) continue;
        if (QueryFullProcessImageNameW(process, 0, image, &length) &&
                !_wcsicmp(image, expected)) result = entry.th32ProcessID;
        CloseHandle(process);
        if (result) break;
    } while (Process32NextW(snapshot, &entry));
    CloseHandle(snapshot);
    return result;
}

static BOOL run_write_round(HDESK desktop, LPCWSTR desktop_name,
    LPCWSTR runtime, DWORD round)
{
    WCHAR image[MAX_PATH], command[MAX_PATH * 3];
    STARTUPINFOW startup;
    PROCESS_INFORMATION process;
    ULONGLONG deadline;
    HWND window = NULL;
    DWORD exit_code = STILL_ACTIVE, worker = 0;

    if (swprintf_s(image, ARRAYSIZE(image), L"%ls\\system32\\run16.exe", runtime) < 0 ||
            swprintf_s(command, ARRAYSIZE(command),
                L"\"%ls\" --wait \"%ls\\WRITE.EXE\"", image, runtime) < 0)
        return FALSE;
    ZeroMemory(&startup, sizeof(startup));
    startup.cb = sizeof(startup);
    startup.lpDesktop = (LPWSTR)desktop_name;
    ZeroMemory(&process, sizeof(process));
    if (!CreateProcessW(image, command, NULL, NULL, FALSE, 0, NULL, runtime,
            &startup, &process)) return FALSE;
    CloseHandle(process.hThread);
    deadline = GetTickCount64() + 20000;
    while (GetTickCount64() < deadline) {
        worker = find_owned_worker(runtime);
        if (worker && !find_write_window_on(desktop, worker, &window)) goto done;
        if (window) break;
        Sleep(25);
    }
    if (!window) SetLastError(ERROR_TIMEOUT);
    if (!window || !PostMessageW(window, WM_CLOSE, 0, 0)) goto done;
    deadline = GetTickCount64() + 20000;
    while (GetTickCount64() < deadline) {
        if (!find_write_window_on(desktop, worker, &window)) goto done;
        if (!window) break;
        Sleep(25);
    }
    if (window) SetLastError(ERROR_TIMEOUT);
    if (window || WaitForSingleObject(process.hProcess, 20000) != WAIT_OBJECT_0 ||
            !GetExitCodeProcess(process.hProcess, &exit_code) || exit_code != 0) goto done;
    wprintf(L"PASS WRITE round=%lu close=observed exit=0\n", (unsigned long)round);
    CloseHandle(process.hProcess);
    return TRUE;
done:
    fwprintf(stderr, L"FAIL WRITE round=%lu error=%lu exit=%lu\n", (unsigned long)round,
        GetLastError(), (unsigned long)exit_code);
    if (WaitForSingleObject(process.hProcess, 0) != WAIT_OBJECT_0)
        TerminateProcess(process.hProcess, 1);
    CloseHandle(process.hProcess);
    return FALSE;
}

int wmain(int argc, WCHAR **argv)
{
    WCHAR desktop_name[96];
    HDESK desktop;
    BOOL result;
    if (argc != 2 || !argv[1][0] ||
            swprintf_s(desktop_name, ARRAYSIZE(desktop_name), L"NTVDMConsoleTest-WRITE-%lu",
                (unsigned long)GetCurrentProcessId()) < 0)
        return ERROR_INVALID_PARAMETER;
    desktop = CreateDesktopW(desktop_name, NULL, NULL, 0, GENERIC_ALL, NULL);
    if (!desktop) return (int)GetLastError();
    result = run_write_round(desktop, desktop_name, argv[1], 1) &&
        run_write_round(desktop, desktop_name, argv[1], 2);
    CloseDesktop(desktop);
    return result ? 0 : 1;
}
