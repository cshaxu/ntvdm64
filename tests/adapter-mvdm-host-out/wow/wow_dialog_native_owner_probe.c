#include <windows.h>
#include <stdio.h>
#include "wow_dialog_procedure_bindings.h"
#include "wow_user_task_lifecycle.h"
#include "wow_window_words_binding.h"
#include "dialog-getset/precomp.h"

/* Real dialog contract research for the approved S3 owner split. These are
 * test subjects, not decoder/helper windows in the product. No private USER
 * structure, token inspection, desktop switch or guest is used. */
static unsigned failures, callbacks, terminal_callbacks;
static BOOL suppress_terminal;
static unsigned native_depth;
static DIALOG recovered;
static DLG recovered_data;
#define CHECK(c) do { if (!(c)) { ++failures; printf("FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)
/* This slot/encoding fixture invokes terminal NCDESTROY only. Real retained
 * resource-owner dispatch is tested by the registered CCPU lifecycle. */
BOOL WINAPI wow_window_words_consume_resource_destroy(HWND w,
    wow_window_resource_destroy expected,BOOL invoke)
{(void)w;(void)expected;(void)invoke;CHECK(FALSE);return FALSE;}

/* Original get/set bodies use a retained-view double; actual HWND slots,
 * opaque tokens and native A/W conversion are real. No GetWindowData or
 * original CPD decoder is linked into this selected nonnegative contract. */
PWND WINAPI wow_window_client_validate_hwnd(HWND w)
{
    if (w == recovered.wnd.head.h && IsWindow(w)) return &recovered.wnd;
    SetLastError(ERROR_INVALID_WINDOW_HANDLE); return NULL;
}
BOOL fixture_same_process(PWND w) { return w == &recovered.wnd; }
BOOL fixture_caller_ok(PWND w) { return w == &recovered.wnd; }
void fixture_check(BOOL condition, int line)
{ if (!condition) { ++failures; printf("FAIL original line=%d\n", line); } }
DWORD fixture_control_size(WORD fnid)
{ (void)fnid; CHECK(FALSE); return 0; }
DWORD WINAPI xxxSetWindowData(PWND w, int index, DWORD value, BOOL ansi)
{ (void)w; (void)index; (void)value; (void)ansi; CHECK(FALSE); return 0; }
LONG WINAPI wow_window_client_set_window_long(HWND w, int index, LONG value, BOOL ansi)
{ PWND state = wow_window_client_validate_hwnd(w); return state ? (LONG)xxxSetWindowLong(state, index, value, ansi) : 0; }
static LONG_PTR read_a(HWND w, int index)
{ PWND state = wow_window_client_validate_hwnd(w); return state ? _GetWindowLong(state, index, TRUE) : 0; }
static LONG_PTR read_w(HWND w, int index)
{ PWND state = wow_window_client_validate_hwnd(w); return state ? _GetWindowLong(state, index, FALSE) : 0; }
static LONG_PTR write_a(HWND w, int index, LONG_PTR value)
{ return _SetWindowLong(w, index, (LONG)value, TRUE); }
static LONG_PTR write_w(HWND w, int index, LONG_PTR value)
{ return _SetWindowLong(w, index, (LONG)value, FALSE); }
#undef GetWindowLongPtrA
#undef GetWindowLongPtrW
#undef SetWindowLongPtrA
#undef SetWindowLongPtrW
#define GetWindowLongPtrA read_a
#define GetWindowLongPtrW read_w
#define SetWindowLongPtrA write_a
#define SetWindowLongPtrW write_w

/* Explicit scheduler/publication doubles: real native slots and the actual
 * production procedure adapter, but not a proof of WOW execution restoration. */
BOOL WINAPI wow_user_native_call_begin(wow_user_native_call *call, HWND w)
{ ZeroMemory(call, sizeof(*call)); CHECK(IsWindow(w)); ++native_depth; return TRUE; }
BOOL WINAPI wow_user_native_call_end(wow_user_native_call *call)
{ (void)call; CHECK(native_depth != 0); --native_depth; return TRUE; }
void WINAPI wow_user_native_call_result(wow_user_native_call *c, UINT m, WPARAM a, LPARAM b, LRESULT r)
{ (void)c; (void)m; (void)a; (void)b; (void)r; }
__declspec(noreturn) void WINAPI wow_user_native_call_failed(const char *reason, DWORD error)
{ printf("FATAL %s %lu\n", reason, error); ExitProcess(2); }
LRESULT WINAPI wow_user_call_window_procA(WNDPROC p, HWND w, UINT m, WPARAM a, LPARAM b)
{ return CallWindowProcA(p, w, m, a, b); }
LRESULT WINAPI wow_user_call_window_procW(WNDPROC p, HWND w, UINT m, WPARAM a, LPARAM b)
{ return CallWindowProcW(p, w, m, a, b); }

static INT_PTR CALLBACK dialog_a(HWND w, UINT m, WPARAM a, LPARAM b)
{
    ++callbacks;
    if (m == WM_NCDESTROY) ++terminal_callbacks;
    if (m == WM_GETTEXT) { lstrcpynA((LPSTR)b, "alpha", (int)a); return 5; }
    if (m == WM_APP) { SetWindowLongPtrW(w, DWLP_MSGRESULT, 0x3141); return TRUE; }
    if (m == WM_APP + 1) { DestroyWindow(w); return 17; }
    return FALSE;
}
static INT_PTR CALLBACK dialog_w(HWND w, UINT m, WPARAM a, LPARAM b)
{
    ++callbacks;
    if (m == WM_NCDESTROY) ++terminal_callbacks;
    if (m == WM_GETTEXT) { lstrcpynW((LPWSTR)b, L"omega", (int)a); return 5; }
    if (m == WM_APP) { SetWindowLongPtrW(w, DWLP_MSGRESULT, 0x3141); return TRUE; }
    if (m == WM_APP + 1) { DestroyWindow(w); return 17; }
    return FALSE;
}
static LRESULT CALLBACK subject(HWND w, UINT m, WPARAM a, LPARAM b)
{
    WNDPROC proc;
    if (m == WM_CREATE) {
        ZeroMemory(&recovered, sizeof(recovered));
        ZeroMemory(&recovered_data, sizeof(recovered_data));
        recovered.wnd.head.h = w;
        recovered.wnd.fnid = FNID_DIALOG;
        recovered.wnd.cbwndExtra = DLGWINDOWEXTRA;
        recovered.pdlg = &recovered_data;
        SetWF(&recovered.wnd, WFDIALOGWINDOW);
        /* First promotion: no application procedure has yet been installed. */
        CHECK(GetWindowLongPtrW(w, DWLP_DLGPROC) == 0);
        CHECK(wow_dialog_host_initialize(w));
        CHECK(GetWindowLongPtrW(w, DWLP_DLGPROC) == 0);
        return 0;
    }
    if (m == WM_NOTIFYFORMAT) return wow_dialog_host_notify(w, m, a, b, FALSE);
    if (m == WM_NCDESTROY) {
        if (suppress_terminal) return wow_dialog_host_terminal_proc(w, m, a, b);
        proc = (WNDPROC)GetWindowLongPtrW(w, DWLP_DLGPROC);
        if (proc && !suppress_terminal) CallWindowProcW(proc, w, m, a, b);
        /* Native cleanup must not call the application a second time. */
        wow_dialog_host_retire(w);
        return 0;
    }
    if (m >= WM_APP) {
        proc = (WNDPROC)GetWindowLongPtrW(w, DWLP_DLGPROC);
        SetWindowLongPtrW(w, DWLP_MSGRESULT, 0);
        if (proc) {
            LRESULT r = wow_dialog_host_call(w, m, a, b, FALSE);
            if (!IsWindow(w)) return r;
        }
        return GetWindowLongPtrW(w, DWLP_MSGRESULT);
    }
    return DefWindowProcW(w, m, a, b);
}

int __cdecl main(void)
{
    WNDCLASSW wc = {0};
    unsigned i;
    wc.lpfnWndProc = subject;
    wc.cbWndExtra = DLGWINDOWEXTRA;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.lpszClassName = L"T447ActualDialogOwnerSubject";
    CHECK(RegisterClassW(&wc) != 0);
    for (i = 0; i != 64; ++i) {
        HWND w = CreateWindowExW(0, wc.lpszClassName, L"", WS_POPUP,
            0, 0, 80, 40, NULL, NULL, wc.hInstance, NULL);
        LONG_PTR saved, prior, replaced;
        unsigned before;
        WCHAR wide[16];
        CHAR narrow[16];
        BOOL ansi = (i & 1) != 0;
        CHECK(w != NULL);
        if (!w) break;
        SetLastError(0);
        CHECK(_GetWindowLong(&recovered.wnd, GWL_STYLE, TRUE) == 0);
        CHECK(GetLastError() == ERROR_INVALID_INDEX);
        before = callbacks;
        prior = ansi ? GetWindowLongPtrA(w, DWLP_DLGPROC) : GetWindowLongPtrW(w, DWLP_DLGPROC);
        SetLastError(0);
        replaced = ansi ? SetWindowLongPtrA(w, DWLP_DLGPROC, (LONG_PTR)dialog_a) :
            SetWindowLongPtrW(w, DWLP_DLGPROC, (LONG_PTR)dialog_w);
        if (i < 2) printf("ansi=%d prior=%p replaced=%p\n", ansi, (void *)prior, (void *)replaced);
        CHECK(GetLastError() == 0);
        CHECK(prior == 0);
        /* A native A/W transition can return a token even for an empty old
         * slot. Verify restoration, not token bits or getter/setter equality. */
        if (ansi) SetWindowLongPtrA(w, DWLP_DLGPROC, replaced);
        else SetWindowLongPtrW(w, DWLP_DLGPROC, replaced);
        CHECK(GetWindowLongPtrW(w, DWLP_DLGPROC) == 0);
        if (ansi) SetWindowLongPtrA(w, DWLP_DLGPROC, (LONG_PTR)dialog_a);
        else SetWindowLongPtrW(w, DWLP_DLGPROC, (LONG_PTR)dialog_w);
        CHECK(callbacks == before);
        saved = ansi ? GetWindowLongPtrW(w, DWLP_DLGPROC) : GetWindowLongPtrA(w, DWLP_DLGPROC);
        CHECK(saved != 0);
        /* Replace then restore a token through the API that issued it. */
        if (ansi) {
            SetWindowLongPtrW(w, DWLP_DLGPROC, (LONG_PTR)dialog_w);
            SetWindowLongPtrW(w, DWLP_DLGPROC, saved);
            CHECK(CallWindowProcW((WNDPROC)GetWindowLongPtrW(w, DWLP_DLGPROC),
                w, WM_GETTEXT, 16, (LPARAM)wide) == 5);
            CHECK(lstrcmpW(wide, L"alpha") == 0);
        } else {
            SetWindowLongPtrA(w, DWLP_DLGPROC, (LONG_PTR)dialog_a);
            SetWindowLongPtrA(w, DWLP_DLGPROC, saved);
            CHECK(CallWindowProcA((WNDPROC)GetWindowLongPtrA(w, DWLP_DLGPROC),
                w, WM_GETTEXT, 16, (LPARAM)narrow) == 5);
            CHECK(lstrcmpA(narrow, "omega") == 0);
        }
        CHECK(callbacks == before + 1);
        before = callbacks;
        CHECK(SendMessageW(w, WM_NOTIFYFORMAT, 0, NF_QUERY) == (ansi ? NFR_ANSI : NFR_UNICODE));
        CHECK(callbacks == before + 1);
        before = callbacks;
        CHECK(SendMessageW(w, WM_APP, 0, 0) == 0x3141);
        CHECK(callbacks == before + 1);
        CHECK(recovered.resultWP == 0 && recovered_data.lpfnDlg == NULL);
        before = terminal_callbacks;
        suppress_terminal = (i & 2) != 0;
        if (i & 4) CHECK(SendMessageW(w, WM_APP + 1, 0, 0) == 17);
        else CHECK(DestroyWindow(w));
        CHECK(!IsWindow(w));
        CHECK(terminal_callbacks == before + (suppress_terminal ? 0 : 1));
    }
    CHECK(UnregisterClassW(wc.lpszClassName, wc.hInstance));
    CHECK(native_depth == 0);
    printf("WOW_DIALOG_NATIVE_OWNER errors=%u iterations=%u callbacks=%u terminal=%u\n",
        failures, i, callbacks, terminal_callbacks);
    return failures ? 1 : 0;
}
