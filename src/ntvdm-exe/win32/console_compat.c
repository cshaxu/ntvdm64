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

#undef SetConsoleScreenBufferSize
#undef SetConsoleWindowInfo

/* DIVERGENCE(ADAPTER-WIN32-052): OpenNT ntcon/server/output.c's
 * ResizeScreenBuffer copies rows; it never reflows paragraphs. ConPTY also
 * resizes/reflows storage through SetConsoleWindowInfo. Keep the original
 * cell-grid rule at both reached calls. Private SCREEN_INFORMATION/ROW and
 * CSR/GDI prevent whole-unit composition; no guest or persistent screen
 * state is added. The public handle must support the reached read/write
 * operations. Native failures and incomplete transfers remain failures. */
static BOOL console_grid_transfer(BOOL write, HANDLE output, CHAR_INFO *cells,
    COORD size, SMALL_RECT rect)
{
    COORD origin = {0, 0};
    SMALL_RECT actual = rect;
    BOOL ok = write ? WriteConsoleOutputW(output, cells, size, origin, &actual)
                    : ReadConsoleOutputW(output, cells, size, origin, &actual);
    if (ok && (actual.Left != rect.Left || actual.Top != rect.Top ||
        actual.Right != rect.Right || actual.Bottom != rect.Bottom)) {
        SetLastError(write ? ERROR_WRITE_FAULT : ERROR_READ_FAULT);
        return FALSE;
    }
    return ok;
}

static BOOL console_resize_grid(HANDLE output, const COORD *size,
    BOOL absolute, const SMALL_RECT *window)
{
    CONSOLE_SCREEN_BUFFER_INFO before, after;
    CHAR_INFO *saved, *resized = NULL;
    SMALL_RECT rect;
    COORD cursor;
    int top, rows, columns, x, y;
    BOOL ok;
    DWORD error;

    if (!GetConsoleScreenBufferInfo(output, &before) ||
        (size && (size->X <= 0 || size->Y <= 0 ||
         (size->X == before.dwSize.X && size->Y == before.dwSize.Y))))
        return size ? SetConsoleScreenBufferSize(output, *size)
                    : SetConsoleWindowInfo(output, absolute, window);
    saved = (CHAR_INFO *)calloc((size_t)before.dwSize.X * before.dwSize.Y,
        sizeof(*saved));
    if (!saved) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    rect.Left = rect.Top = 0;
    rect.Right = before.dwSize.X - 1; rect.Bottom = before.dwSize.Y - 1;
    ok = console_grid_transfer(FALSE, output, saved, before.dwSize, rect);
    if (ok) ok = size ? SetConsoleScreenBufferSize(output, *size)
                      : SetConsoleWindowInfo(output, absolute, window);
    if (ok) ok = GetConsoleScreenBufferInfo(output, &after);
    if (ok && (before.dwSize.X != after.dwSize.X ||
               before.dwSize.Y != after.dwSize.Y)) {
        resized = (CHAR_INFO *)calloc((size_t)after.dwSize.X * after.dwSize.Y,
            sizeof(*resized));
        if (!resized) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); ok = FALSE; }
        if (ok) {
            /* Original ResizeScreenBuffer: retain the cursor-containing rows,
             * clip columns, extend each row's final attribute, blank new rows. */
            top = before.dwCursorPosition.Y >= after.dwSize.Y
                ? before.dwCursorPosition.Y - after.dwSize.Y + 1 : 0;
            rows = before.dwSize.Y - top;
            if (rows > after.dwSize.Y) rows = after.dwSize.Y;
            columns = before.dwSize.X < after.dwSize.X
                ? before.dwSize.X : after.dwSize.X;
            for (y = 0; y < after.dwSize.Y; ++y) {
                for (x = 0; x < after.dwSize.X; ++x) {
                    CHAR_INFO *cell = &resized[y * after.dwSize.X + x];
                    if (y < rows && x < columns)
                        *cell = saved[(y + top) * before.dwSize.X + x];
                    else {
                        cell->Char.UnicodeChar = L' ';
                        cell->Attributes = y < rows
                            ? saved[(y + top + 1) * before.dwSize.X - 1].Attributes
                            : before.wAttributes;
                    }
                }
            }
            rect.Right = after.dwSize.X - 1; rect.Bottom = after.dwSize.Y - 1;
            ok = console_grid_transfer(TRUE, output, resized, after.dwSize, rect);
            cursor = before.dwCursorPosition;
            if (cursor.X >= after.dwSize.X) cursor.X = 0;
            if (cursor.Y >= after.dwSize.Y) cursor.Y = after.dwSize.Y - 1;
            if (ok) ok = SetConsoleCursorPosition(output, cursor);
        }
    }
    error = GetLastError();
    free(resized); free(saved);
    SetLastError(error);
    return ok;
}

BOOL WINAPI MvdmSetConsoleScreenBufferSize(HANDLE output, COORD size)
{
    return console_resize_grid(output, &size, FALSE, NULL);
}

BOOL WINAPI MvdmSetConsoleWindowInfo(HANDLE output, BOOL absolute,
    const SMALL_RECT *window)
{
    return console_resize_grid(output, NULL, absolute, window);
}

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

    if (rect == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
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
    /* DIVERGENCE(ADAPTER-WIN32-030): NT4 supplied a Console Server wait
     * object through this source-facing call.  A modern console input handle
     * is itself waitable, so preserve the call shape and return the process
     * console input endpoint without manufacturing an event or MVDM token. */
    return GetStdHandle(STD_INPUT_HANDLE);
}

/* DIVERGENCE(ADAPTER-WIN32-051): OpenNT's Console Server serializes
 * `PrependInputBuffer` and `ReadInputBuffer` under its Console lock. Public
 * Win32 exposes only append-style WriteConsoleInputW. Preserve the reached
 * VDM-only prepend operation in a process-local FIFO ahead of public CONIN$;
 * original nt_event remains the only consumer and waits on this event beside
 * the existing public input and suspend handles. */
typedef struct mvdm_console_prepend_node {
    struct mvdm_console_prepend_node *next;
    HANDLE input;
    DWORD first;
    DWORD count;
    INPUT_RECORD records[1];
} mvdm_console_prepend_node;

static INIT_ONCE mvdm_console_prepend_once = INIT_ONCE_STATIC_INIT;
static CRITICAL_SECTION mvdm_console_prepend_lock;
static HANDLE mvdm_console_prepend_event;
static mvdm_console_prepend_node *mvdm_console_prepend_head;
static volatile LONG mvdm_console_alt_enter_pending;

static BOOL CALLBACK initialize_console_prepend_queue(PINIT_ONCE once,
    PVOID parameter, PVOID *context)
{
    (void)once;
    (void)parameter;
    (void)context;
    InitializeCriticalSection(&mvdm_console_prepend_lock);
    mvdm_console_prepend_event = CreateEventW(NULL, TRUE, FALSE, NULL);
    return mvdm_console_prepend_event != NULL;
}

static BOOL ensure_console_prepend_queue(VOID)
{
    return InitOnceExecuteOnce(&mvdm_console_prepend_once,
        initialize_console_prepend_queue, NULL, NULL);
}

HANDLE WINAPI MvdmConsoleInputPrependWaitHandle(VOID)
{
    if (!ensure_console_prepend_queue()) return NULL;
    return mvdm_console_prepend_event;
}

static DWORD read_console_prepend(HANDLE input, PINPUT_RECORD records,
    DWORD count, LPDWORD read, BOOL remove)
{
    mvdm_console_prepend_node *node;
    DWORD available;

    if (!ensure_console_prepend_queue()) return (DWORD)-1;
    EnterCriticalSection(&mvdm_console_prepend_lock);
    node = mvdm_console_prepend_head;
    if (node == NULL || node->input != input) {
        LeaveCriticalSection(&mvdm_console_prepend_lock);
        return 0u;
    }
    available = node->count < count ? node->count : count;
    if (available != 0u && records != NULL)
        CopyMemory(records, &node->records[node->first],
            (SIZE_T)available * sizeof(*records));
    if (remove && available != 0u) {
        node->first += available;
        node->count -= available;
        if (node->count == 0u) {
            mvdm_console_prepend_head = node->next;
            free(node);
        }
        if (mvdm_console_prepend_head == NULL)
            ResetEvent(mvdm_console_prepend_event);
    }
    LeaveCriticalSection(&mvdm_console_prepend_lock);
    if (read != NULL) *read = available;
    return 1u;
}

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
    DWORD local_result;
    INPUT_RECORD raw_record;
    if ((flags & ~CONSOLE_READ_VALID) != 0u) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    local_result = read_console_prepend(input, records, count, read,
        (flags & CONSOLE_READ_NOREMOVE) == 0u);
    if (local_result == (DWORD)-1) return FALSE;
    if (local_result != 0u) return TRUE;
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
    mvdm_console_prepend_node *node;
    DWORD mode;
    size_t bytes;

    if (written != NULL) *written = 0u;
    if (!GetConsoleMode(input, &mode) || (count != 0u && records == NULL))
        return FALSE;
    if (count == 0u) return TRUE;
    if ((size_t)count > (SIZE_MAX - offsetof(mvdm_console_prepend_node,
                                              records)) / sizeof(*records)) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }
    bytes = offsetof(mvdm_console_prepend_node, records) +
        (size_t)count * sizeof(*records);
    node = (mvdm_console_prepend_node *)malloc(bytes);
    if (node == NULL) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return FALSE;
    }
    if (!ensure_console_prepend_queue()) {
        free(node);
        return FALSE;
    }
    node->next = NULL;
    node->input = input;
    node->first = 0u;
    node->count = count;
    CopyMemory(node->records, records, (SIZE_T)count * sizeof(*records));
    EnterCriticalSection(&mvdm_console_prepend_lock);
    /* Every VDM write is a prepend. A newer return must be consumed before
     * an older returned batch, matching repeated PrependInputBuffer calls. */
    node->next = mvdm_console_prepend_head;
    mvdm_console_prepend_head = node;
    SetEvent(mvdm_console_prepend_event);
    LeaveCriticalSection(&mvdm_console_prepend_lock);
    if (written != NULL) *written = count;
    return TRUE;
}

/* DIVERGENCE(ADAPTER-WIN32-050): OpenNT Console Server maintained this
 * count for the mouse pointer on the active screen buffer.  The public
 * Console API exposes only the *text insertion cursor*, so mapping this call
 * to SetConsoleCursorInfo corrupts the guest-visible text cursor.  The
 * selected standalone worker owns one active Console output surface; retain
 * the source-facing counter locally and deliberately leave the host pointer
 * under conhost/Terminal ownership. */
static volatile LONG mvdm_pointer_display_count;

int WINAPI ShowConsoleCursor(HANDLE output, BOOL show)
{
    DWORD mode;

    /* Preserve the original output-handle validation rather than treating a
     * pointer-visibility request as unconditional success. */
    if (!GetConsoleMode(output, &mode)) return -1;
    return (int)(show ? InterlockedIncrement(&mvdm_pointer_display_count) :
        InterlockedDecrement(&mvdm_pointer_display_count));
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
