#include "app/presentation_window.h"
#include "presentation_surface.h"

#include <stdlib.h>
#include <string.h>

#define APP_PRESENTATION_WINDOW_CLASS L"MvdmPresentationWindow"
#define APP_PRESENTATION_WINDOW_READY_TIMEOUT 5000u
#define APP_PRESENTATION_MESSAGE_REPAINT (WM_APP + 1u)
#define APP_PRESENTATION_MESSAGE_SHUTDOWN (WM_APP + 2u)

static LRESULT CALLBACK presentation_window_proc(HWND handle, UINT message,
    WPARAM wparam, LPARAM lparam);

static int presentation_event(void *context, const session_video_event *event)
{
    app_presentation_window *window = (app_presentation_window *)context;
    HWND handle;

    (void)event;
    if (!app_presentation_window_valid(window)) return 0;
    if (InterlockedCompareExchange(&window->state, 0, 0) ==
            APP_PRESENTATION_WINDOW_CLOSING ||
        InterlockedCompareExchange(&window->state, 0, 0) ==
            APP_PRESENTATION_WINDOW_CLOSED)
        return 1;
    handle = (HWND)InterlockedCompareExchangePointer(
        (PVOID volatile *)&window->window, NULL, NULL);
    if (handle == NULL) return 0;
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

static void presentation_paint(app_presentation_window *window, HDC target)
{
    uint32_t width, height, bits, stride, bytes;
    uint8_t *pixels;
    BITMAPINFO *info;
    size_t info_bytes;
    uint32_t palette[SESSION_PRESENTATION_PALETTE_ENTRIES];
    uint32_t palette_entries;
    uint32_t index;

    InterlockedExchange(&window->repaint_pending, 0);
    if (!mvdm_presentation_graphics_describe(window->owner,
            &width, &height, &bits, &stride, &bytes)) {
        uint32_t columns, rows, text_bytes, row, column;
        uint8_t *text;
        char *characters;
        HFONT previous;
        if (!session_presentation_text_describe(window->owner, &columns, &rows,
                &text_bytes) || columns == 0u || rows == 0u) return;
        text = (uint8_t *)malloc(text_bytes);
        characters = (char *)malloc((size_t)columns + 1u);
        if (text == NULL || characters == NULL ||
            !session_presentation_text_snapshot(window->owner, text,
                text_bytes, &columns, &rows, &text_bytes)) {
            free(characters);
            free(text);
            return;
        }
        previous = (HFONT)SelectObject(target, GetStockObject(ANSI_FIXED_FONT));
        SetBkColor(target, RGB(0, 0, 0));
        SetTextColor(target, RGB(192, 192, 192));
        for (row = 0u; row < rows; ++row) {
            for (column = 0u; column < columns; ++column)
                characters[column] = (char)text[(row * columns + column) * 2u];
            characters[columns] = '\0';
            TextOutA(target, 0, (int)(row * 16u), characters, (int)columns);
        }
        SelectObject(target, previous);
        free(characters);
        free(text);
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
    (void)StretchDIBits(target, 0, 0, (int)width, (int)height,
        0, 0, (int)width, (int)height, pixels, info, DIB_RGB_COLORS,
        SRCCOPY);
    free(info);
    free(pixels);
}

static void presentation_toggle_fullscreen(app_presentation_window *window,
    HWND handle)
{
    MONITORINFO monitor;
    if (InterlockedCompareExchange(&window->fullscreen, 0, 0) == 0) {
        window->windowed_style = GetWindowLongPtrW(handle, GWL_STYLE);
        (void)GetWindowRect(handle, &window->windowed_rect);
        ZeroMemory(&monitor, sizeof(monitor));
        monitor.cbSize = sizeof(monitor);
        if (GetMonitorInfoW(MonitorFromWindow(handle,
                MONITOR_DEFAULTTONEAREST), &monitor)) {
            SetWindowLongPtrW(handle, GWL_STYLE, WS_POPUP | WS_VISIBLE);
            SetWindowPos(handle, HWND_TOP, monitor.rcMonitor.left,
                monitor.rcMonitor.top,
                monitor.rcMonitor.right - monitor.rcMonitor.left,
                monitor.rcMonitor.bottom - monitor.rcMonitor.top,
                SWP_FRAMECHANGED | SWP_SHOWWINDOW);
            InterlockedExchange(&window->fullscreen, 1);
        }
    } else {
        SetWindowLongPtrW(handle, GWL_STYLE, window->windowed_style);
        SetWindowPos(handle, HWND_NOTOPMOST, window->windowed_rect.left,
            window->windowed_rect.top,
            window->windowed_rect.right - window->windowed_rect.left,
            window->windowed_rect.bottom - window->windowed_rect.top,
            SWP_FRAMECHANGED | SWP_SHOWWINDOW);
        InterlockedExchange(&window->fullscreen, 0);
    }
}

static DWORD WINAPI presentation_thread(void *context)
{
    app_presentation_window *window = (app_presentation_window *)context;
    WNDCLASSW window_class;
    MSG message;
    HWND handle;

    ZeroMemory(&window_class, sizeof(window_class));
    window_class.lpfnWndProc = presentation_window_proc;
    window_class.hInstance = GetModuleHandleW(NULL);
    window_class.hCursor = LoadCursorW(NULL, MAKEINTRESOURCEW(32512));
    window_class.lpszClassName = APP_PRESENTATION_WINDOW_CLASS;
    if (RegisterClassW(&window_class) == 0u && GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
        goto failed;
    handle = CreateWindowExW(0u, APP_PRESENTATION_WINDOW_CLASS,
        L"MVDM presentation", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        960, 720, NULL, NULL, window_class.hInstance, window);
    if (handle == NULL) goto failed;
    window->input = CreateFileW(L"CONIN$", GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0u, NULL);
    if (window->input == INVALID_HANDLE_VALUE) window->input = NULL;
    InterlockedExchangePointer((PVOID volatile *)&window->window, handle);
    InterlockedExchange(&window->state, APP_PRESENTATION_WINDOW_OPEN);
    SetEvent(window->ready_event);
    ShowWindow(handle, SW_SHOW);
    UpdateWindow(handle);
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
    case WM_PAINT:
        if (window != NULL) {
            PAINTSTRUCT paint;
            HDC target = BeginPaint(handle, &paint);
            presentation_paint(window, target);
            EndPaint(handle, &paint);
        }
        return 0;
    case WM_SYSKEYDOWN:
        if (window != NULL && wparam == VK_RETURN &&
            (GetKeyState(VK_MENU) & 0x8000) != 0) {
            presentation_toggle_fullscreen(window, handle);
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
        window->owner->state != SESSION_STATE_READY ||
        window->state != APP_PRESENTATION_WINDOW_READY) return 0;
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
