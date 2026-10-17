#include "wow_window_words_binding.h"
#include "wow_task_order_bindings.h"
#include <stdio.h>
typedef VOID (WINAPI *PFNW32ET)(VOID);
#include "opennt-abi/source/public/internal/windows/inc/wowuserp.h"

static unsigned errors, enters, leaves, native_calls;
static BOOL raise_native;
static unsigned unicode_calls;
static BOOL destroy_unicode;
#define CHECK(x) do { if (!(x)) { ++errors; fprintf(stderr, \
    "FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)

/* Deliberately deny guest entry. The actual original DispatchClientMessage
 * native branch must still execute; no real scheduler behavior is claimed. */
BOOL WINAPI wow_task_callback_enter(wow_task_callback_scope *scope)
{
    (void)scope; ++enters; SetLastError(ERROR_BUSY); return FALSE;
}
BOOL WINAPI wow_task_callback_leave(wow_task_callback_scope *scope)
{ (void)scope; ++leaves; return TRUE; }
static DWORD WINAPI guest_callback(HWND window, UINT message, UINT wp,
    LONG lp, DWORD procedure, PVOID words)
{
    (void)window; (void)message; (void)wp; (void)lp;
    (void)procedure; (void)words;
    CHECK(FALSE); return 0;
}
static LRESULT CALLBACK native_callback(HWND window, UINT message,
    WPARAM wp, LPARAM lp)
{
    CHECK(IsWindow(window) && message == WM_APP && wp == 7 && lp == 9);
    ++native_calls;
    if (raise_native) {
        wow_window_words_detach(window);
        RaiseException(0xe0447003u, 0, 0, NULL);
    }
    return 0x5678;
}

static LRESULT CALLBACK unicode_callback(HWND window, UINT message,
    WPARAM wp, LPARAM lp)
{
    if (message == WM_SETTEXT) {
        CHECK(!lstrcmpW((LPCWSTR)lp, L"native CPD text"));
        ++unicode_calls;
        if (destroy_unicode) {
            wow_window_words_detach(window);
            CHECK(DestroyWindow(window));
        }
        return 0x3141;
    }
    return DefWindowProcW(window, message, wp, lp);
}

static void verify_callproc(void)
{
    WNDCLASSW wc = {0};
    WW words = {0};
    wow_window_dispatch_target target = {0};
    wow_window_words_binding *binding;
    DWORD token, current, previous;
    HWND window;
    unsigned old_enters = enters, old_leaves = leaves;
    wc.lpfnWndProc = unicode_callback;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.lpszClassName = L"WOW native CallProc fixture";
    CHECK(RegisterClassW(&wc));
    window = CreateWindowExW(0, wc.lpszClassName, NULL, WS_POPUP,
        0, 0, 32, 32, NULL, NULL, wc.hInstance, NULL);
    CHECK(window != NULL);
    if (!window) return;
    token = (DWORD)GetWindowLongA(window, GWL_WNDPROC);
    /* Actual public USER A/W identity, not a fabricated FFFF value. */
    CHECK(HIWORD(token) == WNDPROC_HANDLE);
    target.procedure = token;
    target.callback = guest_callback;
    CHECK(wow_window_words_attach_target(window, &words, NULL, &target));
    CHECK(wow_window_procedure_read(window, &current) && current == token);
    binding = wow_window_words_acquire(window);
    CHECK(binding != NULL);
    if (binding) {
        CHECK(!wow_window_words_cleanup_value(binding)->procedure_is_guest);
        wow_window_words_release(binding);
    }
    CHECK(SendMessageA(window, WM_SETTEXT, 0,
        (LPARAM)"native CPD text") == 0x3141);
    CHECK(unicode_calls == 1 && enters == old_enters && leaves == old_leaves);
    previous = (DWORD)SetWindowLongA(window, GWL_WNDPROC, (LONG)native_callback);
    CHECK(previous == token);
    CHECK(SendMessageA(window, WM_APP, 7, 9) == 0x5678);
    previous = (DWORD)SetWindowLongA(window, GWL_WNDPROC, (LONG)token);
    CHECK(previous == (DWORD)native_callback);
    CHECK(wow_window_procedure_read(window, &current) && current == token);
    destroy_unicode = TRUE;
    CHECK(SendMessageA(window, WM_SETTEXT, 0,
        (LPARAM)"native CPD text") == 0x3141);
    CHECK(unicode_calls == 2 && !IsWindow(window));
    CHECK(enters == old_enters && leaves == old_leaves);
    CHECK(UnregisterClassW(wc.lpszClassName, wc.hInstance));
    fprintf(stderr, "WOW_NATIVE_CALLPROC token=%08lX calls=%u restored=1\n",
        token, unicode_calls);
}
int __cdecl main(void)
{
    WW words = {0};
    LONG result = 0;
    BOOL caught = FALSE;
    HWND window = CreateWindowExA(0, "STATIC", "", WS_POPUP,
        0, 0, 32, 32, NULL, NULL, GetModuleHandleA(NULL), NULL);
    CHECK(window && wow_window_words_attach(window, &words));
    CHECK(wow_window_dispatch_bound(window, WM_APP, 7, 9,
        (DWORD)(ULONG_PTR)native_callback, NULL, &result));
    CHECK(result == 0x5678 && native_calls == 1 && !enters && !leaves);
    CHECK(!wow_window_dispatch_bound(window, WM_APP, 7, 9,
        0x81234567u, guest_callback, &result));
    CHECK(GetLastError() == ERROR_BUSY && enters == 1 && !leaves);
    raise_native = TRUE;
    __try {
        (void)wow_window_dispatch_bound(window, WM_APP, 7, 9,
            (DWORD)(ULONG_PTR)native_callback, NULL, &result);
    } __except (GetExceptionCode() == 0xe0447003u ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        caught = TRUE;
    }
    CHECK(caught && native_calls == 2 && enters == 1 && !leaves);
    CHECK(!wow_window_words_acquire(window));
    CHECK(DestroyWindow(window));
    raise_native = FALSE;
    verify_callproc();
    fprintf(stderr, "WOW_NATIVE_DISPATCH_GATE errors=%u native=%u guest-entry=%u\n",
        errors, native_calls, enters);
    return errors ? 1 : 0;
}
