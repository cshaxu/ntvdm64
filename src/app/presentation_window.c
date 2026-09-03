#include "app/presentation_window.h"
#include "presentation_surface.h"
#include "adapter-mvdm-host-out/softpc/include/mvdm_softpc_presentation_font.h"

#include <stdlib.h>
#include <string.h>

#define APP_PRESENTATION_WINDOW_CLASS L"MvdmPresentationWindow"
#define APP_PRESENTATION_WINDOW_READY_TIMEOUT 5000u
#define APP_PRESENTATION_MESSAGE_REPAINT (WM_APP + 1u)
#define APP_PRESENTATION_MESSAGE_SHUTDOWN (WM_APP + 2u)
#define APP_PRESENTATION_TEXT_COLUMNS 80u
#define APP_PRESENTATION_TEXT_ROWS 25u
#define APP_PRESENTATION_WINDOW_STYLE \
    (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)

static LRESULT CALLBACK presentation_window_proc(HWND handle, UINT message,
    WPARAM wparam, LPARAM lparam);

static const uint32_t presentation_ega_rgb[16] = {
    0x00000000u, 0x000000aau, 0x0000aa00u, 0x0000aaaau,
    0x00aa0000u, 0x00aa00aau, 0x00aa5500u, 0x00aaaaaau,
    0x00555555u, 0x005555ffu, 0x0055ff55u, 0x0055ffffu,
    0x00ff5555u, 0x00ff55ffu, 0x00ffff55u, 0x00ffffffu
};

static int presentation_surface_dimensions(app_presentation_window *window,
    uint32_t *width_out, uint32_t *height_out)
{
    uint32_t width, height, bits, stride, bytes;
    uint32_t columns, rows, text_bytes;

    if (window == NULL || width_out == NULL || height_out == NULL) return 0;
    if (mvdm_presentation_graphics_describe(window->owner, &width, &height,
            &bits, &stride, &bytes) && width != 0u && height != 0u &&
        bits != 0u && bytes != 0u) {
        *width_out = width;
        *height_out = height;
        return 1;
    }
    if (session_presentation_text_describe(window->owner, &columns, &rows,
            &text_bytes) && columns != 0u && rows != 0u) {
        *width_out = columns * APP_PRESENTATION_GLYPH_WIDTH;
        *height_out = rows * APP_PRESENTATION_GLYPH_HEIGHT;
        return 1;
    }
    *width_out = APP_PRESENTATION_TEXT_COLUMNS * APP_PRESENTATION_GLYPH_WIDTH;
    *height_out = APP_PRESENTATION_TEXT_ROWS * APP_PRESENTATION_GLYPH_HEIGHT;
    return 1;
}

static void presentation_resize_client(app_presentation_window *window,
    HWND handle)
{
    RECT client;
    RECT outer;
    uint32_t width, height;
    DWORD style;
    DWORD extended_style;

    if (window == NULL || handle == NULL ||
        InterlockedCompareExchange(&window->fullscreen, 0, 0) != 0 ||
        !presentation_surface_dimensions(window, &width, &height)) return;
    if (GetClientRect(handle, &client) &&
        (uint32_t)(client.right - client.left) == width &&
        (uint32_t)(client.bottom - client.top) == height) return;
    SetRect(&outer, 0, 0, (int)width, (int)height);
    style = (DWORD)GetWindowLongPtrW(handle, GWL_STYLE);
    extended_style = (DWORD)GetWindowLongPtrW(handle, GWL_EXSTYLE);
    if (!AdjustWindowRectEx(&outer, style, FALSE, extended_style)) return;
    SetWindowPos(handle, NULL, 0, 0, outer.right - outer.left,
        outer.bottom - outer.top, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

static int presentation_event(void *context, const session_video_event *event)
{
    app_presentation_window *window = (app_presentation_window *)context;
    HWND handle;

    if (!app_presentation_window_valid(window)) return 0;
    if (event == NULL) return 0;
    if (event->kind == SESSION_VIDEO_EVENT_GRAPHICS_READY) {
        /* The original graphicsResize call is the only automatic entry into
         * the app surface.  Once Alt+Enter returned a graphics guest to the
         * Console, later paint notifications must not silently reopen it. */
        if (InterlockedCompareExchange(&window->state, 0, 0) ==
            APP_PRESENTATION_WINDOW_READY)
            return app_presentation_window_open(window);
        return 1;
    }
    if (event->kind == SESSION_VIDEO_EVENT_DISPLAY_TOGGLE) {
        LONG state = InterlockedCompareExchange(&window->state, 0, 0);
        if (state == APP_PRESENTATION_WINDOW_READY ||
            state == APP_PRESENTATION_WINDOW_CLOSED)
            return app_presentation_window_open(window);
        return 1;
    }
    if (InterlockedCompareExchange(&window->state, 0, 0) ==
            APP_PRESENTATION_WINDOW_CLOSING ||
        InterlockedCompareExchange(&window->state, 0, 0) ==
            APP_PRESENTATION_WINDOW_CLOSED)
        return 1;
    handle = (HWND)InterlockedCompareExchangePointer(
        (PVOID volatile *)&window->window, NULL, NULL);
    if (handle == NULL) return 1;
    if (InterlockedExchange(&window->repaint_pending, 1) == 0)
        (void)PostMessageW(handle, APP_PRESENTATION_MESSAGE_REPAINT, 0u, 0);
    return 1;
}

void app_presentation_window_initialize(app_presentation_window *window)
{
    if (window == NULL) return;
    memset(window, 0, sizeof(*window));
    window->abi_version = APP_PRESENTATION_WINDOW_VERSION;
    window->struct_bytes = (uint32_t)sizeof(*window);
    window->state = APP_PRESENTATION_WINDOW_READY;
}

int app_presentation_window_valid(const app_presentation_window *window)
{
    return window != NULL &&
        window->abi_version == APP_PRESENTATION_WINDOW_VERSION &&
        window->struct_bytes == sizeof(*window) &&
        window->state >= APP_PRESENTATION_WINDOW_READY &&
        window->state <= APP_PRESENTATION_WINDOW_CLOSED;
}

int app_presentation_window_prepare(app_presentation_window *window,
    session *owner)
{
    if (!app_presentation_window_valid(window) || owner == NULL ||
        !session_valid(owner) || owner->state != SESSION_STATE_READY ||
        window->owner != NULL || window->thread != NULL) return 0;
    if (!session_set_video_event_sink(owner, presentation_event, window))
        return 0;
    window->owner = owner;
    return 1;
}

static void presentation_input_key(app_presentation_window *window,
    UINT message, WPARAM wparam, LPARAM lparam)
{
    INPUT_RECORD record;
    DWORD written;
    DWORD state = 0u;

    if (window->input == NULL || (message != WM_KEYDOWN &&
        message != WM_KEYUP && message != WM_SYSKEYDOWN &&
        message != WM_SYSKEYUP)) return;
    ZeroMemory(&record, sizeof(record));
    record.EventType = KEY_EVENT;
    record.Event.KeyEvent.bKeyDown = message == WM_KEYDOWN ||
        message == WM_SYSKEYDOWN;
    record.Event.KeyEvent.wRepeatCount = 1u;
    record.Event.KeyEvent.wVirtualKeyCode = (WORD)wparam;
    record.Event.KeyEvent.wVirtualScanCode = (WORD)((lparam >> 16u) & 0xffu);
    if ((GetKeyState(VK_SHIFT) & 0x8000) != 0) state |= SHIFT_PRESSED;
    if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) state |= LEFT_CTRL_PRESSED;
    if ((GetKeyState(VK_MENU) & 0x8000) != 0) state |= LEFT_ALT_PRESSED;
    record.Event.KeyEvent.dwControlKeyState = state;
    (void)WriteConsoleInputW(window->input, &record, 1u, &written);
}

static void presentation_paint_text(app_presentation_window *window, HDC target)
{
    uint32_t columns, rows, text_bytes, row, column, glyph_row;
    uint8_t *text;
    uint32_t *pixels;
    uint32_t width, height;
    BITMAPINFO info;

    if (window == NULL ||
        !session_presentation_text_describe(window->owner, &columns, &rows,
            &text_bytes) || columns == 0u || rows == 0u ||
        columns > UINT32_MAX / APP_PRESENTATION_GLYPH_WIDTH ||
        rows > UINT32_MAX / APP_PRESENTATION_GLYPH_HEIGHT) return;
    width = columns * APP_PRESENTATION_GLYPH_WIDTH;
    height = rows * APP_PRESENTATION_GLYPH_HEIGHT;
    if (width == 0u || height == 0u ||
        (size_t)width > SIZE_MAX / height / sizeof(*pixels)) return;
    text = (uint8_t *)malloc(text_bytes);
    pixels = (uint32_t *)malloc((size_t)width * height * sizeof(*pixels));
    if (text == NULL || pixels == NULL ||
        !session_presentation_text_snapshot(window->owner, text, text_bytes,
            &columns, &rows, &text_bytes)) {
        free(pixels);
        free(text);
        return;
    }
    if (!mvdm_softpc_presentation_font_snapshot(window->owner, window->current_font,
            APP_PRESENTATION_FONT_BYTES)) {
        free(pixels);
        free(text);
        return;
    }
    window->current_font_loaded = 1u;
    for (row = 0u; row < rows; ++row) {
        for (column = 0u; column < columns; ++column) {
            uint8_t character = text[(row * columns + column) * 2u];
            uint8_t attribute = text[(row * columns + column) * 2u + 1u];
            uint32_t foreground = presentation_ega_rgb[attribute & 0x0fu];
            uint32_t background = presentation_ega_rgb[(attribute >> 4u) & 0x07u];
            const uint8_t *glyph = window->current_font +
                (size_t)character * APP_PRESENTATION_GLYPH_HEIGHT;
            for (glyph_row = 0u; glyph_row < APP_PRESENTATION_GLYPH_HEIGHT;
                ++glyph_row) {
                uint32_t bit;
                uint32_t *destination = pixels +
                    (size_t)(row * APP_PRESENTATION_GLYPH_HEIGHT + glyph_row) *
                    width + column * APP_PRESENTATION_GLYPH_WIDTH;
                for (bit = 0u; bit < APP_PRESENTATION_GLYPH_WIDTH; ++bit)
                    destination[bit] = (glyph[glyph_row] & (0x80u >> bit)) != 0u ?
                        foreground : background;
            }
        }
    }
    ZeroMemory(&info, sizeof(info));
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = (LONG)width;
    info.bmiHeader.biHeight = -(LONG)height;
    info.bmiHeader.biPlanes = 1u;
    info.bmiHeader.biBitCount = 32u;
    info.bmiHeader.biCompression = BI_RGB;
    (void)SetDIBitsToDevice(target, 0, 0, width, height, 0, 0, 0, height,
        pixels, &info, DIB_RGB_COLORS);
    free(pixels);
    free(text);
}

static void presentation_paint(app_presentation_window *window, HDC target)
{
    uint32_t width, height, bits, stride, bytes;
    uint8_t *pixels;
    BITMAPINFO *info;
    size_t info_bytes;
    uint32_t palette[SESSION_PRESENTATION_PALETTE_ENTRIES];
    uint32_t palette_entries;
    uint32_t index;

    RECT client;
    InterlockedExchange(&window->repaint_pending, 0);
    if (GetClientRect((HWND)InterlockedCompareExchangePointer(
            (PVOID volatile *)&window->window, NULL, NULL), &client)) {
        (void)FillRect(target, &client, (HBRUSH)GetStockObject(BLACK_BRUSH));
    }
    if (!mvdm_presentation_graphics_describe(window->owner,
            &width, &height, &bits, &stride, &bytes)) {
        presentation_paint_text(window, target);
        return;
    }
    if (width == 0u || height == 0u || bytes == 0u || bits == 0u) return;
    pixels = (uint8_t *)malloc(bytes);
    if (pixels == NULL) return;
    if (!mvdm_presentation_graphics_snapshot(window->owner, pixels, bytes,
            &width, &height, &bits, &stride, &bytes, palette,
            SESSION_PRESENTATION_PALETTE_ENTRIES, &palette_entries)) {
        free(pixels);
        return;
    }
    info_bytes = sizeof(BITMAPINFOHEADER) +
        SESSION_PRESENTATION_PALETTE_ENTRIES * sizeof(RGBQUAD);
    info = (BITMAPINFO *)calloc(1u, info_bytes);
    if (info == NULL) {
        free(pixels);
        return;
    }
    info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info->bmiHeader.biWidth = (LONG)width;
    info->bmiHeader.biHeight = -(LONG)height;
    info->bmiHeader.biPlanes = 1u;
    info->bmiHeader.biBitCount = (WORD)bits;
    info->bmiHeader.biCompression = BI_RGB;
    info->bmiHeader.biSizeImage = bytes;
    if (bits <= 8u && palette_entries != 0u) {
        for (index = 0u; index < palette_entries; ++index) {
            info->bmiColors[index].rgbRed = (BYTE)(palette[index] >> 16u);
            info->bmiColors[index].rgbGreen = (BYTE)(palette[index] >> 8u);
            info->bmiColors[index].rgbBlue = (BYTE)palette[index];
        }
        info->bmiHeader.biClrUsed = palette_entries;
    }
    (void)SetDIBitsToDevice(target, 0, 0, width, height,
        0, 0, 0u, height, pixels, info, DIB_RGB_COLORS);
    free(info);
    free(pixels);
}

static DWORD WINAPI presentation_thread(void *context)
{
    app_presentation_window *window = (app_presentation_window *)context;
    WNDCLASSW window_class;
    MSG message;
    HWND handle;
    RECT outer;
    uint32_t width;
    uint32_t height;

    /* The client rectangle is the emulated SoftPC surface, not a host-scaled
     * suggestion.  Use the public system-DPI process mode before this first
     * app window; if another host surface has already selected awareness this
     * harmlessly fails and that existing mode remains authoritative. */
    (void)SetProcessDPIAware();
    ZeroMemory(&window_class, sizeof(window_class));
    window_class.lpfnWndProc = presentation_window_proc;
    window_class.hInstance = GetModuleHandleW(NULL);
    window_class.hCursor = LoadCursorW(NULL, MAKEINTRESOURCEW(32512));
    window_class.lpszClassName = APP_PRESENTATION_WINDOW_CLASS;
    if (RegisterClassW(&window_class) == 0u && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
        goto failed;
    if (!presentation_surface_dimensions(window, &width, &height)) goto failed;
    SetRect(&outer, 0, 0, (int)width, (int)height);
    if (!AdjustWindowRectEx(&outer, APP_PRESENTATION_WINDOW_STYLE, FALSE, 0u))
        goto failed;
    handle = CreateWindowExW(0u, APP_PRESENTATION_WINDOW_CLASS,
        L"MVDM presentation", APP_PRESENTATION_WINDOW_STYLE,
        CW_USEDEFAULT, CW_USEDEFAULT, outer.right - outer.left,
        outer.bottom - outer.top, NULL, NULL, window_class.hInstance, window);
    if (handle == NULL) goto failed;
    window->input = CreateFileW(L"CONIN$", GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0u, NULL);
    if (window->input == INVALID_HANDLE_VALUE) window->input = NULL;
    InterlockedExchangePointer((PVOID volatile *)&window->window, handle);
    InterlockedExchange(&window->state, APP_PRESENTATION_WINDOW_OPEN);
    ShowWindow(handle, SW_SHOW);
    UpdateWindow(handle);
    /* The ready contract includes the source-sized client surface; signal
     * only after User32 has applied ShowWindow/UpdateWindow. */
    SetEvent(window->ready_event);
    while (GetMessageW(&message, NULL, 0u, 0u) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    if (window->input != NULL) CloseHandle(window->input);
    InterlockedExchangePointer((PVOID volatile *)&window->window, NULL);
    InterlockedExchange(&window->state, APP_PRESENTATION_WINDOW_CLOSED);
    SetEvent(window->closed_event);
    return 0u;

failed:
    InterlockedExchange(&window->state, APP_PRESENTATION_WINDOW_FAILED);
    SetEvent(window->ready_event);
    SetEvent(window->closed_event);
    return 1u;
}

static LRESULT CALLBACK presentation_window_proc(HWND handle, UINT message,
    WPARAM wparam, LPARAM lparam)
{
    app_presentation_window *window = (app_presentation_window *)
        GetWindowLongPtrW(handle, GWLP_USERDATA);
    switch (message) {
    case WM_NCCREATE:
        SetWindowLongPtrW(handle, GWLP_USERDATA,
            (LONG_PTR)((CREATESTRUCTW *)lparam)->lpCreateParams);
        return TRUE;
    case APP_PRESENTATION_MESSAGE_REPAINT:
        InvalidateRect(handle, NULL, FALSE);
        return 0;
    case WM_ERASEBKGND:
        /* WM_PAINT fills the exact client surface from copied SoftPC data.
         * Suppress User32's class-background erase so it cannot flash or
         * introduce a host-coloured border between source frames. */
        return 1;
    case WM_PAINT:
        if (window != NULL) {
            PAINTSTRUCT paint;
            HDC target = BeginPaint(handle, &paint);
            presentation_resize_client(window, handle);
            presentation_paint(window, target);
            EndPaint(handle, &paint);
        }
        return 0;
    case WM_SYSKEYDOWN:
        if (window != NULL && wparam == VK_RETURN &&
            (GetKeyState(VK_MENU) & 0x8000) != 0) {
            /* This is the reverse half of the same host-only Alt+Enter
             * transition consumed by console_compat.c.  Do not manufacture
             * a guest key; returning to Console is a presentation decision.
             * A graphics-only guest may therefore leave Console blank. */
            DestroyWindow(handle);
            {
                HWND console = GetConsoleWindow();
                if (console != NULL) (void)SetForegroundWindow(console);
            }
            return 0;
        }
        /* FALLTHROUGH */
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        if (window != NULL) presentation_input_key(window, message, wparam,
            lparam);
        return 0;
    case WM_CLOSE:
        if (window != NULL && window->owner != NULL)
            (void)session_request_cancellation(window->owner,
                SESSION_CANCELLATION_REQUESTED);
        DestroyWindow(handle);
        return 0;
    case APP_PRESENTATION_MESSAGE_SHUTDOWN:
        DestroyWindow(handle);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProcW(handle, message, wparam, lparam);
    }
}

int app_presentation_window_open(app_presentation_window *window)
{
    DWORD wait;
    if (!app_presentation_window_valid(window) || window->owner == NULL ||
        window->owner->state != SESSION_STATE_ACTIVE) return 0;
    if (window->state == APP_PRESENTATION_WINDOW_CLOSED) {
        if (window->thread != NULL &&
            WaitForSingleObject(window->thread, INFINITE) != WAIT_OBJECT_0)
            return 0;
        if (window->thread != NULL) CloseHandle(window->thread);
        if (window->ready_event != NULL) CloseHandle(window->ready_event);
        if (window->closed_event != NULL) CloseHandle(window->closed_event);
        window->thread = NULL;
        window->ready_event = NULL;
        window->closed_event = NULL;
        InterlockedExchange(&window->state, APP_PRESENTATION_WINDOW_READY);
    }
    if (window->state != APP_PRESENTATION_WINDOW_READY) return 0;
    window->ready_event = CreateEventW(NULL, TRUE, FALSE, NULL);
    window->closed_event = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (window->ready_event == NULL || window->closed_event == NULL) return 0;
    InterlockedExchange(&window->state, APP_PRESENTATION_WINDOW_STARTING);
    window->thread = CreateThread(NULL, 0u, presentation_thread, window, 0u,
        &window->thread_id);
    if (window->thread == NULL) {
        InterlockedExchange(&window->state, APP_PRESENTATION_WINDOW_FAILED);
        return 0;
    }
    wait = WaitForSingleObject(window->ready_event,
        APP_PRESENTATION_WINDOW_READY_TIMEOUT);
    return wait == WAIT_OBJECT_0 && window->state == APP_PRESENTATION_WINDOW_OPEN;
}

int app_presentation_window_close(app_presentation_window *window)
{
    HWND handle;
    if (!app_presentation_window_valid(window)) return 0;
    handle = (HWND)InterlockedCompareExchangePointer(
        (PVOID volatile *)&window->window, NULL, NULL);
    if (handle != NULL) {
        InterlockedExchange(&window->state, APP_PRESENTATION_WINDOW_CLOSING);
        (void)PostMessageW(handle, APP_PRESENTATION_MESSAGE_SHUTDOWN, 0u, 0);
    }
    if (window->thread != NULL) {
        /* The app owns this UI thread and its shutdown message always exits
         * its message loop.  Do not release its handle after a timeout: that
         * would let session teardown free the callback owner while the thread
         * can still paint or accept input. */
        if (WaitForSingleObject(window->thread, INFINITE) != WAIT_OBJECT_0)
            return 0;
        CloseHandle(window->thread);
        window->thread = NULL;
    }
    if (window->ready_event != NULL) CloseHandle(window->ready_event);
    if (window->closed_event != NULL) CloseHandle(window->closed_event);
    window->ready_event = NULL;
    window->closed_event = NULL;
    return 1;
}

int app_presentation_window_active(const app_presentation_window *window)
{
    return app_presentation_window_valid(window) &&
        window->state == APP_PRESENTATION_WINDOW_OPEN;
}
