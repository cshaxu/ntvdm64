/*
 * DIVERGENCE(ADAPTER-WIN32-030): Same-shaped public-Win32 replacement for
 * reached NT4 Console Server client calls.  Modern public Console/User32 APIs
 * provide record and coordinate operations but no private server export or
 * cursor reference count.  Original SoftPC callers retain their names,
 * parameters and ordering; unsupported operations fail explicitly.
 */
#include <windows.h>
#include <stddef.h>
#include <stdlib.h>
#include "conapi.h"
#include "ntvdm-exe/session/session.h"
#include "console_client.h"
#include "product-abi/console_io.h"


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
     * API. The modern kernel32 exports exist but fail on the tested Console;
     * the retained fallback queries this thread's input locale. Its buffer
     * shape matches, but that does not prove active Console-layout identity.
     * Keep this existing fallback until the input-owner layout contract is
     * verified; do not silently substitute a frontend thread or cache. */
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

/*
 * The original Console Server owned this operation.  SoftPC's nt_text()
 * copies character/attribute cells into the buffer returned by
 * RegisterConsoleVDM, then invalidates precisely this rectangle.  On NT4 the
 * Console Server made that shared buffer visible itself.  Public Console has
 * no shared-VDM text-buffer facility, so preserve the caller's buffer and
 * rectangle contract and copy only the invalidated cells to CONOUT$ here.
 *
 * Return 1 when this is the active text Console surface, 0 when `output`
 * names another source-owned presentation surface, and -1 on a real Console
 * failure.  In particular, a graphics-buffer duplicate must continue to use
 * the app presentation event below rather than being mistaken for text.
 */
static int present_text_invalidation(HANDLE output, const SMALL_RECT *rect)
{
    session *owner = session_thread_current();
    uint32_t columns = 0u, rows = 0u, text_bytes = 0u, cell_bytes;
    uint8_t *text = NULL;
    CHAR_INFO *cells = NULL;
    uint32_t row, column, nonblank = 0u;
    int width, height;
    COORD source_size;
    COORD source_origin = { 0, 0 };
    SMALL_RECT destination;
    BOOL wrote;

    if (output != GetStdHandle(STD_OUTPUT_HANDLE)) {
        return 0;
    }
    if (owner == NULL || rect == NULL ||
        !session_presentation_text_describe(owner, &columns, &rows,
                                            &text_bytes)) return 0;
    if (rect->Left < 0 || rect->Top < 0 || rect->Right < rect->Left ||
        rect->Bottom < rect->Top || (uint32_t)rect->Right >= columns ||
        (uint32_t)rect->Bottom >= rows) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return -1;
    }
    width = rect->Right - rect->Left + 1;
    height = rect->Bottom - rect->Top + 1;
    if (width <= 0 || height <= 0 || width > SHRT_MAX || height > SHRT_MAX ||
        text_bytes == 0u || columns == 0u || rows == 0u ||
        text_bytes / columns / rows < 2u || (size_t)width > SIZE_MAX / (size_t)height ||
        (size_t)width * (size_t)height > SIZE_MAX / sizeof(*cells)) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return -1;
    }
    cell_bytes = text_bytes / columns / rows;
    text = (uint8_t *)malloc(text_bytes);
    cells = (CHAR_INFO *)calloc((size_t)width * (size_t)height, sizeof(*cells));
    if (text == NULL || cells == NULL ||
        !session_presentation_text_snapshot(owner, text, text_bytes, NULL,
                                            NULL, NULL)) {
        free(cells);
        free(text);
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return -1;
    }
    for (row = 0u; row < (uint32_t)height; ++row) {
        for (column = 0u; column < (uint32_t)width; ++column) {
            uint32_t source_cell =
                ((uint32_t)rect->Top + row) * columns +
                (uint32_t)rect->Left + column;
            CHAR_INFO *destination_cell = &cells[row * (uint32_t)width + column];
            destination_cell->Char.AsciiChar = text[source_cell * cell_bytes];
            destination_cell->Attributes = text[source_cell * cell_bytes + 1u];
            if (destination_cell->Char.AsciiChar != '\0' &&
                destination_cell->Char.AsciiChar != ' ') ++nonblank;
        }
    }
    source_size.X = (SHORT)width;
    source_size.Y = (SHORT)height;
    destination = *rect;
    wrote = WriteConsoleOutputA(output, cells, source_size, source_origin,
                                &destination);
    free(cells);
    free(text);
    return wrote ? 1 : -1;
}

BOOL WINAPI InvalidateConsoleDIBits(HANDLE output, PSMALL_RECT rect)
{
    int text_result;
    int graphics_result;

    if (rect == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    graphics_result=ntvdm_console_graphics_invalidate(output,rect);
    if (graphics_result) return graphics_result>0;
    /* DIVERGENCE(ADAPTER-WIN32-048): public Console lacks the NT4 shared VDM
     * text-buffer presentation server.  Re-present the exact original text
     * buffer rectangle at the same invalidation boundary.  This does not
     * parse DOS output or guest memory; the original SoftPC text painter has
     * already selected and populated the cells. */
    text_result = present_text_invalidation(output, rect);
    if (text_result > 0) {
        /* A bound app surface may also repaint the copied text plane after an
         * Alt+Enter transfer.  Its absence must not turn a successful Console
         * presentation into an original-call failure. */
        (void)console_video_event(SESSION_VIDEO_EVENT_INVALIDATE, output,
            NULL, rect, 0u);
        return TRUE;
    }
    if (text_result < 0) return FALSE;
    return console_video_event(SESSION_VIDEO_EVENT_INVALIDATE, output, NULL,
        rect, 0u);
}

BOOL WINAPI SetConsolePalette(HANDLE output, HPALETTE palette, DWORD flags)
{
    int result=ntvdm_console_graphics_palette(output,palette,flags);
    if (result) return result>0;
    /* DIVERGENCE(ADAPTER-WIN32-043): this is an NT4 Console Server operation
     * over a graphics screen buffer, not a public Console palette API.  Do
     * not report success for an unpresented copied palette. */
    (void)output;
    (void)palette;
    (void)flags;
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
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
        /* DIVERGENCE(ADAPTER-WIN32-032): the NT4 console-server unregister
         * operation detached the VDM from its presentation registration; it
         * did not revoke the caller's text-buffer mapping.  SoftPC retains
         * textBuffer across doNullRegister()/subsequent registration and may
         * write it while the host child is blocked.  Keep this session-owned
         * backing allocation alive until session disposal or a later safe
         * replacement, so the source-facing pointer lifetime is preserved. */
        return TRUE;
    }
    if ((flags != CONSOLE_REGISTER_VDM && flags != CONSOLE_REGISTER_WOW) ||
        state_length == NULL || state == NULL || buffer == NULL ||
        buffer_size.X <= 0 || buffer_size.Y <= 0) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if (!session_presentation_text_acquire_writable(owner,
            (uint32_t)buffer_size.X, (uint32_t)buffer_size.Y,
            /* Match original nt_cga.c TEXT_INCVAL without MONITOR: CCPU40
             * uses four-byte VGA-interleaved cells even on an x86 host. */
            4u,
            &text_buffer)) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }
    *state_length = 0u;
    *state = NULL;
    *buffer = text_buffer;
    return TRUE;
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
    /* The DOS frontend supplies a typed synchronize-only readiness event.
     * No-channel callers retain the existing process Console wait handle. */
    return ntvdm_console_input_wait_handle();
}

static volatile LONG mvdm_console_alt_enter_pending;

/* DIVERGENCE(ADAPTER-WIN32-047): the NT4 Console Server handled WM_SYSKEY
 * Alt+Enter before it reached the VDM input buffer.  Modern public Console
 * returns records after that retired server stage.  Consume only the paired
 * reserved host shortcut here; it neither toggles a display nor manufactures
 * a guest input record. */
static BOOL consume_console_alt_enter(PINPUT_RECORD record)
{
    KEY_EVENT_RECORD *key;

    if (record->EventType != KEY_EVENT) return FALSE;
    key = &record->Event.KeyEvent;
    if (key->wVirtualKeyCode != VK_RETURN) return FALSE;
    if (key->bKeyDown &&
        (key->dwControlKeyState & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)) != 0u &&
        (key->dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) == 0u) {
        InterlockedExchange(&mvdm_console_alt_enter_pending, 1);
        return TRUE;
    }
    if (!key->bKeyDown &&
        InterlockedExchangeAdd(&mvdm_console_alt_enter_pending, 0) != 0) {
        InterlockedExchange(&mvdm_console_alt_enter_pending, 0);
        return TRUE;
    }
    return FALSE;
}

BOOL WINAPI ReadConsoleInputExW(HANDLE input, PINPUT_RECORD records, DWORD count,
                                LPDWORD read, USHORT flags)
{
    DWORD available;
    INPUT_RECORD raw_record;
    if ((flags & ~CONSOLE_READ_VALID) != 0u) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if (count == 0u)
        return (flags & CONSOLE_READ_NOREMOVE) != 0u ?
            PeekConsoleInputW(input, records, count, read) :
            ReadConsoleInputW(input, records, count, read);
    for (;;) {
        /* Peek tests the same public queue as Read.  Do not use the old
         * GetNumberOfConsoleInputEvents/Read pair: it races a queue change. */
        if ((flags & (CONSOLE_READ_NOWAIT | CONSOLE_READ_NOREMOVE)) != 0u) {
            if (!PeekConsoleInputW(input, &raw_record, 1u, &available)) return FALSE;
            if (available == 0u) { if (read != NULL) *read = 0u; return TRUE; }
            if (!consume_console_alt_enter(&raw_record)) {
                if ((flags & CONSOLE_READ_NOREMOVE) != 0u) {
                    records[0] = raw_record;
                    if (read != NULL) *read = 1u;
                    return TRUE;
                }
                if (!ReadConsoleInputW(input, records, 1u, read)) return FALSE;
                return TRUE;
            }
            /* The reserved host shortcut must be removed even for NOREMOVE:
             * the original server had already consumed it before this API. */
            if (!ReadConsoleInputW(input, &raw_record, 1u, &available)) return FALSE;
            continue;
        }
        if (!ReadConsoleInputW(input, &raw_record, 1u, &available)) return FALSE;
        if (available == 0u || consume_console_alt_enter(&raw_record)) continue;
        records[0] = raw_record;
        if (read != NULL) *read = 1u;
        return TRUE;
    }
}

BOOL WINAPI WriteConsoleInputVDMW(HANDLE input, PINPUT_RECORD records, DWORD count,
                                  LPDWORD written)
{
    /* ADAPTER-WIN32-051: original atomic Console prepend, executed by the
     * authenticated frontend; no worker-only returned-key queue. */
    return ntvdm_console_prepend_keys(input,records,count,written);
}

BOOL WINAPI VDMConsoleOperation(DWORD operation, LPVOID data)
{
    HWND window;
    DWORD query;
    LONG values[4]={0};
    int remote;
    if (!data) { SetLastError(ERROR_CALL_NOT_IMPLEMENTED);return FALSE; }
    switch (operation) {
    case VDM_IS_ICONIC: query=CONSOLE_WINDOW_ICONIC;break;
    case VDM_CLIENT_RECT: query=CONSOLE_WINDOW_CLIENT_RECT;break;
    case VDM_CLIENT_TO_SCREEN:
        query=CONSOLE_WINDOW_CLIENT_TO_SCREEN;
        values[0]=((POINT *)data)->x;values[1]=((POINT *)data)->y;break;
    default: SetLastError(ERROR_CALL_NOT_IMPLEMENTED);return FALSE;
    }
    remote=ntvdm_console_window_query(query,values);
    if (remote>=0) {
        if (!remote) return FALSE;
        if (operation==VDM_IS_ICONIC) *(BOOL *)data=values[0];
        else if (operation==VDM_CLIENT_RECT) {
            RECT *rect=data;
            rect->left=values[0];rect->top=values[1];
            rect->right=values[2];rect->bottom=values[3];
        } else { ((POINT *)data)->x=values[0];((POINT *)data)->y=values[1]; }
        return TRUE;
    }
    window = GetConsoleWindow();
    if (window == NULL || data == NULL) { SetLastError(ERROR_CALL_NOT_IMPLEMENTED); return FALSE; }
    switch (operation) {
    case VDM_IS_ICONIC: *(BOOL *)data = IsIconic(window); return TRUE;
    case VDM_CLIENT_RECT: return GetClientRect(window, (RECT *)data);
    case VDM_CLIENT_TO_SCREEN: return ClientToScreen(window, (POINT *)data);
    default: SetLastError(ERROR_CALL_NOT_IMPLEMENTED); return FALSE;
    }
}
