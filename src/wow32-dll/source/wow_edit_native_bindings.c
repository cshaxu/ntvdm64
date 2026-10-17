#include "wow_edit_native_bindings.h"
#include "wow_native_text_descriptor.h"
#include <stddef.h>

BOOL WINAPI wow_edit_native_ensure_gui_thread(void)
{
    DWORD saved = GetLastError(), error;
    BOOL converted;
    /* Original ConnectIfNecessary establishes the native USER thread, not
     * a WOW task or our metadata domain. Use its public host owner rather
     * than NT4 TEB offsets or NtUserGetThreadState's private -1 operation. */
    if (IsGUIThread(FALSE)) { SetLastError(saved); return TRUE; }
    SetLastError(ERROR_SUCCESS);
    converted = IsGUIThread(TRUE);
    error = GetLastError();
    /* The documented conversion failure can be the nonzero error value 8.
     * Query actual GUI-thread state instead of treating any nonzero result
     * as success. Do not keep another cached connection flag. */
    if (!IsGUIThread(FALSE)) {
        SetLastError(converted == ERROR_NOT_ENOUGH_MEMORY ?
            ERROR_NOT_ENOUGH_MEMORY : (error ? error : ERROR_NOT_READY));
        return FALSE;
    }
    SetLastError(saved);
    return TRUE;
}

PVOID WINAPI wow_edit_native_highest_user_address(void)
{
    DWORD error = GetLastError();
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    SetLastError(error);
    return info.lpMaximumApplicationAddress;
}

/* ADAPTER-WOW-057: the original editor remains the algorithm owner. These
 * private services are owned by the installed USER/GDI, not reimplemented
 * from kernel source. Exact source/PE ABI evidence and the grouped acceptance
 * contract are recorded in m0-t447-s3-control-source-audit.md.
 *
 * The staged build selects these leaves; complete original creation/storage/
 * dispatch/retirement and grouped acceptance are still required for delivery. */
typedef int (WINAPI *char_dimensions_fn)(HDC, TEXTMETRICW *, LPINT);
typedef BOOL (WINAPI *char_width_fn)(HDC, PCHWIDTHINFO);
typedef BOOL (WINAPI *poly_pat_blt_fn)(HDC, DWORD, PPOLYPATBLT, DWORD, DWORD);
/* Current x86 NtUserSetSystemTimer has THREE arguments, unlike NT4's four.
 * Never cast the installed entry to the original four-argument declaration. */
typedef UINT (WINAPI *system_timer_fn)(HWND, UINT, DWORD);
typedef BOOL (WINAPI *kill_system_timer_fn)(HWND, UINT);
typedef DWORD (WINAPI *get_cpd_fn)(HWND, DWORD, DWORD);
typedef VOID (WINAPI *window_state_fn)(HWND, UINT);
typedef VOID (WINAPI *alter_window_style_fn)(HWND, DWORD, DWORD);
typedef BOOL (WINAPI *fill_window_fn)(HWND, HWND, HDC, HBRUSH);
typedef HBRUSH (WINAPI *control_color_fn)(HWND, HWND, HDC, UINT);
typedef BOOL (WINAPI *def_set_text_fn)(HWND, const wow_native_text_descriptor *);

typedef struct edit_native_exports {
    char_dimensions_fn char_dimensions;
    char_width_fn char_width;
    poly_pat_blt_fn poly_pat_blt;
    system_timer_fn system_timer;
    kill_system_timer_fn kill_system_timer;
    get_cpd_fn get_cpd;
    window_state_fn set_window_state;
    window_state_fn clear_window_state;
    alter_window_style_fn alter_window_style;
    fill_window_fn fill_window;
    control_color_fn control_color;
    def_set_text_fn def_set_text;
} edit_native_exports;

C_ASSERT(sizeof(void *) == 4);
C_ASSERT(sizeof(CHWIDTHINFO) == 12);
C_ASSERT(sizeof(POLYPATBLT) == 20);
C_ASSERT(offsetof(POLYPATBLT, BrClr) == 16);

static INIT_ONCE exports_once = INIT_ONCE_STATIC_INIT;
static edit_native_exports exports;

static BOOL CALLBACK resolve_exports(PINIT_ONCE once, PVOID parameter,
    PVOID *context)
{
    edit_native_exports candidate = {0};
    HMODULE gdi = GetModuleHandleW(L"gdi32.dll");
    HMODULE user = GetModuleHandleW(L"win32u.dll");
    (void)once; (void)parameter; (void)context;
    /* WOW32 imports GDI/USER. Borrow their loaded system dependencies;
     * never search CWD/PATH or load an alternate DLL on export failure. */
    if (!gdi || !user) {
        SetLastError(ERROR_MOD_NOT_FOUND);
        return FALSE;
    }
    candidate.char_dimensions = (char_dimensions_fn)GetProcAddress(gdi,
        "GdiGetCharDimensions");
    candidate.char_width = (char_width_fn)GetProcAddress(gdi,
        "GetCharWidthInfo");
    candidate.poly_pat_blt = (poly_pat_blt_fn)GetProcAddress(gdi, "PolyPatBlt");
    candidate.system_timer = (system_timer_fn)GetProcAddress(user,
        "NtUserSetSystemTimer");
    candidate.kill_system_timer = (kill_system_timer_fn)GetProcAddress(user,
        "NtUserKillSystemTimer");
    candidate.get_cpd = (get_cpd_fn)GetProcAddress(user, "NtUserGetCPD");
    candidate.set_window_state = (window_state_fn)GetProcAddress(user,
        "NtUserSetWindowState");
    candidate.clear_window_state = (window_state_fn)GetProcAddress(user,
        "NtUserClearWindowState");
    candidate.alter_window_style = (alter_window_style_fn)GetProcAddress(user,
        "NtUserAlterWindowStyle");
    candidate.fill_window = (fill_window_fn)GetProcAddress(user, "NtUserFillWindow");
    candidate.control_color = (control_color_fn)GetProcAddress(user, "NtUserGetControlColor");
    candidate.def_set_text = (def_set_text_fn)GetProcAddress(user, "NtUserDefSetText");
    if (!candidate.char_dimensions || !candidate.char_width ||
            !candidate.poly_pat_blt || !candidate.system_timer ||
            !candidate.kill_system_timer || !candidate.get_cpd ||
            !candidate.set_window_state || !candidate.clear_window_state ||
            !candidate.alter_window_style ||
            !candidate.fill_window ||
            !candidate.control_color || !candidate.def_set_text) {
        SetLastError(ERROR_PROC_NOT_FOUND);
        return FALSE;
    }
    exports = candidate;
    return TRUE;
}

BOOL WINAPI wow_edit_native_NtUserDefSetText(HWND window, const LARGE_STRING *text)
{
    wow_native_text_descriptor native_text;
    if (!wow_edit_native_ready()) return FALSE;
    if (text) wow_native_text_capture(&native_text, text);
    return exports.def_set_text(window, text ? &native_text : NULL);
}

BOOL WINAPI wow_edit_native_ready(void)
{
    DWORD error = GetLastError();
    if (!InitOnceExecuteOnce(&exports_once, resolve_exports, NULL, NULL))
        return FALSE;
    SetLastError(error);
    return TRUE;
}

int WINAPI wow_edit_native_GdiGetCharDimensions(HDC dc, TEXTMETRICW *metrics,
    LPINT height)
{
    if (!wow_edit_native_ready()) return 0;
    return exports.char_dimensions(dc, metrics, height);
}

BOOL WINAPI wow_edit_native_GetCharWidthInfo(HDC dc, PCHWIDTHINFO widths)
{
    if (!wow_edit_native_ready()) return FALSE;
    return exports.char_width(dc, widths);
}

BOOL WINAPI wow_edit_native_PolyPatBlt(HDC dc, DWORD operation,
    PPOLYPATBLT rectangles, DWORD count, DWORD mode)
{
    if (!wow_edit_native_ready()) return FALSE;
    return exports.poly_pat_blt(dc, operation, rectangles, count, mode);
}

UINT WINAPI wow_edit_native_NtUserSetSystemTimer(HWND window, UINT id,
    DWORD interval, WNDPROC callback)
{
    if (callback) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }
    if (!wow_edit_native_ready()) return 0;
    return exports.system_timer(window, id, interval);
}

BOOL WINAPI wow_edit_native_NtUserKillSystemTimer(HWND window, UINT id)
{
    if (!wow_edit_native_ready()) return FALSE;
    return exports.kill_system_timer(window, id);
}

DWORD WINAPI wow_edit_native_NtUserGetCPD(HWND window, DWORD options,
    DWORD procedure)
{
    if (!wow_edit_native_ready()) return 0;
    return exports.get_cpd(window, options, procedure);
}

VOID WINAPI wow_edit_native_NtUserSetWindowState(HWND window, UINT flags)
{
    if (!wow_edit_native_ready()) return;
    exports.set_window_state(window, flags);
}

VOID WINAPI wow_edit_native_NtUserClearWindowState(HWND window, UINT flags)
{
    if (!wow_edit_native_ready()) return;
    exports.clear_window_state(window, flags);
}

VOID WINAPI wow_edit_native_NtUserAlterWindowStyle(HWND window, DWORD mask,
    DWORD flags)
{
    if (!wow_edit_native_ready()) return;
    exports.alter_window_style(window, mask, flags);
}

BOOL WINAPI wow_edit_native_NtUserFillWindow(HWND brush_window, HWND paint_window,
    HDC dc, HBRUSH brush)
{
    if (!wow_edit_native_ready()) return FALSE;
    return exports.fill_window(brush_window, paint_window, dc, brush);
}

/* ADAPTER-WOW-068: native brush validation/fallback; original code selects
 * the explicit recipient. The caller owns WOW callout permission. */
HBRUSH WINAPI wow_edit_native_NtUserGetControlColor(HWND parent, HWND control,
    HDC dc, UINT message)
{
    if (!wow_edit_native_ready()) return NULL;
    return exports.control_color(parent, control, dc, message);
}
