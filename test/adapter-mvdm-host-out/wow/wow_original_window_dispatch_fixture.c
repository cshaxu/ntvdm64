#include "wow_window_dispatch_bindings.h"
#include <stdio.h>

static unsigned errors, guest_calls, native_calls;
static DWORD words[8] = {1,2,3,4,5,6,7,8};
static DWORD expected_proc;
static const HWND window = (HWND)0x1234;
#define CHECK(x) do { if (!(x)) { ++errors; printf("FAIL line=%d\n", __LINE__); } } while (0)
static LRESULT CALLBACK native_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    ++native_calls;
    CHECK(hwnd == window && msg == WM_USER + 7 && wp == 0x4321 && lp == -17);
    return (LRESULT)0x89abcdef;
}
static DWORD WINAPI guest_callback(HWND hwnd, UINT msg, UINT wp, LONG lp, DWORD proc, PVOID ww)
{
    wow_window_dispatch_view nested = {window, words, NULL};
    ++guest_calls;
    CHECK(hwnd == window && msg == WM_USER + 7 && wp == 0x4321 && lp == -17);
    CHECK(proc == expected_proc && ww == words);
    /* Nested native dispatch needs neither a guest callback nor global state. */
    CHECK((DWORD)DispatchClientMessage(&nested, msg, wp, lp, (DWORD)native_proc) == 0x89abcdef);
    words[2] += 1;
    return 0x76543210;
}
int __cdecl main(void)
{
    wow_window_dispatch_view view = {window, words, guest_callback};
    CHECK(!((DWORD)native_proc & 0x80000000));
    CHECK((DWORD)DispatchClientMessage(&view, WM_USER + 7, 0x4321, -17, (DWORD)native_proc) == 0x89abcdef);
    expected_proc = 0x81234567;
    CHECK((DWORD)DispatchClientMessage(&view, WM_USER + 7, 0x4321, -17, expected_proc) == 0x76543210);
    expected_proc = 0xf2304567;
    CHECK((DWORD)DispatchClientMessage(&view, WM_USER + 7, 0x4321, -17, expected_proc) == 0x76543210);
    CHECK(guest_calls == 2 && native_calls == 3 && words[2] == 5);
    CHECK(words[0] == 1 && words[7] == 8 && view.callback == guest_callback);
    printf("WOW_ORIGINAL_WINDOW_DISPATCH errors=%u guest=%u native=%u\n", errors, guest_calls, native_calls);
    return errors ? 3 : 0;
}
