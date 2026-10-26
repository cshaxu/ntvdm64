/* Immutable SOL Options boundary probe.  The command ID is discovered from
 * SOL's own menu; the test does not encode guest resources or synthesize a
 * replacement dialog. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <psapi.h>
#include <dbghelp.h>

static DWORD owner_pid;
static HWND game_window;
static HWND option_dialog;

static void *wow32_base(DWORD pid)
{
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                 FALSE, pid);
    HMODULE modules[256];
    DWORD bytes = 0, index;
    if (!process || !EnumProcessModules(process, modules, sizeof(modules), &bytes)) {
        if (process) CloseHandle(process);
        return NULL;
    }
    for (index = 0; index < bytes / sizeof(modules[0]); ++index) {
        WCHAR name[MAX_PATH];
        if (GetModuleBaseNameW(process, modules[index], name, ARRAYSIZE(name)) &&
            !_wcsicmp(name, L"WOW32.DLL")) {
            CloseHandle(process);
            return modules[index];
        }
    }
    CloseHandle(process);
    return NULL;
}

static void report_wow32_module(DWORD pid)
{
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                 FALSE, pid);
    HMODULE modules[256];
    DWORD bytes = 0, index;
    if (!process || !EnumProcessModules(process, modules, sizeof(modules), &bytes)) {
        if (process) CloseHandle(process);
        return;
    }
    for (index = 0; index < bytes / sizeof(modules[0]); ++index) {
        WCHAR name[MAX_PATH], path[MAX_PATH];
        if (GetModuleBaseNameW(process, modules[index], name, ARRAYSIZE(name)) &&
                !_wcsicmp(name, L"WOW32.DLL")) {
            if (GetModuleFileNameExW(process, modules[index], path, ARRAYSIZE(path)))
                printf("SOL options: wow32-module=%ls\n", path);
            break;
        }
    }
    CloseHandle(process);
}

static void report_fault_module(DWORD pid, const void *address)
{
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                 FALSE, pid);
    HMODULE modules[256];
    DWORD bytes = 0, index;
    if (!process || !EnumProcessModules(process, modules, sizeof(modules), &bytes)) {
        if (process) CloseHandle(process);
        return;
    }
    for (index = 0; index < bytes / sizeof(modules[0]); ++index) {
        MODULEINFO info;
        WCHAR name[MAX_PATH];
        if (!GetModuleInformation(process, modules[index], &info, sizeof(info))) continue;
        if ((const unsigned char *)address >= (unsigned char *)info.lpBaseOfDll &&
            (const unsigned char *)address < (unsigned char *)info.lpBaseOfDll + info.SizeOfImage) {
            GetModuleBaseNameW(process, modules[index], name, ARRAYSIZE(name));
            printf("SOL options: fault-module=%ls base=%p size=%lu rva=%08lx\n",
                name, info.lpBaseOfDll, info.SizeOfImage,
                (DWORD)((const unsigned char *)address - (unsigned char *)info.lpBaseOfDll));
            break;
        }
    }
    CloseHandle(process);
}

static void report_unknown_code(DWORD pid, const void *address)
{
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                 FALSE, pid);
    MEMORY_BASIC_INFORMATION memory;
    BYTE bytes[16] = {0};
    SIZE_T read = 0;
    unsigned index;
    if (!process || !VirtualQueryEx(process, address, &memory, sizeof(memory))) goto done;
    printf("SOL options: code-region base=%p size=%llu state=%lx protect=%lx type=%lx bytes@pc=",
        memory.BaseAddress, (unsigned long long)memory.RegionSize, memory.State,
        memory.Protect, memory.Type);
    if (ReadProcessMemory(process, address, bytes, sizeof(bytes), &read)) {
        for (index = 0; index < read; ++index) printf("%02x", bytes[index]);
    }
    puts("");
done:
    if (process) CloseHandle(process);
}

static void report_data_region(DWORD pid, const char *label, const void *address)
{
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                 FALSE, pid);
    MEMORY_BASIC_INFORMATION memory;
    if (!process || !VirtualQueryEx(process, address, &memory, sizeof(memory))) {
        printf("SOL options: %s-region=unmapped address=%p\n", label, address);
        if (process) CloseHandle(process);
        return;
    }
    printf("SOL options: %s-region base=%p size=%llu state=%lx protect=%lx type=%lx address=%p\n",
        label, memory.BaseAddress, (unsigned long long)memory.RegionSize,
        memory.State, memory.Protect, memory.Type, address);
    CloseHandle(process);
}

static void report_stack_words(DWORD pid, DWORD stack)
{
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                 FALSE, pid);
    DWORD words[16] = {0};
    SIZE_T read = 0;
    unsigned index;
    if (!process || !ReadProcessMemory(process, (const void *)(ULONG_PTR)stack,
            words, sizeof(words), &read)) {
        printf("SOL options: stack-words unavailable esp=%08lx\n", stack);
        if (process) CloseHandle(process);
        return;
    }
    printf("SOL options: stack-words esp=%08lx", stack);
    for (index = 0; index < read / sizeof(words[0]); ++index)
        printf(" %08lx", words[index]);
    puts("");
    for (index = 0; index < read / sizeof(words[0]); ++index)
        if (words[index] >= 0x10000)
            report_fault_module(pid, (const void *)(ULONG_PTR)words[index]);
    CloseHandle(process);
}

static void report_fault_stack(DWORD pid, DWORD tid)
{
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                 FALSE, pid);
    HANDLE thread = OpenThread(THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION,
                               FALSE, tid);
    CONTEXT context = { .ContextFlags = CONTEXT_FULL };
    STACKFRAME64 frame = {0};
    unsigned index;
    if (!process || !thread || !GetThreadContext(thread, &context)) goto done;
    frame.AddrPC.Offset = context.Eip; frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = context.Ebp; frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrStack.Offset = context.Esp; frame.AddrStack.Mode = AddrModeFlat;
    SymInitialize(process, NULL, TRUE);
    for (index = 0; index != 8 && StackWalk64(IMAGE_FILE_MACHINE_I386, process,
             thread, &frame, &context, NULL, SymFunctionTableAccess64,
             SymGetModuleBase64, NULL); ++index) {
        printf("SOL options: stack[%u]=%08llx\n", index,
               (unsigned long long)frame.AddrPC.Offset);
        report_fault_module(pid, (const void *)(ULONG_PTR)frame.AddrPC.Offset);
        if (index == 1) report_unknown_code(pid, (const void *)(ULONG_PTR)frame.AddrPC.Offset);
    }
    SymCleanup(process);
done:
    if (thread) CloseHandle(thread);
    if (process) CloseHandle(process);
}

/* Capture an actual first-chance AV before Windows terminates the worker.
 * This is diagnostic-only and leaves all application input on its normal menu
 * route. */
static BOOL capture_fault(HWND window, UINT command, DWORD pid)
{
    DEBUG_EVENT event;
    ULONGLONG deadline = GetTickCount64() + 10000;
    BOOL attached = FALSE;
    unsigned char *base = wow32_base(pid);
    report_wow32_module(pid);
    DebugSetProcessKillOnExit(FALSE);
    if (!DebugActiveProcess(pid)) {
        printf("SOL options: debug attach failed error=%lu\n", GetLastError());
        return FALSE;
    }
    attached = TRUE;
    if (!PostMessageW(window, WM_COMMAND, command, 0)) {
        printf("SOL options: command post failed error=%lu\n", GetLastError());
        DebugActiveProcessStop(pid);
        return FALSE;
    }
    while (GetTickCount64() < deadline) {
        DWORD remaining = (DWORD)(deadline - GetTickCount64());
        DWORD disposition = DBG_CONTINUE;
        if (!WaitForDebugEvent(&event, remaining)) break;
        if (event.dwDebugEventCode == EXCEPTION_DEBUG_EVENT) {
            DWORD code = event.u.Exception.ExceptionRecord.ExceptionCode;
            if (code == EXCEPTION_ACCESS_VIOLATION) {
                CONTEXT context = { .ContextFlags = CONTEXT_INTEGER | CONTEXT_CONTROL };
                HANDLE fault_thread = OpenThread(THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION,
                    FALSE, event.dwThreadId);
                BOOL have_context = FALSE;
                printf("SOL options: access-violation=%p wow32-base=%p rva=%08lx\n",
                    event.u.Exception.ExceptionRecord.ExceptionAddress, base,
                    base ? (DWORD)((unsigned char *)event.u.Exception.ExceptionRecord.ExceptionAddress - base) : 0);
                printf("SOL options: access-kind=%llu address=%p parameters=%lu\n",
                    (unsigned long long)event.u.Exception.ExceptionRecord.ExceptionInformation[0],
                    (void *)(ULONG_PTR)event.u.Exception.ExceptionRecord.ExceptionInformation[1],
                    event.u.Exception.ExceptionRecord.NumberParameters);
                if (fault_thread && GetThreadContext(fault_thread, &context)) {
                    have_context = TRUE;
                    printf("SOL options: regs eax=%08lx ebx=%08lx ecx=%08lx edx=%08lx esi=%08lx edi=%08lx ebp=%08lx esp=%08lx\n",
                        context.Eax, context.Ebx, context.Ecx, context.Edx,
                        context.Esi, context.Edi, context.Ebp, context.Esp);
                }
                if (have_context)
                    report_data_region(pid, "esi", (const void *)(ULONG_PTR)context.Esi);
                report_data_region(pid, "access", (const void *)(ULONG_PTR)
                    event.u.Exception.ExceptionRecord.ExceptionInformation[1]);
                if (have_context) report_stack_words(pid, context.Esp);
                if (fault_thread) CloseHandle(fault_thread);
                report_fault_module(pid, event.u.Exception.ExceptionRecord.ExceptionAddress);
                report_fault_stack(pid, event.dwThreadId);
                ContinueDebugEvent(event.dwProcessId, event.dwThreadId,
                    DBG_EXCEPTION_NOT_HANDLED);
                DebugActiveProcessStop(pid);
                return TRUE;
            }
            if (!event.u.Exception.dwFirstChance)
                disposition = DBG_EXCEPTION_NOT_HANDLED;
        }
        ContinueDebugEvent(event.dwProcessId, event.dwThreadId, disposition);
    }
    if (attached) DebugActiveProcessStop(pid);
    return FALSE;
}

static BOOL CALLBACK find_game(HWND window, LPARAM unused)
{
    WCHAR name[64];
    DWORD pid = 0;
    (void)unused;
    GetWindowThreadProcessId(window, &pid);
    if ((!owner_pid || pid == owner_pid) && IsWindowVisible(window) &&
        GetClassNameW(window, name, ARRAYSIZE(name)) &&
        !wcscmp(name, L"Solitaire")) {
        owner_pid = pid;
        game_window = window;
    }
    return TRUE;
}

static BOOL CALLBACK find_options(HWND window, LPARAM unused)
{
    WCHAR name[64], title[128];
    DWORD pid = 0;
    (void)unused;
    GetWindowThreadProcessId(window, &pid);
    if (pid != owner_pid || !IsWindowVisible(window) ||
        !GetClassNameW(window, name, ARRAYSIZE(name)) ||
        wcscmp(name, L"#32770") ||
        !GetWindowTextW(window, title, ARRAYSIZE(title))) {
        return TRUE;
    }
    if (wcsstr(title, L"Options") != NULL) option_dialog = window;
    return TRUE;
}

static UINT options_command(HWND window)
{
    HMENU top = GetMenu(window);
    int outer;
    if (!top) return 0;
    for (outer = 0; outer < GetMenuItemCount(top); ++outer) {
        HMENU popup = GetSubMenu(top, (int)outer);
        int item;
        if (!popup) continue;
        for (item = 0; item < GetMenuItemCount(popup); ++item) {
            WCHAR text[128] = L"";
            MENUITEMINFOW info = { sizeof(info), MIIM_STRING | MIIM_ID };
            info.dwTypeData = text;
            info.cch = ARRAYSIZE(text) - 1;
            if (!GetMenuItemInfoW(popup, item, TRUE, &info)) continue;
            if (wcsstr(text, L"Options") != NULL) return info.wID;
        }
    }
    return 0;
}

static int probe_active_sol(DWORD pid, HDESK desktop)
{
    HANDLE process;
    DWORD_PTR reply = 0;
    UINT command;
    ULONGLONG deadline;
    owner_pid = pid;
    process = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION,
                          FALSE, owner_pid);
    if (!process || !desktop) return 3;
    deadline = GetTickCount64() + 10000;
    do {
        EnumDesktopWindows(desktop, find_game, 0);
        if (game_window) break;
        Sleep(25);
    } while (GetTickCount64() < deadline &&
             WaitForSingleObject(process, 0) == WAIT_TIMEOUT);
    if (!game_window ||
        !SendMessageTimeoutW(game_window, WM_NULL, 0, 0, SMTO_ABORTIFHUNG,
                             2000, &reply)) {
        printf("SOL options: no responsive game window\n");
        return 4;
    }
    command = options_command(game_window);
    if (!command) {
        printf("SOL options: no Options command in guest menu\n");
        return 5;
    }
    printf("SOL options: command=%u\n", command);
    /* The attribution mode must stop at the first host exception.  A repaired
     * candidate instead needs the ordinary modal completion witness; attaching
     * a debugger while the host delivers the owner-disable callback changes
     * that delivery context and is not that witness. */
    if (GetEnvironmentVariableA("MVDM_WOW_SOL_OPTIONS_NO_DEBUG", NULL, 0)) {
        if (!PostMessageW(game_window, WM_COMMAND, command, 0)) {
            printf("SOL options: command post failed error=%lu\n", GetLastError());
            return 6;
        }
    } else if (capture_fault(game_window, command, owner_pid)) return 11;
    deadline = GetTickCount64() + 10000;
    do {
        option_dialog = NULL;
        EnumDesktopWindows(desktop, find_options, 0);
        if (option_dialog) break;
        if (WaitForSingleObject(process, 0) != WAIT_TIMEOUT) {
            printf("SOL options: worker exited before modal dialog\n");
            return 7;
        }
        Sleep(25);
    } while (GetTickCount64() < deadline);
    if (!option_dialog) {
        printf("SOL options: modal dialog did not appear\n");
        return 8;
    }
    if (!PostMessageW(option_dialog, WM_CLOSE, 0, 0)) {
        printf("SOL options: dialog close post failed error=%lu\n", GetLastError());
        return 9;
    }
    deadline = GetTickCount64() + 10000;
    do {
        option_dialog = NULL;
        EnumDesktopWindows(desktop, find_options, 0);
        if (!option_dialog) break;
        Sleep(25);
    } while (GetTickCount64() < deadline);
    if (option_dialog || WaitForSingleObject(process, 0) != WAIT_TIMEOUT ||
        !SendMessageTimeoutW(game_window, WM_NULL, 0, 0, SMTO_ABORTIFHUNG,
                             2000, &reply)) {
        printf("SOL options: modal close or parent recovery failed\n");
        return 10;
    }
    puts("PASS SOL Options command, modal dialog, close and parent recovery");
    CloseHandle(process);
    return 0;
}

/* The controller owns the test desktop and starts both the product and the
 * observer child on it.  An outside process cannot reliably open a desktop
 * created in another test window station, so this deliberately avoids the
 * former cross-station attach race.  It remains an unswitched, message-only
 * probe: no foreground activation, SendInput or guest modification. */
static int run_private_probe(const char *runtime)
{
    char run16[MAX_PATH], sol[MAX_PATH], command[2 * MAX_PATH + 64];
    STARTUPINFOA startup = { sizeof(startup) };
    PROCESS_INFORMATION launcher = {0};
    HDESK desktop = GetThreadDesktop(GetCurrentThreadId());
    ULONGLONG deadline;
    DWORD code = ERROR_PROCESS_ABORTED;
    int result;
    if (!desktop || !GetFullPathNameA(runtime, MAX_PATH, run16, NULL) ||
            strlen(run16) + sizeof("\\system32\\run16.exe") > sizeof(run16))
        return 21;
    strcat_s(run16, sizeof(run16), "\\system32\\run16.exe");
    if (strlen(runtime) + sizeof("\\SOL.EXE") > sizeof(sol)) return 21;
    strcpy_s(sol, sizeof(sol), runtime); strcat_s(sol, sizeof(sol), "\\SOL.EXE");
    sprintf_s(command, sizeof(command), "\"%s\" --wait \"%s\"", run16, sol);
    /* The child owns a private desktop, so it may create the real visible
     * application window there.  SW_HIDE is inherited by the WOW child and
     * would prevent the very Options dialog this probe is meant to witness. */
    startup.dwFlags = STARTF_USESHOWWINDOW;
    startup.wShowWindow = SW_SHOWNORMAL;
    if (!CreateProcessA(run16, command, NULL, NULL, FALSE, CREATE_NEW_CONSOLE,
            NULL, runtime, &startup, &launcher)) {
        printf("SOL options: launch failed error=%lu\n", GetLastError());
        return 21;
    }
    CloseHandle(launcher.hThread);
    deadline = GetTickCount64() + 60000;
    owner_pid = 0; game_window = NULL;
    do {
        EnumDesktopWindows(desktop, find_game, 0);
        if (game_window) break;
        if (WaitForSingleObject(launcher.hProcess, 0) != WAIT_TIMEOUT) break;
        Sleep(25);
    } while (GetTickCount64() < deadline);
    if (!game_window || !owner_pid) {
        GetExitCodeProcess(launcher.hProcess, &code);
        printf("SOL options: no owned Solitaire window launcher=%lu\n", code);
        result = 22;
    } else {
        result = probe_active_sol(owner_pid, desktop);
        if (!result && !PostMessageW(game_window, WM_CLOSE, 0, 0)) {
            printf("SOL options: game close post failed error=%lu\n", GetLastError());
            result = 23;
        }
    }
    if (WaitForSingleObject(launcher.hProcess, 65000) != WAIT_OBJECT_0) {
        printf("SOL options: launcher did not finish after owned close\n");
        TerminateProcess(launcher.hProcess, ERROR_PROCESS_ABORTED);
        WaitForSingleObject(launcher.hProcess, 5000);
        result = result ? result : 24;
    } else if (!result && (!GetExitCodeProcess(launcher.hProcess, &code) || code)) {
        printf("SOL options: launcher exit=%lu\n", code);
        result = 24;
    }
    CloseHandle(launcher.hProcess);
    return result;
}

/* Invoked by the ordinary console observer after it has started SOL.  This
 * child inherits that observer's private desktop and Console ownership. */
static int attach_existing_sol(const char *report)
{
    HDESK desktop = GetThreadDesktop(GetCurrentThreadId());
    ULONGLONG deadline = GetTickCount64() + 60000;
    int result;
    if (!freopen(report, "w", stdout)) return 26;
    setvbuf(stdout, NULL, _IONBF, 0);
    owner_pid = 0; game_window = NULL;
    do {
        EnumDesktopWindows(desktop, find_game, 0);
        if (game_window && owner_pid) break;
        Sleep(25);
    } while (GetTickCount64() < deadline);
    if (!game_window || !owner_pid) {
        puts("SOL options: no owned Solitaire window");
        return 22;
    }
    result = probe_active_sol(owner_pid, desktop);
    if (!result && !PostMessageW(game_window, WM_CLOSE, 0, 0)) {
        printf("SOL options: game close post failed error=%lu\n", GetLastError());
        result = 23;
    }
    return result;
}

static int launch_private_probe(const char *runtime, const char *report)
{
    char self[MAX_PATH], desktop_name[96], command[3 * MAX_PATH + 96];
    STARTUPINFOA startup = { sizeof(startup) };
    PROCESS_INFORMATION child = {0};
    HDESK desktop;
    DWORD code = 25;
    if (!GetModuleFileNameA(NULL, self, sizeof(self))) return 25;
    sprintf_s(desktop_name, sizeof(desktop_name), "NTVDMSolOptions-%lu",
        GetCurrentProcessId());
    desktop = CreateDesktopA(desktop_name, NULL, NULL, 0, GENERIC_ALL, NULL);
    if (!desktop) return 25;
    sprintf_s(command, sizeof(command), "\"%s\" child \"%s\" \"%s\"",
        self, runtime, report);
    startup.lpDesktop = desktop_name;
    startup.dwFlags = STARTF_USESHOWWINDOW;
    startup.wShowWindow = SW_HIDE;
    if (CreateProcessA(self, command, NULL, NULL, FALSE, CREATE_NEW_CONSOLE,
            NULL, runtime, &startup, &child)) {
        CloseHandle(child.hThread);
        if (WaitForSingleObject(child.hProcess, 140000) == WAIT_OBJECT_0)
            GetExitCodeProcess(child.hProcess, &code);
        else {
            TerminateProcess(child.hProcess, ERROR_PROCESS_ABORTED);
            WaitForSingleObject(child.hProcess, 5000);
        }
        CloseHandle(child.hProcess);
    }
    CloseDesktop(desktop);
    return (int)code;
}

int main(int argc, char **argv)
{
    if (argc == 3)
        return !strcmp(argv[1], "attach") ? attach_existing_sol(argv[2]) :
            launch_private_probe(argv[1], argv[2]);
    if (argc == 4 && !strcmp(argv[1], "child")) {
        if (!freopen(argv[3], "w", stdout)) return 26;
        setvbuf(stdout, NULL, _IONBF, 0);
        return run_private_probe(argv[2]);
    }
    return 2;
}
