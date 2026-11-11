/*
 * Hidden native-Console regression runner for the standalone NTVDM package.
 *
 * ConPTY remains the terminal-presentation test surface.  This companion
 * runner deliberately uses a normal Console input buffer and WriteConsoleInput
 * so that DOS guest regressions do not depend on a terminal renderer or VT
 * focus handshake.  It owns the complete child tree through a Job object.
 *
 * Usage: native-console-observer <raw-log> [guest-command]
 */
#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <tlhelp32.h>
#include <dbghelp.h>
#include <stdio.h>
#include <string.h>

static HANDLE g_input;

static DWORD
find_descendant(DWORD root, const char *image)
{
    HANDLE snapshot;
    PROCESSENTRY32 entry;
    DWORD parents[32] = { root };
    DWORD count = 1u, index;

    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return 0u;
    for (index = 0u; index < count && count < ARRAYSIZE(parents); ++index) {
        entry.dwSize = sizeof(entry);
        if (Process32First(snapshot, &entry)) {
            do {
                if (entry.th32ParentProcessID != parents[index]) continue;
                if (_stricmp(entry.szExeFile, image) == 0) {
                    DWORD result = entry.th32ProcessID;
                    CloseHandle(snapshot);
                    return result;
                }
                if (count < ARRAYSIZE(parents)) parents[count++] = entry.th32ProcessID;
            } while (Process32Next(snapshot, &entry));
        }
    }
    CloseHandle(snapshot);
    return 0u;
}

static void
record_modules(FILE *log, DWORD process_id)
{
    HANDLE snapshot;
    MODULEENTRY32 entry;

    if (process_id == 0u) return;
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32,
                                        process_id);
    if (snapshot == INVALID_HANDLE_VALUE) return;
    entry.dwSize = sizeof(entry);
    if (Module32First(snapshot, &entry)) {
        do {
            fprintf(log, "module=%s base=%08lX size=%08lX\n", entry.szModule,
                    (unsigned long)(ULONG_PTR)entry.modBaseAddr,
                    (unsigned long)entry.modBaseSize);
        } while (Module32Next(snapshot, &entry));
    }
    CloseHandle(snapshot);
}

static void
record_thread_ips(FILE *log, DWORD process_id)
{
    HANDLE snapshot;
    HANDLE process;
    THREADENTRY32 entry;

    fprintf(log, "process=%lu\n", (unsigned long)process_id);
    process = process_id ? OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION,
                                       FALSE, process_id) : NULL;
    if (process) (void)SymInitialize(process, NULL, TRUE);
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return;
    entry.dwSize = sizeof(entry);
    if (Thread32First(snapshot, &entry)) {
        do {
            HANDLE thread;
            CONTEXT context = { 0 };
            if (entry.th32OwnerProcessID != process_id) continue;
            thread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT,
                                FALSE, entry.th32ThreadID);
            if (thread == NULL) continue;
            if (SuspendThread(thread) != (DWORD)-1) {
                context.ContextFlags = CONTEXT_CONTROL;
                if (GetThreadContext(thread, &context)) {
                    STACKFRAME64 frame = { 0 };
                    CONTEXT walk = context;
                    DWORD stack[8] = { 0 };
                    SIZE_T bytes = 0u;
                    fprintf(log, "thread=%lu eip=%08lX esp=%08lX\n",
                            (unsigned long)entry.th32ThreadID,
                            (unsigned long)context.Eip,
                            (unsigned long)context.Esp);
                    if (process && ReadProcessMemory(process,
                            (const void *)(ULONG_PTR)context.Esp, stack,
                            sizeof(stack), &bytes) && bytes == sizeof(stack)) {
                        DWORD index;
                        for (index = 0u; index < ARRAYSIZE(stack); ++index) {
                            fprintf(log, "thread=%lu stack%lu=%08lX\n",
                                    (unsigned long)entry.th32ThreadID,
                                    (unsigned long)index,
                                    (unsigned long)stack[index]);
                        }
                    }
                    if (process) {
                        DWORD frame_index;
                        frame.AddrPC.Offset = walk.Eip;
                        frame.AddrPC.Mode = AddrModeFlat;
                        frame.AddrStack.Offset = walk.Esp;
                        frame.AddrStack.Mode = AddrModeFlat;
                        frame.AddrFrame.Offset = walk.Ebp;
                        frame.AddrFrame.Mode = AddrModeFlat;
                        for (frame_index = 0u; frame_index < 16u && StackWalk64(
                                IMAGE_FILE_MACHINE_I386, process, thread,
                                &frame, &walk, NULL, SymFunctionTableAccess64,
                                SymGetModuleBase64, NULL); ++frame_index) {
                            fprintf(log, "thread=%lu frame%lu=%08llX\n",
                                    (unsigned long)entry.th32ThreadID,
                                    (unsigned long)frame_index,
                                    (unsigned long long)frame.AddrPC.Offset);
                            if (frame.AddrReturn.Offset == 0u) break;
                        }
                    }
                }
                (void)ResumeThread(thread);
            }
            CloseHandle(thread);
        } while (Thread32Next(snapshot, &entry));
    }
    CloseHandle(snapshot);
    if (process) { SymCleanup(process); CloseHandle(process); }
}

static BOOL
send_text(const char *text)
{
    INPUT_RECORD records[2];
    DWORD written;

    while (*text) {
        WORD vk = (*text == '\r') ? VK_RETURN : VkKeyScanA(*text);
        ZeroMemory(records, sizeof(records));
        records[0].EventType = KEY_EVENT;
        records[0].Event.KeyEvent.bKeyDown = TRUE;
        records[0].Event.KeyEvent.wRepeatCount = 1;
        records[0].Event.KeyEvent.wVirtualKeyCode = vk & 0xff;
        records[0].Event.KeyEvent.uChar.AsciiChar = *text;
        records[1] = records[0];
        records[1].Event.KeyEvent.bKeyDown = FALSE;
        if (!WriteConsoleInputA(g_input, records, ARRAYSIZE(records), &written) ||
            written != ARRAYSIZE(records)) {
            return FALSE;
        }
        ++text;
    }
    return TRUE;
}

static BOOL
snapshot(FILE *log)
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    CHAR *text;
    DWORD count, read;
    COORD origin = { 0, 0 };

    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info)) {
        return FALSE;
    }
    count = (DWORD)info.dwSize.X * (DWORD)info.dwSize.Y;
    text = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, count + 1);
    if (!text) {
        return FALSE;
    }
    if (!ReadConsoleOutputCharacterA(GetStdHandle(STD_OUTPUT_HANDLE), text,
                                     count, origin, &read)) {
        HeapFree(GetProcessHeap(), 0, text);
        return FALSE;
    }
    fwrite(text, 1, read, log);
    HeapFree(GetProcessHeap(), 0, text);
    return TRUE;
}

static BOOL
log_contains(const char *path, const char *needle)
{
    HANDLE file;
    DWORD size, read;
    CHAR *text;
    BOOL found = FALSE;

    file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                       0, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        return FALSE;
    }
    size = GetFileSize(file, NULL);
    text = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size + 1);
    if (text && ReadFile(file, text, size, &read, NULL)) {
        found = strstr(text, needle) != NULL;
    }
    if (text) {
        HeapFree(GetProcessHeap(), 0, text);
    }
    CloseHandle(file);
    return found;
}

int
main(int argc, char **argv)
{
    STARTUPINFOA startup = { sizeof(startup) };
    PROCESS_INFORMATION child = { 0 };
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits = { 0 };
    HANDLE job, output;
    FILE *log;
    DWORD wait, code = 0, worker_id;
    const char *guest = argc == 3 ? argv[2] : "MEM.EXE";
    char command[MAX_PATH + 64];

    if (argc != 2 && argc != 3) {
        return 64;
    }
    if (fopen_s(&log, argv[1], "wb") != 0 || !log) {
        return 65;
    }
    snprintf(command, sizeof(command), "O:\\winnt\\run16.exe COMMAND.COM");
    startup.dwFlags = STARTF_USESHOWWINDOW;
    startup.wShowWindow = SW_HIDE;
    job = CreateJobObjectA(NULL, NULL);
    if (!job || !CreateProcessA(NULL, command, NULL, NULL, FALSE,
                               CREATE_NEW_CONSOLE | CREATE_SUSPENDED, NULL,
                               "O:\\winnt", &startup, &child)) {
        fclose(log);
        return 66;
    }
    limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    SetInformationJobObject(job, JobObjectExtendedLimitInformation, &limits,
                            sizeof(limits));
    AssignProcessToJobObject(job, child.hProcess);
    ResumeThread(child.hThread);
    CloseHandle(child.hThread);

    Sleep(3000);
    FreeConsole();
    if (!AttachConsole(child.dwProcessId)) {
        CloseHandle(job);
        fclose(log);
        return 67;
    }
    g_input = CreateFileA("CONIN$", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                          NULL, OPEN_EXISTING, 0, NULL);
    output = CreateFileA("CONOUT$", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                         NULL, OPEN_EXISTING, 0, NULL);
    SetStdHandle(STD_OUTPUT_HANDLE, output);
    if (g_input == INVALID_HANDLE_VALUE || output == INVALID_HANDLE_VALUE ||
        !send_text(guest) || !send_text("\r")) {
        CloseHandle(job);
        fclose(log);
        return 68;
    }
    Sleep(2500);
    snapshot(log);
    record_thread_ips(log, child.dwProcessId);
    worker_id = find_descendant(child.dwProcessId, "ntvdm.exe");
    record_modules(log, worker_id);
    record_thread_ips(log, worker_id);
    send_text("exit\r");
    wait = WaitForSingleObject(child.hProcess, 5000);
    if (wait == WAIT_OBJECT_0) {
        GetExitCodeProcess(child.hProcess, &code);
    }
    fclose(log);
    CloseHandle(job);
    CloseHandle(child.hProcess);
    return wait == WAIT_OBJECT_0 && code == 1 && log_contains(argv[1], guest) ? 0 : 1;
}
