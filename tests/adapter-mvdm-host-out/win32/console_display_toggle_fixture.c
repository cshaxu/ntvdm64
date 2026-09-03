#include <windows.h>

#include "conapi.h"
#include "session/session.h"

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
    session owner;
    HANDLE input;
    INPUT_RECORD supplied[2];
    INPUT_RECORD delivered[2];
    DWORD mode;
    DWORD written;
    DWORD read;
    int allocated_console = 0;

    input = CreateFileW(L"CONIN$", GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0u, NULL);
    if (input == INVALID_HANDLE_VALUE || !GetConsoleMode(input, &mode)) {
        if (input != INVALID_HANDLE_VALUE) CloseHandle(input);
        if (!AllocConsole()) return 1;
        allocated_console = 1;
        input = CreateFileW(L"CONIN$", GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0u,
            NULL);
        if (input == INVALID_HANDLE_VALUE) {
            FreeConsole();
            return 2;
        }
    }

    session_initialize(&owner, 3885u);
    if (!session_set_video_event_sink(&owner, observe_video_event, NULL) ||
        !session_activate(&owner) || !session_thread_bind(&owner)) return 3;
    ZeroMemory(supplied, sizeof(supplied));
    supplied[0].EventType = KEY_EVENT;
    supplied[0].Event.KeyEvent.bKeyDown = TRUE;
    supplied[0].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
    supplied[0].Event.KeyEvent.dwControlKeyState = LEFT_ALT_PRESSED;
    supplied[1].EventType = KEY_EVENT;
    supplied[1].Event.KeyEvent.bKeyDown = TRUE;
    supplied[1].Event.KeyEvent.wVirtualKeyCode = 'A';
    if (!FlushConsoleInputBuffer(input) ||
        !WriteConsoleInputW(input, supplied, 2u, &written) || written != 2u ||
        !ReadConsoleInputExW(input, delivered, 2u, &read, 0u)) return 4;
    if (read != 1u || delivered[0].EventType != KEY_EVENT ||
        delivered[0].Event.KeyEvent.wVirtualKeyCode != 'A' ||
        observed_count != 1u ||
        observed_event.kind != SESSION_VIDEO_EVENT_DISPLAY_TOGGLE) return 5;
    if (!session_thread_unbind(&owner) || !session_dispose(&owner)) return 6;
    CloseHandle(input);
    if (allocated_console) FreeConsole();
    return 0;
}
