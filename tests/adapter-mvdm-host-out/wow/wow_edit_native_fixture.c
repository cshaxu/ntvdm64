#include "wow_edit_native_bindings.h"
#include "wow_edit_entry_bindings.h"
#include "wow_edit_drawing_bindings.h"
#include <stdio.h>
#include <string.h>
#include <wchar.h>

/* Supplemental native ABI witness, not original EDIT or guest acceptance.
 * Run only as part of the completed dependency group's background suite.
 * No visible window, guest, global Hook or desktop mutation is needed. */
#ifndef WM_SYSTIMER
#define WM_SYSTIMER 0x0118
#endif

static UINT ordinary_ticks, system_ticks;
static UINT ansi_calls, wide_calls;
static UINT replacement_calls;
static UINT style_notifications;
static UINT color_calls;
static UINT set_text_messages;
static HWND color_receiver, color_child;
static HDC color_dc;
static HBRUSH color_brush;
static POINT color_origin;
static int failed;

#define CHECK(label, expression) do { \
    int ok = !!(expression); \
    printf("%s=%d\n", label, ok); \
    if (!ok) ++failed; \
} while (0)

static LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    if (message == WM_SETTEXT) ++set_text_messages;
    if (message == WM_CTLCOLOREDIT || message == WM_CTLCOLORSTATIC ||
            message == WM_CTLCOLORLISTBOX) {
        ++color_calls;
        color_receiver = window;
        color_child = (HWND)lp;
        color_dc = (HDC)wp;
        GetBrushOrgEx(color_dc, &color_origin);
        return (LRESULT)color_brush;
    }
    if (message == WM_TIMER && wp == 73) { ++ordinary_ticks; return 0; }
    if (message == WM_SYSTIMER && wp == 73) { ++system_ticks; return 0; }
    if (message == WM_STYLECHANGING || message == WM_STYLECHANGED)
        ++style_notifications;
    return DefWindowProcA(window, message, wp, lp);
}

static LRESULT CALLBACK ansi_target(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    (void)window; (void)wp;
    if (message == WM_APP + 17)
        return wp == 73 && lp == 91 ? 202 : 0;
    if (message == WM_SETTEXT) {
        ++ansi_calls;
        return lp && !strcmp((LPCSTR)lp, "edit conversion") ? 102 : 0;
    }
    return 0;
}

static LRESULT CALLBACK wide_target(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    (void)window; (void)wp;
    if (message == WM_APP + 17)
        return wp == 73 && lp == 91 ? 201 : 0;
    if (message == WM_SETTEXT) {
        ++wide_calls;
        return lp && !wcscmp((LPCWSTR)lp, L"edit conversion") ? 101 : 0;
    }
    return 0;
}

static LRESULT CALLBACK replacement_target(HWND window, UINT message,
    WPARAM wp, LPARAM lp)
{
    ++replacement_calls;
    return DefWindowProcA(window, message, wp, lp);
}

static void check_stored_text(HWND window)
{
    LARGE_STRING text = {0};
    WCHAR wide[] = {L'A', 0x4e2d, L'X', 0};
    CHAR narrow[] = "ABX";
    WCHAR result[8];
    UINT before = set_text_messages;
    text.Length = 4;
    text.MaximumLength = sizeof(wide);
    text.Buffer = wide;
    CHECK("stored_text_unicode", wow_edit_native_NtUserDefSetText(window, &text));
    CHECK("stored_text_unicode_count", GetWindowTextW(window, result, 8) == 2 &&
        result[0] == L'A' && result[1] == 0x4e2d && !result[2]);
    text.Length = 2;
    text.MaximumLength = sizeof(narrow);
    text.bAnsi = TRUE;
    text.Buffer = narrow;
    CHECK("stored_text_ansi", wow_edit_native_NtUserDefSetText(window, &text));
    CHECK("stored_text_ansi_count", GetWindowTextW(window, result, 8) == 2 &&
        !wcscmp(result, L"AB"));
    CHECK("stored_text_null", wow_edit_native_NtUserDefSetText(window, NULL));
    CHECK("stored_text_empty", GetWindowTextW(window, result, 8) == 0 && !result[0]);
    CHECK("stored_text_no_message", set_text_messages == before);
    CHECK("stored_text_invalid_window", !wow_edit_native_NtUserDefSetText(NULL, &text));
}

static void check_saved_cpd(HWND peer, HINSTANCE instance, LPCSTR class_name)
{
    HWND source = CreateWindowExA(0, class_name, "", 0,
        0, 0, 8, 8, NULL, NULL, instance, NULL);
    DWORD to_wide, to_ansi;
    LONG previous;
    UINT before;
    CHECK("cpd_source_window", source != NULL);
    if (!source) return;
    to_wide = wow_edit_native_NtUserGetCPD(source, 0x21, (DWORD)wide_target);
    to_ansi = wow_edit_native_NtUserGetCPD(source, 0x22, (DWORD)ansi_target);
    CHECK("saved_cpd_pair", to_wide && to_ansi);
    /* Original class-owned CPDs keep their destination, not the current
     * HWND procedure. Both public entry points describe INPUT encoding;
     * the token describes TARGET encoding (clmsg.c / kernel/server.c). */
    SetLastError(0);
    previous = SetWindowLongA(source, GWL_WNDPROC, (LONG)replacement_target);
    CHECK("cpd_replace_current", previous != 0);
    if (previous && to_wide && to_ansi) {
        before = replacement_calls;
        CHECK("saved_wide_from_a", CallWindowProcA((WNDPROC)to_wide,
            source, WM_SETTEXT, 0, (LPARAM)"edit conversion") == 101);
        CHECK("saved_wide_from_w", CallWindowProcW((WNDPROC)to_wide,
            source, WM_SETTEXT, 0, (LPARAM)L"edit conversion") == 101);
        CHECK("saved_ansi_from_a", CallWindowProcA((WNDPROC)to_ansi,
            source, WM_SETTEXT, 0, (LPARAM)"edit conversion") == 102);
        CHECK("saved_ansi_from_w", CallWindowProcW((WNDPROC)to_ansi,
            source, WM_SETTEXT, 0, (LPARAM)L"edit conversion") == 102);
        CHECK("saved_cpd_not_current", replacement_calls == before);
        CHECK("cpd_restore_current", SetWindowLongA(source, GWL_WNDPROC,
            previous) == (LONG)replacement_target);
    }
    CHECK("cpd_source_destroy", DestroyWindow(source));
    /* Keep peer and its class alive. Never invoke a stale HWND or test a
     * token after class destruction. Non-string messages pass unchanged. */
    if (to_wide && to_ansi) {
        CHECK("class_cpd_survives_source_w", CallWindowProcA((WNDPROC)to_wide,
            peer, WM_SETTEXT, 0, (LPARAM)"edit conversion") == 101);
        CHECK("class_cpd_survives_source_a", CallWindowProcW((WNDPROC)to_ansi,
            peer, WM_SETTEXT, 0, (LPARAM)L"edit conversion") == 102);
        CHECK("cpd_opaque_payload_w", CallWindowProcW((WNDPROC)to_wide,
            peer, WM_APP + 17, 73, 91) == 201);
        CHECK("cpd_opaque_payload_a", CallWindowProcA((WNDPROC)to_ansi,
            peer, WM_APP + 17, 73, 91) == 202);
    }
}

static void pump(DWORD duration)
{
    ULONGLONG end = GetTickCount64() + duration;
    MSG message;
    do {
        while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE))
            DispatchMessageA(&message);
        if (GetTickCount64() >= end) break;
        MsgWaitForMultipleObjectsEx(0, NULL, 10, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
    } while (TRUE);
}

static INT_PTR CALLBACK ansi_dialog(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    (void)window; (void)message; (void)wp; (void)lp;
    return FALSE;
}

static INT_PTR CALLBACK wide_dialog(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    (void)window; (void)message; (void)wp; (void)lp;
    return FALSE;
}

static LRESULT CALLBACK wide_class_target(HWND window, UINT message,
    WPARAM wp, LPARAM lp)
{
    if (message == WM_SETTEXT || message == WM_APP + 17)
        return wide_target(window, message, wp, lp);
    return DefWindowProcW(window, message, wp, lp);
}

static void check_setter_cpd_encoding(HINSTANCE instance)
{
    WNDCLASSW cls = {0};
    ATOM atom;
    HWND window;
    LONG saved_window;
    DWORD saved_class;
    cls.hInstance = instance;
    cls.lpfnWndProc = wide_class_target;
    cls.lpszClassName = L"WOW.Edit.NativeSetterCPD";
    atom = RegisterClassW(&cls);
    CHECK("cpd_setter_class", atom != 0);
    if (!atom) return;
    window = CreateWindowExW(0, MAKEINTRESOURCEW(atom), L"", 0,
        0, 0, 8, 8, NULL, NULL, instance, NULL);
    CHECK("cpd_setter_window", window != NULL);
    if (!window) goto cleanup;

    saved_window = SetWindowLongA(window, GWL_WNDPROC,
        (LONG)replacement_target);
    CHECK("cpd_window_setter_saved", saved_window != 0 &&
        saved_window != (LONG)wide_class_target);
    CHECK("cpd_window_setter_ansi", !IsWindowUnicode(window));
    if (saved_window) {
        CHECK("cpd_window_setter_restore", SetWindowLongA(window,
            GWL_WNDPROC, saved_window) == (LONG)replacement_target);
        CHECK("cpd_window_restores_target_encoding", IsWindowUnicode(window) &&
            GetWindowLongW(window, GWL_WNDPROC) == (LONG)wide_class_target);
    }

    saved_class = SetClassLongA(window, GCL_WNDPROC,
        (LONG)replacement_target);
    CHECK("cpd_class_setter_saved", saved_class != 0 &&
        saved_class != (DWORD)wide_class_target);
    if (saved_class) {
        CHECK("cpd_class_setter_restore", SetClassLongA(window,
            GCL_WNDPROC, (LONG)saved_class) == (DWORD)replacement_target);
        /* Unlike window/dialog setters, original xxxSetClassData unwraps
         * the target but takes encoding from THIS setter's bAnsi argument.
         * Never run string messages through this intentionally A-labelled
         * wide callback; inspect the public procedure views instead. */
        CHECK("cpd_class_retains_setter_encoding",
            GetClassLongA(window, GCL_WNDPROC) == (DWORD)wide_class_target &&
            GetClassLongW(window, GCL_WNDPROC) != (DWORD)wide_class_target);
        CHECK("cpd_class_does_not_relabel_window", IsWindowUnicode(window));
    }
    (void)SetClassLongW(window, GCL_WNDPROC, (LONG)wide_class_target);
    (void)SetWindowLongW(window, GWL_WNDPROC, (LONG)wide_class_target);
    CHECK("cpd_setter_window_destroy", DestroyWindow(window));
cleanup:
    CHECK("cpd_setter_class_unregister",
        UnregisterClassW(MAKEINTRESOURCEW(atom), instance));
}

static void check_class_cpd_dialog_restore(HINSTANCE instance)
{
    WNDCLASSEXW cls = {0}, native_wide = {0};
    WNDCLASSEXA native_ansi = {0};
    struct {
        DLGTEMPLATE dialog;
        WORD menu, class_name, title;
    } layout = {0};
    ATOM atom;
    HWND source = NULL, dialog = NULL;
    LONG from_info, from_window;
    cls.cbSize = sizeof(cls);
    cls.hInstance = instance;
    cls.lpfnWndProc = wide_class_target;
    cls.lpszClassName = L"WOW.Edit.NativeClassCPD";
    atom = RegisterClassExW(&cls);
    CHECK("cpd_class_register_wide", atom != 0);
    if (!atom) return;
    native_ansi.cbSize = sizeof(native_ansi);
    native_wide.cbSize = sizeof(native_wide);
    CHECK("cpd_class_info_ansi", GetClassInfoExA(instance,
        MAKEINTRESOURCEA(atom), &native_ansi));
    CHECK("cpd_class_info_wide", GetClassInfoExW(instance,
        MAKEINTRESOURCEW(atom), &native_wide));
    from_info = (LONG)native_ansi.lpfnWndProc;
    CHECK("cpd_class_info_actual", native_wide.lpfnWndProc == wide_class_target);
    CHECK("cpd_class_info_conversion", from_info != 0 &&
        from_info != (LONG)wide_class_target);
    source = CreateWindowExW(0, MAKEINTRESOURCEW(atom), L"", 0,
        0, 0, 8, 8, NULL, NULL, instance, NULL);
    CHECK("cpd_class_source", source != NULL);
    if (!source) goto cleanup;
    from_window = GetClassLongA(source, GCL_WNDPROC);
    CHECK("cpd_class_window_conversion", from_window != 0 &&
        from_window != (LONG)wide_class_target);
    CHECK("cpd_class_window_actual", GetClassLongW(source, GCL_WNDPROC) ==
        (DWORD)wide_class_target);
    /* CPD_CLASS and CPD_WNDTOCLS are different keys in original GetCPD.
     * Do not assert that these two native tokens have equal identities. */
    layout.dialog.style = WS_POPUP;
    layout.dialog.cx = layout.dialog.cy = 8;
    dialog = CreateDialogIndirectParamA(instance, &layout.dialog, NULL,
        ansi_dialog, 0);
    CHECK("cpd_class_dialog", dialog != NULL);
    if (!dialog) goto cleanup;
    if (from_info) {
        CHECK("cpd_class_info_restore", SetWindowLongA(dialog, DWL_DLGPROC,
            from_info) == (LONG)ansi_dialog);
        CHECK("cpd_class_info_restored_actual", GetWindowLongW(dialog,
            DWL_DLGPROC) == (LONG)wide_class_target);
        CHECK("cpd_class_info_saved_call", CallWindowProcA((WNDPROC)from_info,
            source, WM_SETTEXT, 0, (LPARAM)"edit conversion") == 101);
    }
    (void)SetWindowLongA(dialog, DWL_DLGPROC, (LONG)ansi_dialog);
    if (from_window) {
        CHECK("cpd_class_window_restore", SetWindowLongA(dialog, DWL_DLGPROC,
            from_window) == (LONG)ansi_dialog);
        CHECK("cpd_class_window_restored_actual", GetWindowLongW(dialog,
            DWL_DLGPROC) == (LONG)wide_class_target);
    }
    /* No native callback from destruction should depend on a test window
     * procedure being used as a DLGPROC. Restore the real dialog callback. */
    (void)SetWindowLongA(dialog, DWL_DLGPROC, (LONG)ansi_dialog);
cleanup:
    if (dialog) CHECK("cpd_class_dialog_destroy", DestroyWindow(dialog));
    if (source) CHECK("cpd_class_source_destroy", DestroyWindow(source));
    CHECK("cpd_class_unregister", UnregisterClassW(MAKEINTRESOURCEW(atom), instance));
}

static void check_dialog_cpd_restore(HINSTANCE instance)
{
    /* Independently authored empty, invisible native dialog. This witnesses
     * permanent procedure replacement on its real owner, not a scratch
     * decoder to be used by production and not a guest template. */
    struct {
        DLGTEMPLATE dialog;
        WORD menu, class_name, title;
    } layout = {0};
    HWND window;
    LONG saved_wide, saved_ansi;
    layout.dialog.style = WS_POPUP;
    layout.dialog.cx = layout.dialog.cy = 8;
    window = CreateDialogIndirectParamW(instance, &layout.dialog, NULL,
        wide_dialog, 0);
    CHECK("cpd_dialog_create", window != NULL);
    if (!window) return;
    saved_wide = GetWindowLongA(window, DWL_DLGPROC);
    CHECK("cpd_dialog_wide_saved", saved_wide != 0 &&
        saved_wide != (LONG)wide_dialog);
    CHECK("cpd_dialog_install_ansi", SetWindowLongA(window, DWL_DLGPROC,
        (LONG)ansi_dialog) == saved_wide);
    CHECK("cpd_dialog_ansi_notify", DefDlgProcA(window, WM_NOTIFYFORMAT,
        (WPARAM)window, NF_QUERY) == NFR_ANSI);
    CHECK("cpd_dialog_restore_wide", SetWindowLongA(window, DWL_DLGPROC,
        saved_wide) == (LONG)ansi_dialog);
    CHECK("cpd_dialog_wide_actual", GetWindowLongW(window, DWL_DLGPROC) ==
        (LONG)wide_dialog);
    CHECK("cpd_dialog_wide_notify", DefDlgProcW(window, WM_NOTIFYFORMAT,
        (WPARAM)window, NF_QUERY) == NFR_UNICODE);
    CHECK("cpd_dialog_install_ansi_again", SetWindowLongA(window, DWL_DLGPROC,
        (LONG)ansi_dialog) == saved_wide);
    saved_ansi = GetWindowLongW(window, DWL_DLGPROC);
    CHECK("cpd_dialog_ansi_saved", saved_ansi != 0 &&
        saved_ansi != (LONG)ansi_dialog);
    CHECK("cpd_dialog_install_wide", SetWindowLongW(window, DWL_DLGPROC,
        (LONG)wide_dialog) == saved_ansi);
    CHECK("cpd_dialog_restore_ansi", SetWindowLongW(window, DWL_DLGPROC,
        saved_ansi) == (LONG)wide_dialog);
    CHECK("cpd_dialog_ansi_actual", GetWindowLongA(window, DWL_DLGPROC) ==
        (LONG)ansi_dialog);
    CHECK("cpd_dialog_restored_ansi_notify", DefDlgProcA(window, WM_NOTIFYFORMAT,
        (WPARAM)window, NF_QUERY) == NFR_ANSI);
    CHECK("cpd_dialog_destroy", DestroyWindow(window));
}

static void check_drawing(void)
{
    HDC dc = CreateCompatibleDC(NULL);
    BITMAPINFO info = {0};
    HBITMAP bitmap, previous_bitmap;
    HFONT font, previous_font;
    HBRUSH red = NULL, blue = NULL;
    void *bits;
    TEXTMETRICW metrics = {0};
    CHWIDTHINFO widths = {0};
    POLYPATBLT rectangles[2] = {0};
    int height = 0;
    CHECK("drawing_dc", dc != NULL);
    if (!dc) return;
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = 8;
    info.bmiHeader.biHeight = -8;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    bitmap = CreateDIBSection(dc, &info, DIB_RGB_COLORS, &bits, NULL, 0);
    CHECK("drawing_bitmap", bitmap != NULL);
    if (!bitmap) { DeleteDC(dc); return; }
    previous_bitmap = (HBITMAP)SelectObject(dc, bitmap);
    font = CreateFontW(-16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH, L"Arial");
    CHECK("outline_font", font != NULL);
    if (font) {
        previous_font = (HFONT)SelectObject(dc, font);
        CHECK("outline_font_available", GetOutlineTextMetricsW(dc, 0, NULL) != 0);
        CHECK("character_dimensions",
            wow_edit_native_GdiGetCharDimensions(dc, &metrics, &height) > 0 &&
            height == metrics.tmHeight && height > 0);
        CHECK("character_widths", wow_edit_native_GetCharWidthInfo(dc, &widths));
        SelectObject(dc, previous_font);
        DeleteObject(font);
    }
    red = CreateSolidBrush(RGB(255, 0, 0));
    blue = CreateSolidBrush(RGB(0, 0, 255));
    CHECK("drawing_brushes", red && blue);
    if (red && blue) {
        rectangles[0].cx = rectangles[0].cy = 8;
        rectangles[0].BrClr.hbr = red;
        rectangles[1].x = rectangles[1].y = 2;
        rectangles[1].cx = rectangles[1].cy = 4;
        rectangles[1].BrClr.hbr = blue;
        CHECK("poly_pat_blt", wow_edit_native_PolyPatBlt(dc, PATCOPY,
            rectangles, 2, PPB_BRUSH));
        CHECK("rectangle_order", GetPixel(dc, 0, 0) == RGB(255, 0, 0) &&
            GetPixel(dc, 3, 3) == RGB(0, 0, 255));
    }
    if (blue) DeleteObject(blue);
    if (red) DeleteObject(red);
    SelectObject(dc, previous_bitmap);
    DeleteObject(bitmap);
    DeleteDC(dc);
}

static void check_window_state(HWND window)
{
    /* Original user.h flag encoding, independently traced through the
     * installed native EDIT callers; no native WND address is used. */
    static const struct { UINT flag; DWORD style; int index; } flags[] = {
        {0x0e80, WS_BORDER, GWL_STYLE}, {0x0e10, WS_HSCROLL, GWL_STYLE},
        {0x0d08, ES_READONLY, GWL_STYLE}, {0x0c20, ES_PASSWORD, GWL_STYLE},
        {0x0d02, CBS_HASSTRINGS, GWL_STYLE},
        {0x0eb0, WS_BORDER | WS_VSCROLL | WS_HSCROLL, GWL_STYLE},
        {0x0903, WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE, GWL_EXSTYLE}
    };
    UINT index, notifications = style_notifications;
    for (index = 0; index < ARRAYSIZE(flags); ++index) {
        DWORD before = (DWORD)GetWindowLongA(window, flags[index].index);
        wow_edit_native_NtUserSetWindowState(window, flags[index].flag);
        CHECK("native_state_set", (DWORD)GetWindowLongA(window, flags[index].index) ==
            (before | flags[index].style));
        wow_edit_native_NtUserClearWindowState(window, flags[index].flag);
        CHECK("native_state_clear", (DWORD)GetWindowLongA(window, flags[index].index) ==
            (before & ~flags[index].style));
        if (before & flags[index].style) {
            UINT byte_index = HIBYTE(flags[index].flag) -
                (flags[index].index == GWL_STYLE ? 12 : 8);
            UINT restore = (flags[index].flag & 0xff00) |
                ((before >> (8 * byte_index)) & LOBYTE(flags[index].flag));
            wow_edit_native_NtUserSetWindowState(window, restore);
        }
        CHECK("native_state_restore", (DWORD)GetWindowLongA(window,
            flags[index].index) == before);
    }
    {
        DWORD before = (DWORD)GetWindowLongA(window, GWL_STYLE);
        wow_edit_native_NtUserAlterWindowStyle(window, WS_VSCROLL | WS_HSCROLL,
            WS_VSCROLL);
        CHECK("list_scroll_seed", (DWORD)GetWindowLongA(window, GWL_STYLE) ==
            ((before & ~(WS_VSCROLL | WS_HSCROLL)) | WS_VSCROLL));
        wow_edit_native_NtUserAlterWindowStyle(window, WS_VSCROLL, 0);
        CHECK("list_scroll_modern", (DWORD)GetWindowLongA(window, GWL_STYLE) ==
            (before & ~(WS_VSCROLL | WS_HSCROLL)));
        wow_edit_native_NtUserAlterWindowStyle(window, WS_VSCROLL | WS_HSCROLL,
            WS_HSCROLL);
        CHECK("list_scroll_original_compat", (DWORD)GetWindowLongA(window, GWL_STYLE) ==
            ((before & ~(WS_VSCROLL | WS_HSCROLL)) | WS_HSCROLL));
        wow_edit_native_NtUserAlterWindowStyle(window, WS_VSCROLL | WS_HSCROLL,
            before);
        CHECK("list_scroll_restore", (DWORD)GetWindowLongA(window, GWL_STYLE) == before);
    }
    CHECK("no_extra_style_notifications", style_notifications == notifications);
}

static void check_fill_window(HWND parent, HINSTANCE instance, LPCSTR class_name)
{
    HWND child = CreateWindowExA(0, class_name, "", WS_CHILD,
        2, 3, 8, 8, parent, NULL, instance, NULL);
    HDC dc = CreateCompatibleDC(NULL);
    BITMAPINFO info = {0};
    HBITMAP bitmap = NULL, old_bitmap = NULL;
    HBRUSH direct = NULL;
    POINT parent_origin = {0}, child_origin = {0}, restored;
    void *bits;
    UINT calls;
    CHECK("fill_child", child != NULL);
    CHECK("fill_dc", dc != NULL);
    if (!child || !dc) goto cleanup;
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = 8;
    info.bmiHeader.biHeight = -8;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    bitmap = CreateDIBSection(dc, &info, DIB_RGB_COLORS, &bits, NULL, 0);
    color_brush = CreateSolidBrush(RGB(255, 0, 0));
    direct = CreateSolidBrush(RGB(0, 0, 255));
    CHECK("fill_resources", bitmap && color_brush && direct);
    if (!bitmap || !color_brush || !direct) goto cleanup;
    old_bitmap = (HBITMAP)SelectObject(dc, bitmap);
    CHECK("fill_seed", PatBlt(dc, 0, 0, 8, 8, WHITENESS));
    CHECK("fill_clip", IntersectClipRect(dc, 1, 1, 7, 7) == SIMPLEREGION);
    CHECK("fill_origin_set", SetBrushOrgEx(dc, 5, 6, NULL));
    ClientToScreen(parent, &parent_origin);
    ClientToScreen(child, &child_origin);
    calls = color_calls;
    /* Original CTLCOLOR_EDIT = 1, CTLCOLOR_STATIC = 6. Small brush
     * values request a callback, not a system-color brush. */
    CHECK("fill_color_dispatch", wow_edit_native_NtUserFillWindow(
        parent, child, dc, (HBRUSH)(ULONG_PTR)1));
    CHECK("fill_color_identity", color_calls == calls + 1 &&
        color_receiver == parent && color_child == child && color_dc == dc);
    CHECK("fill_callback_origin", color_origin.x == parent_origin.x - child_origin.x &&
        color_origin.y == parent_origin.y - child_origin.y);
    CHECK("fill_origin_restored", GetBrushOrgEx(dc, &restored) &&
        restored.x == 5 && restored.y == 6);
    CHECK("fill_pixels", GetPixel(dc, 3, 3) == RGB(255, 0, 0));
    /* GetPixel also obeys the selected clip. Inspect the DIB after flushing
     * instead of mistaking an excluded pixel's CLR_INVALID for its color. */
    GdiFlush();
    CHECK("fill_clip_preserved", (((DWORD *)bits)[0] & 0x00ffffff) == 0x00ffffff &&
        (((DWORD *)bits)[63] & 0x00ffffff) == 0x00ffffff);
    calls = color_calls;
    CHECK("fill_direct_brush", wow_edit_native_NtUserFillWindow(parent, child, dc, direct));
    CHECK("fill_direct_no_callback", color_calls == calls &&
        GetPixel(dc, 3, 3) == RGB(0, 0, 255));
    CHECK("fill_null_brush_window", wow_edit_native_NtUserFillWindow(
        NULL, child, dc, (HBRUSH)(ULONG_PTR)6));
    CHECK("fill_fallback_identity", color_calls == calls + 1 &&
        color_receiver == child && color_child == child &&
        color_origin.x == 0 && color_origin.y == 0);
    CHECK("fill_fallback_origin_restored", GetBrushOrgEx(dc, &restored) &&
        restored.x == 5 && restored.y == 6);
    calls = color_calls;
    CHECK("fill_empty_clip", IntersectClipRect(dc, 0, 0, 0, 0) == NULLREGION);
    CHECK("fill_empty_success", wow_edit_native_NtUserFillWindow(
        parent, child, dc, (HBRUSH)(ULONG_PTR)1));
    CHECK("fill_empty_no_callback", color_calls == calls);
cleanup:
    if (old_bitmap) SelectObject(dc, old_bitmap);
    if (bitmap) CHECK("fill_bitmap_release", DeleteObject(bitmap));
    if (color_brush) CHECK("fill_color_release", DeleteObject(color_brush));
    color_brush = NULL;
    if (direct) CHECK("fill_direct_release", DeleteObject(direct));
    if (dc) CHECK("fill_dc_release", DeleteDC(dc));
    if (child) CHECK("fill_child_release", DestroyWindow(child));
}

static void check_control_color(HWND parent, HINSTANCE instance, LPCSTR class_name)
{
    /* Host reference only. Production uses the retained original recipient
     * algorithm plus its explicit-recipient scoped native color service. */
    HBRUSH (WINAPI *reference_brush)(HWND, HDC, UINT) =
        (HBRUSH (WINAPI *)(HWND, HDC, UINT))GetProcAddress(
            GetModuleHandleW(L"win32u.dll"), "NtUserGetControlBrush");
    HWND child = NULL, popup = NULL, standalone = NULL, dead;
    HDC dc = CreateCompatibleDC(NULL);
    HBRUSH brush = CreateSolidBrush(RGB(23, 47, 71)), saved = color_brush;
    HBRUSH expected, actual;
    HBITMAP wrong_type = CreateBitmap(1, 1, 1, 1, NULL);
    UINT calls;
    child = CreateWindowExA(0, class_name, "", WS_CHILD, 0, 0, 8, 8,
        parent, (HMENU)991, instance, NULL);
    popup = CreateWindowExA(0, class_name, "", WS_POPUP, 0, 0, 8, 8,
        parent, NULL, instance, NULL);
    standalone = CreateWindowExA(0, class_name, "", WS_POPUP, 0, 0, 8, 8,
        NULL, NULL, instance, NULL);
    CHECK("color_resources", dc && brush && wrong_type && child && popup && standalone);
    CHECK("color_reference_export", reference_brush != NULL);
    if (!reference_brush || !dc || !brush || !wrong_type || !child || !popup || !standalone) goto cleanup;
    color_brush = brush;
    calls = color_calls;
    CHECK("color_explicit_result", wow_edit_native_NtUserGetControlColor(
        parent, child, dc, WM_CTLCOLORLISTBOX) == brush);
    CHECK("color_explicit_recipient", color_calls == calls + 1 &&
        color_receiver == parent && color_child == child && color_dc == dc);
    calls = color_calls;
    CHECK("color_child_brush", reference_brush(
        child, dc, WM_CTLCOLORLISTBOX) == brush);
    CHECK("color_child_parent", color_calls == calls + 1 &&
        color_receiver == parent && color_child == child);
    calls = color_calls;
    CHECK("color_popup_brush", reference_brush(
        popup, dc, WM_CTLCOLORLISTBOX) == brush);
    CHECK("color_popup_owner", color_calls == calls + 1 &&
        color_receiver == parent && color_child == popup);
    calls = color_calls;
    CHECK("color_unowned_brush", reference_brush(
        standalone, dc, WM_CTLCOLORLISTBOX) == brush);
    CHECK("color_unowned_self", color_calls == calls + 1 &&
        color_receiver == standalone && color_child == standalone);
    expected = (HBRUSH)DefWindowProcA(parent, WM_CTLCOLORLISTBOX,
        (WPARAM)dc, (LPARAM)child);
    CHECK("color_default_brush", expected && GetObjectType(expected) == OBJ_BRUSH);
    color_brush = NULL;
    calls = color_calls;
    actual = wow_edit_native_NtUserGetControlColor(parent, child, dc, WM_CTLCOLORLISTBOX);
    CHECK("color_null_fallback", actual == expected && color_calls == calls + 1);
    color_brush = (HBRUSH)wrong_type;
    calls = color_calls;
    actual = wow_edit_native_NtUserGetControlColor(parent, child, dc, WM_CTLCOLORLISTBOX);
    CHECK("color_invalid_brush_fallback", actual == expected && color_calls == calls + 1);
    color_brush = brush;
    dead = child;
    CHECK("color_child_destroy", DestroyWindow(child));
    child = NULL;
    calls = color_calls;
    CHECK("color_retired_control", !wow_edit_native_NtUserGetControlColor(
        parent, dead, dc, WM_CTLCOLORLISTBOX));
    CHECK("color_retired_brush", !reference_brush(
        dead, dc, WM_CTLCOLORLISTBOX));
    CHECK("color_retired_no_callback", color_calls == calls);
cleanup:
    color_brush = saved;
    if (child) CHECK("color_cleanup_child", DestroyWindow(child));
    if (popup) CHECK("color_cleanup_popup", DestroyWindow(popup));
    if (standalone) CHECK("color_cleanup_standalone", DestroyWindow(standalone));
    if (wrong_type) CHECK("color_cleanup_bitmap", DeleteObject(wrong_type));
    if (brush) CHECK("color_cleanup_brush", DeleteObject(brush));
    if (dc) CHECK("color_cleanup_dc", DeleteDC(dc));
}

static void check_original_tab_text(HDC dc, wow_edit_context *context)
{
    static const WCHAR *samples[] = {L"abc", L"a\tb", L"\ta\t", L"a\t\tb"};
    int tabs[] = {32, 80};
    wow_edit_context *previous = wow_edit_current;
    unsigned i;
    wow_edit_current = context;
    __try {
        CHECK("tab_null", TabTextOut(dc, 0, 0, NULL, 3, 0, NULL, 0, FALSE) == 0);
        CHECK("tab_empty", TabTextOut(dc, 0, 0, L"a", 0, 0, NULL, 0, FALSE) == 0);
        CHECK("tab_invalid_count", TabTextOut(dc, 0, 0, L"a", 1, -1, NULL, 0, FALSE) == 0);
        for (i = 0; i < ARRAYSIZE(samples); ++i) {
            int count = (int)wcslen(samples[i]);
            LONG extent = TabTextOut(dc, 0, 0, samples[i], count, 0, NULL, 0, FALSE);
            CHECK("tab_default_extent", (DWORD)extent ==
                GetTabbedTextExtentW(dc, samples[i], count, 0, NULL));
            CHECK("tab_default_draw_extent", TabTextOut(dc, 0, 0, samples[i],
                count, 0, NULL, 0, TRUE) == extent);
            extent = TabTextOut(dc, 0, 0, samples[i], count, ARRAYSIZE(tabs), tabs, 0, FALSE);
            CHECK("tab_custom_extent", (DWORD)extent ==
                GetTabbedTextExtentW(dc, samples[i], count, ARRAYSIZE(tabs), tabs));
            CHECK("tab_custom_draw_extent", TabTextOut(dc, 0, 0, samples[i],
                count, ARRAYSIZE(tabs), tabs, 0, TRUE) == extent);
        }
    } __finally {
        wow_edit_current = previous;
    }
}

static void check_edit_context(void)
{
    wow_edit_context context = {0}, *outer = wow_edit_current;
    TEXTMETRICW metrics = {0};
    HDC dc;
    HGDIOBJ previous;
    UINT lines = 0;
    int width, height = 0;
    CHECK("context_null", !wow_edit_capture_context(NULL) &&
        GetLastError() == ERROR_INVALID_PARAMETER);
    SetLastError(0x7319);
    CHECK("context_capture", wow_edit_capture_context(&context));
    CHECK("context_error", GetLastError() == 0x7319);
    dc = CreateICW(L"DISPLAY", NULL, NULL, NULL);
    CHECK("context_display_ic", dc != NULL);
    if (dc) {
        CHECK("context_screen", !context.screen_rect.left && !context.screen_rect.top &&
            context.screen_rect.right == GetDeviceCaps(dc, DESKTOPHORZRES) &&
            context.screen_rect.bottom == GetDeviceCaps(dc, DESKTOPVERTRES));
        CHECK("context_display_depth", context.screen.BitCount > 0 &&
            context.screen.BitCount == (UINT)GetDeviceCaps(dc, PLANES) *
                (UINT)GetDeviceCaps(dc, BITSPIXEL));
        DeleteDC(dc);
    }
    CHECK("context_does_not_publish_tls", wow_edit_current == outer);
    CHECK("context_scroll_interval", context.dtScroll == GetDoubleClickTime() * 4 / 5);
    CHECK("context_search_interval", context.dtLBSearch == GetDoubleClickTime() * 4);
    CHECK("context_timer_relationship", context.dtScroll == context.dtLBSearch / 5);
    CHECK("context_caret_interval", context.dtCaretBlink == GetCaretBlinkTime());
    CHECK("context_wheel_lines", SystemParametersInfoW(SPI_GETWHEELSCROLLLINES,
        0, &lines, 0) && context.ucWheelScrollLines == lines);
    dc = CreateCompatibleDC(NULL);
    CHECK("context_reference_dc", dc != NULL);
    if (!dc) return;
    previous = SelectObject(dc, GetStockObject(SYSTEM_FONT));
    CHECK("context_reference_font", previous && previous != HGDI_ERROR);
    if (previous && previous != HGDI_ERROR) {
        width = wow_edit_native_GdiGetCharDimensions(dc, &metrics, &height);
        CHECK("context_system_font", width > 0 &&
            context.cxSysFontChar == width && context.cySysFontChar == height &&
            !memcmp(&context.tmSysFont, &metrics, sizeof(metrics)));
        check_original_tab_text(dc, &context);
        SelectObject(dc, previous);
    }
    DeleteDC(dc);
}

static DWORD WINAPI connect_thread_fixture(void *unused)
{
    DWORD failures = 0;
    (void)unused;
    SetLastError(0x1234);
    if (!wow_edit_native_ensure_gui_thread()) failures |= 1;
    if (GetLastError() != 0x1234) failures |= 2;
    if (!IsGUIThread(FALSE)) failures |= 4;
    SetLastError(0x5678);
    if (!wow_edit_native_ensure_gui_thread()) failures |= 8;
    if (GetLastError() != 0x5678) failures |= 16;
    return failures;
}

int main(void)
{
    WNDCLASSA cls = {0};
    HWND window;
    DWORD cpd, error;
    UINT ordinary_before;
    {
        DWORD status = 1;
        HANDLE thread = CreateThread(NULL, 0, connect_thread_fixture, NULL, 0, NULL);
        CHECK("gui_thread_create", thread != NULL);
        if (!thread) return 1;
        /* No stack-owned state may escape into an unjoined test thread. */
        if (WaitForSingleObject(thread, 10000) != WAIT_OBJECT_0) ExitProcess(3);
        CHECK("gui_thread_result", GetExitCodeThread(thread, &status) && status == 0);
        CloseHandle(thread);
    }
    SetLastError(0x1234);
    CHECK("native_readiness", wow_edit_native_ready());
    CHECK("readiness_preserves_error", GetLastError() == 0x1234);
    if (failed) return 1;
    check_edit_context();
    check_drawing();
    cls.lpfnWndProc = window_proc;
    cls.hInstance = GetModuleHandleW(NULL);
    cls.lpszClassName = "WOW.Edit.NativeBoundary";
    CHECK("register_window", RegisterClassA(&cls) != 0);
    window = CreateWindowExA(0, cls.lpszClassName, "", 0,
        0, 0, 8, 8, NULL, NULL, cls.hInstance, NULL);
    CHECK("create_window", window != NULL);
    if (!window) return 1;
    check_window_state(window);
    check_fill_window(window, cls.hInstance, cls.lpszClassName);
    check_control_color(window, cls.hInstance, cls.lpszClassName);

    /* NT4 user.h CPD_WND (0x20) plus A_TO_W (1) or W_TO_A (2),
     * independently matched to the installed x86 USER caller in the ledger. */
    cpd = wow_edit_native_NtUserGetCPD(window, 0x21, (DWORD)wide_target);
    CHECK("ansi_to_wide_cpd", cpd != 0);
    if (cpd)
        CHECK("ansi_to_wide_dispatch", CallWindowProcA((WNDPROC)cpd, window,
            WM_SETTEXT, 0, (LPARAM)"edit conversion") == 101 && wide_calls == 1);
    cpd = wow_edit_native_NtUserGetCPD(window, 0x22, (DWORD)ansi_target);
    CHECK("wide_to_ansi_cpd", cpd != 0);
    if (cpd)
        CHECK("wide_to_ansi_dispatch", CallWindowProcW((WNDPROC)cpd, window,
            WM_SETTEXT, 0, (LPARAM)L"edit conversion") == 102 && ansi_calls == 1);
    check_saved_cpd(window, cls.hInstance, cls.lpszClassName);
    check_stored_text(window);
    check_dialog_cpd_restore(cls.hInstance);
    check_class_cpd_dialog_restore(cls.hInstance);
    check_setter_cpd_encoding(cls.hInstance);

    SetLastError(0);
    CHECK("reject_timer_callback", !wow_edit_native_NtUserSetSystemTimer(
        window, 73, 10, window_proc));
    error = GetLastError();
    CHECK("timer_callback_error", error == ERROR_NOT_SUPPORTED);
    CHECK("ordinary_timer", SetTimer(window, 73, 10, NULL) == 73);
    CHECK("system_timer", wow_edit_native_NtUserSetSystemTimer(window, 73, 10, NULL) == 73);
    pump(300);
    CHECK("distinct_timer_delivery", ordinary_ticks > 0 && system_ticks > 0);
    CHECK("kill_system_timer", wow_edit_native_NtUserKillSystemTimer(window, 73));
    pump(50); /* Drain an already queued notification; cancellation is not recall. */
    system_ticks = 0;
    ordinary_before = ordinary_ticks;
    pump(150);
    CHECK("system_cancel_independent", !system_ticks && ordinary_ticks > ordinary_before);
    CHECK("kill_ordinary_timer", KillTimer(window, 73));
    CHECK("destroy_window", DestroyWindow(window));
    CHECK("unregister_window", UnregisterClassA(cls.lpszClassName, cls.hInstance));
    if (!failed) puts("WOW_EDIT_NATIVE_BOUNDARY_OK");
    return failed ? 1 : 0;
}
