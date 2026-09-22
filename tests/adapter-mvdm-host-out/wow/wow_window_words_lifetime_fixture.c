#include "wow_window_words_binding.h"
#include "wow_window_dispatch_bindings.h"
#include <stdio.h>
#include <string.h>
typedef VOID (APIENTRY *PFNW32ET)(VOID);
#pragma warning(push)
#pragma warning(disable:4201)
#include "opennt-abi/source/public/internal/windows/inc/wowuserp.h"
#pragma warning(pop)

static unsigned errors, active, destroyed;
static void *pages[8];
static BOOL freed[8];
static unsigned allocations, releases;
static BOOL fail_allocate, fail_property;
static BOOL fail_info, fail_instance;
static DWORD WINAPI callback(HWND, UINT, UINT, LONG, DWORD, PVOID);
static unsigned native_calls;
static DWORD expected_style, expected_exstyle;
static HANDLE expected_instance;
static const char class_name[] = "S40_WOW_WORDS_LIFETIME";
#define CHECK(x) do { if (!(x)) { ++errors; printf("FAIL line=%d\n", __LINE__); } } while (0)
void *WINAPI wow_words_fixture_alloc(HANDLE heap, DWORD flags, SIZE_T bytes)
{
    void *p;
    CHECK(heap == GetProcessHeap() && flags == 0 && bytes <= 4096 && allocations < 8);
    if (fail_allocate) return NULL;
    if (bytes > 4096 || allocations >= 8) return NULL;
    p = VirtualAlloc(NULL, 4096, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (p) pages[allocations++] = p;
    return p;
}
BOOL WINAPI wow_words_fixture_free(HANDLE heap, DWORD flags, void *p)
{
    unsigned i;
    DWORD old;
    CHECK(heap == GetProcessHeap() && flags == 0);
    for (i = 0; i < allocations; ++i) if (pages[i] == p) {
        CHECK(!freed[i]);
        if (freed[i]) return FALSE;
        CHECK(VirtualProtect(p, 4096, PAGE_NOACCESS, &old));
        freed[i] = TRUE; ++releases;
        return TRUE;
    }
    CHECK(FALSE); return FALSE;
}
BOOL WINAPI wow_words_fixture_setprop(HWND window, LPCWSTR name, HANDLE value)
{
    if (fail_property) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    return SetPropW(window, name, value);
}
BOOL WINAPI wow_words_fixture_info(HWND window, PWINDOWINFO info)
{
    if (fail_info) { SetLastError(ERROR_INVALID_WINDOW_HANDLE); return FALSE; }
    return GetWindowInfo(window, info);
}
LONG WINAPI wow_words_fixture_long(HWND window, int index)
{
    if (index == GWL_WNDPROC) return GetWindowLongA(window, index);
    CHECK(index == GWL_HINSTANCE);
    if (fail_instance) { SetLastError(ERROR_ACCESS_DENIED); return 0; }
    return GetWindowLongA(window, index);
}
static LRESULT CALLBACK native_callback(HWND window, UINT msg, WPARAM wp, LPARAM lp)
{
    CHECK(IsWindow(window) && msg == WM_USER + 1 && wp == 7 && lp == -8);
    ++native_calls;
    return 0x2468;
}
static LRESULT CALLBACK procedure(HWND w, UINT m, WPARAM wp, LPARAM lp)
{
    if (m == WM_USER) {
        LONG result = 0;
        CHECK(wow_window_dispatch_bound(w, m, wp, lp, 0x81234567, callback, &result));
        return result;
    }
    if (m == WM_NCDESTROY) {
        ++destroyed;
        wow_window_words_detach(w);
    }
    return DefWindowProcA(w, m, wp, lp);
}
static DWORD WINAPI callback(HWND window, UINT msg, UINT wp, LONG lp, DWORD proc, PVOID words)
{
    WW *ww = words;
    wow_window_words_binding *nested = wow_window_words_acquire(window);
    (void)wp; (void)lp;
    CHECK(msg == WM_USER && proc == 0x81234567 && nested);
    CHECK(wow_window_words_value(nested) == ww);
    CHECK(ww->dwStyle == expected_style && ww->dwExStyle == expected_exstyle &&
        ww->hInstance == expected_instance);
    CHECK(DestroyWindow(window));
    CHECK(!IsWindow(window));
    CHECK(!wow_window_words_acquire(window));
    CHECK(releases == 2); /* Failed publication and duplicate attach only. */
    /* The two outstanding native references still own the exact same WW. */
    CHECK(ww->adwWOW[1] == 0x12345678 && ww->style == expected_style);
    ww->adwWOW[2] = 0xcafebabe;
    wow_window_words_release(nested);
    CHECK(releases == 2);
    CHECK(ww->adwWOW[2] == 0xcafebabe);
    ++active;
    return 0x55aa;
}
static DWORD WINAPI exception_callback(HWND window, UINT msg, UINT wp, LONG lp,
    DWORD proc, PVOID words)
{
    (void)msg; (void)wp; (void)lp; (void)proc;
    CHECK(((WW *)words)->adwWOW[1] == 0x12345678);
    CHECK(DestroyWindow(window));
    CHECK(releases == 3);
    RaiseException(0xe0000040, 0, 0, NULL);
    return 0;
}
int __cdecl main(void)
{
    WNDCLASSA wc = {0};
    WW initial = {0};
    HWND window;
    wow_window_words_binding *first, *second;
    LONG result = 0x123456;
    BOOL caught = FALSE;
    unsigned i;
    wc.hInstance = GetModuleHandleA(NULL); wc.lpfnWndProc = procedure; wc.lpszClassName = class_name;
    if (!RegisterClassA(&wc)) return 1;
    window = CreateWindowExA(0, class_name, "", 0, 0, 0, 32, 32, NULL, NULL, wc.hInstance, NULL);
    if (!window) { UnregisterClassA(class_name, wc.hInstance); return 2; }
    CHECK(!wow_window_words_acquire(window));
    CHECK(!wow_window_dispatch_bound(window, WM_USER, 0, 0, 0x81234567, callback, &result));
    CHECK(GetLastError() == ERROR_NOT_FOUND && result == 0x123456);
    initial.adwWOW[1] = 0x12345678; initial.style = WS_OVERLAPPED;
    CHECK(!wow_window_words_attach(window, NULL) && GetLastError() == ERROR_INVALID_PARAMETER);
    fail_allocate = TRUE;
    CHECK(!wow_window_words_attach(window, &initial) && GetLastError() == ERROR_NOT_ENOUGH_MEMORY);
    CHECK(!wow_window_words_acquire(window));
    fail_allocate = FALSE; fail_property = TRUE;
    CHECK(!wow_window_words_attach(window, &initial) && GetLastError() == ERROR_NOT_ENOUGH_MEMORY);
    CHECK(!wow_window_words_acquire(window));
    fail_property = FALSE;
    CHECK(wow_window_words_attach(window, &initial));
    fail_info = TRUE;
    CHECK(!wow_window_dispatch_bound(window, WM_USER, 0, 0, 0x81234567, callback, &result));
    CHECK(GetLastError() == ERROR_INVALID_WINDOW_HANDLE && result == 0x123456 && !active);
    fail_info = FALSE; fail_instance = TRUE;
    CHECK(!wow_window_dispatch_bound(window, WM_USER, 0, 0, 0x81234567, callback, &result));
    CHECK(GetLastError() == ERROR_ACCESS_DENIED && result == 0x123456 && !active);
    fail_instance = FALSE;
    first = wow_window_words_acquire(window);
    CHECK(first && !memcmp(wow_window_words_value(first), &initial, sizeof(initial)));
    wow_window_words_release(first);
    CHECK(!wow_window_dispatch_bound(window, WM_USER, 0, 0, 0x81234567, NULL, &result));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER && result == 0x123456);
    CHECK(!wow_window_dispatch_bound(window, WM_USER, 0, 0, 0, callback, &result));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER && result == 0x123456);
    CHECK(!wow_window_dispatch_bound(window, WM_USER, 0, 0, 0x81234567, callback, NULL));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    CHECK(wow_window_dispatch_bound(window, WM_USER + 1, 7, -8, (DWORD)native_callback, NULL, &result));
    CHECK(result == 0x2468 && native_calls == 1);
    result = 0x123456;
    CHECK(!wow_window_words_attach(window, &initial) && GetLastError() == ERROR_ALREADY_EXISTS);
    first = wow_window_words_acquire(window); second = wow_window_words_acquire(window);
    CHECK(first && first == second);
    wow_window_words_release(second);
    wow_window_words_release(first);
    /* Mutate native attributes after publication. The original WW aliases
     * the live WND fields, not the initial zero-filled fixture snapshot. */
    SetWindowLongA(window, GWL_STYLE, GetWindowLongA(window, GWL_STYLE) | WS_DISABLED);
    SetWindowLongA(window, GWL_EXSTYLE, GetWindowLongA(window, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
    SetWindowLongA(window, GWL_HINSTANCE, 0x23455678);
    expected_style = (DWORD)GetWindowLongA(window, GWL_STYLE);
    expected_exstyle = (DWORD)GetWindowLongA(window, GWL_EXSTYLE);
    expected_instance = (HANDLE)GetWindowLongA(window, GWL_HINSTANCE);
    CHECK((expected_style & WS_DISABLED) && (expected_exstyle & WS_EX_TOOLWINDOW) &&
        expected_instance == (HANDLE)0x23455678);
    CHECK(SendMessageA(window, WM_USER, 0, 0) == 0x55aa);
    CHECK(active == 1 && destroyed == 1);
    wow_window_words_detach(window); /* Already destroyed: no stale free. */
    wow_window_words_release(NULL);
    CHECK(wow_window_words_value(NULL) == NULL);
    CHECK(allocations == 3 && releases == 3);
    window = CreateWindowExA(0, class_name, "", 0, 0, 0, 32, 32, NULL, NULL, wc.hInstance, NULL);
    CHECK(window && wow_window_words_attach(window, &initial));
    __try {
        wow_window_dispatch_bound(window, WM_USER, 0, 0, 0x81234567, exception_callback, &result);
        CHECK(FALSE);
    } __except (GetExceptionCode() == 0xe0000040 ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        caught = TRUE;
    }
    CHECK(caught && !IsWindow(window) && destroyed == 2);
    CHECK(result == 0x123456 && allocations == 4 && releases == 4);
    CHECK(UnregisterClassA(class_name, wc.hInstance));
    for (i = 0; i < allocations; ++i) CHECK(VirtualFree(pages[i], 0, MEM_RELEASE));
    printf("WOW_WINDOW_WORDS_DESTROY_DURING_CALLBACK errors=%u active=%u destroyed=%u\n",
        errors, active, destroyed);
    printf("WOW_WINDOW_WORDS_POISONED_FREE allocations=%u releases=%u\n", allocations, releases);
    printf("WOW_WINDOW_DISPATCH_GATEWAY native_message=1 exception_cleanup=%d\n", caught);
    puts("WOW_WINDOW_DISPATCH_LIVE_NATIVE_FIELDS_OK");
    return errors ? 3 : 0;
}
