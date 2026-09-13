#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "conapi.h"
#include "session/session.h"

HANDLE GetConsoleInputWaitHandle(VOID);

static session_video_event observed_event;
static uint32_t observed_count;

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

    if (ReadConsoleInputExW(INVALID_HANDLE_VALUE, NULL, 0u, &count, 0x8000u) ||
        GetLastError() != ERROR_INVALID_PARAMETER) return 1;
    if (VDMConsoleOperation(0xffffffffu, &state) ||
        GetLastError() != ERROR_CALL_NOT_IMPLEMENTED) return 2;
    if (ShowConsoleCursor(INVALID_HANDLE_VALUE, TRUE) != -1) return 3;
    if (GetConsoleInputWaitHandle() == NULL) return 4;
    /* This private NT4 graphics-buffer operation must not claim a public
     * Console palette implementation.  Keep it before the interactive-input
     * guard so a pipe-hosted run still proves this exact negative contract. */
    if (SetConsolePalette(INVALID_HANDLE_VALUE, NULL, 7u) ||
        GetLastError() != ERROR_CALL_NOT_IMPLEMENTED) return 8;
    input = GetStdHandle(STD_INPUT_HANDLE);
    if (input == NULL || input == INVALID_HANDLE_VALUE ||
        !FlushConsoleInputBuffer(input)) return 18;
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
    ((uint8_t *)text_buffer)[0] = 'O';
    ((uint8_t *)text_buffer)[1] = 0x07u;
    ((uint8_t *)text_buffer)[2] = 'K';
    ((uint8_t *)text_buffer)[3] = 0x07u;
    if (!InvalidateConsoleDIBits(text_output, &text_rect) ||
        !ReadConsoleOutputCharacterA(text_output, text_result, 2u,
            (COORD){ 0, 0 }, &text_read) || text_read != 2u ||
        memcmp(text_result, "OK", 2u) != 0 || observed_count != 2u) return 16;
    (void)RegisterConsoleVDM(CONSOLE_UNREGISTER_VDM, NULL, NULL, NULL, 0u,
        &text_state_length, &text_state, NULL, 0u, text_size, &text_buffer);
    (void)SetStdHandle(STD_OUTPUT_HANDLE, saved_output);
    CloseHandle(text_output);
    SetLastConsoleEventActive();
    if (observed_count != 3u || observed_event.kind != SESSION_VIDEO_EVENT_ACTIVE ||
        session_video_event_active(&instance) != 1u) return 10;
    if (!session_thread_unbind(&instance) || !session_dispose(&instance)) return 13;

    puts("PASS: console compatibility input and video contracts");
    return 0;
}
