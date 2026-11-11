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
#include <tlhelp32.h>

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

#define WOW_PROBE_PREFIX "SOL options: "
#include "wow_probe_thread_snapshot.h"

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

/* One diagnostic snapshot after a failed assertion, never part of successful
 * interaction timing. Only this probe's selected worker is sampled. */


/* Capture access violations and ownership assertions before worker termination.
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
            if (code == EXCEPTION_ACCESS_VIOLATION || code == 0xc0000420u) {
                CONTEXT context = { .ContextFlags = CONTEXT_INTEGER | CONTEXT_CONTROL };
                HANDLE fault_thread = OpenThread(THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION,
                    FALSE, event.dwThreadId);
                BOOL have_context = FALSE;
                printf("SOL options: exception-code=%08lx first-chance=%lu\n",
                    code, event.u.Exception.dwFirstChance);
                printf("SOL options: exception-address=%p wow32-base=%p rva=%08lx\n",
                    event.u.Exception.ExceptionRecord.ExceptionAddress, base,
                    base ? (DWORD)((unsigned char *)event.u.Exception.ExceptionRecord.ExceptionAddress - base) : 0);
                if (code == EXCEPTION_ACCESS_VIOLATION)
                    printf("SOL options: access-kind=%llu address=%p parameters=%lu\n",
                    (unsigned long long)event.u.Exception.ExceptionRecord.ExceptionInformation[0],
                    (void *)(ULONG_PTR)event.u.Exception.ExceptionRecord.ExceptionInformation[1],
                        event.u.Exception.ExceptionRecord.NumberParameters);
                else if (event.u.Exception.ExceptionRecord.NumberParameters)
                    printf("SOL options: assertion-error=%llu\n",
                        (unsigned long long)event.u.Exception.ExceptionRecord.ExceptionInformation[0]);
                if (fault_thread && GetThreadContext(fault_thread, &context)) {
                    have_context = TRUE;
                    printf("SOL options: regs eax=%08lx ebx=%08lx ecx=%08lx edx=%08lx esi=%08lx edi=%08lx ebp=%08lx esp=%08lx\n",
                        context.Eax, context.Ebx, context.Ecx, context.Edx,
                        context.Esi, context.Edi, context.Ebp, context.Esp);
                }
                if (have_context)
                    report_data_region(pid, "esi", (const void *)(ULONG_PTR)context.Esi);
                if (code == EXCEPTION_ACCESS_VIOLATION)
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
            if (code != EXCEPTION_BREAKPOINT || !event.u.Exception.dwFirstChance)
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

static BOOL worker_cpu_time(HANDLE process, ULONGLONG *milliseconds)
{
    FILETIME created, exited, kernel, user;
    ULARGE_INTEGER k, u;
    if (!GetProcessTimes(process, &created, &exited, &kernel, &user)) return FALSE;
    k.LowPart = kernel.dwLowDateTime; k.HighPart = kernel.dwHighDateTime;
    u.LowPart = user.dwLowDateTime; u.HighPart = user.dwHighDateTime;
    *milliseconds = (k.QuadPart + u.QuadPart) / 10000u;
    return TRUE;
}

static BOOL game_thread_cpu_time(DWORD thread_id, ULONGLONG *milliseconds)
{
    HANDLE thread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, thread_id);
    FILETIME created, exited, kernel, user;
    ULARGE_INTEGER k, u;
    BOOL ok;
    if (!thread) return FALSE;
    ok = GetThreadTimes(thread, &created, &exited, &kernel, &user);
    CloseHandle(thread);
    if (!ok) return FALSE;
    k.LowPart = kernel.dwLowDateTime; k.HighPart = kernel.dwHighDateTime;
    u.LowPart = user.dwLowDateTime; u.HighPart = user.dwHighDateTime;
    *milliseconds = (k.QuadPart + u.QuadPart) / 10000;
    return TRUE;
}

/* Diagnostic only: sample the owned game thread, then immediately resume it.
 * Module identities are emitted once so addresses can be resolved offline. */
static void sample_worker_modules(DWORD pid)
{
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    HMODULE modules[256]; DWORD bytes = 0, i;
    if (!process) return;
    if (EnumProcessModules(process, modules, sizeof(modules), &bytes)) {
        for (i = 0; i < bytes / sizeof(modules[0]) && i < ARRAYSIZE(modules); ++i) {
            MODULEINFO info; WCHAR name[MAX_PATH] = L"", path[MAX_PATH] = L"";
            if (GetModuleInformation(process, modules[i], &info, sizeof(info)) &&
                    GetModuleBaseNameW(process, modules[i], name, ARRAYSIZE(name))) {
                printf("SOL sample: module=%ls base=%p size=%lu\n", name,
                    info.lpBaseOfDll, info.SizeOfImage);
                if (GetModuleFileNameExW(process, modules[i], path, ARRAYSIZE(path)))
                    printf("SOL module-path: module=%ls path=%ls\n", name, path);
            }
        }
    }
    CloseHandle(process);
}

static void sample_worker_pc(DWORD thread_id, DWORD pid, const char *phase)
{
    HANDLE thread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT, FALSE, thread_id);
    HANDLE process;
    CONTEXT context = { .ContextFlags = CONTEXT_CONTROL };
    DWORD stack[32];
    SIZE_T stack_bytes = 0;
    unsigned i;
    BOOL captured = FALSE;
    DWORD resumed = 0;
    if (!thread) return;
    process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (SuspendThread(thread) != (DWORD)-1) {
        __try {
            captured = GetThreadContext(thread, &context);
            if (captured && process)
                (void)ReadProcessMemory(process, (const void *)(ULONG_PTR)context.Esp,
                    stack, sizeof(stack), &stack_bytes);
        }
        __finally { resumed = ResumeThread(thread); }
    }
    CloseHandle(thread);
    if (process) CloseHandle(process);
    if (resumed == (DWORD)-1) {
        puts("SOL sample: failed to resume owned game thread");
        ExitProcess(14);
    }
    if (captured) {
        printf("SOL sample: phase=%s pc=%08lx esp=%08lx ebp=%08lx stack=",
            phase, context.Eip, context.Esp, context.Ebp);
        for (i = 0; i < stack_bytes / sizeof(stack[0]); ++i)
            printf("%s%08lx", i ? "," : "", stack[i]);
        puts("");
    }
}

static int probe_active_sol(DWORD pid, HDESK desktop)
{
    HANDLE process;
    DWORD_PTR reply = 0;
    UINT command;
    ULONGLONG deadline, started, shown, cpu_before = 0, cpu_shown = 0, cpu_done = 0;
    BOOL cpu_before_ok, cpu_shown_ok;
    ULONGLONG thread_before = 0, thread_shown = 0, thread_done = 0;
    BOOL thread_before_ok, thread_shown_ok;
    BOOL pc_sampling = GetEnvironmentVariableA("MVDM_SOL_SAMPLE_PC", NULL, 0) != 0;
    BOOL clock_sampling = GetEnvironmentVariableA("MVDM_SOL_PROFILE_CLOCK", NULL, 0) != 0;
    DWORD game_thread;
    DWORD wait_ms = 10000;
    char wait_text[16];
    DWORD wait_chars = GetEnvironmentVariableA("MVDM_WOW_SOL_DIALOG_WAIT_MS", wait_text, sizeof(wait_text));
    if (wait_chars && wait_chars < sizeof(wait_text)) {
        unsigned long selected = strtoul(wait_text, NULL, 10);
        if (selected >= 10000 && selected <= 120000) wait_ms = selected;
    }
    owner_pid = pid;
    process = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION,
                          FALSE, owner_pid);
    if (!process || !desktop) return 3;
    deadline = GetTickCount64() + wait_ms;
    do {
        EnumDesktopWindows(desktop, find_game, 0);
        if (game_window) break;
        Sleep(25);
    } while (GetTickCount64() < deadline &&
             WaitForSingleObject(process, 0) == WAIT_TIMEOUT);
    if (!game_window ||
        !SendMessageTimeoutW(game_window, WM_NULL, 0, 0, SMTO_ABORTIFHUNG,
                             wait_ms == 10000 ? 2000 : wait_ms, &reply)) {
        printf("SOL options: no responsive game window\n");
        return 4;
    }
    game_thread = GetWindowThreadProcessId(game_window, NULL);
    printf("SOL options: game-thread=%lu\n", game_thread);
    if (pc_sampling) sample_worker_modules(owner_pid);
    command = options_command(game_window);
    if (!command) {
        printf("SOL options: no Options command in guest menu\n");
        return 5;
    }
    printf("SOL options: command=%u\n", command);
    cpu_before_ok = worker_cpu_time(process, &cpu_before);
    thread_before_ok = game_thread_cpu_time(game_thread, &thread_before);
    started = GetTickCount64();
    if (clock_sampling) printf("SOL profile: open-start=%llu\n", (unsigned long long)started);
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
    deadline = GetTickCount64() + wait_ms;
    do {
        option_dialog = NULL;
        EnumDesktopWindows(desktop, find_options, 0);
        if (option_dialog) break;
        if (WaitForSingleObject(process, 0) != WAIT_TIMEOUT) {
            DWORD worker_exit = STILL_ACTIVE;
            BOOL known = GetExitCodeProcess(process, &worker_exit);
            printf("SOL options: worker exited before modal dialog "
                "exit_known=%u exit=%08lX elapsed_ms=%llu\n",
                known, worker_exit,
                (unsigned long long)(GetTickCount64() - started));
            return 7;
        }
        if (pc_sampling) sample_worker_pc(game_thread, owner_pid, "open");
        Sleep(25);
    } while (GetTickCount64() < deadline);
    if (!option_dialog) {
        printf("SOL options: modal dialog did not appear\n");
        report_stalled_worker(owner_pid);
        return 8;
    }
    shown = GetTickCount64();
    if (clock_sampling) printf("SOL profile: shown=%llu\n", (unsigned long long)shown);
    cpu_shown_ok = worker_cpu_time(process, &cpu_shown);
    thread_shown_ok = game_thread_cpu_time(game_thread, &thread_shown);
    printf("SOL options: dialog-visible-ms=%llu\n", (unsigned long long)(shown - started));
    if (!PostMessageW(option_dialog, WM_CLOSE, 0, 0)) {
        printf("SOL options: dialog close post failed error=%lu\n", GetLastError());
        return 9;
    }
    deadline = GetTickCount64() + wait_ms;
    do {
        option_dialog = NULL;
        EnumDesktopWindows(desktop, find_options, 0);
        if (!option_dialog) break;
        if (pc_sampling) sample_worker_pc(game_thread, owner_pid, "close");
        Sleep(25);
    } while (GetTickCount64() < deadline);
    {
        DWORD worker_wait = WAIT_TIMEOUT, parent_error = 0, worker_exit = STILL_ACTIVE;
        BOOL parent_responded = FALSE;
        if (!option_dialog) {
            worker_wait = WaitForSingleObject(process, 0);
            if (worker_wait == WAIT_TIMEOUT) {
                SetLastError(ERROR_SUCCESS);
                parent_responded = SendMessageTimeoutW(game_window, WM_NULL,
                    0, 0, SMTO_ABORTIFHUNG,
                    wait_ms == 10000 ? 2000 : wait_ms, &reply) != 0;
                parent_error = GetLastError();
            }
        }
        if (option_dialog || worker_wait != WAIT_TIMEOUT || !parent_responded) {
            (void)GetExitCodeProcess(process, &worker_exit);
            printf("SOL options: modal close or parent recovery failed "
                "dialog=%u worker_wait=%lu worker_exit=%08lX "
                "parent_responded=%u parent_error=%lu elapsed_ms=%llu\n",
                option_dialog != NULL, worker_wait, worker_exit,
                parent_responded, parent_error,
                (unsigned long long)(GetTickCount64() - shown));
            if (worker_exit == STILL_ACTIVE) report_stalled_worker(owner_pid);
            return 10;
        }
    }
    printf("SOL options: dialog-close-recovery-ms=%llu\n", (unsigned long long)(GetTickCount64() - shown));
    if (clock_sampling) printf("SOL profile: close-done=%llu\n", (unsigned long long)GetTickCount64());
    if (cpu_before_ok && cpu_shown_ok && worker_cpu_time(process, &cpu_done))
        printf("SOL options: worker-cpu-open-ms=%llu worker-cpu-close-ms=%llu\n",
            (unsigned long long)(cpu_shown - cpu_before),
            (unsigned long long)(cpu_done - cpu_shown));
    if (thread_before_ok && thread_shown_ok && game_thread_cpu_time(game_thread, &thread_done))
        printf("SOL options: game-thread-cpu-open-ms=%llu game-thread-cpu-close-ms=%llu\n",
            (unsigned long long)(thread_shown - thread_before),
            (unsigned long long)(thread_done - thread_shown));
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
static int attach_existing_sol(const char *report, DWORD selected_pid, unsigned rounds)
{
    HDESK desktop = GetThreadDesktop(GetCurrentThreadId());
    ULONGLONG deadline = GetTickCount64() + 60000;
    int result = 0;
    unsigned round, i, external_count = 0, created = 0;
    HWND *external = NULL;
    char count_text[16];
    DWORD count_chars = GetEnvironmentVariableA("MVDM_SOL_EXTERNAL_WINDOWS", count_text, sizeof(count_text));
    if (count_chars && count_chars < sizeof(count_text)) {
        external_count = (unsigned)strtoul(count_text, NULL, 10);
        if (external_count > 300 || (external_count && rounds < 3)) return 27;
    }
    if (!freopen(report, "w", stdout)) return 26;
    setvbuf(stdout, NULL, _IONBF, 0);
    owner_pid = selected_pid; game_window = NULL;
    do {
        EnumDesktopWindows(desktop, find_game, 0);
        if (game_window && owner_pid) break;
        Sleep(25);
    } while (GetTickCount64() < deadline);
    if (!game_window || !owner_pid) {
        puts("SOL options: no owned Solitaire window");
        return 22;
    }
    for (round = 0; round < rounds; ++round) {
        printf("SOL options: round=%u\n", round + 1);
        result = probe_active_sol(owner_pid, desktop);
        if (result) break;
        if (external_count && round == 0) {
            external = calloc(external_count, sizeof(*external));
            if (!external) { result = 28; break; }
            for (i = 0; i < external_count; ++i) {
                external[i] = CreateWindowExA(0, "STATIC", "S11 probe external",
                    WS_OVERLAPPED, 0, 0, 100, 100, NULL, NULL,
                    GetModuleHandleW(NULL), NULL);
                if (!external[i]) { result = 28; break; }
                ++created;
            }
            printf("SOL options: external-created=%u\n", created);
            if (result) break;
            {
                HWND current = GetWindow(GetDesktopWindow(), GW_CHILD);
                unsigned found = 0, traversed = 0;
                while (current && traversed++ < 65536u) {
                    for (i = 0; i < created; ++i)
                        if (external[i] == current) { ++found; break; }
                    current = GetWindow(current, GW_HWNDNEXT);
                }
                printf("SOL options: external-desktop-chain=%u traversed=%u\n", found, traversed);
                if (current || found != created) { result = 29; break; }
            }
        }
        if (external && round == 1) {
            while (created) DestroyWindow(external[--created]);
            free(external); external = NULL;
            puts("SOL options: external-removed");
        }
    }
    if (external) {
        while (created) DestroyWindow(external[--created]);
        free(external);
    }
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
        return !strcmp(argv[1], "attach") ? attach_existing_sol(argv[2], 0, 1) :
            launch_private_probe(argv[1], argv[2]);
    if (argc == 4 && !strcmp(argv[1], "attach-pid")) {
        DWORD pid = strtoul(argv[2], NULL, 10);
        return pid ? attach_existing_sol(argv[3], pid, 1) : 2;
    }
    if (argc == 5 && !strcmp(argv[1], "attach-pid")) {
        DWORD pid = strtoul(argv[2], NULL, 10);
        unsigned rounds = strtoul(argv[4], NULL, 10);
        return pid && rounds && rounds <= 32 ? attach_existing_sol(argv[3], pid, rounds) : 2;
    }
    if (argc == 4 && !strcmp(argv[1], "child")) {
        if (!freopen(argv[3], "w", stdout)) return 26;
        setvbuf(stdout, NULL, _IONBF, 0);
        return run_private_probe(argv[2]);
    }
    return 2;
}
