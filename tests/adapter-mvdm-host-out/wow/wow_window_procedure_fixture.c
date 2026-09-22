#include "wow_window_words_binding.h"
#include "wow_user_private_access.h"
#include <stdio.h>
typedef VOID (APIENTRY *PFNW32ET)(VOID);
#pragma warning(push)
#pragma warning(disable:4201)
#include "opennt-abi/source/public/internal/windows/inc/wowuserp.h"
#pragma warning(pop)
static unsigned errors, calls;
static unsigned unicode_text_calls;
#define CHECK(x) do { if (!(x)) { ++errors; printf("FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)
static DWORD WINAPI callback(HWND hwnd, UINT msg, UINT wp, LONG lp, DWORD proc, PVOID words)
{
    CHECK(((PWW)words)->vpfnWndProc == 0x11112222);
    if (msg == WM_USER) { ++calls; return proc; }
    return DefWindowProcA(hwnd, msg, wp, lp);
}
static LRESULT CALLBACK native(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    if (msg == WM_USER) return 42;
    return DefWindowProcA(hwnd, msg, wp, lp);
}
static LRESULT CALLBACK unicode_native(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    if (msg == WM_USER) return 84;
    if (msg == WM_SETTEXT) {
        CHECK(lstrcmpW((LPCWSTR)lp, L"S40 conversion") == 0);
        ++unicode_text_calls;
        return TRUE;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}
static void native_callproc_contract(void)
{
    WNDCLASSEXW cls = {sizeof(cls)};
    HWND hwnd;
    WNDPROC ansi_view, previous;
    LONG restored;
    WW words = {0};
    wow_window_dispatch_target target = {0, callback};
    DWORD unchanged = 0x12345678;
    LONG result = 0x12345678;
    cls.lpfnWndProc = unicode_native;
    cls.hInstance = GetModuleHandleA(NULL);
    cls.lpszClassName = L"S40_UNICODE_CALLPROC";
    CHECK(RegisterClassExW(&cls) != 0);
    hwnd = CreateWindowExW(0, cls.lpszClassName, L"", 0,
        0, 0, 16, 16, NULL, NULL, cls.hInstance, NULL);
    CHECK(hwnd != NULL);
    if (!hwnd) return;
    CHECK(IsWindowUnicode(hwnd));
    CHECK((WNDPROC)GetWindowLongW(hwnd, GWL_WNDPROC) == unicode_native);
    ansi_view = (WNDPROC)GetWindowLongA(hwnd, GWL_WNDPROC);
    CHECK(ansi_view != NULL);
    CHECK((WNDPROC)wow_user_get_window_longA(hwnd, GWL_WNDPROC) == ansi_view);
    /* A native callproc token must go through CallWindowProc, never the
     * original DispatchClientMessage raw-procedure entry or a guest callback.
     * Do not assume modern USER exposes NT4's FFFF token representation. */
    CHECK(CallWindowProcA(ansi_view, hwnd, WM_USER, 0, 0) == 84);
    CHECK(CallWindowProcA(ansi_view, hwnd, WM_SETTEXT, 0,
        (LPARAM)"S40 conversion"));
    /* The original storage invariant forbids CPD tokens, even though their
     * high bit overlaps WNDPROC_WOW. This machine's native token is observed
     * separately; the reserved NT4 encoding is always a negative case. */
    target.procedure = 0xffff1234;
    CHECK(!wow_window_words_attach_target(hwnd, &words, NULL, &target));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    target.procedure = 0x92340056;
    CHECK(wow_window_words_attach_target(hwnd, &words, NULL, &target));
    CHECK(!wow_window_procedure_exchange(hwnd, 0xffff1234, &unchanged));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER && unchanged == 0x12345678);
    CHECK(!wow_window_dispatch_bound(hwnd, WM_USER, 0, 0,
        0xffff1234, callback, &result));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER && result == 0x12345678);
    CHECK(calls == 0);
    wow_window_words_detach(hwnd);
    previous = (WNDPROC)SetWindowLongA(hwnd, GWL_WNDPROC, (LONG)native);
    CHECK(previous != NULL);
    CHECK(!IsWindowUnicode(hwnd));
    CHECK(CallWindowProcA(previous, hwnd, WM_SETTEXT, 0,
        (LPARAM)"S40 conversion"));
    restored = SetWindowLongA(hwnd, GWL_WNDPROC, (LONG)previous);
    CHECK((WNDPROC)restored == native);
    CHECK(IsWindowUnicode(hwnd));
    CHECK((WNDPROC)GetWindowLongW(hwnd, GWL_WNDPROC) == unicode_native);
    CHECK(SendMessageW(hwnd, WM_SETTEXT, 0, (LPARAM)L"S40 conversion"));
    CHECK(unicode_text_calls == 3);
    printf("WOW_NATIVE_CALLPROC ansi=%08lx previous=%08lx native=%08lx text_calls=%u\n",
        (DWORD)ansi_view, (DWORD)previous, (DWORD)unicode_native, unicode_text_calls);
    CHECK(DestroyWindow(hwnd));
    CHECK(UnregisterClassW(cls.lpszClassName, cls.hInstance));
}
int __cdecl main(void)
{
    HWND windows[2];
    WW words = {0};
    wow_window_dispatch_target target = {0x92340056, callback};
    DWORD previous, value;
    unsigned i;
    native_callproc_contract();
    words.vpfnWndProc = 0x11112222;
    for (i = 0; i != 2; ++i) {
        windows[i] = CreateWindowExA(0, "STATIC", "", 0, 0, 0, 16, 16,
            NULL, NULL, GetModuleHandleA(NULL), NULL);
        CHECK(windows[i] != NULL);
        CHECK(wow_window_words_attach_target(windows[i], &words, NULL, &target));
        CHECK(SetWindowLongA(windows[i], GWL_WNDPROC, (LONG)wow_window_native_proc) != 0);
    }
    CHECK(wow_window_procedure_read(windows[0], &value) && value == 0x92340056);
    CHECK(GetWindowLongA(windows[0], GWL_WNDPROC) == (LONG)wow_window_native_proc);
    CHECK((DWORD)wow_user_get_window_longA(windows[0], GWL_WNDPROC) == 0x92340056);
    CHECK(wow_window_procedure_exchange(windows[0], 0x92300056, &previous));
    CHECK(previous == 0x92340056);
    CHECK((DWORD)wow_user_get_window_longA(windows[0], GWL_WNDPROC) == 0x92300056);
    CHECK((DWORD)SendMessageA(windows[0], WM_USER, 0, 0) == 0x92300056);
    CHECK((DWORD)SendMessageA(windows[1], WM_USER, 0, 0) == 0x92340056);
    CHECK(wow_window_procedure_exchange(windows[0], (DWORD)native, &previous));
    CHECK(previous == 0x92300056);
    CHECK(wow_user_get_window_longA(windows[0], GWL_WNDPROC) == (LONG)native);
    CHECK(SendMessageA(windows[0], WM_USER, 0, 0) == 42);
    value = 0xabcdef01;
    CHECK(!wow_window_procedure_exchange(windows[0], (DWORD)wow_window_native_proc, &value));
    CHECK(value == 0xabcdef01 && GetLastError() == ERROR_INVALID_PARAMETER);
    CHECK(wow_window_procedure_exchange(windows[0], target.procedure, &previous));
    CHECK(previous == (DWORD)native);
    CHECK((DWORD)SendMessageA(windows[0], WM_USER, 0, 0) == target.procedure);
    CHECK(calls == 3);
    for (i = 0; i != 2; ++i) CHECK(DestroyWindow(windows[i]));
    CHECK(!wow_window_procedure_read(windows[0], &value));
    CHECK(wow_user_get_window_longA(windows[0], GWL_WNDPROC) == 0);
    windows[0] = CreateWindowExA(0, "STATIC", "", 0, 0, 0, 16, 16,
        NULL, NULL, GetModuleHandleA(NULL), NULL);
    CHECK(windows[0] != NULL);
    CHECK(SetWindowLongA(windows[0], GWL_WNDPROC, (LONG)wow_window_native_proc) != 0);
    CHECK(wow_user_get_window_longA(windows[0], GWL_WNDPROC) == 0);
    CHECK(GetLastError() == ERROR_NOT_FOUND);
    CHECK(DestroyWindow(windows[0]));
    printf("WOW_WINDOW_PROCEDURE errors=%u tagged_calls=%u\n", errors, calls);
    return errors ? 3 : 0;
}
