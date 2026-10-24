/*
 * Explorer-equivalent COMMAND.COM probe.  The target starts in a fresh
 * Console; this observer detaches from its caller only after that launch, so
 * Run16's initial single-member decision has already occurred.  It then
 * drives real CONIN$/CONOUT$ records and records whether a native-command
 * result reaches the direct root Console.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "console_snapshot.h"
#include "input_milestone.h"

#define WAIT_LIMIT_MS 10000u

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
static BOOL active_console_contains(HANDLE fallback, const char *needle)
{
    HANDLE output;
    CONSOLE_SCREEN_BUFFER_INFO info;
    DWORD cells, read = 0;
    char *text;
    BOOL found = FALSE;

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
        found = strstr(text, needle) != NULL;
    }
    HeapFree(GetProcessHeap(), 0, text);
done:
    if (output != fallback && output != INVALID_HANDLE_VALUE) CloseHandle(output);
    return found;
}

static HWND stage_frontend_window(const WCHAR *root, DWORD *pid_out)
{
    WCHAR expected[MAX_PATH * 2], image[MAX_PATH * 2];
    HWND window = NULL;
    if (!pid_out || swprintf_s(expected, ARRAYSIZE(expected), L"%ls\\system32\\ntcon.exe", root) < 0)
        return NULL;
    while ((window = FindWindowExW(NULL, window, L"LibKvmWindow", NULL)) != NULL) {
        DWORD pid = 0, length = ARRAYSIZE(image);
        HANDLE process;
        GetWindowThreadProcessId(window, &pid);
        process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (!process) continue;
        if (QueryFullProcessImageNameW(process, 0, image, &length) &&
            !_wcsicmp(image, expected)) {
            CloseHandle(process);
            *pid_out = pid;
            return window;
        }
        CloseHandle(process);
    }
    return NULL;
}

int wmain(int argc, WCHAR **argv)
{
    WCHAR command[MAX_PATH * 2], image[MAX_PATH];
    STARTUPINFOW startup = { sizeof(startup) };
    PROCESS_INFORMATION child = { 0 };
    HANDLE input = INVALID_HANDLE_VALUE, output = INVALID_HANDLE_VALUE;
    SECURITY_ATTRIBUTES inherit = { sizeof(inherit), NULL, TRUE };
    DWORD started, code = STILL_ACTIVE;
    BOOL attached = FALSE, submitted = FALSE, observed = FALSE;
    DWORD attach_error = ERROR_SUCCESS;
    HWND frontend_window = NULL;
    DWORD frontend_pid = 0;
    observer_text_view view;
    FILE *report;
    if (argc != 4 || _wcsicmp(argv[3], L"ver")) return 64;
    if (_wfopen_s(&report, argv[2], L"w")) return 65;
    fputs("result=started\n", report); fflush(report);
    if (swprintf_s(image, ARRAYSIZE(image), L"%ls\\system32\\run16.exe", argv[1]) < 0 ||
        GetFileAttributesW(image) == INVALID_FILE_ATTRIBUTES ||
        swprintf_s(command, ARRAYSIZE(command), L"\"%ls\" command", image) < 0) {
        fclose(report); return 64;
    }
    if (!CreateProcessW(NULL, command, NULL, NULL, FALSE, CREATE_NEW_CONSOLE,
                        NULL, argv[1], &startup, &child)) {
        fprintf(report, "create-error=%lu\n", GetLastError()); fclose(report); return 66;
    }
    CloseHandle(child.hThread); started = GetTickCount();
    /* CreateProcess gave the child its own Console, while this observer stays
     * attached to the caller's Console.  Detach first: AttachConsole cannot
     * switch an already-attached process. */
    FreeConsole();
    while (GetTickCount() - started < WAIT_LIMIT_MS) {
        if (!attached) {
            if (AttachConsole(child.dwProcessId)) {
                attached = TRUE;
                input = CreateFileW(L"CONIN$", GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ | FILE_SHARE_WRITE, &inherit, OPEN_EXISTING, 0, NULL);
                output = CreateFileW(L"CONOUT$", GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ | FILE_SHARE_WRITE, &inherit, OPEN_EXISTING, 0, NULL);
            } else attach_error = GetLastError();
        }
        /* Do not substitute a wall-clock delay for COMMAND's actual prompt.
         * The direct root may have switched its active buffer to NTCOn before
         * it accepts input; submit exactly once after that visible milestone. */
        if (attached && !submitted && active_console_contains(output, ">"))
            submitted = write_text(input, "ver\r");
        if (!frontend_window) frontend_window = stage_frontend_window(argv[1], &frontend_pid);
        /* Once created, NTCON's KVM Window is the visual truth.  The root
         * Console above remains only an attachment witness. */
        if (submitted && frontend_window && observer_text_read(output, frontend_window, frontend_pid, &view) &&
            observer_view_contains(&view, "MS-DOS Version 5.00.500")) {
            observed = TRUE; break;
        }
        if (WaitForSingleObject(child.hProcess, 20) == WAIT_OBJECT_0) break;
    }
    fprintf(report, "attached=%u attach-error=%lu submitted=%u frontend=%lu observed=%u\n",
        attached, attach_error, submitted, frontend_pid, observed);
    if (input != INVALID_HANDLE_VALUE) write_text(input, "exit\r");
    if (WaitForSingleObject(child.hProcess, 10000) != WAIT_OBJECT_0)
        observed = FALSE;
    GetExitCodeProcess(child.hProcess, &code);
    fprintf(report, "exit=%lu\n", code);
    if (input != INVALID_HANDLE_VALUE) CloseHandle(input);
    if (output != INVALID_HANDLE_VALUE) CloseHandle(output);
    if (attached) FreeConsole();
    if (WaitForSingleObject(child.hProcess, 0) == WAIT_TIMEOUT) TerminateProcess(child.hProcess, 0x53504354u);
    CloseHandle(child.hProcess); fclose(report);
    return observed && code == 0 ? 0 : 1;
}
