#include <windows.h>

#include "app/presentation_window.h"
#include "adapter-mvdm-host-out/win32/include/presentation_surface.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_softpc_presentation_font.h"

int mvdm_presentation_graphics_describe(session *owner, uint32_t *width_out,
    uint32_t *height_out, uint32_t *bits_out, uint32_t *stride_out,
    uint32_t *bytes_out)
{
    (void)owner;
    (void)width_out;
    (void)height_out;
    (void)bits_out;
    (void)stride_out;
    (void)bytes_out;
    return 0;
}

int mvdm_presentation_graphics_snapshot(session *owner, uint8_t *bytes,
    uint32_t capacity, uint32_t *width_out, uint32_t *height_out,
    uint32_t *bits_out, uint32_t *stride_out, uint32_t *bytes_out,
    uint32_t *palette, uint32_t palette_capacity, uint32_t *entries_out)
{
    (void)owner;
    (void)bytes;
    (void)capacity;
    (void)width_out;
    (void)height_out;
    (void)bits_out;
    (void)stride_out;
    (void)bytes_out;
    (void)palette;
    (void)palette_capacity;
    (void)entries_out;
    return 0;
}

int mvdm_softpc_presentation_font_snapshot(session *owner, uint8_t *bytes,
    uint32_t capacity)
{
    uint32_t index;
    if (owner == NULL || bytes == NULL ||
        capacity != MVDM_SOFTPC_PRESENTATION_FONT_BYTES) return 0;
    for (index = 0u; index < capacity; ++index) bytes[index] = 0u;
    return 1;
}

int main(void)
{
    session owner;
    app_presentation_window window;
    RECT client;
    uint32_t columns, rows, text_bytes;
    uint8_t *text;

    session_initialize(&owner, 361u);
    app_presentation_window_initialize(&window);
    if (!session_valid(&owner) || !session_select_machine_backend(&owner,
            SESSION_MACHINE_BACKEND_SOFTPC) ||
        !app_presentation_window_prepare(&window, &owner)) return 1;
    if (!session_activate(&owner) ||
        !session_presentation_text_acquire_writable(&owner, 80u, 25u, 2u,
            &text)) return 2;
    {
        session_video_event event;
        ZeroMemory(&event, sizeof(event));
        event.kind = SESSION_VIDEO_EVENT_GRAPHICS_READY;
        if (!session_notify_video_event(&owner, &event)) return 3;
    }
    if (!app_presentation_window_active(&window) || window.window == NULL)
        return 4;
    if (!GetClientRect(window.window, &client)) return 51;
    if (client.right <= 0 || client.bottom <= 0) return 52;
    if (!session_presentation_text_describe(&owner, &columns, &rows,
            &text_bytes)) return 53;
    if (columns == 0u || rows == 0u) return 54;
    {
        INPUT_RECORD record;
        DWORD read;
        if (!FlushConsoleInputBuffer(window.input) ||
            SendMessageW(window.window, WM_MOUSEMOVE, 0,
                MAKELPARAM(client.right / 2, client.bottom / 2)) != 0 ||
            !ReadConsoleInputW(window.input, &record, 1u, &read) || read != 1u ||
            record.EventType != MOUSE_EVENT ||
            record.Event.MouseEvent.dwMousePosition.X != (SHORT)(columns / 2u) ||
            record.Event.MouseEvent.dwMousePosition.Y != (SHORT)(rows / 2u) ||
            record.Event.MouseEvent.dwEventFlags != MOUSE_MOVED ||
            record.Event.MouseEvent.dwButtonState != 0u)
            return 6;
        if (SendMessageW(window.window, WM_LBUTTONDOWN, MK_LBUTTON,
                MAKELPARAM(client.right - 1, client.bottom - 1)) != 0 ||
            !ReadConsoleInputW(window.input, &record, 1u, &read) || read != 1u ||
            record.EventType != MOUSE_EVENT ||
            record.Event.MouseEvent.dwMousePosition.X != (SHORT)(columns - 1u) ||
            record.Event.MouseEvent.dwMousePosition.Y != (SHORT)(rows - 1u) ||
            record.Event.MouseEvent.dwButtonState != FROM_LEFT_1ST_BUTTON_PRESSED ||
            record.Event.MouseEvent.dwEventFlags != 0u)
            return 7;
        if (SendMessageW(window.window, WM_LBUTTONUP, 0,
                MAKELPARAM(client.right + 60, client.bottom + 50)) != 0 ||
            !ReadConsoleInputW(window.input, &record, 1u, &read) || read != 1u ||
            record.EventType != MOUSE_EVENT ||
            record.Event.MouseEvent.dwMousePosition.X != (SHORT)(columns - 1u) ||
            record.Event.MouseEvent.dwMousePosition.Y != (SHORT)(rows - 1u) ||
            record.Event.MouseEvent.dwButtonState != 0u)
            return 8;
    }
    if (!app_presentation_window_close(&window)) return 6;
    if (!session_dispose(&owner)) return 7;
    return 0;
}
