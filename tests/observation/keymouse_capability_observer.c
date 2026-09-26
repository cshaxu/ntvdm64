/*
 * Native-Console S25 observer.  It supplies ordinary Windows KEY_EVENT and
 * MOUSE_EVENT records to the product's own CONIN$ handle, then accepts only
 * the DOS guest's visible completion strings.  It owns the launched tree.
 */
#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <stdio.h>
#include <string.h>

static HANDLE input_handle;

static BOOL write_record(INPUT_RECORD *record)
{
    DWORD written = 0u;
    return WriteConsoleInputA(input_handle, record, 1u, &written) && written == 1u;
}

static BOOL send_text(const char *text)
{
    while (*text != '\0') {
        INPUT_RECORD records[2];
        SHORT translated = *text == '\r' ? (SHORT)VK_RETURN : VkKeyScanA(*text);
        WORD key;
        BYTE modifiers;
        DWORD state = 0u;
        if (translated == (SHORT)-1) return FALSE;
        key = (WORD)(translated & 0xff);
        modifiers = (BYTE)((unsigned short)translated >> 8);
        if ((modifiers & 1u) != 0u) {
            INPUT_RECORD modifier = { 0 };
            modifier.EventType = KEY_EVENT;
            modifier.Event.KeyEvent.bKeyDown = TRUE;
            modifier.Event.KeyEvent.wRepeatCount = 1u;
            modifier.Event.KeyEvent.wVirtualKeyCode = VK_SHIFT;
            modifier.Event.KeyEvent.wVirtualScanCode = 0x2au;
            modifier.Event.KeyEvent.dwControlKeyState = SHIFT_PRESSED;
            if (!write_record(&modifier)) return FALSE;
            state |= SHIFT_PRESSED;
        }
        if ((modifiers & 2u) != 0u) {
            INPUT_RECORD modifier = { 0 };
            modifier.EventType = KEY_EVENT;
            modifier.Event.KeyEvent.bKeyDown = TRUE;
            modifier.Event.KeyEvent.wRepeatCount = 1u;
            modifier.Event.KeyEvent.wVirtualKeyCode = VK_CONTROL;
            modifier.Event.KeyEvent.wVirtualScanCode = 0x1du;
            modifier.Event.KeyEvent.dwControlKeyState = state | LEFT_CTRL_PRESSED;
            if (!write_record(&modifier)) return FALSE;
            state |= LEFT_CTRL_PRESSED;
        }
        if ((modifiers & 4u) != 0u) {
            INPUT_RECORD modifier = { 0 };
            modifier.EventType = KEY_EVENT;
            modifier.Event.KeyEvent.bKeyDown = TRUE;
            modifier.Event.KeyEvent.wRepeatCount = 1u;
            modifier.Event.KeyEvent.wVirtualKeyCode = VK_MENU;
            modifier.Event.KeyEvent.wVirtualScanCode = 0x38u;
            modifier.Event.KeyEvent.dwControlKeyState = state | LEFT_ALT_PRESSED;
            if (!write_record(&modifier)) return FALSE;
            state |= LEFT_ALT_PRESSED;
        }
        ZeroMemory(records, sizeof(records));
        records[0].EventType = KEY_EVENT;
        records[0].Event.KeyEvent.bKeyDown = TRUE;
        records[0].Event.KeyEvent.wRepeatCount = 1u;
        records[0].Event.KeyEvent.wVirtualKeyCode = key;
        records[0].Event.KeyEvent.wVirtualScanCode = (WORD)MapVirtualKeyA(key,
            MAPVK_VK_TO_VSC);
        records[0].Event.KeyEvent.uChar.AsciiChar = *text;
        records[0].Event.KeyEvent.dwControlKeyState = state;
        records[1] = records[0];
        records[1].Event.KeyEvent.bKeyDown = FALSE;
        if (!write_record(&records[0]) || !write_record(&records[1])) return FALSE;
        if ((modifiers & 4u) != 0u) {
            INPUT_RECORD modifier = { 0 };
            modifier.EventType = KEY_EVENT;
            modifier.Event.KeyEvent.wRepeatCount = 1u;
            modifier.Event.KeyEvent.wVirtualKeyCode = VK_MENU;
            modifier.Event.KeyEvent.wVirtualScanCode = 0x38u;
            modifier.Event.KeyEvent.dwControlKeyState = state & ~LEFT_ALT_PRESSED;
            if (!write_record(&modifier)) return FALSE;
            state &= ~LEFT_ALT_PRESSED;
        }
        if ((modifiers & 2u) != 0u) {
            INPUT_RECORD modifier = { 0 };
            modifier.EventType = KEY_EVENT;
            modifier.Event.KeyEvent.wRepeatCount = 1u;
            modifier.Event.KeyEvent.wVirtualKeyCode = VK_CONTROL;
            modifier.Event.KeyEvent.wVirtualScanCode = 0x1du;
            modifier.Event.KeyEvent.dwControlKeyState = state & ~LEFT_CTRL_PRESSED;
            if (!write_record(&modifier)) return FALSE;
            state &= ~LEFT_CTRL_PRESSED;
        }
        if ((modifiers & 1u) != 0u) {
            INPUT_RECORD modifier = { 0 };
            modifier.EventType = KEY_EVENT;
            modifier.Event.KeyEvent.wRepeatCount = 1u;
            modifier.Event.KeyEvent.wVirtualKeyCode = VK_SHIFT;
            modifier.Event.KeyEvent.wVirtualScanCode = 0x2au;
            modifier.Event.KeyEvent.dwControlKeyState = state & ~SHIFT_PRESSED;
            if (!write_record(&modifier)) return FALSE;
        }
        ++text;
    }
    return TRUE;
}

static BOOL screen_contains(HANDLE output, const char *needle)
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    COORD origin = { 0, 0 };
    DWORD cells, read = 0u;
    char *text;
    BOOL found = FALSE;

    if (!GetConsoleScreenBufferInfo(output, &info)) return FALSE;
    cells = (DWORD)info.dwSize.X * (DWORD)info.dwSize.Y;
    text = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (SIZE_T)cells + 1u);
    if (text == NULL) return FALSE;
    if (ReadConsoleOutputCharacterA(output, text, cells, origin, &read)) {
        text[read] = '\0';
        found = strstr(text, needle) != NULL;
    }
    HeapFree(GetProcessHeap(), 0u, text);
    return found;
}

static BOOL wait_screen(HANDLE output, const char *needle, DWORD timeout_ms)
{
    DWORD started = GetTickCount();
    do {
        if (screen_contains(output, needle)) return TRUE;
        Sleep(25u);
    } while ((DWORD)(GetTickCount() - started) < timeout_ms);
    return FALSE;
}

/* A probe's success text precedes its INT 21h exit.  For this sequential
 * capability test, require the current input line, not an old prompt in
 * scrollback. Nested/typeahead behavior has its own regression cases. */
static BOOL wait_prompt(HANDLE output, DWORD timeout_ms)
{
    DWORD started = GetTickCount();
    do {
        CONSOLE_SCREEN_BUFFER_INFO info;
        const char prompt[] = "O:\\WINNT>";
        char line[sizeof(prompt)] = { 0 };
        DWORD read = 0;
        if (GetConsoleScreenBufferInfo(output, &info) &&
            info.dwCursorPosition.X == sizeof(prompt) - 1) {
            COORD origin = { 0, info.dwCursorPosition.Y };
            if (ReadConsoleOutputCharacterA(output, line, sizeof(prompt) - 1,
                    origin, &read) && read == sizeof(prompt) - 1 &&
                memcmp(line, prompt, sizeof(prompt) - 1) == 0) return TRUE;
        }
        Sleep(25u);
    } while ((DWORD)(GetTickCount() - started) < timeout_ms);
    SetLastError(ERROR_TIMEOUT);
    return FALSE;
}

/*
 * INT 33h function 1 reaches the original host-side transition asynchronously.
 * Do not infer readiness from the guest's prior text alone: wait until that
 * transition has put the actual CONIN$ handle into its original mouse-record
 * mode before injecting native records.
 */
static BOOL wait_for_mouse_mode(DWORD timeout_ms)
{
    DWORD started = GetTickCount();
    DWORD mode;

    do {
        if (GetConsoleMode(input_handle, &mode) &&
            (mode & (ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS)) ==
            (ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS)) return TRUE;
        Sleep(25u);
    } while ((DWORD)(GetTickCount() - started) < timeout_ms);
    return FALSE;
}

static BOOL snapshot(HANDLE output, const char *path)
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    COORD origin = { 0, 0 };
    DWORD cells, read = 0u;
    char *text;
    FILE *file;

    if (!GetConsoleScreenBufferInfo(output, &info)) return FALSE;
    cells = (DWORD)info.dwSize.X * (DWORD)info.dwSize.Y;
    text = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (SIZE_T)cells + 1u);
    if (text == NULL) return FALSE;
    if (!ReadConsoleOutputCharacterA(output, text, cells, origin, &read) ||
        fopen_s(&file, path, "wb") != 0 || file == NULL) {
        HeapFree(GetProcessHeap(), 0u, text);
        return FALSE;
    }
    fwrite(text, 1u, read, file);
    fclose(file);
    HeapFree(GetProcessHeap(), 0u, text);
    return TRUE;
}

static BOOL append_confirmed_guest_markers(const char *path)
{
    FILE *file;
    if (fopen_s(&file, path, "ab") != 0 || file == NULL) return FALSE;
    fputs("\nobserver-confirmed-guest=S25_KEYBOARD_OK S25_MODIFIER_OK "
          "S25_MODIFIER_RELEASE_OK S25_PPI_OK S25_MOUSE_RESET_OK S25_MOUSE_POSITION_OK S25_MOUSE_CALLBACK_OK "
          "S25_MOUSE_TEARDOWN_OK S25_KEYMOUSE_OK\n", file);
    fclose(file);
    return TRUE;
}

static BOOL send_mouse(SHORT x, SHORT y)
{
    INPUT_RECORD record;
    const DWORD buttons[] = { 0u, FROM_LEFT_1ST_BUTTON_PRESSED, 0u };
    const DWORD flags[] = { MOUSE_MOVED, 0u, 0u };
    DWORD index;

    for (index = 0u; index != ARRAYSIZE(buttons); ++index) {
        ZeroMemory(&record, sizeof(record));
        record.EventType = MOUSE_EVENT;
        record.Event.MouseEvent.dwMousePosition.X = x;
        record.Event.MouseEvent.dwMousePosition.Y = y;
        record.Event.MouseEvent.dwButtonState = buttons[index];
        record.Event.MouseEvent.dwEventFlags = flags[index];
        if (!write_record(&record)) return FALSE;
        Sleep(100u);
    }
    return TRUE;
}

static BOOL send_ctrl_k_down(void)
{
    INPUT_RECORD record;
    ZeroMemory(&record, sizeof(record));
    record.EventType = KEY_EVENT;
    record.Event.KeyEvent.bKeyDown = TRUE;
    record.Event.KeyEvent.wRepeatCount = 1u;
    record.Event.KeyEvent.wVirtualKeyCode = VK_CONTROL;
    record.Event.KeyEvent.wVirtualScanCode = 0x1du;
    record.Event.KeyEvent.dwControlKeyState = LEFT_CTRL_PRESSED;
    if (!write_record(&record)) return FALSE;
    record.Event.KeyEvent.wVirtualKeyCode = 'K';
    record.Event.KeyEvent.wVirtualScanCode = 0x25u;
    record.Event.KeyEvent.uChar.AsciiChar = 0x0bu;
    if (!write_record(&record)) return FALSE;
    record.Event.KeyEvent.bKeyDown = FALSE;
    return write_record(&record);
}

static BOOL send_ctrl_up(void)
{
    INPUT_RECORD record;
    ZeroMemory(&record, sizeof(record));
    record.EventType = KEY_EVENT;
    record.Event.KeyEvent.bKeyDown = FALSE;
    record.Event.KeyEvent.wRepeatCount = 1u;
    record.Event.KeyEvent.wVirtualKeyCode = VK_CONTROL;
    record.Event.KeyEvent.wVirtualScanCode = 0x1du;
    return write_record(&record);
}

int main(int argc, char **argv)
{
    STARTUPINFOA startup = { sizeof(startup) };
    PROCESS_INFORMATION child = { 0 };
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits = { 0 };
    HANDLE job = NULL, output = INVALID_HANDLE_VALUE;
    char runtime[MAX_PATH] = "O:\\winnt";
    char command[MAX_PATH + 32];
    char guest[MAX_PATH] = "W:\\KMTST.COM";
    BOOL passed = FALSE;
    BOOL captured_capability = FALSE;
    DWORD wait, exit_code = 0u;
    const char *stage = "create";
    DWORD failure_error = 0u;
    BOOL shared_console = GetEnvironmentVariableA("MVDM_TEST_KEYMOUSE_SHARED_CONSOLE",NULL,0)!=0;

    if (argc != 2) return 64;
    GetEnvironmentVariableA("TEST_RUNTIME_ROOT", runtime, sizeof(runtime));
    GetEnvironmentVariableA("MVDM_TEST_KEYMOUSE_COMMAND", guest, sizeof(guest));
    snprintf(command, sizeof(command), "%s\\run16.exe COMMAND.COM", runtime);
    startup.dwFlags = STARTF_USESHOWWINDOW;
    startup.wShowWindow = SW_HIDE;
    job = CreateJobObjectA(NULL, NULL);
    if (job == NULL || !CreateProcessA(NULL, command, NULL, NULL, FALSE,
            (shared_console ? 0 : CREATE_NEW_CONSOLE) | CREATE_SUSPENDED,
            NULL, runtime, &startup, &child)) goto done;
    limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    stage = "job";
    if (!SetInformationJobObject(job, JobObjectExtendedLimitInformation, &limits,
            sizeof(limits)) || !AssignProcessToJobObject(job, child.hProcess)) goto done;
    ResumeThread(child.hThread);
    CloseHandle(child.hThread);
    child.hThread = NULL;
    Sleep(2500u);
    if (!shared_console) {
        stage = "detach";
        if (!FreeConsole()) goto done;
        stage = "attach";
        if (!AttachConsole(child.dwProcessId)) goto done;
    }
    input_handle = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0u, NULL);
    output = CreateFileA("CONOUT$", GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0u, NULL);
    stage = "guest-ready";
    if (input_handle == INVALID_HANDLE_VALUE || output == INVALID_HANDLE_VALUE ||
        !wait_screen(output, "O:\\WINNT>", 6000u) || !send_text(guest) ||
        !send_text("\r") || !wait_screen(output, "S25_READY", 6000u) ||
        !wait_for_mouse_mode(6000u)) goto done;
    stage = "callback";
    /* Original ConsoleEventThread calls DelayMouseEvents(2), discarding
     * startup mouse input for 110*(2+1) ms. CONIN$ mouse mode becomes visible
     * before that guard expires. Test steady-state delivery after the guard,
     * not an unsupported inference that ENABLE_MOUSE_INPUT means ready. */
    Sleep(400u);
    if (!send_mouse(26, 11) || !send_ctrl_k_down() ||
        !wait_screen(output, "S25_DISABLE_READY", 6000u)) goto done;
    stage = "teardown";
    if (!send_ctrl_up() || !send_mouse(54, 21) ||
        !wait_screen(output, "S25_KEYMOUSE_OK", 8000u)) goto done;
    if (!screen_contains(output, "S25_KEYBOARD_OK") ||
        !screen_contains(output, "S25_MODIFIER_OK") ||
        !screen_contains(output, "S25_MODIFIER_RELEASE_OK") ||
        !screen_contains(output, "S25_PPI_OK") ||
        !screen_contains(output, "S25_MOUSE_RESET_OK") ||
        !screen_contains(output, "S25_MOUSE_POSITION_OK") ||
        !screen_contains(output, "S25_MOUSE_CALLBACK_OK") ||
        !screen_contains(output, "S25_MOUSE_TEARDOWN_OK") ||
        !snapshot(output, argv[1])) goto done;
    if (!append_confirmed_guest_markers(argv[1])) goto done;
    captured_capability = TRUE;
    stage = "probe-return-prompt";
    if (!wait_prompt(output, 6000u)) goto done;
    stage = "mem-return";
    if (!send_text("mem\r")) goto done;
    stage = "mem-output";
    if (!wait_screen(output, "bytes total conventional memory", 6000u)) goto done;
    stage = "mem-return-prompt";
    if (!wait_prompt(output, 6000u)) goto done;
    stage = "command-exit";
    if (!send_text("exit\r")) goto done;
    wait = WaitForSingleObject(child.hProcess, 6000u);
    if (wait != WAIT_OBJECT_0) goto done;
    GetExitCodeProcess(child.hProcess, &exit_code);
    passed = exit_code == 1u;

done:
    failure_error = passed ? ERROR_SUCCESS : GetLastError();
    if (output != INVALID_HANDLE_VALUE && !captured_capability)
        (void)snapshot(output, argv[1]);
    if (!passed && output != INVALID_HANDLE_VALUE && captured_capability) {
        char failure_path[MAX_PATH];
        if (snprintf(failure_path, sizeof(failure_path), "%s.failure.txt",
                argv[1]) < (int)sizeof(failure_path))
            (void)snapshot(output, failure_path);
    }
    printf("keymouse passed=%s exit=%lu\n", passed ? "yes" : "no",
        (unsigned long)exit_code);
    { FILE *diagnostic = NULL;
      if (fopen_s(&diagnostic, argv[1], "ab") == 0 && diagnostic) {
          fprintf(diagnostic,"\nkeymouse passed=%s stage=%s error=%lu exit=%lu\n",
              passed ? "yes" : "no",stage,failure_error,exit_code);
          fclose(diagnostic);
      }
    }
    if (child.hThread != NULL) CloseHandle(child.hThread);
    if (child.hProcess != NULL) CloseHandle(child.hProcess);
    if (input_handle != INVALID_HANDLE_VALUE) CloseHandle(input_handle);
    if (output != INVALID_HANDLE_VALUE) CloseHandle(output);
    if (job != NULL) CloseHandle(job);
    return passed ? 0 : 1;
}
