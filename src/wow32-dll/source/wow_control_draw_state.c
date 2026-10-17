#include "wow_control_draw_state.h"
#include "wow_edit_drawing_bindings.h"
#include "wow_edit_entry_bindings.h"
#include <limits.h>

__declspec(thread) wow_control_draw_context *wow_control_draw_current;
BOOL WINAPI ClientDrawState(HDC, HBRUSH, DRAWSTATEPROC,
    LPARAM, WPARAM, int, int, int, int, UINT);

static void release_context(wow_control_draw_context *context)
{
    DWORD error = GetLastError();
    if (context->gray_dc) {
        /* The original body may replace/delete its first scratch bitmap. */
        if (context->original_gray_bitmap) {
            SelectObject(context->gray_dc, context->original_gray_bitmap);
            DeleteDC(context->gray_dc);
        } else DeleteDC(context->gray_dc);
        if (context->owned_gray_bitmap) DeleteObject(context->owned_gray_bitmap);
    }
    /* Any bitmap left selected here by an exceptional callback is borrowed. */
    if (context->bits_dc) DeleteDC(context->bits_dc);
    wow_button_resources_release(&context->atlas);
    if (context->lock_ready) DeleteCriticalSection(&context->lock);
    SetLastError(error);
}

static BOOL create_context(wow_control_draw_context *context)
{
    HBITMAP bitmap;
    /* Original clinit InitClientDrawing's monochrome pattern and size. */
    static const WORD pattern[8] = {0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa};
    if (!wow_edit_current || !wow_edit_current->screen.BitCount) {
        SetLastError(ERROR_INVALID_STATE); return FALSE;
    }
    if (!wow_edit_drawing_ready() || !wow_button_resources_initialize(&context->atlas))
        return FALSE;
    context->atlas.BitCount = wow_edit_current->screen.BitCount;
    if (!InitializeCriticalSectionEx(&context->lock, 0, 0)) return FALSE;
    context->lock_ready = TRUE;
    context->gray_dc = CreateCompatibleDC(NULL);
    context->bits_dc = CreateCompatibleDC(NULL);
    if (!context->gray_dc || !context->bits_dc) return FALSE;
    bitmap = CreateBitmap(8, 8, 1, 1, pattern);
    if (!bitmap) return FALSE;
    context->original_gray_bitmap = SelectObject(context->gray_dc, bitmap);
    if (!context->original_gray_bitmap || context->original_gray_bitmap == HGDI_ERROR) {
        context->original_gray_bitmap = NULL;
        DeleteObject(bitmap);
        return FALSE;
    }
    context->owned_gray_bitmap = bitmap;
    SelectObject(context->gray_dc, GetStockObject(SYSTEM_FONT));
    SelectObject(context->gray_dc, wow_edit_gray_brush);
    SetBkMode(context->gray_dc, OPAQUE);
    SetTextColor(context->gray_dc, RGB(0, 0, 0));
    SetBkColor(context->gray_dc, RGB(255, 255, 255));
    SetTextColor(context->bits_dc, RGB(0, 0, 0));
    SetBkColor(context->bits_dc, RGB(255, 255, 255));
    context->gray_width = context->gray_height = 8;
    return TRUE;
}

BOOL WINAPI wow_control_draw_state(HDC dc, HBRUSH brush, DRAWSTATEPROC callback,
    LPARAM data, WPARAM count, int x, int y, int width, int height, UINT flags)
{
    wow_control_draw_context context = {0};
    wow_control_draw_context *previous = wow_control_draw_current;
    DWORD error = GetLastError();
    POINT viewport;
    BOOL result = FALSE, completed = FALSE;
    /* Not a public USER export: private menu glyphs have a different owner. */
    if ((flags & 7) > DST_BITMAP) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    if (!GetViewportOrgEx(dc, &viewport)) return FALSE;
    __try {
        if (!create_context(&context)) return FALSE;
        wow_control_draw_current = &context;
        SetLastError(error);
        result = ClientDrawState(dc, brush, callback, data, count,
            x, y, width, height, flags);
        completed = TRUE;
    } __finally {
        error = GetLastError();
        wow_control_draw_current = previous;
        if (!completed) SetViewportOrgEx(dc, viewport.x, viewport.y, NULL);
        release_context(&context);
        SetLastError(error);
    }
    return result;
}

BOOL WINAPI wow_control_atlas_blit(HDC dc, int x, int y, int width, int height,
    int source_x, int source_y, DWORD operation)
{
    if (!wow_control_draw_current) { SetLastError(ERROR_INVALID_STATE); return FALSE; }
    return wow_button_resources_blit(&wow_control_draw_current->atlas,
        dc, x, y, width, height, source_x, source_y, operation);
}

HGDIOBJ WINAPI wow_control_draw_select(HDC dc, HGDIOBJ object)
{
    HGDIOBJ previous = SelectObject(dc, object);
    DWORD error = GetLastError();
    /* Only original ClientDrawState's scratch growth uses this binding.
     * Arbitrary callback selections do not transfer object ownership. */
    if (previous && previous != HGDI_ERROR && wow_control_draw_current &&
            dc == wow_control_draw_current->gray_dc && GetObjectType(object) == OBJ_BITMAP)
        wow_control_draw_current->owned_gray_bitmap = (HBITMAP)object;
    SetLastError(error);
    return previous;
}

BOOL WINAPI wow_control_icon_size(HICON icon, UINT step, int *width, int *height)
{
    ICONINFO info = {0};
    BITMAP bitmap;
    BOOL result = FALSE;
    DWORD error;
    /* All recovered consumers request frame zero; no private cursor layout. */
    if (step || !width || !height) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if (!GetIconInfo(icon, &info)) return FALSE;
    if (GetObjectW(info.hbmColor ? info.hbmColor : info.hbmMask,
            sizeof(bitmap), &bitmap) == sizeof(bitmap) && bitmap.bmWidth > 0 &&
            bitmap.bmHeight > 0 && (!info.hbmColor || bitmap.bmHeight <= INT_MAX / 2)) {
        *width = bitmap.bmWidth;
        *height = info.hbmColor ? bitmap.bmHeight * 2 : bitmap.bmHeight;
        result = TRUE;
    }
    error = GetLastError();
    if (info.hbmColor) DeleteObject(info.hbmColor);
    if (info.hbmMask) DeleteObject(info.hbmMask);
    SetLastError(error);
    return result;
}
