#include <windows.h>

#include "app/presentation_window.h"

int main(void)
{
    session owner;
    app_presentation_window window;
    session_video_event event;
    uint8_t *text;

    session_initialize(&owner, 346u);
    app_presentation_window_initialize(&window);
    if (!session_valid(&owner) || !session_select_machine_backend(&owner,
            SESSION_MACHINE_BACKEND_SOFTPC) ||
        !app_presentation_window_prepare(&window, &owner) ||
        !app_presentation_window_open(&window) ||
        session_machine_backend(&owner) != SESSION_MACHINE_BACKEND_SOFTPC)
        return 1;
    if (!session_activate(&owner) || !session_presentation_text_acquire_writable(
            &owner, 2u, 1u, &text)) return 2;
    text[0] = 'O';
    text[1] = 7u;
    text[2] = 'K';
    text[3] = 7u;
    ZeroMemory(&event, sizeof(event));
    event.kind = SESSION_VIDEO_EVENT_INVALIDATE;
    if (!session_notify_video_event(&owner, &event)) return 3;
    Sleep(25u);
    if (!app_presentation_window_active(&window) ||
        !app_presentation_window_close(&window) ||
        owner.state != SESSION_STATE_ACTIVE ||
        session_machine_backend(&owner) != SESSION_MACHINE_BACKEND_SOFTPC ||
        !session_dispose(&owner))
        return 4;
    return 0;
}
