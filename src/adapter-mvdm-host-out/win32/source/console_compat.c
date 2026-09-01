/*
 * DIVERGENCE(ADAPTER-WIN32-030): Same-shaped public-Win32 replacement for
 * reached NT4 Console Server client calls.  Modern public Console/User32 APIs
 * provide record and coordinate operations but no private server export or
 * cursor reference count.  Original SoftPC callers retain their names,
 * parameters and ordering; unsupported operations fail explicitly.
 */
#include <windows.h>
#include "conapi.h"
#include "session/session.h"

/*
 * The historical Console Server owned the allocation and lifetime of the VDM
 * registration buffers.  The selected windowed CPU40 profile needs only the
 * text-buffer portion of that contract: original nt_det.c writes characters
 * into the returned host-local buffer, and no pointer crosses into guest
 * state.  The buffer is owned by session so a later app presentation consumer
 * can receive only a bounded copy, not an adapter-private pointer.
 */
BOOL WINAPI GetConsoleKeyboardLayoutNameA(LPSTR layout_name)
{
    /* DIVERGENCE(ADAPTER-WIN32-034): the NT4 Console Server returned the
     * active console keyboard-layout name through this source-facing BOOL
     * API. Modern public Win32 no longer exposes that Console Server entry,
     * but GetKeyboardLayoutNameA has the same fixed-layout-name result and
     * failure contract for the process input locale. Keep the original name
     * and buffer ownership at the MVDM boundary; do not add a console broker
     * or a separate keyboard-layout cache. */
    return GetKeyboardLayoutNameA(layout_name);
}

static BOOL console_video_event(uint32_t kind, HANDLE output, HPALETTE palette,
                                const SMALL_RECT *rect, DWORD flags)
{
    session *owner = session_thread_current();
    session_video_event event;

    if (owner == NULL) {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    ZeroMemory(&event, sizeof(event));
    event.kind = kind;
    event.output_handle = (uintptr_t)output;
    event.palette_handle = (uintptr_t)palette;
    event.flags = flags;
    if (rect != NULL) {
        event.left = rect->Left;
        event.top = rect->Top;
        event.right = rect->Right;
        event.bottom = rect->Bottom;
    }
    if (!session_notify_video_event(owner, &event)) {
        SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
        return FALSE;
    }
    return TRUE;
}

BOOL WINAPI InvalidateConsoleDIBits(HANDLE output, PSMALL_RECT rect)
{
    if (rect == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    return console_video_event(SESSION_VIDEO_EVENT_INVALIDATE, output, NULL,
        rect, 0u);
}

BOOL WINAPI SetConsolePalette(HANDLE output, HPALETTE palette, DWORD flags)
{
    return console_video_event(SESSION_VIDEO_EVENT_PALETTE, output, palette,
        NULL, flags);
}

VOID WINAPI SetLastConsoleEventActive(VOID)
{
    (void)console_video_event(SESSION_VIDEO_EVENT_ACTIVE, NULL, NULL, NULL,
        0u);
}

BOOL WINAPI RegisterConsoleVDM(DWORD flags, HANDLE start_event,
                               HANDLE end_event, LPWSTR state_name,
                               DWORD state_name_length, LPDWORD state_length,
                               PVOID *state, LPWSTR buffer_name,
                               DWORD buffer_name_length, COORD buffer_size,
                               PVOID *buffer)
{
    session *owner = session_thread_current();
    uint8_t *text_buffer;

    /* DIVERGENCE(ADAPTER-WIN32-032): preserve the full NT4 source-facing
     * registration ABI.  Modern public Console APIs do not expose the
     * Console-Server controller/fullscreen protocol, but the selected
     * windowed CPU40 path requires its host-local text-buffer result.  Supply
     * only that result through session-owned, bounded storage.  The
     * state mapping remains absent (length zero), exactly as original nt_det
     * accepts when fullscreen hardware is unavailable. */
    (void)start_event;
    (void)end_event;
    (void)state_name;
    (void)state_name_length;
    (void)buffer_name;
    (void)buffer_name_length;
    if (owner == NULL || !session_valid(owner) ||
        owner->state != SESSION_STATE_ACTIVE) {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    if (flags == CONSOLE_UNREGISTER_VDM) {
        session_presentation_text_clear(owner);
        return TRUE;
    }
    if ((flags != CONSOLE_REGISTER_VDM && flags != CONSOLE_REGISTER_WOW) ||
        state_length == NULL || state == NULL || buffer == NULL ||
        buffer_size.X <= 0 || buffer_size.Y <= 0) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if (!session_presentation_text_acquire_writable(owner,
            (uint32_t)buffer_size.X, (uint32_t)buffer_size.Y, &text_buffer)) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }
    *state_length = 0u;
    *state = NULL;
    *buffer = text_buffer;
    return TRUE;
}

BOOL WINAPI MvdmPresentationGraphicsBuffer(HANDLE output,
                                           PCONSOLE_GRAPHICS_BUFFER_INFO info,
                                           HANDLE *screen_buffer)
{
    session *owner = session_thread_current();
    BITMAPINFOHEADER *header;
    uint64_t bits_per_line;
    uint64_t stride;
    uint64_t height;
    uint8_t *pixels;
    HANDLE duplicate;
    HANDLE mutex;

    if (screen_buffer != NULL) *screen_buffer = NULL;
    if (owner == NULL || !session_valid(owner) ||
        owner->state != SESSION_STATE_ACTIVE || info == NULL ||
        info->lpBitMapInfo == NULL || screen_buffer == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    header = &info->lpBitMapInfo->bmiHeader;
    if (header->biWidth <= 0 || header->biHeight == 0 ||
        header->biBitCount == 0u || header->biCompression != BI_RGB) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return FALSE;
    }
    height = header->biHeight < 0 ? -(int64_t)header->biHeight :
        (uint64_t)header->biHeight;
    bits_per_line = (uint64_t)(uint32_t)header->biWidth *
        (uint64_t)header->biBitCount;
    stride = ((bits_per_line + 31u) / 32u) * sizeof(DWORD);
    if (height == 0u || height > UINT32_MAX || stride == 0u ||
        stride > UINT32_MAX ||
        !session_presentation_graphics_acquire_writable(owner,
            (uint32_t)header->biWidth, (uint32_t)height,
            (uint32_t)header->biBitCount, (uint32_t)stride, &pixels)) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }
    if (!DuplicateHandle(GetCurrentProcess(), output, GetCurrentProcess(),
            &duplicate, 0u, FALSE, DUPLICATE_SAME_ACCESS)) {
        session_presentation_graphics_clear(owner);
        return FALSE;
    }
    mutex = CreateMutexW(NULL, FALSE, NULL);
    if (mutex == NULL) {
        CloseHandle(duplicate);
        session_presentation_graphics_clear(owner);
        return FALSE;
    }
    info->hMutex = mutex;
    info->lpBitMap = pixels;
    *screen_buffer = duplicate;
    return TRUE;
}

VOID WINAPI MvdmPresentationGraphicsClear(VOID)
{
    session *owner = session_thread_current();
    if (owner != NULL) session_presentation_graphics_clear(owner);
}

/* DIVERGENCE(ADAPTER-WIN32-033): these NT4 Console Server calls carried
 * private per-console command-range and shortcut-reservation state. Modern
 * public Console APIs expose neither operation. Preserve their source-facing
 * forms and explicitly decline; never fabricate a system menu or globally
 * intercept host keystrokes. */
HMENU WINAPI ConsoleMenuControl(HANDLE output, UINT command_low,
                                UINT command_high)
{
    (void)output;
    (void)command_low;
    (void)command_high;
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return NULL;
}

BOOL WINAPI SetConsoleKeyShortcuts(BOOL set, BYTE reserve_keys,
                                   LPAPPKEY app_keys, DWORD key_count)
{
    (void)set;
    (void)reserve_keys;
    (void)app_keys;
    (void)key_count;
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}

HANDLE GetConsoleInputWaitHandle(VOID)
{
    /* DIVERGENCE(ADAPTER-WIN32-030): NT4 supplied a Console Server wait
     * object through this source-facing call.  A modern console input handle
     * is itself waitable, so preserve the call shape and return the process
     * console input endpoint without manufacturing an event or MVDM token. */
    return GetStdHandle(STD_INPUT_HANDLE);
}

BOOL WINAPI ReadConsoleInputExW(HANDLE input, PINPUT_RECORD records, DWORD count,
                                LPDWORD read, USHORT flags)
{
    DWORD available;
    if ((flags & ~CONSOLE_READ_VALID) != 0u) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if ((flags & CONSOLE_READ_NOWAIT) != 0u) {
        if (!GetNumberOfConsoleInputEvents(input, &available)) return FALSE;
        if (available == 0u) { if (read != NULL) *read = 0u; return TRUE; }
    }
    if ((flags & CONSOLE_READ_NOREMOVE) != 0u) return PeekConsoleInputW(input, records, count, read);
    return ReadConsoleInputW(input, records, count, read);
}

BOOL WINAPI WriteConsoleInputVDMW(HANDLE input, PINPUT_RECORD records, DWORD count,
                                  LPDWORD written)
{
    return WriteConsoleInputW(input, records, count, written);
}

int WINAPI ShowConsoleCursor(HANDLE output, BOOL show)
{
    CONSOLE_CURSOR_INFO info;
    if (!GetConsoleCursorInfo(output, &info)) return -1;
    info.bVisible = show;
    if (!SetConsoleCursorInfo(output, &info)) return -1;
    /* Original callers loop while hiding until a negative count and while
       showing until a nonnegative count.  Modern Console API has no count. */
    return show ? 0 : -1;
}

BOOL WINAPI VDMConsoleOperation(DWORD operation, LPVOID data)
{
    HWND window = GetConsoleWindow();
    if (window == NULL || data == NULL) { SetLastError(ERROR_CALL_NOT_IMPLEMENTED); return FALSE; }
    switch (operation) {
    case VDM_IS_ICONIC: *(BOOL *)data = IsIconic(window); return TRUE;
    case VDM_CLIENT_RECT: return GetClientRect(window, (RECT *)data);
    case VDM_CLIENT_TO_SCREEN: return ClientToScreen(window, (POINT *)data);
    default: SetLastError(ERROR_CALL_NOT_IMPLEMENTED); return FALSE;
    }
}
