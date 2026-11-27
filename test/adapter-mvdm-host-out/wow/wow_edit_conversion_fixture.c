#include "wow_edit_conversion_bindings.h"
#include "compat/include/opennt_user_multipass.h"
#include "wow_edit_entry_bindings.h"
#include <stdio.h>

#define PROTOS_ONLY
#include "messages.h"
#undef PROTOS_ONLY
C_ASSERT(sizeof(MessageTable) / sizeof(MessageTable[0]) == WM_USER);

/* Supplemental transport contract only. Link the real conversion binding
 * with these boundary doubles. This does not prove native CPD translation,
 * original EDIT behavior, guest execution or destruction ordering. */
static DWORD options_seen, target_seen, error_seen;
static HWND window_seen;
static UINT message_seen;
static WPARAM wp_seen;
static LPARAM lp_seen;
static unsigned requests, dispatches, wide_dispatch;
static BOOL reject_cpd;
static int failures;
#define CHECK(x) do { if (!(x)) { printf("FAIL line %d: %s\n", __LINE__, #x); ++failures; } } while (0)

LONG WINAPI EditWndProcA(HWND h, UINT m, WPARAM w, LONG l)
{ (void)h; (void)m; (void)w; (void)l; return 11; }
LONG WINAPI EditWndProcW(HWND h, UINT m, WPARAM w, LONG l)
{ (void)h; (void)m; (void)w; (void)l; return 12; }
LRESULT CALLBACK wow_edit_window_procA(HWND h, UINT m, WPARAM w, LPARAM l)
{ (void)h; (void)m; (void)w; (void)l; return 21; }
LRESULT CALLBACK wow_edit_window_procW(HWND h, UINT m, WPARAM w, LPARAM l)
{ (void)h; (void)m; (void)w; (void)l; return 22; }

DWORD WINAPI wow_edit_native_NtUserGetCPD(HWND h, DWORD options, DWORD target)
{
    ++requests;
    window_seen = h; options_seen = options; target_seen = target;
    SetLastError(reject_cpd ? ERROR_INVALID_WINDOW_HANDLE : 888);
    return reject_cpd ? 0 : 0xffff1234;
}

static LRESULT dispatch(WNDPROC p, HWND h, UINT m, WPARAM w, LPARAM l, BOOL wide)
{
    error_seen = GetLastError();
    CHECK((DWORD)(ULONG_PTR)p == 0xffff1234);
    CHECK(h == window_seen);
    ++dispatches; wide_dispatch = wide;
    message_seen = m; wp_seen = w; lp_seen = l;
    SetLastError(777);
    return -123;
}
LRESULT WINAPI wow_user_call_window_procA(WNDPROC p, HWND h, UINT m, WPARAM w, LPARAM l)
{ return dispatch(p, h, m, w, l, FALSE); }
LRESULT WINAPI wow_user_call_window_procW(WNDPROC p, HWND h, UINT m, WPARAM w, LPARAM l)
{ return dispatch(p, h, m, w, l, TRUE); }

int main(void)
{
    static const wow_edit_conversion_provider provider = { EditWndProcA, EditWndProcW,
        FNID_CALLWINDOWPROC, CPD_WND | CPD_ANSI_TO_UNICODE, CPD_WND | CPD_UNICODE_TO_ANSI };
    HWND h = (HWND)(ULONG_PTR)0x1234;
    unsigned direction;
    CHECK(wow_edit_conversion_bind(&provider));
    /* Link the production original table, never a fixture classification.
     * EditWndProcWorker's existing >=WM_USER guard owns the upper bound. */
    CHECK(MessageTable[WM_CREATE].bThunkMessage && MessageTable[WM_SETTEXT].bThunkMessage &&
        MessageTable[WM_GETTEXT].bThunkMessage && MessageTable[WM_GETTEXTLENGTH].bThunkMessage);
    CHECK(MessageTable[WM_CHAR].bThunkMessage && MessageTable[EM_REPLACESEL].bThunkMessage &&
        MessageTable[EM_GETLINE].bThunkMessage);
    CHECK(!MessageTable[WM_PAINT].bThunkMessage && !MessageTable[WM_DESTROY].bThunkMessage &&
        !MessageTable[EM_SETSEL].bThunkMessage && !MessageTable[WM_USER - 1].bThunkMessage);
    for (direction = 0; direction != 2; ++direction) {
        BOOL ansi = direction == 0;
        DWORD original = (DWORD)(ULONG_PTR)(ansi ? EditWndProcW : EditWndProcA);
        SetLastError(456);
        CHECK(wow_edit_CsSendMessage(h, WM_SETTEXT, 17, 19,
            original, 0x2b2, ansi) == -123);
        CHECK(GetLastError() == 777 && error_seen == 456);
        CHECK(options_seen == (ansi ? 0x21u : 0x22u));
        CHECK(target_seen == (DWORD)(ULONG_PTR)(ansi ? wow_edit_window_procW : wow_edit_window_procA));
        CHECK(wide_dispatch == !ansi);
        CHECK(message_seen == WM_SETTEXT && wp_seen == 17 && lp_seen == 19);
    }
    CHECK(requests == 2 && dispatches == 2);
    CHECK(!wow_edit_CsSendMessage(h, WM_SETTEXT, 0, 0,
        (DWORD)(ULONG_PTR)EditWndProcA, 0x2b2, TRUE));
    CHECK(GetLastError() == ERROR_NOT_SUPPORTED);
    CHECK(!wow_edit_CsSendMessage(h, WM_SETTEXT, 0, 0,
        (DWORD)(ULONG_PTR)EditWndProcW, 0, TRUE));
    CHECK(GetLastError() == ERROR_NOT_SUPPORTED);
    CHECK(requests == 2 && dispatches == 2);
    reject_cpd = TRUE;
    CHECK(!wow_edit_CsSendMessage(h, WM_SETTEXT, 0, 0,
        (DWORD)(ULONG_PTR)EditWndProcW, 0x2b2, TRUE));
    CHECK(GetLastError() == ERROR_INVALID_WINDOW_HANDLE);
    CHECK(requests == 3 && dispatches == 2);
    printf("conversion_transport_failures=%d\n", failures);
    return failures != 0;
}
