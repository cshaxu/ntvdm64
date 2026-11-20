/* Source-shaped NT4 Console operations implemented against the existing
 * host text and graphics surfaces or the public Windows Console APIs. */
#include "include/conapi.h"
#include "include/console_graphics.h"
#include "include/console_services.h"
#include "include/host_context.h"
#include "include/mvdm_keyboard_history.h"
#include "include/mvdm_softpc_text_video.h"
#include "include/console_io.h"
#include <limits.h>
#include <stdlib.h>

extern mvdm_keyboard_history nt_keyboard_history;

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
 * the graphics surface path rather than being mistaken for text.
 */
static int present_text_invalidation(HANDLE output, const SMALL_RECT *rect)
{
    compat_host_context *owner = compat_host_context_current();
    uint32_t columns = 0u, rows = 0u, text_bytes = 0u, cell_bytes;
    uint8_t *text = NULL;
    CHAR_INFO *cells = NULL;
    uint32_t row, column;
    int width, height;
    COORD source_size;
    COORD source_origin = { 0, 0 };
    SMALL_RECT destination;
    BOOL wrote;

    if (output != GetStdHandle(STD_OUTPUT_HANDLE)) {
        return 0;
    }
    if (owner == NULL || rect == NULL ||
        !compat_host_text_describe(owner, &columns, &rows,
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
        !compat_host_text_snapshot(owner, text, text_bytes, NULL,
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
    /* Software VGA text is copied completely by its guest owner. Do not
     * transport intermediate rectangles from mouse IRQ/painter callbacks. */
    if(mvdm_softpc_text_video_local() && output==GetStdHandle(STD_OUTPUT_HANDLE))return TRUE;
    graphics_result=ntvdm_console_graphics_invalidate(output,rect);
    if (graphics_result) return graphics_result>0;
    /* DIVERGENCE(ADAPTER-WIN32-048): public Console lacks the NT4 shared VDM
     * text-buffer presentation server.  Re-present the exact original text
     * buffer rectangle at the same invalidation boundary.  This does not
     * parse DOS output or guest memory; the original SoftPC text painter has
     * already selected and populated the cells. */
    text_result = present_text_invalidation(output, rect);
    if (text_result < 0) return FALSE;
    /* Preserve the production fallback result and last error. The retired
     * video notification had no production sink. The routed text write,
     * when successful, still succeeds even without a graphics consumer. */
    SetLastError(compat_host_context_current() ? ERROR_CALL_NOT_IMPLEMENTED : ERROR_INVALID_HANDLE);
    return text_result > 0;
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
    SetLastError(compat_host_context_current() ? ERROR_CALL_NOT_IMPLEMENTED : ERROR_INVALID_HANDLE);
}

BOOL WINAPI RegisterConsoleVDM(DWORD flags, HANDLE start_event,
                               HANDLE end_event, LPWSTR state_name,
                               DWORD state_name_length, LPDWORD state_length,
                               PVOID *state, LPWSTR buffer_name,
                               DWORD buffer_name_length, COORD buffer_size,
                               PVOID *buffer)
{
    compat_host_context *owner = compat_host_context_current();
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
    if (!compat_host_context_valid(owner) ||
        owner->state != COMPAT_HOST_CONTEXT_ACTIVE) {
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
    if (!compat_host_text_acquire_writable(owner,
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

BOOL WINAPI WriteConsoleInputVDMW(HANDLE input, PINPUT_RECORD records, DWORD count,
                                  LPDWORD written)
{
    uint64_t receipts[MVDM_KEY_HISTORY_CAPACITY];DWORD i;
    /* ADAPTER-WIN32-051: original atomic Console prepend, executed by the
     * authenticated frontend; no worker-only returned-key queue. */
    /* This adapter has only the original event-thread callers. During
     * ReturnUnusedKeyEvents the selected history remains live until the
     * call returns; BIOS-buffer writes happen after InitKeyHistory. The
     * selected ordinals are newest-first while the returned batch is oldest-first. */
    if(count && count<=MVDM_KEY_HISTORY_CAPACITY &&
        mvdm_keyboard_history_selected(&nt_keyboard_history)>=count) {
        for(i=0;i<count;++i)receipts[i]=mvdm_keyboard_history_receipt(&nt_keyboard_history,count-i);
        return ntvdm_console_prepend_keys_tagged(input,records,receipts,count,written);
    }
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
