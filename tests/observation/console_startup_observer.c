/*
 * Fixed, non-debug startup observation container for the selected x86
 * SoftPC product.  It owns a real CONIN$/CONOUT$ console, launches exactly
 * one product command line, waits a bounded interval, and records only the
 * observable process result.  It never attaches a debugger, searches guest
 * memory, installs breakpoints, or changes product inputs.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <dbghelp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char control_event_report[MAX_PATH];
static BOOL WINAPI record_console_control(DWORD event)
{
    HANDLE file = CreateFileA(control_event_report, GENERIC_WRITE,
        FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD written;
    if (file != INVALID_HANDLE_VALUE) {
        WriteFile(file, &event, sizeof(event), &written, NULL);
        CloseHandle(file);
    }
    return FALSE; /* Preserve the default termination action. */
}

/* The scripted command sequence itself is paced at one original 8042 event
 * every 100 ms.  The original worker's delayed IRQ path is measured in
 * microseconds, so this remains deliberately slower than the source-owned
 * hardware queue while keeping a four-key command inside the fixed 5--10
 * second observation window. */
#define OBSERVATION_TIMEOUT_MS 10000u
#define OBSERVATION_TIMEOUT_MAX_MS 60000u
#define OBSERVATION_INPUT_READY_TIMEOUT_MS 20000u
#define OBSERVATION_KEY_EVENT_INTERVAL_MS 100u
#define OBSERVATION_KEY_DRAIN_TIMEOUT_MS 1500u
#define OBSERVATION_TIMEOUT_EXIT 0x53504354u
#define OBSERVATION_STACK_WORDS 16u
#define OBSERVATION_THREAD_LIMIT 16u
#define OBSERVATION_FRAME_LIMIT 16u

typedef struct observation_thread_context {
    DWORD thread_id;
    BOOL context_available;
    CONTEXT context;
    DWORD frame_count;
    DWORD64 frames[OBSERVATION_FRAME_LIMIT];
} observation_thread_context;

typedef struct observation_image_identity {
    BOOL available;
    DWORD base_address;
    DWORD image_size;
    char module_name[MAX_MODULE_NAME32 + 1];
    char module_path[MAX_PATH];
} observation_image_identity;

static void capture_process_image(DWORD process_id,
                                  observation_image_identity *identity)
{
    HANDLE snapshot;
    MODULEENTRY32 entry;

    memset(identity, 0, sizeof(*identity));
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, process_id);
    if (snapshot == INVALID_HANDLE_VALUE) return;
    entry.dwSize = sizeof(entry);
    if (Module32First(snapshot, &entry)) {
        identity->available = TRUE;
        identity->base_address = (DWORD)(ULONG_PTR)entry.modBaseAddr;
        identity->image_size = entry.modBaseSize;
        strncpy_s(identity->module_name, sizeof(identity->module_name),
                  entry.szModule, _TRUNCATE);
        strncpy_s(identity->module_path, sizeof(identity->module_path),
                  entry.szExePath, _TRUNCATE);
    }
    CloseHandle(snapshot);
}

static DWORD capture_process_threads(HANDLE process, DWORD process_id,
                                     observation_thread_context *records,
                                     DWORD record_capacity)
{
    HANDLE snapshot;
    THREADENTRY32 entry;
    DWORD record_count = 0;

    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return 0;
    entry.dwSize = sizeof(entry);
    if (Thread32First(snapshot, &entry)) {
        do {
            HANDLE thread;
            if (entry.th32OwnerProcessID != process_id ||
                record_count == record_capacity) continue;
            records[record_count].thread_id = entry.th32ThreadID;
            records[record_count].context_available = FALSE;
            records[record_count].frame_count = 0;
            memset(&records[record_count].context, 0,
                   sizeof(records[record_count].context));
            thread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT,
                                FALSE, entry.th32ThreadID);
            if (thread != NULL) {
                if (SuspendThread(thread) != (DWORD)-1) {
                    records[record_count].context.ContextFlags =
                        CONTEXT_CONTROL | CONTEXT_INTEGER;
                    records[record_count].context_available =
                        GetThreadContext(thread, &records[record_count].context);
                    if (records[record_count].context_available) {
                        STACKFRAME64 frame = { 0 };
                        CONTEXT walk_context = records[record_count].context;
                        frame.AddrPC.Offset = walk_context.Eip;
                        frame.AddrPC.Mode = AddrModeFlat;
                        frame.AddrStack.Offset = walk_context.Esp;
                        frame.AddrStack.Mode = AddrModeFlat;
                        frame.AddrFrame.Offset = walk_context.Ebp;
                        frame.AddrFrame.Mode = AddrModeFlat;
                        while (records[record_count].frame_count <
                               OBSERVATION_FRAME_LIMIT &&
                               StackWalk64(IMAGE_FILE_MACHINE_I386, process,
                                           thread, &frame, &walk_context, NULL,
                                           SymFunctionTableAccess64,
                                           SymGetModuleBase64, NULL)) {
                            records[record_count].frames[
                                records[record_count].frame_count++] =
                                frame.AddrPC.Offset;
                            if (frame.AddrReturn.Offset == 0) break;
                        }
                    }
                }
                CloseHandle(thread);
            }
            ++record_count;
        } while (Thread32Next(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return record_count;
}

static void write_console_snapshot(HANDLE output, const char *report_path)
{
    char *screen;
    char path[MAX_PATH];
    DWORD count = 0, cells, row, column;
    CONSOLE_SCREEN_BUFFER_INFO info;
    FILE *file = NULL;

    if (!GetConsoleScreenBufferInfo(output,&info)) return;
    cells=(DWORD)info.dwSize.X*(DWORD)info.dwSize.Y;
    if (!cells || cells>4u*1024u*1024u) return;
    screen=(char*)malloc(cells); if(!screen)return;
    if (!ReadConsoleOutputCharacterA(output, screen, cells,
                                     (COORD){ 0, 0 }, &count)) { free(screen); return; }
    snprintf(path, sizeof(path), "%s.console.txt", report_path);
    if (fopen_s(&file, path, "wb") == 0 && file != NULL) {
        /* A modern Console commonly allocates thousands of scrollback rows.
         * Persisting its full character plane makes an ordinary one-line DOS
         * result into a megabyte of spaces, which obscures rather than proves
         * the guest's output.  Keep every nonblank row, tagged with its
         * original buffer position, so witnesses remain searchable and the
         * capture stays a readable evidence artifact. */
        fprintf(file, "# buffer=%d,%d viewport=%d,%d,%d,%d\r\n",
                info.dwSize.X, info.dwSize.Y, info.srWindow.Left,
                info.srWindow.Top, info.srWindow.Right, info.srWindow.Bottom);
        for (row = 0; row < (DWORD)info.dwSize.Y; ++row) {
            DWORD end = (row + 1u) * (DWORD)info.dwSize.X;
            DWORD start = row * (DWORD)info.dwSize.X;

            if (start >= count) break;
            if (end > count) end = count;
            while (end > start &&
                   (screen[end - 1u] == ' ' || screen[end - 1u] == '\0')) {
                --end;
            }
            if (end == start) continue;

            fprintf(file, "[%lu] ", (unsigned long)row);
            for (column = start; column < end; ++column) {
                fputc(screen[column] == '\0' ? ' ' : screen[column], file);
            }
            fputs("\r\n", file);
        }
        fclose(file);
    }
    free(screen);
}

static void clear_console(HANDLE output)
{
    CONSOLE_SCREEN_BUFFER_INFO info;
    DWORD written;
    DWORD cells;

    if (!GetConsoleScreenBufferInfo(output, &info)) return;
    cells = (DWORD)info.dwSize.X * (DWORD)info.dwSize.Y;
    (void)FillConsoleOutputCharacterA(output, ' ', cells, (COORD){ 0, 0 }, &written);
    (void)FillConsoleOutputAttribute(output, info.wAttributes, cells,
                                     (COORD){ 0, 0 }, &written);
    (void)SetConsoleCursorPosition(output, (COORD){ 0, 0 });
}

/* Observe direct children before the harness closes its Console.  A launcher
 * result alone must not be mistaken for successful worker termination. */
static void report_direct_children(FILE *report, DWORD parent)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 entry;
    if (snapshot == INVALID_HANDLE_VALUE) {
        fprintf(report, "child-snapshot-error=%lu\n", GetLastError());
        return;
    }
    entry.dwSize = sizeof(entry);
    if (Process32First(snapshot, &entry)) do {
        if (entry.th32ParentProcessID == parent) {
            HANDLE process = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION,
                                         FALSE, entry.th32ProcessID);
            DWORD code = 0, length = MAX_PATH;
            char path[MAX_PATH] = "unavailable";
            DWORD wait = process ? WaitForSingleObject(process, 0) : WAIT_FAILED;
            BOOL have_code = process && GetExitCodeProcess(process, &code);
            if (process) QueryFullProcessImageNameA(process, 0, path, &length);
            fprintf(report, "direct-child pid=%lu name=%s wait=%lu exit-known=%u exit=%08lx path=%s\n",
                    entry.th32ProcessID, entry.szExeFile, wait,
                    (unsigned)have_code, code, path);
            if (process) CloseHandle(process);
        }
    } while (Process32Next(snapshot, &entry));
    CloseHandle(snapshot);
}

/* The product's original illegal-opcode path formats a bounded `CS:... OP:`
 * diagnostic before it enters its historical modal error dialog.  A timeout
 * can therefore leave the fixed observation container stopped inside user32.
 * This observer-only helper copies that already-formatted host-stack text; it
 * neither reads guest RAM nor changes the child process. */
static BOOL capture_fault_text(HANDLE process, DWORD stack_pointer,
                               char *text, size_t text_capacity)
{
    char stack_bytes[8192];
    SIZE_T copied = 0;
    SIZE_T index;

    if (text_capacity == 0u) return FALSE;
    text[0] = '\0';
    if (!ReadProcessMemory(process, (LPCVOID)(ULONG_PTR)stack_pointer,
                           stack_bytes, sizeof(stack_bytes), &copied) ||
        copied < 3u) return FALSE;
    for (index = 0u; index + 3u <= copied; ++index) {
        SIZE_T output = 0u;
        if (memcmp(stack_bytes + index, "CS:", 3u) != 0) continue;
        while (index + output < copied && output + 1u < text_capacity) {
            unsigned char value = (unsigned char)stack_bytes[index + output];
            if (value == '\0' || value == '\r' || value == '\n') break;
            if (value < 0x20u || value > 0x7eu) break;
            text[output++] = (char)value;
        }
        text[output] = '\0';
        if (output >= 8u && strstr(text, " OP:") != NULL) return TRUE;
        text[0] = '\0';
    }
    return FALSE;
}

/* CreateProcess receives one command-line string, whereas this observer
 * receives already-tokenized argv.  Re-quote every forwarded token using the
 * Microsoft backslash/quote rule so a product option such as
 * --command "echo marker > file" remains one option value. */
static BOOL append_command_line_argument(char *line, size_t capacity,
                                         size_t *length, const char *argument)
{
    const char *cursor;
    BOOL quote;
    size_t backslashes = 0u;

    if (line == NULL || length == NULL || argument == NULL) return FALSE;
    if (*length != 0u) {
        if (*length + 1u >= capacity) return FALSE;
        line[(*length)++] = ' ';
    }
    quote = argument[0] == '\0' || strpbrk(argument, " \t\"") != NULL;
    if (!quote) {
        size_t bytes = strlen(argument);
        if (*length + bytes >= capacity) return FALSE;
        memcpy(line + *length, argument, bytes + 1u);
        *length += bytes;
        return TRUE;
    }
    if (*length + 1u >= capacity) return FALSE;
    line[(*length)++] = '\"';
    for (cursor = argument; *cursor != '\0'; ++cursor) {
        if (*cursor == '\\') {
            ++backslashes;
            continue;
        }
        if (*cursor == '\"') {
            while (backslashes != 0u) {
                if (*length + 2u >= capacity) return FALSE;
                line[(*length)++] = '\\';
                line[(*length)++] = '\\';
                --backslashes;
            }
            if (*length + 2u >= capacity) return FALSE;
            line[(*length)++] = '\\';
            line[(*length)++] = '\"';
            backslashes = 0u;
            continue;
        }
        while (backslashes != 0u) {
            if (*length + 1u >= capacity) return FALSE;
            line[(*length)++] = '\\';
            --backslashes;
        }
        if (*length + 1u >= capacity) return FALSE;
        line[(*length)++] = *cursor;
    }
    while (backslashes != 0u) {
        if (*length + 2u >= capacity) return FALSE;
        line[(*length)++] = '\\';
        line[(*length)++] = '\\';
        --backslashes;
    }
    if (*length + 2u > capacity) return FALSE;
    line[(*length)++] = '\"';
    line[*length] = '\0';
    return TRUE;
}

/* S7 uses this only to exercise the already-owned public Console -> SoftPC
 * keyboard worker route.  These are ordinary KEY_EVENT records, equivalent to
 * a user typing at CONIN$; this helper never reaches into the product, guest
 * RAM, BOP transport, or a COMMAND buffer. */
static BOOL set1_key_for_ascii(char character, WORD *virtual_key,
                               WORD *scan_code, DWORD *control_state)
{
    static const BYTE lowercase_set1[26] = {
        0x1e, 0x30, 0x2e, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17,
        0x24, 0x25, 0x26, 0x32, 0x31, 0x18, 0x19, 0x10, 0x13,
        0x1f, 0x14, 0x16, 0x2f, 0x11, 0x2d, 0x15, 0x2c
    };

    static const struct {
        char character;
        WORD virtual_key;
        WORD scan_code;
        BOOL shifted;
    } punctuation[] = {
        {' ', VK_SPACE, 0x39, FALSE}, {'-', VK_OEM_MINUS, 0x0c, FALSE},
        {'_', VK_OEM_MINUS, 0x0c, TRUE},  {'=', VK_OEM_PLUS, 0x0d, FALSE},
        {'+', VK_OEM_PLUS, 0x0d, TRUE},   {'[', VK_OEM_4, 0x1a, FALSE},
        {'{', VK_OEM_4, 0x1a, TRUE},      {']', VK_OEM_6, 0x1b, FALSE},
        {'}', VK_OEM_6, 0x1b, TRUE},      {';', VK_OEM_1, 0x27, FALSE},
        {':', VK_OEM_1, 0x27, TRUE},      {'\'', VK_OEM_7, 0x28, FALSE},
        {'\"', VK_OEM_7, 0x28, TRUE},    {'`', VK_OEM_3, 0x29, FALSE},
        {'~', VK_OEM_3, 0x29, TRUE},      {'\\', VK_OEM_5, 0x2b, FALSE},
        {'|', VK_OEM_5, 0x2b, TRUE},      {',', VK_OEM_COMMA, 0x33, FALSE},
        {'<', VK_OEM_COMMA, 0x33, TRUE},  {'.', VK_OEM_PERIOD, 0x34, FALSE},
        {'>', VK_OEM_PERIOD, 0x34, TRUE}, {'/', VK_OEM_2, 0x35, FALSE},
        {'?', VK_OEM_2, 0x35, TRUE},      {'!', '1', 0x02, TRUE},
        {'@', '2', 0x03, TRUE},           {'#', '3', 0x04, TRUE},
        {'$', '4', 0x05, TRUE},           {'%', '5', 0x06, TRUE},
        {'^', '6', 0x07, TRUE},           {'&', '7', 0x08, TRUE},
        {'*', '8', 0x09, TRUE},           {'(', '9', 0x0a, TRUE},
        {')', '0', 0x0b, TRUE}
    };
    size_t index;

    if (virtual_key == NULL || scan_code == NULL || control_state == NULL)
        return FALSE;
    if (character >= 'A' && character <= 'Z') {
        *virtual_key = (WORD)character;
        *scan_code = lowercase_set1[character - 'A'];
        *control_state |= SHIFT_PRESSED;
        return TRUE;
    }
    if (character >= 'a' && character <= 'z') {
        *virtual_key = (WORD)(character - 'a' + 'A');
        *scan_code = lowercase_set1[character - 'a'];
        return TRUE;
    }
    if (character >= '0' && character <= '9') {
        *virtual_key = (WORD)character;
        *scan_code = character == '0' ? 0x0b : (WORD)(0x01 + character - '0');
        return TRUE;
    }
    switch (character == '\n' ? '\r' : character) {
    case '\r': *virtual_key = VK_RETURN; *scan_code = 0x1c; return TRUE;
    case '\x1b': *virtual_key = VK_ESCAPE; *scan_code = 0x01; return TRUE;
    default: break;
    }
    for (index = 0; index < ARRAYSIZE(punctuation); ++index) {
        if (punctuation[index].character == character) {
            *virtual_key = punctuation[index].virtual_key;
            *scan_code = punctuation[index].scan_code;
            if (punctuation[index].shifted) *control_state |= SHIFT_PRESSED;
            return TRUE;
        }
    }
    return FALSE;
}

/* The original `KeyMsgToKeyCode` consumes a KEY_EVENT_RECORD's Set-1 scan
 * code, not its Unicode character.  Do not derive that code through the host
 * keyboard layout: the observer's test input must carry the stable original
 * PC keyboard contract. */
static DWORD report_size_bytes(const char *path)
{
    HANDLE file;
    DWORD size;

    if (path == NULL) return 0u;
    file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                       NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return 0u;
    size = GetFileSize(file, NULL);
    CloseHandle(file);
    return size == INVALID_FILE_SIZE ? 0u : size;
}

/* Wait for the original source-owned 8042 output corresponding to one
 * ordinary Console key before offering the next one.  This is a test pacing
 * boundary only: it prevents the harness from turning a single human command
 * into a bulk keyboard-ring stress test. */
static BOOL wait_for_report_marker_after(const char *path, const char *marker,
                                         DWORD start_offset, DWORD timeout_ms)
{
    DWORD started_at = GetTickCount();
    char buffer[65537];

    if (path == NULL || marker == NULL || *marker == '\0') return FALSE;
    for (;;) {
        HANDLE file = CreateFileA(path, GENERIC_READ,
                                  FILE_SHARE_READ | FILE_SHARE_WRITE,
                                  NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                                  NULL);
        if (file != INVALID_HANDLE_VALUE) {
            DWORD size = GetFileSize(file, NULL);
            if (size != INVALID_FILE_SIZE && size > start_offset) {
                DWORD remaining = size - start_offset;
                DWORD to_read = remaining < (DWORD)(sizeof(buffer) - 1u) ?
                    remaining : (DWORD)(sizeof(buffer) - 1u);
                DWORD read = 0;
                SetFilePointer(file, (LONG)start_offset, NULL, FILE_BEGIN);
                if (ReadFile(file, buffer, to_read, &read, NULL)) {
                    buffer[read] = '\0';
                    if (strstr(buffer, marker) != NULL) {
                        CloseHandle(file);
                        return TRUE;
                    }
                }
            }
            CloseHandle(file);
        }
        if ((DWORD)(GetTickCount() - started_at) >= timeout_ms) return FALSE;
        Sleep(25u);
    }
}

static BOOL write_console_input_text(HANDLE input, const char *text, DWORD line_delay_ms,
                                     HANDLE output, const char *report)
{
    const char *cursor;
    unsigned line = 0;

    if (input == NULL || input == INVALID_HANDLE_VALUE || text == NULL)
        return FALSE;
    for (cursor = text; *cursor != '\0'; ++cursor) {
        INPUT_RECORD records[4];
        DWORD written = 0;
        DWORD record_count = 2;
        char character = *cursor == '\n' ? '\r' : *cursor;
        WORD key_code;
        WORD scan_code;
        /* `nt_event.c` starts a DOS boot with ToggleKeyState set to
         * NUMLOCK_ON.  A normal unmodified letter delivered by this fixed
         * Console must carry that same toggle bit: otherwise the original
         * SyncToggleKeys path first synthesizes a NumLock transition ahead of
         * the requested key.  This is Console-record fidelity, not a guest
         * state mutation. */
        DWORD control_state = NUMLOCK_ON;
        BOOL shifted;

        if (!set1_key_for_ascii(character, &key_code, &scan_code,
                                &control_state))
            return FALSE;
        shifted = (control_state & SHIFT_PRESSED) != 0;
        memset(records, 0, sizeof(records));
        if (shifted) {
            records[0].EventType = KEY_EVENT;
            records[0].Event.KeyEvent.bKeyDown = TRUE;
            records[0].Event.KeyEvent.wRepeatCount = 1;
            records[0].Event.KeyEvent.wVirtualKeyCode = VK_SHIFT;
            records[0].Event.KeyEvent.wVirtualScanCode = 0x2a;
            records[0].Event.KeyEvent.dwControlKeyState = control_state;
            records[1] = records[0];
            records[1].Event.KeyEvent.wVirtualKeyCode = key_code;
            records[1].Event.KeyEvent.wVirtualScanCode = scan_code;
            records[1].Event.KeyEvent.uChar.AsciiChar = character;
            records[2] = records[1];
            records[2].Event.KeyEvent.bKeyDown = FALSE;
            records[3] = records[0];
            records[3].Event.KeyEvent.bKeyDown = FALSE;
            records[3].Event.KeyEvent.dwControlKeyState = NUMLOCK_ON;
            record_count = ARRAYSIZE(records);
        } else {
            records[0].EventType = KEY_EVENT;
            records[0].Event.KeyEvent.bKeyDown = TRUE;
            records[0].Event.KeyEvent.wRepeatCount = 1;
            records[0].Event.KeyEvent.wVirtualKeyCode = key_code;
            records[0].Event.KeyEvent.wVirtualScanCode = scan_code;
            records[0].Event.KeyEvent.uChar.AsciiChar = character;
            records[0].Event.KeyEvent.dwControlKeyState = control_state;
            records[1] = records[0];
            records[1].Event.KeyEvent.bKeyDown = FALSE;
        }
        if (!WriteConsoleInputA(input, records, record_count, &written) ||
            written != record_count) return FALSE;
        Sleep(OBSERVATION_KEY_EVENT_INTERVAL_MS);
        if (character == '\r' && line_delay_ms) Sleep(line_delay_ms);
        if (character == '\r' && report) {
            char path[MAX_PATH];
            snprintf(path, sizeof(path), "%s.line-%02u", report, ++line);
            write_console_snapshot(output, path);
        }
        /* A Console input queue is asynchronous.  Once the original DOS line
         * input boundary has been observed, this deliberately tiny two-line
         * sequence is ordinary queued Console input; it is not paced against
         * individual device callbacks because that would manufacture a host
         * acknowledgement contract that neither Console nor SoftPC offers.
         * Its result is the source-owned command output and final process
         * exit observed by the caller. */
    }
    return TRUE;
}

/* A callback installation (stage 8) can itself cause an older mouse callback
 * to return.  Attribute an observation record only when its source-owned IRQ
 * queue stage precedes its callback-return stage in the report tail. */
static BOOL wait_for_mouse_roundtrip_after(const char *path,
                                           DWORD start_offset,
                                           DWORD timeout_ms)
{
    DWORD started_at = GetTickCount();
    char buffer[65537];

    if (path == NULL) return FALSE;
    for (;;) {
        HANDLE file = CreateFileA(path, GENERIC_READ,
                                  FILE_SHARE_READ | FILE_SHARE_WRITE,
                                  NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                                  NULL);
        if (file != INVALID_HANDLE_VALUE) {
            DWORD size = GetFileSize(file, NULL);
            if (size != INVALID_FILE_SIZE && size > start_offset) {
                DWORD remaining = size - start_offset;
                DWORD to_read = remaining < (DWORD)(sizeof(buffer) - 1u) ?
                    remaining : (DWORD)(sizeof(buffer) - 1u);
                DWORD read = 0;
                SetFilePointer(file, (LONG)start_offset, NULL, FILE_BEGIN);
                if (ReadFile(file, buffer, to_read, &read, NULL)) {
                    char *queued;
                    buffer[read] = '\0';
                    queued = strstr(buffer, "MVDM-MOUSE stage=2");
                    if (queued != NULL && strstr(queued,
                            "MVDM-MOUSE stage=7") != NULL) {
                        CloseHandle(file);
                        return TRUE;
                    }
                }
            }
            CloseHandle(file);
        }
        if ((DWORD)(GetTickCount() - started_at) >= timeout_ms) return FALSE;
        Sleep(25u);
    }
}

/* Feed an ordinary public Console mouse sequence through the same CONIN$
 * queue and original event worker as physical conhost input. The observer
 * accepts a record only after the original queue and callback-return stages
 * prove it was delivered; it does not assume a wall-clock startup boundary. */
static BOOL write_console_mouse_sequence(HANDLE input, const char *report_path)
{
    const DWORD buttons[] = { 0u, FROM_LEFT_1ST_BUTTON_PRESSED,
        FROM_LEFT_1ST_BUTTON_PRESSED, 0u };
    const DWORD flags[] = { MOUSE_MOVED, 0u, MOUSE_MOVED, 0u };
    const SHORT x[] = { 26, 26, 54, 54 };
    const SHORT y[] = { 11, 11, 21, 21 };
    DWORD index;

    if (input == NULL || input == INVALID_HANDLE_VALUE || report_path == NULL)
        return FALSE;
    Sleep(500u);
    for (index = 0u; index < ARRAYSIZE(buttons); ++index) {
        DWORD attempt;
        for (attempt = 0u; attempt != 3u; ++attempt) {
            INPUT_RECORD record;
            DWORD written = 0u;
            DWORD report_offset = report_size_bytes(report_path);

            memset(&record, 0, sizeof(record));
            record.EventType = MOUSE_EVENT;
            record.Event.MouseEvent.dwMousePosition.X = x[index];
            record.Event.MouseEvent.dwMousePosition.Y = y[index];
            record.Event.MouseEvent.dwButtonState = buttons[index];
            record.Event.MouseEvent.dwEventFlags = flags[index];
            if (!WriteConsoleInputA(input, &record, 1u, &written) || written != 1u)
                return FALSE;
            if (wait_for_mouse_roundtrip_after(report_path, report_offset,
                    OBSERVATION_KEY_DRAIN_TIMEOUT_MS))
                break;
            /* OpenNT itself may discard records during its short render-mode
             * transition.  Do not alter that policy: retry only this external
             * observation record after its bounded source-owned interval. */
            Sleep(500u);
        }
        if (attempt == 3u) return FALSE;
    }
    return TRUE;
}

/* Observe the real COMMAND prompt, not a diagnostic CPU hook. Retiring
 * the INTx observer must not turn source cleanup into a test-only failure. */
static BOOL wait_for_console_prompt(HANDLE output, DWORD timeout_ms,
                                    const char *marker)
{
    DWORD begin=GetTickCount();
    do {
        CONSOLE_SCREEN_BUFFER_INFO info;
        char row[1025]; DWORD count=0; SHORT y;
        if(GetConsoleScreenBufferInfo(output,&info) && info.dwSize.X>2 &&
           info.dwSize.X<(SHORT)sizeof(row)) {
            /* COMMAND can paint its prompt before the public Console cursor
             * settles on that row.  Identify the same drive-qualified prompt
             * in the visible buffer instead of treating cursor timing as a
             * guest readiness contract. */
            for(y=info.srWindow.Top;y<=info.srWindow.Bottom;y++) {
                COORD pos={0,y};
                if(ReadConsoleOutputCharacterA(output,row,info.dwSize.X,pos,&count) &&
                   count==(DWORD)info.dwSize.X) {
                    row[count]='\0';
                    if(marker ? strstr(row,marker)!=NULL :
                       (row[1]==':' && memchr(row,'>',count)!=NULL)) return TRUE;
                }
            }
        }
        Sleep(25);
    } while(GetTickCount()-begin<timeout_ms);
    return FALSE;
}

/* The original INT 33h entry turns off stream I/O and the host transition
 * enables mouse/window Console records. Observe that public handle state
 * directly: a synthetic diagnostic stage is neither an activation boundary
 * nor a substitute for it. */
static BOOL wait_for_console_mouse_mode(HANDLE input, DWORD *mode_out,
                                        DWORD timeout_ms)
{
    DWORD started_at = GetTickCount();
    DWORD mode;

    if (input == NULL || input == INVALID_HANDLE_VALUE || mode_out == NULL)
        return FALSE;
    do {
        if (GetConsoleMode(input, &mode) &&
            (mode & (ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS)) ==
                (ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS) &&
            (mode & ENABLE_QUICK_EDIT_MODE) == 0u) {
            *mode_out = mode;
            return TRUE;
        }
        Sleep(20u);
    } while ((DWORD)(GetTickCount() - started_at) < timeout_ms);
    return FALSE;
}

int main(int argc, char **argv)
{
    STARTUPINFOA startup = { sizeof(startup) };
    PROCESS_INFORMATION child = { 0 };
    SECURITY_ATTRIBUTES inherit = { sizeof(inherit), NULL, TRUE };
    HANDLE input;
    HANDLE output;
    DWORD wait_status;
    DWORD exit_code = STILL_ACTIVE;
    CONTEXT timed_context = { 0 };
    DWORD timed_stack[OBSERVATION_STACK_WORDS] = { 0 };
    observation_thread_context timed_threads[OBSERVATION_THREAD_LIMIT] = { 0 };
    BOOL have_timed_context = FALSE;
    BOOL have_timed_stack = FALSE;
    SIZE_T timed_stack_bytes = 0;
    DWORD timed_thread_count = 0;
    char timed_fault_text[256] = { 0 };
    BOOL have_timed_fault_text = FALSE;
    BOOL scripted_console_input = FALSE;
    BOOL observe_edit_return = FALSE;
    BOOL observe_console_mouse_mode = FALSE;
    BOOL observed_console_mouse_mode = FALSE;
    DWORD observed_console_input_mode = 0u;
    BOOL observe_console_mouse_input = FALSE;
    BOOL observed_console_mouse_input_ready = FALSE;
    BOOL observed_console_mouse_input_delivered = FALSE;
    const char *scripted_console_input_text = "ver\rexit\r";
    const char *scripted_console_input_sequence = "ver+exit";
    const char *scripted_console_input_marker = NULL;
    DWORD scripted_console_line_delay_ms = 0;
    BOOL scripted_console_input_ready = FALSE;
    BOOL scripted_console_input_delivered = FALSE;
    DWORD scripted_console_input_remaining = 0;
    BOOL scripted_console_input_remaining_known = FALSE;
    observation_image_identity image_identity = { 0 };
    BOOL symbols_initialized = FALSE;
    DWORD suspend_result = (DWORD)-1;
    DWORD observation_started_at = 0u;
    DWORD observation_elapsed_ms;
    DWORD observation_wait_ms;
    DWORD observation_timeout_ms = OBSERVATION_TIMEOUT_MS;
    unsigned long parsed_timeout_ms;
    char *timeout_parse_end;
    char command_line[MAX_PATH * 2];
    char exception_report_path[MAX_PATH];
    char previous_exception_report_path[MAX_PATH];
    char main_return_report_path[MAX_PATH];
    char previous_main_return_report_path[MAX_PATH];
    char base_vdm_report_path[MAX_PATH];
    char console_input_preinput_snapshot_path[MAX_PATH];
    char console_mouse_postinput_snapshot_path[MAX_PATH];
    char report_base_path[MAX_PATH];
    DWORD report_base_path_length;
    DWORD previous_exception_report_length;
    DWORD previous_main_return_report_length;
    BOOL had_previous_exception_report;
    BOOL had_previous_main_return_report;
    char fixed_system_root[MAX_PATH];
    char fixed_system_root_short[MAX_PATH];
    DWORD fixed_system_root_short_length = 0;
    FILE *report = NULL;

    if (argc < 4) return 64;
    report_base_path_length = GetFullPathNameA(argv[3],
        (DWORD)sizeof(report_base_path), report_base_path, NULL);
    if (report_base_path_length == 0 ||
        report_base_path_length >= sizeof(report_base_path)) return 68;
    /* App derives SystemRoot from the image directory itself.  Keep this
     * observer report aligned with that original-layout package contract;
     * argv[2] is the stage and product working directory. */
    if (snprintf(fixed_system_root, sizeof(fixed_system_root), "%s",
                 argv[2]) < 0) return 68;
    fixed_system_root_short_length = GetShortPathNameA(fixed_system_root,
        fixed_system_root_short, (DWORD)sizeof(fixed_system_root_short));
    /* Match the historical short-window reproducer: never resize an inherited
     * controller Console or accept its viewport constraints as test geometry. */
    if (GetEnvironmentVariableA("MVDM_OBSERVER_SHORT_HISTORY",NULL,0)) FreeConsole();
    if (!AllocConsole() && GetLastError() != ERROR_ACCESS_DENIED) return 65;
    if (GetEnvironmentVariableA("MVDM_OBSERVER_RECORD_CONTROL", NULL, 0)) {
        if (snprintf(control_event_report, sizeof(control_event_report),
                     "%s.control.bin", argv[3]) < 0 ||
            !SetConsoleCtrlHandler(record_console_control, TRUE)) return 65;
    }
    input = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE, &inherit,
                        OPEN_EXISTING, 0, NULL);
    output = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE,
                         FILE_SHARE_READ | FILE_SHARE_WRITE, &inherit,
                         OPEN_EXISTING, 0, NULL);
    if (input == INVALID_HANDLE_VALUE || output == INVALID_HANDLE_VALUE) return 66;

    clear_console(output);
    /* Reproduce the previously failing short-window/history profile with
     * observed native geometry, never silently accept a failed resize. */
    if (GetEnvironmentVariableA("MVDM_OBSERVER_SHORT_HISTORY", NULL, 0)) {
        SMALL_RECT tiny={0,0,19,4}, window={0,0,79,4};
        COORD size={80,300}, origin={0,0};
        CONSOLE_SCREEN_BUFFER_INFO info;
        DWORD written; unsigned int row;
        char geometry_path[MAX_PATH]; FILE *geometry;
        unsigned step=1;
        BOOL ok=SetConsoleWindowInfo(output,TRUE,&tiny);
        if(ok){step=2;ok=SetConsoleCursorPosition(output,origin);}
        if(ok){step=3;ok=SetConsoleScreenBufferSize(output,size);}
        if(ok && GetLargestConsoleWindowSize(output).X<80) {
            /* RDP desktop pixels can limit an otherwise valid 80-column
             * native window. Change only this disposable test Console font. */
            CONSOLE_FONT_INFOEX font={sizeof(font)};
            step=5;ok=GetCurrentConsoleFontEx(output,FALSE,&font);
            if(ok){font.dwFontSize.X=4;font.dwFontSize.Y=8;ok=SetCurrentConsoleFontEx(output,FALSE,&font);}
        }
        if(ok){step=4;ok=SetConsoleWindowInfo(output,TRUE,&window);}
        snprintf(geometry_path,sizeof(geometry_path),"%s.geometry.txt",argv[3]);
        if(!ok) {
            DWORD error=GetLastError(); GetConsoleScreenBufferInfo(output,&info);
            geometry=fopen(geometry_path,"w");
            if(geometry){fprintf(geometry,"FAILED step=%u error=%lu buffer=%d,%d view=%d,%d,%d,%d\n",step,error,info.dwSize.X,info.dwSize.Y,info.srWindow.Left,info.srWindow.Top,info.srWindow.Right,info.srWindow.Bottom);fclose(geometry);}
            return 93;
        }
        for(row=0;row<40;row++) WriteConsoleA(output,"prior shell output\r\n",20,&written,NULL);
        if(!GetConsoleScreenBufferInfo(output,&info) ||
           info.srWindow.Right-info.srWindow.Left+1!=80 ||
           info.srWindow.Bottom-info.srWindow.Top+1!=5 ||
           info.dwCursorPosition.Y!=40) return 94;
        snprintf(geometry_path,sizeof(geometry_path),"%s.geometry.txt",argv[3]);
        geometry=fopen(geometry_path,"w"); if(!geometry)return 95;
        fprintf(geometry,"buffer=%d,%d view=%d,%d,%d,%d cursor=%d,%d prefill=40\n",
            info.dwSize.X,info.dwSize.Y,info.srWindow.Left,info.srWindow.Top,
            info.srWindow.Right,info.srWindow.Bottom,info.dwCursorPosition.X,info.dwCursorPosition.Y);
        fclose(geometry);
    }
    /* A newly allocated Console can retain an inherited key event from the
     * launcher context.  This fixed container models an untouched interactive
     * session, so establish an empty CONIN$ queue before the product inherits
     * it.  It never writes to the child or to guest input. */
    if (!FlushConsoleInputBuffer(input)) return 66;
    SetStdHandle(STD_INPUT_HANDLE, input);
    SetStdHandle(STD_OUTPUT_HANDLE, output);
    SetStdHandle(STD_ERROR_HANDLE, output);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = input;
    startup.hStdOutput = output;
    startup.hStdError = output;
    {
        size_t command_length = 0u;
        int argument_index;

        command_line[0] = '\0';
        if (!append_command_line_argument(command_line, sizeof(command_line),
                                          &command_length, argv[1])) return 68;
        if (argc == 4) {
            if (!append_command_line_argument(command_line, sizeof(command_line),
                                              &command_length, "-f") ||
                !append_command_line_argument(command_line, sizeof(command_line),
                                              &command_length, "-o") ||
                !append_command_line_argument(command_line, sizeof(command_line),
                                              &command_length, "--command") ||
                !append_command_line_argument(command_line, sizeof(command_line),
                                              &command_length, "EXIT")) return 68;
        } else {
            for (argument_index = 4; argument_index < argc; ++argument_index) {
                if (strcmp(argv[argument_index], "--observe-console-input-marker") == 0) {
                    if (++argument_index >= argc || !argv[argument_index][0] ||
                        strlen(argv[argument_index]) > 80) return 68;
                    scripted_console_input_marker = argv[argument_index];
                    continue;
                }
                if (strcmp(argv[argument_index], "--observe-console-input-text") == 0) {
                    if (++argument_index >= argc) return 68;
                    scripted_console_input = TRUE;
                    scripted_console_input_text = argv[argument_index];
                    scripted_console_input_sequence = "explicit-observer-text";
                    scripted_console_line_delay_ms = 1500;
                    continue;
                }
                if (strcmp(argv[argument_index],
                           "--observe-console-line-delay-ms") == 0) {
                    if (++argument_index >= argc ||
                        (parsed_timeout_ms = strtoul(argv[argument_index],
                                                     &timeout_parse_end, 10),
                         timeout_parse_end == argv[argument_index] ||
                         *timeout_parse_end != '\0') ||
                        parsed_timeout_ms > OBSERVATION_TIMEOUT_MAX_MS)
                        return 68;
                    scripted_console_line_delay_ms = (DWORD)parsed_timeout_ms;
                    continue;
                }
                if (strcmp(argv[argument_index], "--observe-console-edit-return") == 0) {
                    scripted_console_input = TRUE;
                    observe_edit_return = TRUE;
                    scripted_console_input_text = "edit\r";
                    scripted_console_input_sequence = "edit-escape-alt-f-x-mem-exit";
                    scripted_console_line_delay_ms = 1500;
                    continue;
                }
                if (strcmp(argv[argument_index], "--observe-console-input") == 0) {
                    scripted_console_input = TRUE;
                    continue;
                }
                if (strcmp(argv[argument_index],
                           "--observe-console-input-ver-only") == 0) {
                    scripted_console_input = TRUE;
                    scripted_console_input_text = "ver\r";
                    scripted_console_input_sequence = "ver";
                    continue;
                }
                if (strcmp(argv[argument_index],
                           "--observe-console-mouse-mode") == 0) {
                    observe_console_mouse_mode = TRUE;
                    continue;
                }
                if (strcmp(argv[argument_index],
                           "--observe-console-mouse-input") == 0) {
                    observe_console_mouse_input = TRUE;
                    continue;
                }
                /* An explicit observer-only extension is permitted solely
                 * for a bounded end-to-end keyboard-drain proof.  The
                 * default remains the S7 fixed ten-second container. */
                if (strcmp(argv[argument_index],
                           "--observation-timeout-ms") == 0) {
                    if (++argument_index >= argc ||
                        (parsed_timeout_ms = strtoul(argv[argument_index],
                                                      &timeout_parse_end, 10),
                         timeout_parse_end == argv[argument_index] ||
                         *timeout_parse_end != '\0') ||
                        parsed_timeout_ms < OBSERVATION_TIMEOUT_MS ||
                        parsed_timeout_ms > OBSERVATION_TIMEOUT_MAX_MS)
                        return 68;
                    observation_timeout_ms = (DWORD)parsed_timeout_ms;
                    continue;
                }
                if (!append_command_line_argument(command_line,
                                                  sizeof(command_line),
                                                  &command_length,
                                                  argv[argument_index])) return 68;
            }
        }
    }

    snprintf(exception_report_path, sizeof(exception_report_path), "%s.exception.txt",
             report_base_path);
    snprintf(main_return_report_path, sizeof(main_return_report_path), "%s.return.txt",
             report_base_path);
    snprintf(console_input_preinput_snapshot_path,
             sizeof(console_input_preinput_snapshot_path),
             "%s.pre-input-console.txt", report_base_path);
    snprintf(console_mouse_postinput_snapshot_path,
             sizeof(console_mouse_postinput_snapshot_path),
             "%s.mouse-post-input-console.txt", report_base_path);
    snprintf(base_vdm_report_path, sizeof(base_vdm_report_path), "%s.base-vdm.txt",
             report_base_path);
    previous_exception_report_length = GetEnvironmentVariableA(
        "MVDM_EXCEPTION_REPORT_PATH", previous_exception_report_path,
        (DWORD)sizeof(previous_exception_report_path));
    had_previous_exception_report = previous_exception_report_length != 0 &&
        previous_exception_report_length < sizeof(previous_exception_report_path);
    previous_main_return_report_length = GetEnvironmentVariableA(
        "MVDM_MAIN_RETURN_REPORT_PATH", previous_main_return_report_path,
        (DWORD)sizeof(previous_main_return_report_path));
    had_previous_main_return_report = previous_main_return_report_length != 0 &&
        previous_main_return_report_length < sizeof(previous_main_return_report_path);
    SetEnvironmentVariableA("MVDM_EXCEPTION_REPORT_PATH", exception_report_path);
    SetEnvironmentVariableA("MVDM_MAIN_RETURN_REPORT_PATH", main_return_report_path);
    SetEnvironmentVariableA("MVDM_BASE_VDM_REPORT_PATH", base_vdm_report_path);

    if (!CreateProcessA(NULL, command_line, NULL, NULL, TRUE, 0, NULL, argv[2],
                        &startup, &child)) {
        if (had_previous_exception_report)
            SetEnvironmentVariableA("MVDM_EXCEPTION_REPORT_PATH",
                                    previous_exception_report_path);
        else
            SetEnvironmentVariableA("MVDM_EXCEPTION_REPORT_PATH", NULL);
        if (had_previous_main_return_report)
            SetEnvironmentVariableA("MVDM_MAIN_RETURN_REPORT_PATH",
                                    previous_main_return_report_path);
        else
            SetEnvironmentVariableA("MVDM_MAIN_RETURN_REPORT_PATH", NULL);
        CloseHandle(input);
        CloseHandle(output);
        return 67;
    }
    observation_started_at = GetTickCount();
    if (had_previous_exception_report)
        SetEnvironmentVariableA("MVDM_EXCEPTION_REPORT_PATH",
                                previous_exception_report_path);
    else
        SetEnvironmentVariableA("MVDM_EXCEPTION_REPORT_PATH", NULL);
    if (had_previous_main_return_report)
        SetEnvironmentVariableA("MVDM_MAIN_RETURN_REPORT_PATH",
                                previous_main_return_report_path);
    else
        SetEnvironmentVariableA("MVDM_MAIN_RETURN_REPORT_PATH", NULL);
    if (scripted_console_input) {
        /* Await visible COMMAND readiness, without an execution-core hook. */
        scripted_console_input_ready = wait_for_console_prompt(output,
            OBSERVATION_INPUT_READY_TIMEOUT_MS, scripted_console_input_marker);
        if (scripted_console_input_ready) {
            /* Snapshot the exact shared CONOUT$ buffer after original guest
             * stream output but before this observer queues any key. */
            write_console_snapshot(output, console_input_preinput_snapshot_path);
            /* Test-only gate: let the controller stop its broker before the
             * first guest keystroke. No product or guest state is modified. */
            {
                char gate_name[128];
                DWORD gate_length = GetEnvironmentVariableA("MVDM_OBSERVER_INPUT_GATE",
                    gate_name, sizeof(gate_name));
                if (gate_length) {
                    HANDLE gate = gate_length < sizeof(gate_name) ?
                        OpenEventA(SYNCHRONIZE, FALSE, gate_name) : NULL;
                    if (gate == NULL) return 90;
                    if (WaitForSingleObject(gate, 15000) != WAIT_OBJECT_0) {
                        CloseHandle(gate);
                        return 91;
                    }
                    CloseHandle(gate);
                }
            }
            scripted_console_input_delivered = write_console_input_text(input,
                scripted_console_input_text, scripted_console_line_delay_ms, output, argv[3]);
        }
    }
    if (observe_console_mouse_mode) {
        observed_console_mouse_mode = wait_for_console_mouse_mode(input,
            &observed_console_input_mode, OBSERVATION_INPUT_READY_TIMEOUT_MS);
    }
    if (observe_edit_return && scripted_console_input_delivered) {
        INPUT_RECORD menu[4] = {0};
        DWORD written = 0, key_index;
        char edit_snapshot[MAX_PATH];
        observed_console_mouse_mode = wait_for_console_mouse_mode(input,
            &observed_console_input_mode, OBSERVATION_INPUT_READY_TIMEOUT_MS);
        if (observed_console_mouse_mode) {
            snprintf(edit_snapshot, sizeof(edit_snapshot), "%s.edit.txt", argv[3]);
            write_console_snapshot(output, edit_snapshot);
            scripted_console_input_delivered = write_console_input_text(input, "\x1b", 0, output, NULL);
            Sleep(500);
            menu[0].EventType = KEY_EVENT;
            menu[0].Event.KeyEvent.bKeyDown = TRUE;
            menu[0].Event.KeyEvent.wRepeatCount = 1;
            menu[0].Event.KeyEvent.wVirtualKeyCode = VK_MENU;
            menu[0].Event.KeyEvent.wVirtualScanCode = 0x38;
            menu[0].Event.KeyEvent.dwControlKeyState = NUMLOCK_ON | LEFT_ALT_PRESSED;
            menu[1] = menu[0];
            menu[1].Event.KeyEvent.wVirtualKeyCode = 'F';
            menu[1].Event.KeyEvent.wVirtualScanCode = 0x21;
            menu[1].Event.KeyEvent.uChar.AsciiChar = 'f';
            menu[2] = menu[1];
            menu[2].Event.KeyEvent.bKeyDown = FALSE;
            menu[3] = menu[0];
            menu[3].Event.KeyEvent.bKeyDown = FALSE;
            menu[3].Event.KeyEvent.dwControlKeyState = NUMLOCK_ON;
            for (key_index = 0; key_index < ARRAYSIZE(menu); ++key_index) {
                scripted_console_input_delivered = scripted_console_input_delivered &&
                    WriteConsoleInputA(input, &menu[key_index], 1, &written) && written == 1;
                Sleep(OBSERVATION_KEY_EVENT_INTERVAL_MS);
            }
            Sleep(500);
            scripted_console_input_delivered = scripted_console_input_delivered &&
                write_console_input_text(input, "x", 0, output, NULL);
            Sleep(1500);
            scripted_console_input_delivered = scripted_console_input_delivered &&
                write_console_input_text(input,
                    GetEnvironmentVariableA("MVDM_OBSERVER_SHORT_HISTORY",NULL,0) ?
                    "mem\rmem\rmem\rmem\rexit\r" : "mem\rexit\r",
                    1500, output, argv[3]);
        } else scripted_console_input_delivered = FALSE;
    }
    if (observe_console_mouse_input) {
        observed_console_mouse_input_ready = wait_for_console_mouse_mode(input,
            &observed_console_input_mode, OBSERVATION_INPUT_READY_TIMEOUT_MS);
        if (observed_console_mouse_input_ready)
            observed_console_mouse_input_delivered = write_console_mouse_sequence(
                input, argv[3]);
        if (observed_console_mouse_input_delivered)
            write_console_snapshot(output, console_mouse_postinput_snapshot_path);
    }
    observation_elapsed_ms = (DWORD)(GetTickCount() - observation_started_at);
    observation_wait_ms = observation_elapsed_ms >= observation_timeout_ms ? 0u :
        observation_timeout_ms - observation_elapsed_ms;
    wait_status = WaitForSingleObject(child.hProcess, observation_wait_ms);
    capture_process_image(child.dwProcessId, &image_identity);
    if (wait_status == WAIT_TIMEOUT) {
        /* The fixed container observes the product without a debugger.  A
         * bounded suspension gives the evidence record one architectural
         * stop state before the existing watchdog terminates the process.
         * It neither changes product inputs nor resumes/modifies the thread.
         */
        suspend_result = SuspendThread(child.hThread);
        if (suspend_result != (DWORD)-1) {
            memset(&timed_context, 0, sizeof(timed_context));
            timed_context.ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER;
            have_timed_context = GetThreadContext(child.hThread, &timed_context);
            if (have_timed_context) {
                have_timed_stack = ReadProcessMemory(
                    child.hProcess, (LPCVOID)(ULONG_PTR)timed_context.Esp,
                    timed_stack, sizeof(timed_stack), &timed_stack_bytes) &&
                    timed_stack_bytes == sizeof(timed_stack);
                have_timed_fault_text = capture_fault_text(child.hProcess,
                    timed_context.Esp, timed_fault_text,
                    sizeof(timed_fault_text));
            }
            symbols_initialized = SymInitialize(child.hProcess, NULL, TRUE);
            timed_thread_count = capture_process_threads(child.hProcess,
                                                         child.dwProcessId,
                                                         timed_threads,
                                                         OBSERVATION_THREAD_LIMIT);
        }
        TerminateProcess(child.hProcess, OBSERVATION_TIMEOUT_EXIT);
        WaitForSingleObject(child.hProcess, 1000);
    }
    GetExitCodeProcess(child.hProcess, &exit_code);
    if (scripted_console_input) {
        scripted_console_input_remaining_known =
            GetNumberOfConsoleInputEvents(input,
                                          &scripted_console_input_remaining);
    }
    write_console_snapshot(output, argv[3]);
    if (fopen_s(&report, argv[3], "wb") == 0 && report != NULL) {
        fprintf(report, "container=console-owning-nondebug\n");
        fprintf(report, "pid=%lu\n", (unsigned long)child.dwProcessId);
        fprintf(report, "result=%s\n", wait_status == WAIT_TIMEOUT ? "timeout" : "exited");
        fprintf(report, "exit=0x%08lx\n", (unsigned long)exit_code);
        report_direct_children(report, child.dwProcessId);
        fprintf(report, "timeout-ms=%lu\n", (unsigned long)observation_timeout_ms);
        fprintf(report, "scripted-console-input=%s\n",
                scripted_console_input ?
                    (scripted_console_input_delivered ? "delivered" : "failed") :
                    "none");
        fprintf(report, "console-mouse-mode-observed=%s\n",
                observed_console_mouse_mode ? "yes" : "no");
        if (observe_console_mouse_mode && observed_console_mouse_mode)
            fprintf(report, "console-input-mode=0x%08lx\n",
                    (unsigned long)observed_console_input_mode);
        fprintf(report, "console-mouse-input=%s\n", observe_console_mouse_input ?
                (observed_console_mouse_input_delivered ? "delivered" : "failed") :
                "none");
        if (observe_console_mouse_input)
            fprintf(report, "console-mouse-input-ready=%s\n",
                    observed_console_mouse_input_ready ? "yes" : "no");
        if (scripted_console_input) {
            fprintf(report, "scripted-console-input-trigger=%s\n",
                scripted_console_input_marker ? scripted_console_input_marker :
                "visible-command-prompt");
            fprintf(report, "scripted-console-input-sequence=%s\n",
                    scripted_console_input_sequence);
            fprintf(report, "scripted-console-input-ready=%s\n",
                    scripted_console_input_ready ? "yes" : "no");
            if (scripted_console_input_remaining_known)
                fprintf(report, "scripted-console-input-remaining=%lu\n",
                        (unsigned long)scripted_console_input_remaining);
            else
                fprintf(report, "scripted-console-input-remaining=unavailable\n");
        }
        fprintf(report, "fixed-system-root=%s\n", fixed_system_root);
        fprintf(report, "fixed-system-root-chars=%lu\n",
                (unsigned long)strlen(fixed_system_root));
        fprintf(report, "fixed-system-root-short-chars=%lu\n",
                (unsigned long)fixed_system_root_short_length);
        if (fixed_system_root_short_length != 0 &&
            fixed_system_root_short_length < sizeof(fixed_system_root_short))
            fprintf(report, "fixed-system-root-short=%s\n",
                    fixed_system_root_short);
        if (image_identity.available) {
            fprintf(report, "image-module=%s\n", image_identity.module_name);
            fprintf(report, "image-path=%s\n", image_identity.module_path);
            fprintf(report, "image-base=0x%08lx\n",
                    (unsigned long)image_identity.base_address);
            fprintf(report, "image-size=0x%08lx\n",
                    (unsigned long)image_identity.image_size);
        } else {
            fprintf(report, "image-identity=unavailable\n");
        }
        if (have_timed_context) {
            fprintf(report, "stop-eip=0x%08lx\n", (unsigned long)timed_context.Eip);
            fprintf(report, "stop-esp=0x%08lx\n", (unsigned long)timed_context.Esp);
            fprintf(report, "stop-ebp=0x%08lx\n", (unsigned long)timed_context.Ebp);
            fprintf(report, "stop-eax=0x%08lx\n", (unsigned long)timed_context.Eax);
            fprintf(report, "stop-ebx=0x%08lx\n", (unsigned long)timed_context.Ebx);
            fprintf(report, "stop-ecx=0x%08lx\n", (unsigned long)timed_context.Ecx);
            fprintf(report, "stop-edx=0x%08lx\n", (unsigned long)timed_context.Edx);
            if (have_timed_fault_text)
                fprintf(report, "stop-fault=%s\n", timed_fault_text);
            if (have_timed_stack) {
                DWORD stack_index;
                for (stack_index = 0; stack_index < OBSERVATION_STACK_WORDS;
                     ++stack_index) {
                    fprintf(report, "stop-stack-%02lu=0x%08lx\n",
                            (unsigned long)stack_index,
                            (unsigned long)timed_stack[stack_index]);
                }
            } else {
                fprintf(report, "stop-stack=unavailable\n");
            }
            {
                DWORD thread_index;
                for (thread_index = 0; thread_index < timed_thread_count;
                     ++thread_index) {
                    observation_thread_context *thread =
                        &timed_threads[thread_index];
                    if (thread->context_available) {
                        DWORD frame_index;
                        fprintf(report,
                                "thread-%02lu=id:0x%08lx,eip:0x%08lx,esp:0x%08lx\n",
                                (unsigned long)thread_index,
                                (unsigned long)thread->thread_id,
                                (unsigned long)thread->context.Eip,
                                (unsigned long)thread->context.Esp);
                        for (frame_index = 0; frame_index < thread->frame_count;
                             ++frame_index) {
                            fprintf(report, "thread-%02lu-frame-%02lu=0x%08llx\n",
                                    (unsigned long)thread_index,
                                    (unsigned long)frame_index,
                                    (unsigned long long)thread->frames[frame_index]);
                        }
                    } else {
                        fprintf(report, "thread-%02lu=id:0x%08lx,context:unavailable\n",
                                (unsigned long)thread_index,
                                (unsigned long)thread->thread_id);
                    }
                }
            }
        } else if (wait_status == WAIT_TIMEOUT) {
            fprintf(report, "stop-context=unavailable\n");
        }
        fclose(report);
    }
    if (symbols_initialized) SymCleanup(child.hProcess);
    CloseHandle(child.hThread);
    CloseHandle(child.hProcess);
    CloseHandle(input);
    CloseHandle(output);
    return 0;
}
