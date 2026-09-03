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

/* The scripted command sequence itself is paced at one original 8042 event
 * every 100 ms.  The original worker's delayed IRQ path is measured in
 * microseconds, so this remains deliberately slower than the source-owned
 * hardware queue while keeping a four-key command inside the fixed 5--10
 * second observation window. */
#define OBSERVATION_TIMEOUT_MS 10000u
#define OBSERVATION_TIMEOUT_MAX_MS 30000u
#define OBSERVATION_INPUT_READY_TIMEOUT_MS 5000u
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
    char screen[4097];
    char path[MAX_PATH];
    DWORD count = 0;
    FILE *file = NULL;

    if (!ReadConsoleOutputCharacterA(output, screen, 4096,
                                     (COORD){ 0, 0 }, &count)) return;
    screen[count] = '\0';
    snprintf(path, sizeof(path), "%s.console.txt", report_path);
    if (fopen_s(&file, path, "wb") == 0 && file != NULL) {
        fwrite(screen, 1, count, file);
        fclose(file);
    }
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
static BOOL set1_scan_code_for_ascii(char character, WORD *scan_code)
{
    static const BYTE lowercase_set1[26] = {
        0x1e, 0x30, 0x2e, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17,
        0x24, 0x25, 0x26, 0x32, 0x31, 0x18, 0x19, 0x10, 0x13,
        0x1f, 0x14, 0x16, 0x2f, 0x11, 0x2d, 0x15, 0x2c
    };

    if (scan_code == NULL) return FALSE;
    if (character >= 'A' && character <= 'Z') character += 'a' - 'A';
    if (character >= 'a' && character <= 'z') {
        *scan_code = lowercase_set1[character - 'a'];
        return TRUE;
    }
    switch (character == '\n' ? '\r' : character) {
    case '\r': *scan_code = 0x1c; return TRUE;
    case ' ':  *scan_code = 0x39; return TRUE;
    case '0':  *scan_code = 0x0b; return TRUE;
    case '1':  *scan_code = 0x02; return TRUE;
    case '2':  *scan_code = 0x03; return TRUE;
    case '3':  *scan_code = 0x04; return TRUE;
    case '4':  *scan_code = 0x05; return TRUE;
    case '5':  *scan_code = 0x06; return TRUE;
    case '6':  *scan_code = 0x07; return TRUE;
    case '7':  *scan_code = 0x08; return TRUE;
    case '8':  *scan_code = 0x09; return TRUE;
    case '9':  *scan_code = 0x0a; return TRUE;
    default: return FALSE;
    }
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

static BOOL write_console_input_text(HANDLE input, const char *text,
                                     const char *report_path)
{
    const char *cursor;

    if (input == NULL || input == INVALID_HANDLE_VALUE || text == NULL)
        return FALSE;
    for (cursor = text; *cursor != '\0'; ++cursor) {
        INPUT_RECORD records[2];
        DWORD written = 0;
        char character = *cursor == '\n' ? '\r' : *cursor;
        SHORT virtual_key = VkKeyScanA(character);
        WORD key_code;
        WORD scan_code;
        char break_marker[40];
        DWORD report_offset;
        /* `nt_event.c` starts a DOS boot with ToggleKeyState set to
         * NUMLOCK_ON.  A normal unmodified letter delivered by this fixed
         * Console must carry that same toggle bit: otherwise the original
         * SyncToggleKeys path first synthesizes a NumLock transition ahead of
         * the requested key.  This is Console-record fidelity, not a guest
         * state mutation. */
        DWORD control_state = NUMLOCK_ON;

        if (virtual_key == -1 || !set1_scan_code_for_ascii(character, &scan_code))
            return FALSE;
        key_code = (WORD)(virtual_key & 0xff);
        if ((virtual_key & 0x0100) != 0) control_state |= SHIFT_PRESSED;
        memset(records, 0, sizeof(records));
        records[0].EventType = KEY_EVENT;
        records[0].Event.KeyEvent.bKeyDown = TRUE;
        records[0].Event.KeyEvent.wRepeatCount = 1;
        records[0].Event.KeyEvent.wVirtualKeyCode = key_code;
        records[0].Event.KeyEvent.wVirtualScanCode = scan_code;
        records[0].Event.KeyEvent.uChar.AsciiChar = character;
        records[0].Event.KeyEvent.dwControlKeyState = control_state;
        records[1] = records[0];
        records[1].Event.KeyEvent.bKeyDown = FALSE;
        report_offset = report_size_bytes(report_path);
        if (!WriteConsoleInputA(input, records, ARRAYSIZE(records), &written) ||
            written != ARRAYSIZE(records)) return FALSE;
        /* The real Console route is serviced by the original keyboard worker,
         * not a bulk guest-input API.  Pace this observer-only script like an
         * ordinary typist so it cannot fill the original keyboard ring before
         * COMMAND has an opportunity to consume the preceding key. */
        /* The original 8042 worker deliberately drains one hardware key at
         * a time.  Keep this observer slower than that bounded source-owned
         * queue rather than making a bulk-input shortcut appear reliable. */
        Sleep(OBSERVATION_KEY_EVENT_INTERVAL_MS);
        snprintf(break_marker, sizeof(break_marker),
                 "MVDM-KBD-PORT60 value=%02X", (unsigned int)(scan_code | 0x80u));
        if (!wait_for_report_marker_after(report_path, break_marker,
                                          report_offset,
                                          OBSERVATION_KEY_DRAIN_TIMEOUT_MS))
            return FALSE;
    }
    return TRUE;
}

/* The S7 command row must not use a wall-clock guess for initial COMMAND
 * setup.  The product's default-off source marker records original BIOS
 * keyboard waitio (BOP 16, AH=2), after guest INT 16h has entered its native
 * wait loop.  This is the source-owned point at which a normal Console key
 * can be queued without guessing at startup.  The observer only waits for
 * that external report; it neither alters guest state nor treats the marker
 * as a product result. */
static BOOL wait_for_report_marker(const char *path, const char *marker,
                                   DWORD timeout_ms)
{
    DWORD started_at;
    char buffer[65537];

    if (path == NULL || marker == NULL || *marker == '\0') return FALSE;
    started_at = GetTickCount();
    for (;;) {
        HANDLE file = CreateFileA(path, GENERIC_READ,
                                  FILE_SHARE_READ | FILE_SHARE_WRITE,
                                  NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                                  NULL);
        if (file != INVALID_HANDLE_VALUE) {
            DWORD size = GetFileSize(file, NULL);
            DWORD to_read = size == INVALID_FILE_SIZE ? 0u :
                (size < (DWORD)(sizeof(buffer) - 1u) ? size :
                 (DWORD)(sizeof(buffer) - 1u));
            DWORD read = 0;
            if (to_read != 0u && ReadFile(file, buffer, to_read, &read, NULL)) {
                buffer[read] = '\0';
                if (strstr(buffer, marker) != NULL) {
                    CloseHandle(file);
                    return TRUE;
                }
            }
            CloseHandle(file);
        }
        if ((DWORD)(GetTickCount() - started_at) >= timeout_ms) return FALSE;
        Sleep(25u);
    }
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
    const char *scripted_console_input_text = "ver\rexit\r";
    const char *scripted_console_input_sequence = "ver+exit";
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
    char bop_return_report_path[MAX_PATH];
    char base_vdm_report_path[MAX_PATH];
    char previous_bop_return_report_path[MAX_PATH];
    char console_input_ready_report_path[MAX_PATH];
    char console_input_preinput_snapshot_path[MAX_PATH];
    char previous_console_input_ready_report_path[MAX_PATH];
    char dem_open_report_path[MAX_PATH];
    char previous_dem_open_report_path[MAX_PATH];
    char config_done_report_path[MAX_PATH];
    char previous_config_done_report_path[MAX_PATH];
    char config_command_store_report_path[MAX_PATH];
    char previous_config_command_store_report_path[MAX_PATH];
    char previous_config_command_linear[16];
    char dem_read_report_path[MAX_PATH];
    char previous_dem_read_report_path[MAX_PATH];
    char dem_seek_report_path[MAX_PATH];
    char dem_ioctl_report_path[MAX_PATH];
    char report_base_path[MAX_PATH];
    DWORD report_base_path_length;
    DWORD previous_exception_report_length;
    DWORD previous_main_return_report_length;
    DWORD previous_bop_return_report_length;
    DWORD previous_console_input_ready_report_length;
    DWORD previous_dem_open_report_length;
    DWORD previous_config_done_report_length;
    DWORD previous_config_command_store_report_length;
    DWORD previous_config_command_linear_length;
    DWORD previous_dem_read_report_length;
    BOOL had_previous_exception_report;
    BOOL had_previous_main_return_report;
    BOOL had_previous_bop_return_report;
    BOOL had_previous_console_input_ready_report;
    BOOL had_previous_dem_open_report;
    BOOL had_previous_config_done_report;
    BOOL had_previous_config_command_store_report;
    BOOL had_previous_config_command_linear;
    BOOL had_previous_dem_read_report;
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
    if (!AllocConsole() && GetLastError() != ERROR_ACCESS_DENIED) return 65;
    input = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE, &inherit,
                        OPEN_EXISTING, 0, NULL);
    output = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE,
                         FILE_SHARE_READ | FILE_SHARE_WRITE, &inherit,
                         OPEN_EXISTING, 0, NULL);
    if (input == INVALID_HANDLE_VALUE || output == INVALID_HANDLE_VALUE) return 66;

    clear_console(output);
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
    snprintf(bop_return_report_path, sizeof(bop_return_report_path), "%s.bop-return.txt",
             report_base_path);
    snprintf(console_input_ready_report_path,
             sizeof(console_input_ready_report_path), "%s.console-ready.txt",
             report_base_path);
    snprintf(console_input_preinput_snapshot_path,
             sizeof(console_input_preinput_snapshot_path),
             "%s.pre-input-console.txt", report_base_path);
    snprintf(base_vdm_report_path, sizeof(base_vdm_report_path), "%s.base-vdm.txt",
             report_base_path);
    snprintf(dem_open_report_path, sizeof(dem_open_report_path), "%s.dem-open.txt",
             report_base_path);
    snprintf(config_done_report_path, sizeof(config_done_report_path),
             "%s.config-done.txt", report_base_path);
    snprintf(config_command_store_report_path,
             sizeof(config_command_store_report_path),
             "%s.config-command-store.txt", report_base_path);
    snprintf(dem_read_report_path, sizeof(dem_read_report_path),
             "%s.dem-read.txt", report_base_path);
    snprintf(dem_seek_report_path, sizeof(dem_seek_report_path),
             "%s.dem-seek.txt", report_base_path);
    snprintf(dem_ioctl_report_path, sizeof(dem_ioctl_report_path),
             "%s.dem-ioctl.txt", report_base_path);
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
    previous_bop_return_report_length = GetEnvironmentVariableA(
        "MVDM_BOP_RETURN_REPORT_PATH", previous_bop_return_report_path,
        (DWORD)sizeof(previous_bop_return_report_path));
    had_previous_bop_return_report = previous_bop_return_report_length != 0 &&
        previous_bop_return_report_length < sizeof(previous_bop_return_report_path);
    previous_console_input_ready_report_length = GetEnvironmentVariableA(
        "MVDM_STREAM_IO_REPORT_PATH",
        previous_console_input_ready_report_path,
        (DWORD)sizeof(previous_console_input_ready_report_path));
    had_previous_console_input_ready_report =
        previous_console_input_ready_report_length != 0 &&
        previous_console_input_ready_report_length <
            sizeof(previous_console_input_ready_report_path);
    previous_dem_open_report_length = GetEnvironmentVariableA(
        "MVDM_DEM_OPEN_REPORT_PATH", previous_dem_open_report_path,
        (DWORD)sizeof(previous_dem_open_report_path));
    had_previous_dem_open_report = previous_dem_open_report_length != 0 &&
        previous_dem_open_report_length < sizeof(previous_dem_open_report_path);
    previous_config_done_report_length = GetEnvironmentVariableA(
        "MVDM_CONFIG_DONE_REPORT_PATH", previous_config_done_report_path,
        (DWORD)sizeof(previous_config_done_report_path));
    had_previous_config_done_report = previous_config_done_report_length != 0 &&
        previous_config_done_report_length < sizeof(previous_config_done_report_path);
    previous_config_command_store_report_length = GetEnvironmentVariableA(
        "MVDM_SAS_STORE_REPORT_PATH",
        previous_config_command_store_report_path,
        (DWORD)sizeof(previous_config_command_store_report_path));
    had_previous_config_command_store_report =
        previous_config_command_store_report_length != 0 &&
        previous_config_command_store_report_length <
            sizeof(previous_config_command_store_report_path);
    previous_config_command_linear_length = GetEnvironmentVariableA(
        "MVDM_SAS_STORE_LINEAR", previous_config_command_linear,
        (DWORD)sizeof(previous_config_command_linear));
    had_previous_config_command_linear = previous_config_command_linear_length != 0 &&
        previous_config_command_linear_length < sizeof(previous_config_command_linear);
    previous_dem_read_report_length = GetEnvironmentVariableA(
        "MVDM_DEM_READ_REPORT_PATH", previous_dem_read_report_path,
        (DWORD)sizeof(previous_dem_read_report_path));
    had_previous_dem_read_report = previous_dem_read_report_length != 0 &&
        previous_dem_read_report_length < sizeof(previous_dem_read_report_path);
    SetEnvironmentVariableA("MVDM_EXCEPTION_REPORT_PATH", exception_report_path);
    SetEnvironmentVariableA("MVDM_MAIN_RETURN_REPORT_PATH", main_return_report_path);
    /* Keep the existing scalar CPU/PIC observer enabled for a scripted
     * Console row as well.  It writes a separate report file and never
     * supplies a guest command or changes the Console -> 8042 path. */
    SetEnvironmentVariableA("MVDM_BOP_RETURN_REPORT_PATH", bop_return_report_path);
    if (scripted_console_input)
        SetEnvironmentVariableA("MVDM_STREAM_IO_REPORT_PATH",
            console_input_ready_report_path);
    SetEnvironmentVariableA("MVDM_BASE_VDM_REPORT_PATH", base_vdm_report_path);
    SetEnvironmentVariableA("MVDM_DEM_OPEN_REPORT_PATH", dem_open_report_path);
    SetEnvironmentVariableA("MVDM_CONFIG_DONE_REPORT_PATH", config_done_report_path);
    SetEnvironmentVariableA("MVDM_SAS_STORE_REPORT_PATH",
        config_command_store_report_path);
    /* S5 attributes this selected image's original trys `commnd` store to
     * live CS 8E08 plus map offset 3466. This is a fixed-image observer
     * input, not a guest ABI or an address translation facility. */
    if (!had_previous_config_command_linear)
        SetEnvironmentVariableA("MVDM_SAS_STORE_LINEAR", "0x914e6");
    SetEnvironmentVariableA("MVDM_DEM_READ_REPORT_PATH", dem_read_report_path);
    SetEnvironmentVariableA("MVDM_DEM_SEEK_REPORT_PATH", dem_seek_report_path);
    SetEnvironmentVariableA("MVDM_DEM_IOCTL_REPORT_PATH", dem_ioctl_report_path);

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
        if (had_previous_bop_return_report)
            SetEnvironmentVariableA("MVDM_BOP_RETURN_REPORT_PATH",
                                    previous_bop_return_report_path);
        else
            SetEnvironmentVariableA("MVDM_BOP_RETURN_REPORT_PATH", NULL);
        if (had_previous_console_input_ready_report)
            SetEnvironmentVariableA("MVDM_STREAM_IO_REPORT_PATH",
                                    previous_console_input_ready_report_path);
        else
            SetEnvironmentVariableA("MVDM_STREAM_IO_REPORT_PATH", NULL);
        if (had_previous_dem_open_report)
            SetEnvironmentVariableA("MVDM_DEM_OPEN_REPORT_PATH",
                                    previous_dem_open_report_path);
        else
            SetEnvironmentVariableA("MVDM_DEM_OPEN_REPORT_PATH", NULL);
        if (had_previous_config_done_report)
            SetEnvironmentVariableA("MVDM_CONFIG_DONE_REPORT_PATH",
                                     previous_config_done_report_path);
        else
            SetEnvironmentVariableA("MVDM_CONFIG_DONE_REPORT_PATH", NULL);
        if (had_previous_config_command_store_report)
            SetEnvironmentVariableA("MVDM_SAS_STORE_REPORT_PATH",
                                    previous_config_command_store_report_path);
        else
            SetEnvironmentVariableA("MVDM_SAS_STORE_REPORT_PATH", NULL);
        if (had_previous_config_command_linear)
            SetEnvironmentVariableA("MVDM_SAS_STORE_LINEAR",
                                    previous_config_command_linear);
        else
            SetEnvironmentVariableA("MVDM_SAS_STORE_LINEAR", NULL);
        if (had_previous_dem_read_report)
            SetEnvironmentVariableA("MVDM_DEM_READ_REPORT_PATH",
                                    previous_dem_read_report_path);
        else
            SetEnvironmentVariableA("MVDM_DEM_READ_REPORT_PATH", NULL);
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
    if (had_previous_bop_return_report)
        SetEnvironmentVariableA("MVDM_BOP_RETURN_REPORT_PATH",
                                previous_bop_return_report_path);
    else
        SetEnvironmentVariableA("MVDM_BOP_RETURN_REPORT_PATH", NULL);
    if (had_previous_console_input_ready_report)
        SetEnvironmentVariableA("MVDM_STREAM_IO_REPORT_PATH",
                                previous_console_input_ready_report_path);
    else
        SetEnvironmentVariableA("MVDM_STREAM_IO_REPORT_PATH", NULL);
    if (had_previous_dem_open_report)
        SetEnvironmentVariableA("MVDM_DEM_OPEN_REPORT_PATH",
                                previous_dem_open_report_path);
    else
        SetEnvironmentVariableA("MVDM_DEM_OPEN_REPORT_PATH", NULL);
    if (had_previous_config_done_report)
        SetEnvironmentVariableA("MVDM_CONFIG_DONE_REPORT_PATH",
                                 previous_config_done_report_path);
    else
        SetEnvironmentVariableA("MVDM_CONFIG_DONE_REPORT_PATH", NULL);
    if (had_previous_config_command_store_report)
        SetEnvironmentVariableA("MVDM_SAS_STORE_REPORT_PATH",
                                previous_config_command_store_report_path);
    else
        SetEnvironmentVariableA("MVDM_SAS_STORE_REPORT_PATH", NULL);
    if (had_previous_config_command_linear)
        SetEnvironmentVariableA("MVDM_SAS_STORE_LINEAR",
                                previous_config_command_linear);
    else
        SetEnvironmentVariableA("MVDM_SAS_STORE_LINEAR", NULL);
    if (had_previous_dem_read_report)
        SetEnvironmentVariableA("MVDM_DEM_READ_REPORT_PATH",
                                previous_dem_read_report_path);
    else
        SetEnvironmentVariableA("MVDM_DEM_READ_REPORT_PATH", NULL);
    if (scripted_console_input) {
        /* Original BIOS keyboard waitio is a source-owned CONIN$ readiness
         * boundary, not a timeout or a synthesized BOP. */
        scripted_console_input_ready = wait_for_report_marker(
            console_input_ready_report_path, "MVDM-COMMAND-INPUT-READY",
            OBSERVATION_INPUT_READY_TIMEOUT_MS);
        if (scripted_console_input_ready) {
            /* Snapshot the exact shared CONOUT$ buffer after original guest
             * stream output but before this observer queues any key. */
            write_console_snapshot(output, console_input_preinput_snapshot_path);
            scripted_console_input_delivered = write_console_input_text(input,
                scripted_console_input_text, console_input_ready_report_path);
        }
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
        fprintf(report, "timeout-ms=%lu\n", (unsigned long)observation_timeout_ms);
        fprintf(report, "scripted-console-input=%s\n",
                scripted_console_input ?
                    (scripted_console_input_delivered ? "delivered" : "failed") :
                    "none");
        if (scripted_console_input) {
            fprintf(report, "scripted-console-input-trigger=keyboard-bop-16-waitio\n");
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
