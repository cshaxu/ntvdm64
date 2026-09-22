#include "wow_private_user_compat.h"
#include "wow_menu_parser_bindings.h"

#include <string.h>

/* DIVERGENCE(ADAPTER-WOW-022): Original USER paint.c:xxxFillWindow,
 * xxxPaintRect and UT_GetParentDCClipBox,
 * random.c:xxxGetControlColor. USER server PWND/DC locking is unavailable;
 * bind its observable operations to public USER/GDI, without importing the
 * server or reducing FillWindow to an unconditional FillRect. */
BOOL WINAPI wow_private_user_fill_window(HWND brush_window, HWND paint_window,
    HDC dc, HBRUSH brush)
{
    WINDOWINFO paint_info, brush_info;
    RECT clip, client;
    POINT old_origin;
    LONG brush_procedure;
    UINT message;
    int clip_type;

    if (dc == NULL || GetObjectType(dc) == 0) {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    paint_info.cbSize = sizeof(paint_info);
    if (!GetWindowInfo(paint_window, &paint_info)) return FALSE;
    if (brush_window == NULL) brush_window = paint_window;
    brush_info.cbSize = sizeof(brush_info);
    if (!GetWindowInfo(brush_window, &brush_info)) return FALSE;
    clip_type = GetClipBox(dc, &clip);
    if (clip_type == ERROR) return FALSE;
    if (clip_type == NULLREGION) return TRUE;
    if (GetClassLongA(paint_window, GCL_STYLE) & CS_PARENTDC) {
        client = paint_info.rcClient;
        OffsetRect(&client, -client.left, -client.top);
        if (!IntersectRect(&clip, &clip, &client)) return TRUE;
    }
    if (!SetBrushOrgEx(dc,
            brush_info.rcClient.left - paint_info.rcClient.left,
            brush_info.rcClient.top - paint_info.rcClient.top, &old_origin))
        return FALSE;
    if ((ULONG_PTR)brush < CTLCOLOR_MAX) {
        message = (UINT)(ULONG_PTR)brush + WM_CTLCOLORMSGBOX;
        /* Original xxxGetControlColor tests the process, not the thread.
         * The native procedure query enforces this and accepts short HWNDs. */
        SetLastError(ERROR_SUCCESS);
        brush_procedure = GetWindowLongA(brush_window, GWL_WNDPROC);
        if (!brush_procedure && GetLastError() != ERROR_ACCESS_DENIED) {
            SetBrushOrgEx(dc, old_origin.x, old_origin.y, NULL);
            return FALSE;
        }
        if (!brush_procedure) {
            brush = (HBRUSH)DefWindowProcA(brush_window, message,
                (WPARAM)dc, (LPARAM)paint_window);
        } else {
            brush = (HBRUSH)SendMessageA(brush_window, message,
                (WPARAM)dc, (LPARAM)paint_window);
            if (brush == NULL || GetObjectType(brush) != OBJ_BRUSH)
                brush = (HBRUSH)DefWindowProcA(brush_window, message,
                    (WPARAM)dc, (LPARAM)paint_window);
        }
    }
    FillRect(dc, &clip, brush);
    SetBrushOrgEx(dc, old_origin.x, old_origin.y, NULL);
    return TRUE;
}

/* DIVERGENCE(ADAPTER-WOW-013): source-derived message data from OpenNT
 * windows/core/ntuser/kernel/server.c, gawDefWindowMsgs/gawDefWindowSpecMsgs
 * (Copyright (c) Microsoft Corporation). The private USER shared bitmap is
 * unavailable. Preserve its complete non-FE_IME message union rather than a
 * trace-selected subset. Private message values come from winuserp.h.
 * The original merge owner is client/client.c::WowGetDefWindowProcBits.
 * This publishes the guest dispatch filter, not replacement message policy. */
static const WORD wow_def_window_messages[] = {
    WM_GETHOTKEY, WM_SETHOTKEY, WM_SETREDRAW, WM_SETTEXT, WM_PAINT,
    WM_CLOSE, WM_ERASEBKGND, WM_CANCELMODE, WM_SETCURSOR, WM_PAINTICON,
    WM_ICONERASEBKGND, WM_DRAWITEM, 0x004d /* WM_KEYF1 */,
    0x0035 /* WM_ISACTIVEICON */, WM_QUERYDRAGICON, WM_NCCREATE,
    WM_SETICON, WM_NCCALCSIZE, WM_NCPAINT, WM_NCACTIVATE, WM_NCMOUSEMOVE,
    WM_NCRBUTTONDOWN, WM_NCLBUTTONDOWN, WM_NCLBUTTONUP, WM_NCLBUTTONDBLCLK,
    WM_KEYUP, WM_SYSKEYUP, WM_SYSCHAR, WM_SYSCOMMAND,
    0x022b /* WM_QUERYDROPOBJECT */, 0x003b /* WM_CLIENTSHUTDOWN */,
    WM_SYNCPAINT, WM_PRINT, WM_GETICON, WM_QUERYDRAGICON, WM_CONTEXTMENU,
    0x0313 /* WM_SYSMENU */, WM_INPUTLANGCHANGEREQUEST, WM_INPUTLANGCHANGE
};
static const WORD wow_def_window_special_messages[] = {
    WM_ACTIVATE, WM_GETTEXT, WM_GETTEXTLENGTH, WM_RBUTTONUP,
    WM_QUERYENDSESSION, WM_QUERYOPEN, WM_SHOWWINDOW, WM_MOUSEACTIVATE,
    WM_HELP, WM_VKEYTOITEM, WM_CHARTOITEM, WM_KEYDOWN, WM_SYSKEYDOWN,
    0x022a /* WM_DROPOBJECT */, WM_WINDOWPOSCHANGING, WM_WINDOWPOSCHANGED,
    0x008b /* WM_KLUDGEMINRECT */, 0x0019 /* WM_CTLCOLOR */,
    WM_CTLCOLORMSGBOX, WM_CTLCOLOREDIT, WM_CTLCOLORLISTBOX, WM_CTLCOLORBTN,
    WM_CTLCOLORDLG, WM_CTLCOLORSCROLLBAR, WM_NCHITTEST, WM_CTLCOLORSTATIC,
    WM_NOTIFYFORMAT, WM_DEVICECHANGE, WM_POWERBROADCAST, WM_MOUSEWHEEL
};

WORD WINAPI wow_private_user_get_def_window_proc_bits(PBYTE bits,
    WORD byte_count)
{
    WORD maximum = WM_PRINT;
    size_t index;

    /* Unlike the original assertion-only caller precondition, refuse an
     * invalid span before changing it. USER16 supplies a zeroed 101-byte
     * array; keep bytes beyond the original merged extent untouched. */
    if (bits == NULL || byte_count <= maximum / 8u) return 0u;
    memset(bits, 0, maximum / 8u + 1u);
    for (index = 0; index < ARRAYSIZE(wow_def_window_messages); ++index) {
        WORD message = wow_def_window_messages[index];
        bits[message >> 3] |= (BYTE)(1u << (message & 7u));
    }
    for (index = 0; index < ARRAYSIZE(wow_def_window_special_messages); ++index) {
        WORD message = wow_def_window_special_messages[index];
        bits[message >> 3] |= (BYTE)(1u << (message & 7u));
    }
    return maximum;
}

int WINAPI wow_private_user_get_id_from_directory(PBYTE resource_bits,
    UINT resource_type)
{
    /* DIVERGENCE(ADAPTER-WOW-015): retain the exact original rtlres.c
     * resource-directory selector; the private table slot alone remains the
     * standalone boundary. */
    return LookupIconIdFromDirectoryEx(resource_bits,
        resource_type == (UINT)RT_ICON, 0, 0, LR_DEFAULTSIZE);
}

HCURSOR WINAPI wow_private_user_load_create_cursor_icon(HANDLE module,
    LPTSTR module_name, DWORD expected_windows_version, LPCTSTR name,
    DWORD resource_size, PVOID resource_bits, LPTSTR resource_type,
    BOOL client_side)
{
    BOOL icon = resource_type == RT_ICON;
    UINT flags = LR_SHARED | LR_DEFAULTSIZE;

    /* DIVERGENCE(ADAPTER-WOW-016): OpenNT client/clres.c's private
     * WowServerLoadCreateCursorIcon and public CreateIconFromResourceEx
     * converge on ConvertDIBIcon. Preserve default sizing, cursor monochrome
     * conversion, hotspot scaling and shared-resource flags through that
     * public entry, not a second DIB parser. dwExpWinVer is unused upstream;
     * it is an application version, NOT the resource-format version.
     * WU32LoadCursor has already converted resource names to Unicode.
     * Original wcuricon.c retains guest resource aliasing and replacement. */
    UNREFERENCED_PARAMETER(module_name);
    UNREFERENCED_PARAMETER(expected_windows_version);
    UNREFERENCED_PARAMETER(client_side);
    if (!icon && resource_type != RT_CURSOR) return NULL;
    if (!icon) flags |= LR_MONOCHROME;
    if (resource_bits != NULL) {
        if (resource_size == 0) return NULL;
        return (HCURSOR)CreateIconFromResourceEx((PBYTE)resource_bits,
            resource_size, icon, 0x00030000, 0, 0, flags);
    }
    return (HCURSOR)LoadImageW((HINSTANCE)module, (LPCWSTR)name,
        icon ? IMAGE_ICON : IMAGE_CURSOR, 0, 0, flags);
}

/* DIVERGENCE(ADAPTER-WOW-021): OpenNT kernel/ntstubs.c::NtUserGetMenuIndex
 * (Copyright (c) 1985-95 Microsoft Corporation) validates both menus before
 * searching their immediate items. USER server pointers/critical sections
 * cannot enter this worker; public USER supplies validation and item access.
 * Original HMENU32 is a sign-extended WORD, not an independent ID namespace.
 * Full stale handles must still fail validation before any narrow comparison. */
DWORD WINAPI wow_private_user_get_menu_index(HMENU menu, HMENU submenu)
{
    int index, count;
    if (!IsMenu(menu) || !IsMenu(submenu)) {
        SetLastError(ERROR_INVALID_MENU_HANDLE);
        return 0;
    }
    count = GetMenuItemCount(menu);
    for (index = 0; index < count; ++index) {
        HMENU item = GetSubMenu(menu, index);
        HMENU narrow = (HMENU)(INT_PTR)(SHORT)(WORD)(UINT_PTR)item;
        if (item && (item == submenu || narrow == submenu))
            return (DWORD)index;
    }
    return (DWORD)-1;
}

HMENU WINAPI wow_user_load_menu_indirect(const MENUTEMPLATEW *menu_template)
{
    /* ADAPTER-WOW-037: called only by the original Win16 menu consumers,
     * after ConvertMenu16. Native templates may instead use the original
     * parser with wow16=FALSE; no process-wide task flag is fabricated. */
    if (!menu_template) return NULL;
    return CreateMenuFromResource((LPBYTE)menu_template, TRUE);
}

HMENU WINAPI wow_private_user_load_create_menu(HANDLE module, LPTSTR name,
    CONST LPMENUTEMPLATE menu_template, DWORD resource_size,
    BOOL call_client)
{
    /* DIVERGENCE(ADAPTER-WOW-020): USER16 has already found and locked the
     * original Win16 RT_MENU resource before WU32LoadMenu enters this NT4
     * private output slot.  WU32LoadMenu retains the original ConvertMenu16
     * call and passes its converted menu template here. NT4
     * routed that template through WowServerLoadCreateMenu to
     * CreateMenuFromResource. Use that original parser with the explicit
     * Win16 invocation contract: ConvertMenuItems16 stores a packed guest
     * pointer for owner-draw items, not native Unicode text.
     *
     * The historical module/name/client fields identify server bookkeeping,
     * not the template payload.  They remain deliberately unobserved.  The
     * source route has already rejected an absent resource; retain that
     * failure direction for a missing/empty template. */
    UNREFERENCED_PARAMETER(module);
    UNREFERENCED_PARAMETER(name);
    UNREFERENCED_PARAMETER(call_client);
    if (menu_template == NULL || resource_size == 0u) {
        return NULL;
    }
    return wow_user_load_menu_indirect(menu_template);
}
