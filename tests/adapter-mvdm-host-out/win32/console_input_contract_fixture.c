#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "conapi.h"
#include "ntvdm-exe/session/session.h"

HANDLE GetConsoleInputWaitHandle(VOID);

static session_video_event observed_event;
static uint32_t observed_count;

/* Match the original non-MONITOR nt_cga.c TEXT_INCVAL, not host width.
 * The source-contract verifier checks this against the selected painter. */
enum { SOFTPC_TEXT_CELL_BYTES = 4 };

static int verify_text_plane(session *owner, HANDLE output)
{
    COORD size = { 80, 50 };
    SMALL_RECT rect = { 78, 48, 79, 49 };
    SMALL_RECT bad_rect = { 79, 49, 80, 49 };
    COORD read_size = { 2, 2 }, read_origin = { 0, 0 };
    CHAR_INFO result[4];
    CONSOLE_SCREEN_BUFFER_INFO info;
    DWORD state_length = 1u;
    PVOID state = (PVOID)1, buffer = NULL, repeated = NULL;
    uint32_t columns, rows, capacity, row, column;
    uint8_t short_copy[1];
    if (!RegisterConsoleVDM(CONSOLE_REGISTER_VDM, NULL, NULL, NULL, 0u,
            &state_length, &state, NULL, 0u, size, &buffer) ||
        state_length != 0u || state != NULL || buffer == NULL ||
        !session_presentation_text_describe(owner, &columns, &rows, &capacity) ||
        columns != 80u || rows != 50u ||
        capacity != 80u * 50u * SOFTPC_TEXT_CELL_BYTES) return 30;
    /* Check capacity before touching the last row: the old two-byte
     * allocation must fail deterministically, not corrupt fixture memory. */
    for (row = 0u; row < rows; ++row) {
        for (column = 0u; column < columns; ++column) {
            uint8_t *cell = (uint8_t *)buffer +
                (row * columns + column) * SOFTPC_TEXT_CELL_BYTES;
            cell[0] = (uint8_t)('A' + (row + column) % 26u);
            cell[1] = (uint8_t)(0x10u + column % 16u);
            cell[2] = 0xdeu;
            cell[3] = 0xadu;
        }
    }
    if (session_presentation_text_snapshot(owner, short_copy,
            sizeof(short_copy), NULL, NULL, NULL)) return 31;
    if (!GetConsoleScreenBufferInfo(output, &info)) return 32;
    /* Keep the existing window within the resized test backing buffer. */
    if (info.dwSize.X < size.X) info.dwSize.X = size.X;
    if (info.dwSize.Y < size.Y) info.dwSize.Y = size.Y;
    if (info.dwSize.X <= info.srWindow.Right) info.dwSize.X = info.srWindow.Right + 1;
    if (info.dwSize.Y <= info.srWindow.Bottom) info.dwSize.Y = info.srWindow.Bottom + 1;
    if (!SetConsoleScreenBufferSize(output, info.dwSize)) {
        fprintf(stderr, "text plane resize failed: %lu size=%d,%d window=%d,%d,%d,%d\n",
            GetLastError(), info.dwSize.X, info.dwSize.Y, info.srWindow.Left,
            info.srWindow.Top, info.srWindow.Right, info.srWindow.Bottom);
        return 33;
    }
    if (!InvalidateConsoleDIBits(output, &rect)) {
        fprintf(stderr, "text plane presentation failed: %lu\n", GetLastError());
        return 33;
    }
    if (!ReadConsoleOutputA(output, result, read_size, read_origin, &rect) ||
        rect.Left != 78 || rect.Top != 48 || rect.Right != 79 ||
        rect.Bottom != 49) {
        fprintf(stderr, "text plane read failed: %lu rect=%d,%d,%d,%d size=%d,%d\n",
            GetLastError(), rect.Left, rect.Top, rect.Right, rect.Bottom,
            info.dwSize.X, info.dwSize.Y);
        return 33;
    }
    for (row = 0u; row < 2u; ++row) {
        for (column = 0u; column < 2u; ++column) {
            CHAR_INFO *cell = &result[row * 2u + column];
            if ((unsigned char)cell->Char.AsciiChar != (unsigned char)('A' + (48u + row + 78u + column) % 26u) ||
                cell->Attributes != 0x10u + (78u + column) % 16u) return 34;
        }
    }
    if (InvalidateConsoleDIBits(output, &bad_rect) ||
        GetLastError() != ERROR_INVALID_PARAMETER) return 35;
    if (!RegisterConsoleVDM(CONSOLE_UNREGISTER_VDM, NULL, NULL, NULL, 0u,
            &state_length, &state, NULL, 0u, size, &repeated) ||
        !RegisterConsoleVDM(CONSOLE_REGISTER_VDM, NULL, NULL, NULL, 0u,
            &state_length, &state, NULL, 0u, size, &repeated) ||
        repeated != buffer) return 36;
    return 0;
}

static int observe_video_event(void *context, const session_video_event *event)
{
    (void)context;
    observed_event = *event;
    ++observed_count;
    return 1;
}

int main(void)
{
    DWORD count = 0u;
    BOOL state = FALSE;
    SMALL_RECT rect = { 1, 2, 3, 4 };
    session instance;
    HANDLE saved_output;
    HANDLE text_output;
    HANDLE input;
    DWORD input_mode;
    CONSOLE_CURSOR_INFO cursor_before;
    CONSOLE_CURSOR_INFO cursor_after;
    DWORD text_state_length = 0u;
    PVOID text_state = NULL;
    PVOID text_buffer = NULL;
    COORD text_size = { 2, 1 };
    SMALL_RECT text_rect = { 0, 0, 1, 0 };
    char text_result[2] = { 0, 0 };
    DWORD text_read = 0u;
    INPUT_RECORD queued[2];
    INPUT_RECORD tail = { 0 };
    INPUT_RECORD front = { 0 };
    INPUT_RECORD alt_down = { 0 };
    INPUT_RECORD alt_up = { 0 };
    INPUT_RECORD key = { 0 };
    uint32_t text_columns, text_rows, text_capacity;
    uint32_t events_before_active;
    int text_check;

    /* Own the Console used for queue injection and buffer resizing; never
     * consume input from an inherited user/PTY Console. */
    (void)FreeConsole();
    if (!AllocConsole()) return 18;

    if (ReadConsoleInputExW(INVALID_HANDLE_VALUE, NULL, 0u, &count, 0x8000u) ||
        GetLastError() != ERROR_INVALID_PARAMETER) return 1;
    if (VDMConsoleOperation(0xffffffffu, &state) ||
        GetLastError() != ERROR_CALL_NOT_IMPLEMENTED) return 2;
    if (ShowConsoleCursor(INVALID_HANDLE_VALUE, TRUE) != -1) return 3;
    /* This private NT4 graphics-buffer operation must not claim a public
     * Console palette implementation.  Keep it before the interactive-input
     * guard so a pipe-hosted run still proves this exact negative contract. */
    if (SetConsolePalette(INVALID_HANDLE_VALUE, NULL, 7u) ||
        GetLastError() != ERROR_CALL_NOT_IMPLEMENTED) return 8;
    input = GetStdHandle(STD_INPUT_HANDLE);
    if (input == NULL || input == INVALID_HANDLE_VALUE ||
        !GetConsoleMode(input, &input_mode)) {
        input = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0u,
            NULL);
        if (input == INVALID_HANDLE_VALUE) {
            (void)FreeConsole();
            if (!AllocConsole()) return 18;
            input = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0u,
                NULL);
        }
        if (input == INVALID_HANDLE_VALUE ||
            !SetStdHandle(STD_INPUT_HANDLE, input) ||
            !GetConsoleMode(input, &input_mode)) return 18;
    }
    if (GetConsoleInputWaitHandle() != input || !FlushConsoleInputBuffer(input))
        return 18;
    tail.EventType = KEY_EVENT;
    tail.Event.KeyEvent.bKeyDown = TRUE;
    tail.Event.KeyEvent.uChar.UnicodeChar = L'T';
    front.EventType = KEY_EVENT;
    front.Event.KeyEvent.bKeyDown = TRUE;
    front.Event.KeyEvent.uChar.UnicodeChar = L'F';
    if (!WriteConsoleInputW(input, &tail, 1u, &count) || count != 1u ||
        !WriteConsoleInputVDMW(input, &front, 1u, &count) || count != 1u ||
        WaitForSingleObject(MvdmConsoleInputPrependWaitHandle(), 0u) !=
            WAIT_OBJECT_0 ||
        !ReadConsoleInputExW(input, queued, 1u, &count,
            CONSOLE_READ_NOWAIT | CONSOLE_READ_NOREMOVE) || count != 1u ||
        queued[0].Event.KeyEvent.uChar.UnicodeChar != L'F' ||
        WaitForSingleObject(MvdmConsoleInputPrependWaitHandle(), 0u) !=
            WAIT_OBJECT_0 ||
        !ReadConsoleInputExW(input, queued, 1u, &count,
            CONSOLE_READ_NOWAIT) || count != 1u ||
        queued[0].Event.KeyEvent.uChar.UnicodeChar != L'F' ||
        WaitForSingleObject(MvdmConsoleInputPrependWaitHandle(), 0u) !=
            WAIT_TIMEOUT ||
        !ReadConsoleInputExW(input, queued, 1u, &count,
            CONSOLE_READ_NOWAIT) || count != 1u ||
        queued[0].Event.KeyEvent.uChar.UnicodeChar != L'T') return 19;
    alt_down.EventType = KEY_EVENT;
    alt_down.Event.KeyEvent.bKeyDown = TRUE;
    alt_down.Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
    alt_down.Event.KeyEvent.dwControlKeyState = LEFT_ALT_PRESSED;
    alt_up = alt_down;
    alt_up.Event.KeyEvent.bKeyDown = FALSE;
    alt_up.Event.KeyEvent.dwControlKeyState = 0u;
    key.EventType = KEY_EVENT;
    key.Event.KeyEvent.bKeyDown = TRUE;
    key.Event.KeyEvent.uChar.UnicodeChar = L'K';
    if (!WriteConsoleInputW(input, &alt_down, 1u, &count) || count != 1u ||
        !WriteConsoleInputW(input, &alt_up, 1u, &count) || count != 1u ||
        !WriteConsoleInputW(input, &key, 1u, &count) || count != 1u ||
        !ReadConsoleInputExW(input, queued, 1u, &count,
            CONSOLE_READ_NOWAIT | CONSOLE_READ_NOREMOVE) || count != 1u ||
        queued[0].Event.KeyEvent.uChar.UnicodeChar != L'K' ||
        !ReadConsoleInputExW(input, queued, 1u, &count,
            CONSOLE_READ_NOWAIT) || count != 1u ||
        queued[0].Event.KeyEvent.uChar.UnicodeChar != L'K') return 20;

    SetLastError(ERROR_SUCCESS);
    if (InvalidateConsoleDIBits(INVALID_HANDLE_VALUE, &rect) ||
        GetLastError() != ERROR_INVALID_HANDLE) return 5;
    session_initialize(&instance, 31027u);
    if (!session_set_video_event_sink(&instance, observe_video_event, NULL) ||
        !session_activate(&instance) || !session_thread_bind(&instance)) return 6;
    if (!InvalidateConsoleDIBits(INVALID_HANDLE_VALUE, &rect) ||
        observed_count != 1u ||
        observed_event.kind != SESSION_VIDEO_EVENT_INVALIDATE ||
        observed_event.left != 1 || observed_event.top != 2 ||
        observed_event.right != 3 || observed_event.bottom != 4) return 7;
    /* The original nt_text path fills the RegisterConsoleVDM text plane,
     * then invalidates a character rectangle.  The public Console adapter
     * must make that exact copied plane visible without a guest-memory read. */
    saved_output = GetStdHandle(STD_OUTPUT_HANDLE);
    text_output = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER,
        NULL);
    if (text_output == INVALID_HANDLE_VALUE ||
        !SetConsoleActiveScreenBuffer(text_output) ||
        !SetStdHandle(STD_OUTPUT_HANDLE, text_output) ||
        !GetConsoleCursorInfo(text_output, &cursor_before) ||
        ShowConsoleCursor(text_output, FALSE) != -1 ||
        ShowConsoleCursor(text_output, TRUE) != 0 ||
        !GetConsoleCursorInfo(text_output, &cursor_after) ||
        cursor_before.bVisible != cursor_after.bVisible ||
        !RegisterConsoleVDM(CONSOLE_REGISTER_VDM, NULL, NULL, NULL, 0u,
            &text_state_length, &text_state, NULL, 0u, text_size,
            &text_buffer) || text_state_length != 0u || text_state != NULL ||
        text_buffer == NULL) return 15;
    if (!session_presentation_text_describe(&instance, &text_columns,
            &text_rows, &text_capacity) || text_columns != 2u ||
        text_rows != 1u || text_capacity != 2u * SOFTPC_TEXT_CELL_BYTES) return 24;
    ((uint8_t *)text_buffer)[0] = 'O';
    ((uint8_t *)text_buffer)[1] = 0x07u;
    ((uint8_t *)text_buffer)[2] = 0xdeu;
    ((uint8_t *)text_buffer)[3] = 0xadu;
    ((uint8_t *)text_buffer)[4] = 'K';
    ((uint8_t *)text_buffer)[5] = 0x07u;
    ((uint8_t *)text_buffer)[6] = 0xdeu;
    ((uint8_t *)text_buffer)[7] = 0xadu;
    if (!InvalidateConsoleDIBits(text_output, &text_rect)) return 16;
    if (!ReadConsoleOutputCharacterA(text_output, text_result, 2u,
            (COORD){ 0, 0 }, &text_read)) return 17;
    if (text_read != 2u) return 21;
    if (memcmp(text_result, "OK", 2u) != 0) return 22;
    if (observed_count != 2u) return 23;
    text_check = verify_text_plane(&instance, text_output);
    if (text_check != 0) return text_check;
    (void)RegisterConsoleVDM(CONSOLE_UNREGISTER_VDM, NULL, NULL, NULL, 0u,
        &text_state_length, &text_state, NULL, 0u, text_size, &text_buffer);
    (void)SetStdHandle(STD_OUTPUT_HANDLE, saved_output);
    CloseHandle(text_output);
    events_before_active = observed_count;
    SetLastConsoleEventActive();
    if (observed_count != events_before_active + 1u || observed_event.kind != SESSION_VIDEO_EVENT_ACTIVE ||
        session_video_event_active(&instance) != 1u) return 10;
    if (!session_thread_unbind(&instance) || !session_dispose(&instance)) return 13;

    puts("PASS: console compatibility input and video contracts");
    (void)FreeConsole();
    return 0;
}
