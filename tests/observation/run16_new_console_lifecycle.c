/* Explorer-equivalent Console ownership probe. Runs only on its own desktop,
 * never attaches to the child's Console (which would change sole ownership).
 * Compare with console_startup_observer's retained-parent cases. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

typedef struct child_watch {
    DWORD pid;
    HANDLE process;
    WCHAR name[MAX_PATH];
} child_watch;

static void collect_children(DWORD parent, child_watch watches[8], unsigned *count)
{
    PROCESSENTRY32W entry = { sizeof(entry) };
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return;
    if (Process32FirstW(snapshot, &entry)) do {
        unsigned index;
        if (entry.th32ParentProcessID != parent ||
            (_wcsicmp(entry.szExeFile, L"ntvdm.exe") &&
             _wcsicmp(entry.szExeFile, L"basesrv.exe"))) continue;
        for (index = 0; index < *count; ++index)
            if (watches[index].pid == entry.th32ProcessID) break;
        if (index < *count || *count == 8) continue;
        watches[index].process = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION |
            PROCESS_TERMINATE, FALSE, entry.th32ProcessID);
        if (!watches[index].process) continue;
        watches[index].pid = entry.th32ProcessID;
        wcscpy_s(watches[index].name, MAX_PATH, entry.szExeFile);
        ++*count;
    } while (Process32NextW(snapshot, &entry));
    CloseHandle(snapshot);
}

static BOOL CALLBACK count_consoles(HWND window, LPARAM parameter)
{
    WCHAR name[80];
    if (GetClassNameW(window, name, ARRAYSIZE(name)) &&
        !wcscmp(name, L"ConsoleWindowClass")) ++*(unsigned *)parameter;
    return TRUE;
}

static void timeout_threads(FILE *report, const child_watch *watch)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    HANDLE process = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, watch->pid);
    THREADENTRY32 entry = { sizeof(entry) };
    if (snapshot != INVALID_HANDLE_VALUE && process && Thread32First(snapshot, &entry)) do {
        HANDLE thread;
        CONTEXT context = { 0 };
        DWORD stack[16];
        SIZE_T bytes;
        unsigned index;
        if (entry.th32OwnerProcessID != watch->pid) continue;
        thread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT, FALSE, entry.th32ThreadID);
        if (!thread) continue;
        if (SuspendThread(thread) != (DWORD)-1) {
            context.ContextFlags = CONTEXT_CONTROL;
            if (GetThreadContext(thread, &context)) {
                fprintf(report, "timeout-pid=%lu tid=%lu ip=%08lx sp=%08lx\n",
                    watch->pid, entry.th32ThreadID, context.Eip, context.Esp);
                {
                    MEMORY_BASIC_INFORMATION region;
                    char text[8192];
                    if (VirtualQueryEx(process, (void *)context.Esp, &region, sizeof(region))) {
                        SIZE_T size = (SIZE_T)region.BaseAddress + region.RegionSize - context.Esp;
                        if (size > sizeof(text) - 1) size = sizeof(text) - 1;
                        if (ReadProcessMemory(process, (void *)context.Esp, text, size, &bytes)) {
                            SIZE_T offset;
                            text[bytes] = 0;
                            for (offset = 0; offset + 3 < bytes; ++offset)
                                if (!memcmp(text + offset, "CS:", 3))
                                    fprintf(report, "host-fault-text=%.100s\n", text + offset);
                        }
                    }
                }
                {
                    DWORD frame = context.Ebp, chain[2];
                    unsigned depth;
                    fputs("frame-returns=", report);
                    for (depth = 0; depth < 32 && frame; ++depth) {
                        if (!ReadProcessMemory(process, (void *)frame, chain, sizeof(chain), &bytes) ||
                            bytes != sizeof(chain)) break;
                        fprintf(report, "%08lx ", chain[1]);
                        if (chain[0] <= frame || chain[0] - frame > 0x100000) break;
                        frame = chain[0];
                    }
                    fputc('\n', report);
                }
                if (ReadProcessMemory(process, (void *)context.Esp, stack, sizeof(stack), &bytes)) {
                    fputs("stack=", report);
                    for (index = 0; index < bytes / sizeof(DWORD); ++index)
                        fprintf(report, "%08lx ", stack[index]);
                    fputc('\n', report);
                }
            }
            ResumeThread(thread);
        }
        CloseHandle(thread);
    } while (Thread32Next(snapshot, &entry));
    if (snapshot != INVALID_HANDLE_VALUE) CloseHandle(snapshot);
    if (process) CloseHandle(process);
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, watch->pid);
    if (snapshot != INVALID_HANDLE_VALUE) {
        MODULEENTRY32W module = { sizeof(module) };
        if (Module32FirstW(snapshot, &module)) do {
            fprintf(report, "module=%ls base=%p size=%lu\n", module.szModule,
                module.modBaseAddr, module.modBaseSize);
        } while (Module32NextW(snapshot, &module));
        CloseHandle(snapshot);
    }
}

int wmain(int argc, WCHAR **argv)
{
    WCHAR desktop_name[64], command[2048];
    STARTUPINFOW startup = { sizeof(startup) };
    PROCESS_INFORMATION child = { 0 };
    child_watch watches[8] = { 0 };
    unsigned count = 0, index, windows = 0, workers = 0, live_workers = 0;
    DWORD started, wait = WAIT_TIMEOUT, code = STILL_ACTIVE, enumeration_error;
    DWORD expected_code = 0;
    unsigned expected_workers = 1;
    HDESK desktop;
    FILE *report;
    BOOL enumeration, passed;
    if (argc != 4 && argc != 6) return 64; /* root, command, report, [exit, workers] */
    if (argc == 6) {
        WCHAR *end;
        unsigned __int64 value = _wcstoui64(argv[4], &end, 10);
        if (!argv[4][0] || *end || value > MAXDWORD) return 64;
        expected_code = (DWORD)value;
        if (wcscmp(argv[5], L"0") && wcscmp(argv[5], L"1")) return 64;
        expected_workers = argv[5][0] - L'0';
    }
    if (_wfopen_s(&report, argv[3], L"w")) return 65;
    swprintf_s(desktop_name, ARRAYSIZE(desktop_name), L"NTVDMLifecycle-%lu", GetCurrentProcessId());
    desktop = CreateDesktopW(desktop_name, NULL, NULL, 0, GENERIC_ALL, NULL);
    if (!desktop) { fclose(report); return 66; }
    if (swprintf_s(command, ARRAYSIZE(command), L"\"%s\\run16.exe\" %s", argv[1], argv[2]) < 0) {
        CloseDesktop(desktop); fclose(report); return 64;
    }
    startup.lpDesktop = desktop_name;
    startup.dwFlags = STARTF_USESHOWWINDOW;
    /* Explorer normally supplies a visible window. It is visible only on
     * this non-input desktop, not on the owner's desktop. */
    startup.wShowWindow = SW_SHOWNORMAL;
    if (!CreateProcessW(NULL, command, NULL, NULL, FALSE, CREATE_NEW_CONSOLE,
                        NULL, argv[1], &startup, &child)) {
        fprintf(report, "create-error=%lu\n", GetLastError());
        CloseDesktop(desktop); fclose(report); return 67;
    }
    CloseHandle(child.hThread);
    started = GetTickCount();
    do {
        collect_children(child.dwProcessId, watches, &count);
        wait = WaitForSingleObject(child.hProcess, 20);
    } while (wait == WAIT_TIMEOUT && GetTickCount() - started < 20000);
    GetExitCodeProcess(child.hProcess, &code);
    fprintf(report, "launcher=%lu wait=%lu exit=%lu elapsed-ms=%lu\n",
        child.dwProcessId, wait, code, GetTickCount() - started);
    /* Observe normal teardown without holding the Console or killing any
     * participant. Broker residency is permitted; attached worker is not. */
    if (wait == WAIT_OBJECT_0) Sleep(5000);
    for (index = 0; index < count; ++index) {
        DWORD state = WaitForSingleObject(watches[index].process, 0), exit_code = STILL_ACTIVE;
        GetExitCodeProcess(watches[index].process, &exit_code);
        fprintf(report, "child=%lu name=%ls wait=%lu exit=%lu\n",
            watches[index].pid, watches[index].name, state, exit_code);
        if (!_wcsicmp(watches[index].name, L"ntvdm.exe")) {
            ++workers;
            if (state != WAIT_OBJECT_0) ++live_workers;
            if (wait == WAIT_TIMEOUT) timeout_threads(report, &watches[index]);
        }
    }
    {
        HDESK previous = GetThreadDesktop(GetCurrentThreadId());
        HWND sentinel = NULL;
        enumeration = FALSE;
        /* Give enumeration a known non-Console window: an entirely empty
         * desktop can return FALSE, which is not proof of absence. This
         * hidden sentinel neither switches desktops nor attaches a Console. */
        if (SetThreadDesktop(desktop))
            sentinel = CreateWindowExW(0, L"STATIC", L"lifecycle-observer", WS_POPUP,
                0, 0, 1, 1, NULL, NULL, GetModuleHandleW(NULL), NULL);
        enumeration_error = sentinel ? ERROR_SUCCESS : GetLastError();
        if (sentinel) {
            SetLastError(ERROR_SUCCESS);
            enumeration = EnumDesktopWindows(desktop, count_consoles, (LPARAM)&windows);
            enumeration_error = enumeration ? ERROR_SUCCESS : GetLastError();
            DestroyWindow(sentinel);
        }
        if (!SetThreadDesktop(previous)) { enumeration = FALSE; enumeration_error = GetLastError(); }
    }
    passed = wait == WAIT_OBJECT_0 && code == expected_code && workers == expected_workers &&
        live_workers == 0 && enumeration && windows == 0;
    fprintf(report, "workers=%u live-workers=%u console-windows=%u enumerated=%u error=%lu pass=%u\n",
        workers, live_workers, windows, enumeration, enumeration_error, passed);
    fclose(report);
    /* Cleanup occurs after recording the verdict, only through retained
     * handles of this launch's children. It is never acceptance evidence. */
    if (wait != WAIT_OBJECT_0) TerminateProcess(child.hProcess, 69);
    for (index = 0; index < count; ++index) {
        if (WaitForSingleObject(watches[index].process, 0) == WAIT_TIMEOUT) {
            TerminateProcess(watches[index].process, 69);
            WaitForSingleObject(watches[index].process, 1000);
        }
        CloseHandle(watches[index].process);
    }
    CloseHandle(child.hProcess);
    CloseDesktop(desktop);
    return passed ? 0 : 1;
}
