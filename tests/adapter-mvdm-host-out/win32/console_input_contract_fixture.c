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
    uint32_t snapshot_count = 0u;
    BOOL state = FALSE;
    SMALL_RECT rect = { 1, 2, 3, 4 };
    session instance;
    BITMAPINFO graphics_info;
    CONSOLE_GRAPHICS_BUFFER_INFO graphics_buffer;
    HANDLE graphics_screen;
    uint8_t graphics_copy[8];
    struct {
        LOGPALETTE header;
        PALETTEENTRY extra;
    } palette_info;
    HPALETTE palette;
    uint32_t palette_copy[2];
    HANDLE saved_output;
    HANDLE text_output;
    DWORD text_state_length = 0u;
    PVOID text_state = NULL;
    PVOID text_buffer = NULL;
    COORD text_size = { 2, 1 };
    SMALL_RECT text_rect = { 0, 0, 1, 0 };
    char text_result[2] = { 0, 0 };
    DWORD text_read = 0u;

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
    /* The original nt_text path fills the RegisterConsoleVDM text plane,
     * then invalidates a character rectangle.  The public Console adapter
     * must make that exact copied plane visible without a guest-memory read. */
    saved_output = GetStdHandle(STD_OUTPUT_HANDLE);
    text_output = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER,
        NULL);
    if (text_output == INVALID_HANDLE_VALUE ||
        !SetStdHandle(STD_OUTPUT_HANDLE, text_output) ||
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
    ZeroMemory(&palette_info, sizeof(palette_info));
    palette_info.header.palVersion = 0x300u;
    palette_info.header.palNumEntries = 2u;
    palette_info.header.palPalEntry[0].peRed = 0x11u;
    palette_info.header.palPalEntry[0].peGreen = 0x22u;
    palette_info.header.palPalEntry[0].peBlue = 0x33u;
    palette_info.extra.peRed = 0x44u;
    palette_info.extra.peGreen = 0x55u;
    palette_info.extra.peBlue = 0x66u;
    palette = CreatePalette(&palette_info.header);
    if (palette == NULL || !SetConsolePalette(INVALID_HANDLE_VALUE, palette, 7u) ||
        observed_count != 3u || observed_event.kind != SESSION_VIDEO_EVENT_PALETTE ||
        observed_event.flags != 7u) return 8;
    if (!session_presentation_graphics_palette_snapshot(&instance,
            palette_copy, 2u, &snapshot_count) || snapshot_count != 2u ||
        palette_copy[0] != UINT32_C(0x00112233) ||
        palette_copy[1] != UINT32_C(0x00445566)) return 9;
    SetLastConsoleEventActive();
    if (observed_count != 4u || observed_event.kind != SESSION_VIDEO_EVENT_ACTIVE ||
        session_video_event_active(&instance) != 1u) return 10;
    ZeroMemory(&graphics_info, sizeof(graphics_info));
    graphics_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    graphics_info.bmiHeader.biWidth = 2;
    graphics_info.bmiHeader.biHeight = -2;
    graphics_info.bmiHeader.biPlanes = 1;
    graphics_info.bmiHeader.biBitCount = 8;
    graphics_info.bmiHeader.biCompression = BI_RGB;
    ZeroMemory(&graphics_buffer, sizeof(graphics_buffer));
    graphics_buffer.dwBitMapInfoLength = sizeof(graphics_info);
    graphics_buffer.lpBitMapInfo = &graphics_info;
    graphics_buffer.dwUsage = DIB_PAL_COLORS;
    if (!MvdmPresentationGraphicsBuffer(GetStdHandle(STD_OUTPUT_HANDLE),
            &graphics_buffer, &graphics_screen) || graphics_screen == NULL ||
        graphics_buffer.hMutex == NULL || graphics_buffer.lpBitMap == NULL)
        return 11;
    if (observed_count != 5u ||
        observed_event.kind != SESSION_VIDEO_EVENT_GRAPHICS_READY) return 14;
    ((uint8_t *)graphics_buffer.lpBitMap)[0] = 0x5au;
    if (!session_presentation_graphics_snapshot(&instance, graphics_copy,
            (uint32_t)sizeof(graphics_copy), NULL, NULL, NULL, NULL,
            &snapshot_count) || snapshot_count != sizeof(graphics_copy) ||
        graphics_copy[0] != 0x5au)
        return 12;
    CloseHandle(graphics_screen);
    CloseHandle(graphics_buffer.hMutex);
    DeleteObject(palette);
    MvdmPresentationGraphicsClear();
    if (session_presentation_graphics_snapshot(&instance, graphics_copy,
            (uint32_t)sizeof(graphics_copy), NULL, NULL, NULL, NULL, NULL) ||
        !session_thread_unbind(&instance) || !session_dispose(&instance)) return 13;

    puts("PASS: console compatibility input and video contracts");
    return 0;
}
