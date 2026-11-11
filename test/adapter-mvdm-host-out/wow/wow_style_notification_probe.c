#include <windows.h>
#include <stdio.h>

/* Research only: an ordinary fixture window, never a product helper.
 * Compare the native full setter with its internal masked style leaf. */
typedef BOOL (WINAPI *alter_style_fn)(HWND, DWORD, DWORD);
static unsigned changing, changed, hook_changing, hook_changed, failures;
static BOOL inject, injected;
static unsigned synthetic_seen;
#define CHECK(c) do { if (!(c)) { ++failures; printf("FAIL line=%d\n", __LINE__); } } while (0)
static LRESULT CALLBACK proc(HWND w, UINT m, WPARAM p, LPARAM l)
{
    if (m == WM_STYLECHANGING) ++changing;
    if (m == WM_STYLECHANGED) ++changed;
    if (m == WM_STYLECHANGING || m == WM_STYLECHANGED)
        printf("PROC msg=%x index=%ld old=%08lx new=%08lx send=%lx\n",
            m, (LONG)p, ((STYLESTRUCT *)l)->styleOld,
            ((STYLESTRUCT *)l)->styleNew, InSendMessageEx(NULL));
    return DefWindowProcW(w, m, p, l);
}
static LRESULT CALLBACK hook(int code, WPARAM wp, LPARAM lp)
{
    if (code >= 0) {
        CWPSTRUCT *event = (CWPSTRUCT *)lp;
        if (event->message == WM_STYLECHANGING) {
            ++hook_changing;
            if (inject && !injected) {
                STYLESTRUCT copy = *(STYLESTRUCT *)event->lParam;
                injected = TRUE;
                ++synthetic_seen;
                SendMessageW(event->hwnd, event->message, event->wParam, (LPARAM)&copy);
            }
        }
        if (event->message == WM_STYLECHANGED) ++hook_changed;
    }
    return CallNextHookEx(NULL, code, wp, lp);
}
int main(void)
{
    WNDCLASSW wc = {0};
    HWND w;
    HHOOK listener;
    HMODULE native;
    alter_style_fn alter;
    DWORD before, after;
    wc.hInstance = GetModuleHandleW(NULL); wc.lpfnWndProc = proc;
    wc.lpszClassName = L"WOWStyleNotificationResearch";
    CHECK(RegisterClassW(&wc));
    w = CreateWindowExW(0, wc.lpszClassName, L"Style research", WS_POPUP,
        0, 0, 100, 100, NULL, NULL, wc.hInstance, NULL);
    CHECK(w != NULL);
    if (!w) return 1;
    native = GetModuleHandleW(L"win32u.dll");
    alter = (alter_style_fn)GetProcAddress(native, "NtUserAlterWindowStyle");
    CHECK(alter != NULL);
    listener = SetWindowsHookExW(WH_CALLWNDPROC, hook, NULL, GetCurrentThreadId());
    CHECK(listener != NULL);
    changing = changed = hook_changing = hook_changed = 0;
    before = (DWORD)GetWindowLongW(w, GWL_STYLE);
    SetWindowLongW(w, GWL_STYLE, before | WS_BORDER);
    after = (DWORD)GetWindowLongW(w, GWL_STYLE);
    CHECK((after & WS_BORDER) && changing == 1 && changed == 1);
    CHECK(hook_changing == 1 && hook_changed == 1);
    printf("FULL notifications=%u/%u hooks=%u/%u\n", changing, changed, hook_changing, hook_changed);
    changing = changed = hook_changing = hook_changed = 0;
    if (alter) {
        alter(w, WS_BORDER, 0);
        after = (DWORD)GetWindowLongW(w, GWL_STYLE);
        CHECK(after & WS_BORDER); /* Outside installed service's allowed mask. */
        alter(w, WS_HSCROLL, WS_HSCROLL);
        CHECK((DWORD)GetWindowLongW(w, GWL_STYLE) & WS_HSCROLL);
        CHECK(!changing && !changed && !hook_changing && !hook_changed);
    }
    printf("MASKED border_preserved=%u scroll_set=%u notifications=%u/%u\n",
        !!(after & WS_BORDER), !!((DWORD)GetWindowLongW(w, GWL_STYLE) & WS_HSCROLL), changing, changed);
    inject = TRUE;
    before = (DWORD)GetWindowLongW(w, GWL_STYLE);
    SetWindowLongW(w, GWL_STYLE, before & ~WS_BORDER);
    CHECK(synthetic_seen == 1 && changing == 2 && changed == 1);
    CHECK(hook_changing == 2 && hook_changed == 1);
    printf("REENTRY independent_send=%u changing=%u changed=%u failures=%u\n",
        synthetic_seen, changing, changed, failures);
    if (listener) UnhookWindowsHookEx(listener);
    DestroyWindow(w); UnregisterClassW(wc.lpszClassName, wc.hInstance);
    return failures ? 1 : 0;
}
