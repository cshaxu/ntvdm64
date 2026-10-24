/*
 * Explorer-equivalent COMMAND.COM probe.  The target starts in a fresh
 * Console; this observer detaches from its caller only after that launch, so
 * Run16's initial single-member decision has already occurred.  It then
 * drives real CONIN$/CONOUT$ records and records whether a native-command
 * result reaches the direct root Console.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define WAIT_LIMIT_MS 35000u

static BOOL write_text(HANDLE input, const char *text)
{
    INPUT_RECORD records[256]; DWORD count = 0, done; size_t index;
    for (index = 0; text[index]; ++index) {
        SHORT translated;
        WORD key, scan;
        DWORD modifiers = NUMLOCK_ON;
        INPUT_RECORD *down, *up;
        if (count + 2 > ARRAYSIZE(records)) return FALSE;
        if (text[index] == '\r') {
            key = VK_RETURN;
        } else {
            translated = VkKeyScanA(text[index]);
            if (translated == -1) return FALSE;
            key = LOBYTE(translated);
            if (HIBYTE(translated) & 1) modifiers |= SHIFT_PRESSED;
        }
        scan = (WORD)MapVirtualKeyW(key, MAPVK_VK_TO_VSC);
        if (!scan) return FALSE;
        down = &records[count++]; ZeroMemory(down, sizeof(*down));
        down->EventType = KEY_EVENT; down->Event.KeyEvent.bKeyDown = TRUE;
        down->Event.KeyEvent.wRepeatCount = 1;
        down->Event.KeyEvent.wVirtualKeyCode = key;
        down->Event.KeyEvent.wVirtualScanCode = scan;
        down->Event.KeyEvent.uChar.AsciiChar = text[index];
        down->Event.KeyEvent.dwControlKeyState = modifiers;
        up = &records[count++]; *up = *down;
        up->Event.KeyEvent.bKeyDown = FALSE;
        up->Event.KeyEvent.uChar.AsciiChar = 0;
    }
    return WriteConsoleInputA(input, records, count, &done) && done == count;
}

/* The NTCOn presentation can replace the active screen buffer after this
 * observer attaches.  Reopen CONOUT$ at each observation point: an older
 * handle remains valid, but no longer describes what a direct-console user
 * sees.  Prompt discovery is only a test input milestone; it is not a new
 * product readiness protocol. */
static DWORD active_console_count(HANDLE fallback, const char *needle)
{
    HANDLE output;
    CONSOLE_SCREEN_BUFFER_INFO info;
    DWORD cells, read = 0;
    char *text;
    DWORD found = 0;

    output = CreateFileW(L"CONOUT$", GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (output == INVALID_HANDLE_VALUE) output = fallback;
    if (output == INVALID_HANDLE_VALUE || !GetConsoleScreenBufferInfo(output, &info) ||
        info.dwSize.X <= 0 || info.dwSize.Y <= 0 ||
        (uint64_t)info.dwSize.X * (uint64_t)info.dwSize.Y > 1024u * 1024u) goto done;
    cells = (DWORD)info.dwSize.X * (DWORD)info.dwSize.Y;
    text = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (SIZE_T)cells + 1);
    if (!text) goto done;
    if (ReadConsoleOutputCharacterA(output, text, cells, (COORD){ 0, 0 }, &read)) {
        text[read] = 0;
        const char *match=text;
        while((match=strstr(match,needle))!=NULL){++found;match+=strlen(needle);}
    }
    HeapFree(GetProcessHeap(), 0, text);
done:
    if (output != fallback && output != INVALID_HANDLE_VALUE) CloseHandle(output);
    return found;
}

static DWORD stage_frontend_process(const WCHAR *root,const FILETIME *started)
{
    WCHAR expected[MAX_PATH * 2], image[MAX_PATH * 2];
    HANDLE snapshot;
    PROCESSENTRY32W entry;
    DWORD result = 0;
    if (!root || swprintf_s(expected, ARRAYSIZE(expected), L"%ls\\system32\\ntcon.exe", root) < 0)
        return 0;
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return 0;
    ZeroMemory(&entry, sizeof(entry)); entry.dwSize = sizeof(entry);
    if (Process32FirstW(snapshot, &entry)) do {
        HANDLE process; DWORD length = ARRAYSIZE(image);
        FILETIME created,ended,kernel,user;
        if (_wcsicmp(entry.szExeFile, L"ntcon.exe")) continue;
        process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, entry.th32ProcessID);
        if (process && GetProcessTimes(process,&created,&ended,&kernel,&user) &&
            CompareFileTime(&created,started)>=0 &&
            QueryFullProcessImageNameW(process, 0, image, &length) &&
            !_wcsicmp(image, expected)) result = entry.th32ProcessID;
        if (process) CloseHandle(process);
        if (result) break;
    } while (Process32NextW(snapshot, &entry));
    CloseHandle(snapshot);
    return result;
}

/* COMMAND.COM's launch environment is guest input. Keep this direct-root
 * fixture equivalent to an Explorer invocation instead of inheriting a build
 * shell's unrelated toolchain variables. */
static BOOL direct_environment(const WCHAR *root, WCHAR *block, size_t capacity)
{
    WCHAR system_root[MAX_PATH], windir[MAX_PATH], comspec[MAX_PATH];
    size_t used = 0, index;
    const WCHAR *values[5];
    WCHAR path[MAX_PATH * 2];
    if (!root || !block || !capacity ||
        !GetEnvironmentVariableW(L"SystemRoot", system_root, ARRAYSIZE(system_root)) ||
        !GetEnvironmentVariableW(L"WINDIR", windir, ARRAYSIZE(windir)) ||
        !GetEnvironmentVariableW(L"COMSPEC", comspec, ARRAYSIZE(comspec)) ||
        swprintf_s(path, ARRAYSIZE(path), L"PATH=%ls\\system32;%ls\\System32", root, system_root) < 0)
        return FALSE;
    values[0] = system_root; values[1] = windir; values[2] = comspec;
    for (index = 0; index != ARRAYSIZE(values); ++index) {
        const WCHAR *name = index == 0 ? L"SystemRoot=" : index == 1 ? L"WINDIR=" :
            index == 2 ? L"COMSPEC=" : index == 3 ? L"PATHEXT=.COM;.EXE;.BAT;.CMD" :
            index == 4 ? path : L"";
        const WCHAR *value = index < 3 ? values[index] : NULL;
        int written = value ? swprintf_s(block + used, capacity - used, L"%ls%ls", name, value) :
            swprintf_s(block + used, capacity - used, L"%ls", name);
        if (written < 0 || (size_t)written + 1 >= capacity - used) return FALSE;
        used += (size_t)written + 1;
    }
    block[used] = 0;
    return TRUE;
}

/* The direct-root contract is visual: the frame must reach NTCOn's real
 * Window, not merely a Console buffer that happens to be attached to this
 * test process.  Use the same disposable private-desktop discipline as the
 * regular Console observer, never the user's desktop. */
static int private_desktop_observer(void)
{
    WCHAR name[80], *command;
    HDESK desktop;
    STARTUPINFOW startup = { sizeof(startup) };
    PROCESS_INFORMATION child = { 0 };
    DWORD code = 69, wait;

    if (swprintf_s(name, ARRAYSIZE(name), L"NTVDMConsoleTest-Direct-%lu",
                   GetCurrentProcessId()) < 0) return 69;
    desktop = CreateDesktopW(name, NULL, NULL, 0, GENERIC_ALL, NULL);
    if (!desktop) return 69;
    command = _wcsdup(GetCommandLineW());
    if (!command) { CloseDesktop(desktop); return 69; }
    startup.lpDesktop = name;
    startup.dwFlags = STARTF_USESHOWWINDOW;
    startup.wShowWindow = SW_HIDE;
    SetEnvironmentVariableW(L"MVDM_DIRECT_PRIVATE_DESKTOP", NULL);
    if (CreateProcessW(NULL, command, NULL, NULL, FALSE, CREATE_NEW_CONSOLE,
                       NULL, NULL, &startup, &child)) {
        CloseHandle(child.hThread);
        wait = WaitForSingleObject(child.hProcess, WAIT_LIMIT_MS + 20000u);
        if (wait == WAIT_OBJECT_0) GetExitCodeProcess(child.hProcess, &code);
        else {
            TerminateProcess(child.hProcess, 69);
            WaitForSingleObject(child.hProcess, 1000);
        }
        CloseHandle(child.hProcess);
    }
    free(command);
    CloseDesktop(desktop);
    return (int)code;
}

int wmain(int argc, WCHAR **argv)
{
    WCHAR command[MAX_PATH * 2], image[MAX_PATH], environment[MAX_PATH * 6];
    STARTUPINFOW startup = { sizeof(startup) };
    PROCESS_INFORMATION child = { 0 };
    HANDLE input = INVALID_HANDLE_VALUE, output = INVALID_HANDLE_VALUE;
    HANDLE frontend_process=NULL;
    FILETIME child_created,child_ended,child_kernel,child_user;
    SECURITY_ATTRIBUTES inherit = { sizeof(inherit), NULL, TRUE };
    DWORD started, code = STILL_ACTIVE;
    BOOL attached = FALSE, initial_ready = FALSE, submitted = FALSE, observed = FALSE;
    BOOL console_observed = FALSE;
    const WCHAR *target=L"command";
    const char *steps[2]={"ver\r",NULL};
    const char *markers[2]={"Microsoft Windows",NULL};
    DWORD step=0,step_count=1,before=0,expected_exit=0;
    BOOL native=FALSE,mem_first=FALSE;
    DWORD attach_error = ERROR_SUCCESS;
    DWORD frontend_process_id = 0;
    FILE *report;
    if (argc != 4) return 64;
    if(!_wcsicmp(argv[3],L"cmd-command"))target=L"cmd /d /c command";
    else if(!_wcsicmp(argv[3],L"root-cmd-exit")) {
        target=L"cmd /d";step_count=0;
    }
    else if(!_wcsicmp(argv[3],L"root-command-exit")) {
        step_count=0;
    }
    else if(_wcsicmp(argv[3],L"ver")) {
        if(!_wcsicmp(argv[3],L"root-cmd-ver-mem-exit"))native=TRUE;
        else if(!_wcsicmp(argv[3],L"root-cmd-mem-ver-exit")){native=TRUE;mem_first=TRUE;}
        else if(!_wcsicmp(argv[3],L"root-command-mem-ver-exit"))mem_first=TRUE;
        else if(_wcsicmp(argv[3],L"root-command-ver-mem-exit"))return 64;
        if(native)target=L"cmd /d";
        steps[mem_first ? 0 : 1]="mem\r";
        markers[mem_first ? 0 : 1]="bytes total conventional memory";
        steps[mem_first ? 1 : 0]="ver\r";
        markers[mem_first ? 1 : 0]="Microsoft Windows";
        step_count=2;
        expected_exit=native ? 0 : 1;
    }
    if (GetEnvironmentVariableW(L"MVDM_DIRECT_PRIVATE_DESKTOP", NULL, 0))
        return private_desktop_observer();
    if (_wfopen_s(&report, argv[2], L"w")) return 65;
    fputs("result=started\n", report); fflush(report);
    if (swprintf_s(image, ARRAYSIZE(image), L"%ls\\system32\\run16.exe", argv[1]) < 0 ||
        GetFileAttributesW(image) == INVALID_FILE_ATTRIBUTES ||
        swprintf_s(command, ARRAYSIZE(command), L"\"%ls\" %ls", image,target) < 0) {
        fclose(report); return 64;
    }
    if (!direct_environment(argv[1], environment, ARRAYSIZE(environment)) ||
        !CreateProcessW(NULL, command, NULL, NULL, FALSE, CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT,
                        environment, argv[1], &startup, &child)) {
        fprintf(report, "create-error=%lu\n", GetLastError()); fclose(report); return 66;
    }
    CloseHandle(child.hThread); started = GetTickCount();
    if(!GetProcessTimes(child.hProcess,&child_created,&child_ended,&child_kernel,&child_user)) {
        TerminateProcess(child.hProcess,66);CloseHandle(child.hProcess);fclose(report);return 66;
    }
    fprintf(report,"target-pid=%lu\n",child.dwProcessId);fflush(report);
    while (GetTickCount() - started < WAIT_LIMIT_MS) {
        /* Frontend creation follows Run16's initial Console-member snapshot.
         * Waiting for it works for either carrier and for an existing NTSRV;
         * joining before that snapshot would turn this into a borrowed case. */
        if(!frontend_process_id)frontend_process_id=stage_frontend_process(argv[1],&child_created);
        if(frontend_process_id)initial_ready=TRUE;
        if (initial_ready && !attached) {
            /* CreateProcess gave the child its own Console.  Detach from the
             * observer's caller only after Run16 recorded initial ownership. */
            FreeConsole();
            if (AttachConsole(child.dwProcessId)) {
                attached = TRUE;
                input = CreateFileW(L"CONIN$", GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ | FILE_SHARE_WRITE, &inherit, OPEN_EXISTING, 0, NULL);
                output = CreateFileW(L"CONOUT$", GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ | FILE_SHARE_WRITE, &inherit, OPEN_EXISTING, 0, NULL);
            } else attach_error = GetLastError();
        }
        /* Queue one real Console input record after the child owns the fresh
         * Console. COMMAND consumes it only when its original input loop is
         * ready; the product receives no test-only readiness signal. */
        if (attached && !submitted && active_console_count(output, ">")) {
            if(!step_count) {
                console_observed=TRUE;observed=TRUE;submitted=TRUE;
                fputs("prompt-ready=1 exit-only=1\n",report);fflush(report);break;
            }
            /* The native CMD banner already contains the VER marker. Require
             * a newly produced occurrence, not a stale startup screen. */
            before=active_console_count(output,markers[step]);
            submitted=write_text(input,steps[step]);
        }
        /* Console-mode output is committed to the active Console buffer;
         * LibKvmWindow is only the optional CAF/graphics projection. */
        if (submitted && active_console_count(output,markers[step])>before) {
            fprintf(report,"step=%lu output=%s\n",step,markers[step]);fflush(report);
            if(++step==step_count){console_observed=TRUE;observed=TRUE;break;}
            submitted=FALSE;
        }
        if (WaitForSingleObject(child.hProcess, 20) == WAIT_OBJECT_0) break;
    }
    fprintf(report, "attached=%u attach-error=%lu initial-ready=%u submitted=%u frontend-process=%lu console-observed=%u observed=%u\n",
        attached, attach_error, initial_ready, submitted, frontend_process_id, console_observed, observed);
    if(frontend_process_id)frontend_process=OpenProcess(SYNCHRONIZE,FALSE,frontend_process_id);
    if (input != INVALID_HANDLE_VALUE) write_text(input, "exit\r");
    /* Do not keep the root Console alive while observing launcher retirement.
     * The probe's extra Console membership is not part of an Explorer launch. */
    if (input != INVALID_HANDLE_VALUE) {CloseHandle(input);input=INVALID_HANDLE_VALUE;}
    if (output != INVALID_HANDLE_VALUE) {CloseHandle(output);output=INVALID_HANDLE_VALUE;}
    if (attached) {
        BOOL detached=FreeConsole();
        fprintf(report,"detached-before-exit-wait=%u\n",detached);fflush(report);
        if(!detached)observed=FALSE;
        attached=FALSE;
    }
    if (WaitForSingleObject(child.hProcess, 10000) != WAIT_OBJECT_0)
        observed = FALSE;
    GetExitCodeProcess(child.hProcess, &code);
    fprintf(report, "exit=%lu\n", code);
    {
        BOOL frontend_exited=frontend_process &&
            WaitForSingleObject(frontend_process,10000)==WAIT_OBJECT_0;
        fprintf(report,"frontend-exited=%u\n",frontend_exited);
        if(!frontend_exited)observed=FALSE;
        if(frontend_process)CloseHandle(frontend_process);
    }
    if (input != INVALID_HANDLE_VALUE) CloseHandle(input);
    if (output != INVALID_HANDLE_VALUE) CloseHandle(output);
    if (attached) FreeConsole();
    if (WaitForSingleObject(child.hProcess, 0) == WAIT_TIMEOUT &&
        !GetEnvironmentVariableW(L"MVDM_DIRECT_RETAIN_FAILURE",NULL,0))
        TerminateProcess(child.hProcess, 0x53504354u);
    CloseHandle(child.hProcess); fclose(report);
    return initial_ready && observed && code == expected_exit ? 0 : 1;
}
