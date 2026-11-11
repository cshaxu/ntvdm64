#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "wow_dialog_procedure_bindings.h"
#include "wow_user_task_lifecycle.h"
#include "wow_window_words_binding.h"

/* Actual subject HWND/slots and A/W tokens; scheduler/publication and setter
 * failure injection are explicit doubles. Not guest acceptance. */
static unsigned errors, callbacks, depth, setters, fail_set, cases;
static BOOL raise_result, nesting;
static HWND nested_window;
static DWORD fatal_error;
static const char *fatal_reason;
#define CHECK(x) do { if (!(x)) { ++errors; printf("FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)
#define TEST_EXCEPTION 0xe0447301u
#define TEST_FATAL 0xe0447302u
/* Initialization/unwind fixture, never a terminal-resource substitute. */
BOOL WINAPI wow_window_words_consume_resource_destroy(HWND w,
    wow_window_resource_destroy expected,BOOL invoke)
{(void)w;(void)expected;(void)invoke;CHECK(FALSE);return FALSE;}

LONG WINAPI fixture_dialog_set_window_longW(HWND w, int index, LONG value)
{
    ++setters;
    if (fail_set && setters == fail_set) {
        SetLastError(ERROR_ACCESS_DENIED); return 0;
    }
    return SetWindowLongW(w, index, value);
}
BOOL WINAPI wow_user_native_call_begin(wow_user_native_call *call, HWND w)
{ ZeroMemory(call, sizeof(*call)); CHECK(IsWindow(w)); ++depth; return TRUE; }
BOOL WINAPI wow_user_native_call_end(wow_user_native_call *call)
{ (void)call; CHECK(depth); --depth; return TRUE; }
void WINAPI wow_user_native_call_result(wow_user_native_call *call, UINT m,
    WPARAM a, LPARAM b, LRESULT result)
{
    (void)call; (void)a; (void)b; (void)result;
    if (m != WM_NULL) return;
    if (nested_window && !nesting) {
        LONG previous = GetWindowLongW(nested_window, DWL_DLGPROC);
        nesting = TRUE;
        CHECK(wow_dialog_host_initialize(nested_window));
        CHECK(GetWindowLongW(nested_window, DWL_DLGPROC) == previous);
        CHECK(depth == 1);
        nesting = FALSE;
    }
    if (raise_result && !nesting) {
        SetLastError(7447);
        RaiseException(TEST_EXCEPTION, 0, 0, NULL);
    }
}
__declspec(noreturn) void WINAPI wow_user_native_call_failed(const char *why, DWORD error)
{
    fatal_reason = why; fatal_error = error;
    RaiseException(TEST_FATAL, EXCEPTION_NONCONTINUABLE, 0, NULL);
    ExitProcess(3);
}
LRESULT WINAPI wow_user_call_window_procA(WNDPROC p, HWND w, UINT m, WPARAM a, LPARAM b)
{ return CallWindowProcA(p, w, m, a, b); }
LRESULT WINAPI wow_user_call_window_procW(WNDPROC p, HWND w, UINT m, WPARAM a, LPARAM b)
{ return CallWindowProcW(p, w, m, a, b); }
static INT_PTR CALLBACK proc_a(HWND w, UINT m, WPARAM a, LPARAM b)
{
    (void)w; ++callbacks;
    if (m == WM_GETTEXT) { lstrcpynA((LPSTR)b, "alpha", (int)a); return 5; }
    return FALSE;
}
static INT_PTR CALLBACK proc_w(HWND w, UINT m, WPARAM a, LPARAM b)
{
    (void)w; ++callbacks;
    if (m == WM_GETTEXT) { lstrcpynW((LPWSTR)b, L"omega", (int)a); return 5; }
    return FALSE;
}

static void verify(HWND w, HWND other, BOOL ansi, unsigned mode)
{
    LONG saved;
    DWORD caught = 0, after_error;
    unsigned before = callbacks, before_errors = errors;
    BOOL returned = FALSE;
    WCHAR wide[16];
    setters = 0; fail_set = mode == 2 ? 1 : (mode == 3 || mode == 4) ? 2 : 0;
    raise_result = mode == 1 || mode == 4 || mode == 6;
    nested_window = mode >= 5 ? other : NULL;
    nesting = FALSE; fatal_reason = NULL; fatal_error = 0;
    if (ansi) SetWindowLongA(w, DWL_DLGPROC, (LONG)proc_a);
    else SetWindowLongW(w, DWL_DLGPROC, (LONG)proc_w);
    SetWindowLongA(other, DWL_DLGPROC, (LONG)proc_a);
    saved = GetWindowLongW(w, DWL_DLGPROC);
    CHECK(saved != 0);
    SetLastError(6447);
    __try { returned = wow_dialog_host_initialize(w); }
    __except ((GetExceptionCode() == TEST_EXCEPTION || GetExceptionCode() == TEST_FATAL) ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        caught = GetExceptionCode();
    }
    after_error = GetLastError();
    CHECK(depth == 0 && callbacks == before);
    CHECK(setters == (mode == 2 ? 1u : mode >= 5 ? 4u : 2u));
    if (mode == 3 || mode == 4) {
        CHECK(caught == TEST_FATAL && fatal_error == ERROR_ACCESS_DENIED);
        CHECK(fatal_reason && !strcmp(fatal_reason, "dialog-initialize-restore"));
    } else if (mode == 1 || mode == 6) {
        CHECK(caught == TEST_EXCEPTION && after_error == 7447);
    } else {
        CHECK(!caught && returned == (mode != 2));
        CHECK(after_error == (mode == 2 ? ERROR_ACCESS_DENIED : 6447u));
    }
    if (mode == 3 || mode == 4) {
        CHECK(GetWindowLongW(w, DWL_DLGPROC) == 0);
        /* Production terminates when required state restoration fails.
         * Restore only this test subject before its cleanup. */
        SetWindowLongW(w, DWL_DLGPROC, saved);
    } else CHECK(GetWindowLongW(w, DWL_DLGPROC) == saved);
    CHECK(CallWindowProcW((WNDPROC)GetWindowLongW(w, DWL_DLGPROC),
        w, WM_GETTEXT, ARRAYSIZE(wide), (LPARAM)wide) == 5);
    CHECK(!lstrcmpW(wide, ansi ? L"alpha" : L"omega"));
    CHECK(callbacks == before + 1);
    ++cases;
    printf("DIALOG_INITIALIZATION ansi=%u mode=%u errors=%u\n", ansi, mode, errors - before_errors);
    fail_set = 0; raise_result = FALSE; nested_window = NULL; nesting = FALSE;
}

int __cdecl main(void)
{
    WNDCLASSW wc = {0};
    HWND windows[2] = {0};
    unsigned i, mode, ansi;
    wc.lpfnWndProc = DefWindowProcW; wc.cbWndExtra = DLGWINDOWEXTRA;
    wc.hInstance = GetModuleHandleW(NULL); wc.lpszClassName = L"T447DialogInitializationSubject";
    CHECK(RegisterClassW(&wc));
    for (i = 0; i < 2; ++i) {
        windows[i] = CreateWindowExW(0, wc.lpszClassName, L"", WS_POPUP,
            0, 0, 10, 10, NULL, NULL, wc.hInstance, NULL);
        CHECK(windows[i]);
    }
    if (windows[0] && windows[1])
        for (ansi = 0; ansi < 2; ++ansi)
            for (mode = 0; mode < 7; ++mode) verify(windows[0], windows[1], ansi, mode);
    for (i = 0; i < 2; ++i) if (windows[i]) {
        wow_dialog_host_retire(windows[i]);
        CHECK(DestroyWindow(windows[i]));
    }
    CHECK(UnregisterClassW(wc.lpszClassName, wc.hInstance));
    CHECK(depth == 0 && cases == 14);
    printf("WOW_DIALOG_INITIALIZATION errors=%u cases=%u depth=%u\n", errors, cases, depth);
    return errors ? 1 : 0;
}
