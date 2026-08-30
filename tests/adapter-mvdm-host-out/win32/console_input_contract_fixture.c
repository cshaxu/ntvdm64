#include <stdio.h>
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

    if (ReadConsoleInputExW(INVALID_HANDLE_VALUE, NULL, 0u, &count, 0x8000u) ||
        GetLastError() != ERROR_INVALID_PARAMETER) return 1;
    if (VDMConsoleOperation(0xffffffffu, &state) ||
        GetLastError() != ERROR_CALL_NOT_IMPLEMENTED) return 2;
    if (ShowConsoleCursor(INVALID_HANDLE_VALUE, TRUE) != -1) return 3;
    if (GetConsoleInputWaitHandle() == NULL) return 4;

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
    if (!SetConsolePalette(INVALID_HANDLE_VALUE, (HPALETTE)(uintptr_t)1u, 7u) ||
        observed_count != 2u || observed_event.kind != SESSION_VIDEO_EVENT_PALETTE ||
        observed_event.flags != 7u) return 8;
    SetLastConsoleEventActive();
    if (observed_count != 3u || observed_event.kind != SESSION_VIDEO_EVENT_ACTIVE ||
        session_video_event_active(&instance) != 1u) return 9;
    if (!session_thread_unbind(&instance) || !session_dispose(&instance)) return 10;

    puts("PASS: console compatibility input and video contracts");
    return 0;
}
