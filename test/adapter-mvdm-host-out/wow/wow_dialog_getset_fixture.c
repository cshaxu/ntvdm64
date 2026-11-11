#include "dialog-getset/precomp.h"
#include "wow_control_extent_bindings.h"
#include "wow_user_borrow_scope.h"
#include <stdio.h>

/* Links the three original get/set bodies and the actual access adapter.
 * CPD resolution, process authority and native style writes are explicit
 * doubles, not evidence of product service closure or guest acceptance. */
static DIALOG window;
static PWND selected = &window.wnd;
static DLG data;
static BOOL same_process;
static BOOL caller_ok = TRUE;
static CALLPROCDATA cpd;
static DWORD token, conversion, original_proc;
static unsigned failures, conversions, negatives;
static unsigned style_writes;
static DWORD written_style;
#define FIXTURE_WINDOW ((HWND)(ULONG_PTR)0x101)
#define CHECK(c) do { if (!(c)) { ++failures; printf("FAIL line=%d\n", __LINE__); } } while (0)
void fixture_check(BOOL condition, int line)
{ if (!condition) { ++failures; printf("FAIL original line=%d\n", line); } }
BOOL fixture_caller_ok(PWND value)
{ CHECK(value == selected); return caller_ok; }
PCALLPROCDATA fixture_validate_cpd(HANDLE handle, BYTE type)
{
    CHECK(type == TYPE_CALLPROC);
    return (ULONG_PTR)handle == 0xffff3456 ? &cpd : NULL;
}
LONG WINAPI wow_native_SetWindowLongA(HWND handle, int index, LONG data_value)
{
    CHECK(handle == FIXTURE_WINDOW && index == GWL_STYLE);
    ++style_writes; written_style = data_value;
    return 0x13572468;
}
BOOL fixture_same_process(PWND value)
{ CHECK(value == selected); return same_process; }
PWND WINAPI wow_window_client_validate_hwnd(HWND handle)
{
    if (handle == FIXTURE_WINDOW) return selected;
    SetLastError(ERROR_INVALID_WINDOW_HANDLE);
    return NULL;
}
/* Only the invalid-HWND path reaches the adapter's failure-owner query in
 * this fixture. Valid access goes through all three original owners. */
wow_window_words_binding *WINAPI wow_window_words_acquire(HWND handle)
{ CHECK(handle == NULL); SetLastError(ERROR_INVALID_WINDOW_HANDLE); return NULL; }
PWND WINAPI wow_window_words_client_state_value(wow_window_words_binding *binding)
{ (void)binding; CHECK(FALSE); return NULL; }
BOOL WINAPI wow_window_words_owns_client_state(wow_window_words_binding *binding, PWND view)
{ (void)binding; (void)view; CHECK(FALSE); return FALSE; }
void WINAPI wow_window_words_release(wow_window_words_binding *binding)
{ (void)binding; CHECK(FALSE); }
wow_window_words_binding *WINAPI wow_user_borrow_client_state(HWND handle, PWND view)
{ CHECK(handle == FIXTURE_WINDOW && view == selected);
  return (wow_window_words_binding *)(ULONG_PTR)1; }
LRESULT WINAPI wow_native_SendMessageA(HWND handle, UINT message, WPARAM wp, LPARAM lp)
{ (void)handle; (void)message; (void)wp; (void)lp; CHECK(FALSE); return 0; }
LRESULT WINAPI wow_native_SendMessageW(HWND handle, UINT message, WPARAM wp, LPARAM lp)
{ (void)handle; (void)message; (void)wp; (void)lp; CHECK(FALSE); return 0; }
DWORD WINAPI wow_edit_native_NtUserGetCPD(HWND value, DWORD flags, DWORD procedure)
{
    CHECK(value == FIXTURE_WINDOW);
    ++conversions; conversion = flags; original_proc = procedure;
    return token;
}
DWORD fixture_control_size(WORD fnid)
{
    DWORD extent = 0;
    CHECK(wow_control_registered_extent(fnid, &extent));
    return extent;
}
static void verify_registered_extents(void)
{
    static const WORD kinds[] = { FNID_BUTTON, FNID_COMBOBOX,
        FNID_COMBOLISTBOX, FNID_DIALOG, FNID_EDIT, FNID_LISTBOX,
        FNID_MDICLIENT, FNID_STATIC };
    static const DWORD extras[] = { 4, 4, 4, DLGWINDOWEXTRA,
        CBEDITEXTRA, 4, 8, 4 };
    DWORD extent;
    unsigned i;
    for (i = 0; i < sizeof(kinds) / sizeof(kinds[0]); ++i) {
        SetLastError(0x7341);
        CHECK(wow_control_registered_extent(kinds[i], &extent));
        CHECK(GetLastError() == 0x7341);
        CHECK(extent == sizeof(WND) + extras[i]);
        CHECK(wow_control_registered_extent(kinds[i] | FNID_STATUS_BITS, &extent));
        CHECK(extent == sizeof(WND) + extras[i]);
    }
    extent = 0x7341;
    CHECK(!wow_control_registered_extent(0, &extent));
    CHECK(extent == 0x7341 && GetLastError() == ERROR_INVALID_PARAMETER);
    CHECK(!wow_control_registered_extent(FNID_SCROLLBAR, &extent));
    CHECK(extent == 0x7341);
    CHECK(!wow_control_registered_extent(FNID_EDIT, NULL));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
}
DWORD WINAPI GetWindowData(PWND value, int index, BOOL ansi)
{
    CHECK(value == &window.wnd && index == GWL_STYLE && ansi);
    ++negatives; return 0x12345678;
}
static DWORD get(int index, BOOL ansi)
{ return _GetWindowLong(&window.wnd, index, ansi); }
static void verify_original_edit_publication(void)
{
    struct {
        EDITWND edit;
        BYTE extra[CBEDITEXTRA + sizeof(DWORD)];
    } storage = {0};
    EDITWND *edit = &storage.edit;
    DWORD application_word;
    edit->wnd.fnid = FNID_EDIT;
    edit->wnd.cbwndExtra = CBEDITEXTRA + sizeof(DWORD);
    selected = &edit->wnd;
    CHECK(!wow_window_client_set_window_long(FIXTURE_WINDOW, 0, 0x12345678, FALSE));
    CHECK((DWORD)edit->ped == 0x12345678);
    SetLastError(0);
    CHECK(!wow_window_client_set_window_long(FIXTURE_WINDOW, 0, 0x23456789, TRUE));
    CHECK(GetLastError() == ERROR_INVALID_INDEX && (DWORD)edit->ped == 0x12345678);
    CHECK(!wow_window_client_set_window_long(FIXTURE_WINDOW, 1, 0, FALSE));
    CHECK(GetLastError() == ERROR_INVALID_INDEX && (DWORD)edit->ped == 0x12345678);
    /* Canonical EDIT private bytes are six, even though PED occupies four.
     * A superclass may append public application data beyond that boundary. */
    SetLastError(0);
    CHECK(!wow_window_client_set_window_long(FIXTURE_WINDOW, 4, 0xabcdef12, TRUE));
    CHECK(GetLastError() == ERROR_INVALID_INDEX);
    SetLastError(0);
    CHECK(!wow_window_client_set_window_long(FIXTURE_WINDOW, 5, 0xabcdef12, TRUE));
    CHECK(GetLastError() == ERROR_INVALID_INDEX);
    SetLastError(0x2468);
    CHECK(!wow_window_client_set_window_long(FIXTURE_WINDOW, CBEDITEXTRA, 0x12345678, TRUE));
    CHECK(GetLastError() == 0x2468);
    CopyMemory(&application_word, (BYTE *)(&edit->wnd + 1) +
        CBEDITEXTRA, sizeof(application_word));
    CHECK(application_word == 0x12345678);
    CHECK(_GetWindowLong(&edit->wnd, CBEDITEXTRA, TRUE) == 0x12345678);
    CHECK(!_GetWindowLong(&edit->wnd, 4, TRUE));
    CHECK(GetLastError() == ERROR_INVALID_INDEX);
    CHECK(!_GetWindowLong(&edit->wnd, 5, TRUE));
    CHECK(GetLastError() == ERROR_INVALID_INDEX);
    caller_ok = FALSE;
    SetWF(&edit->wnd, WFDESTROYED);
    CHECK(!wow_window_client_set_window_long(FIXTURE_WINDOW, 0, 0, FALSE));
    CHECK((DWORD)edit->ped == 0x12345678);
    caller_ok = TRUE;
    CHECK(wow_window_client_set_window_long(FIXTURE_WINDOW, 0, 0, FALSE) == 0x12345678);
    CHECK(edit->ped == NULL);
    /* Original EDIT index zero exposes the first PED word only to its own
     * process; a foreign caller receives the slot address, not its content. */
    application_word = 0x87654321;
    ClrWF(&edit->wnd, WFDESTROYED);
    CHECK(!wow_window_client_set_window_long(FIXTURE_WINDOW, 0,
        (LONG)(ULONG_PTR)&application_word, FALSE));
    same_process = TRUE;
    CHECK(_GetWindowLong(&edit->wnd, 0, FALSE) == application_word);
    same_process = FALSE;
    CHECK(_GetWindowLong(&edit->wnd, 0, FALSE) == (DWORD)(ULONG_PTR)(&edit->wnd + 1));
    same_process = TRUE;
    SetWF(&edit->wnd, WFDESTROYED);
    CHECK(wow_window_client_set_window_long(FIXTURE_WINDOW, 0, 0, FALSE) ==
        (LONG)(ULONG_PTR)&application_word);
    CHECK(!edit->ped);
    selected = &window.wnd;
}
static void verify_preserved_listbox_guard(void)
{
    LBWND list = {0};
    LBIV data = {0};
    DWORD proposed = WS_VISIBLE | LBS_OWNERDRAWFIXED;
    LONG previous;
    list.wnd.fnid = FNID_LISTBOX;
    list.wnd.head.h = FIXTURE_WINDOW;
    list.wnd.style = LBS_HASSTRINGS;
    list.pLBIV = &data;
    selected = &list.wnd;
    same_process = TRUE;
    data.fInitialized = TRUE;
    CHECK(wow_window_client_try_set_long(FIXTURE_WINDOW, GWL_STYLE, proposed, TRUE, &previous));
    CHECK((DWORD)previous == 0x13572468);
    CHECK(style_writes == 1 && written_style == (WS_VISIBLE | LBS_HASSTRINGS));
    data.fInitialized = FALSE;
    CHECK(wow_window_client_try_set_long(FIXTURE_WINDOW, GWL_STYLE, proposed, TRUE, &previous));
    CHECK((DWORD)previous == 0x13572468);
    CHECK(style_writes == 2 && written_style == proposed);
    same_process = FALSE;
    CHECK(wow_window_client_try_set_long(FIXTURE_WINDOW, GWL_STYLE, proposed, TRUE, &previous));
    CHECK((DWORD)previous == 0x13572468);
    CHECK(style_writes == 3 && written_style == (WS_VISIBLE | LBS_HASSTRINGS));
    list.pLBIV = NULL;
    CHECK(wow_window_client_try_set_long(FIXTURE_WINDOW, GWL_STYLE, proposed, TRUE, &previous));
    CHECK((DWORD)previous == 0x13572468);
    CHECK(style_writes == 4 && written_style == proposed);
    same_process = TRUE;
    selected = &window.wnd;
}
int main(void)
{
    unsigned before;
    LONG result;
    window.wnd.fnid = FNID_DIALOG;
    window.wnd.head.h = FIXTURE_WINDOW;
    window.wnd.cbwndExtra = DLGWINDOWEXTRA;
    SetWF(&window.wnd, WFDIALOGWINDOW);
    window.pdlg = &data;
    window.resultWP = -17;
    window.unused = 0x2468;
    same_process = TRUE;
    data.lpfnDlg = (WNDPROC)(ULONG_PTR)0x12345678;
    data.flags = DLGF_ANSI;
    CHECK(get(DWL_MSGRESULT, TRUE) == (DWORD)-17);
    CHECK(get(DWL_USER, FALSE) == 0x2468);
    CHECK(get(DWL_DLGPROC, TRUE) == 0x12345678 && !conversions);
    token = 0xffff1234;
    CHECK(get(DWL_DLGPROC, FALSE) == token && conversions == 1);
    CHECK(conversion == (CPD_DIALOG | CPD_UNICODE_TO_ANSI));
    CHECK(original_proc == 0x12345678);
    data.flags = 0;
    CHECK(get(DWL_DLGPROC, FALSE) == 0x12345678 && conversions == 1);
    CHECK(get(DWL_DLGPROC, TRUE) == token && conversions == 2);
    CHECK(conversion == (CPD_DIALOG | CPD_ANSI_TO_UNICODE));
    token = 0;
    CHECK(get(DWL_DLGPROC, TRUE) == 0x12345678 && conversions == 3);
    same_process = FALSE;
    SetLastError(0);
    CHECK(!get(DWL_DLGPROC, TRUE) && GetLastError() == ERROR_ACCESS_DENIED);
    CHECK(conversions == 3);
    CHECK(get(DWL_MSGRESULT, TRUE) == (DWORD)-17);
    same_process = TRUE;
    data.lpfnDlg = NULL;
    before = conversions;
    CHECK(!get(DWL_DLGPROC, TRUE) && conversions == before);
    SetLastError(0);
    CHECK(!get(DWL_DLGPROC + 1, TRUE));
    CHECK(GetLastError() == ERROR_PRIVATE_DIALOG_INDEX);
    SetLastError(0);
    CHECK(!get(DLGWINDOWEXTRA, TRUE) && GetLastError() == ERROR_INVALID_INDEX);
    CHECK(get(GWL_STYLE, TRUE) == 0x12345678 && negatives == 1);
    /* Link the real ownership selector too. A handled zero/error must never
     * be reinterpreted as permission to read the native dialog instead. */
    result = 99;
    CHECK(wow_window_client_try_get_long(FIXTURE_WINDOW, DWL_MSGRESULT, TRUE, &result));
    CHECK(result == -17);
    SetLastError(0);
    CHECK(wow_window_client_try_get_long(FIXTURE_WINDOW, DWL_DLGPROC + 1, TRUE, &result));
    CHECK(!result && GetLastError() == ERROR_PRIVATE_DIALOG_INDEX);
    window.resultWP = 0;
    CHECK(wow_window_client_try_get_long(FIXTURE_WINDOW, DWL_MSGRESULT, TRUE, &result));
    CHECK(!result);
    window.resultWP = -17;
    result = 99;
    SetLastError(0x1234);
    CHECK(!wow_window_client_try_get_long(NULL, DWL_USER, TRUE, &result));
    CHECK(result == 99 && GetLastError() == 0x1234);
    CHECK(!wow_window_client_try_get_long(FIXTURE_WINDOW, GWL_STYLE, TRUE, &result));
    CHECK(result == 99 && GetLastError() == 0x1234 && negatives == 1);
    ClrWF(&window.wnd, WFDIALOGWINDOW);
    window.wnd.fnid = 0; /* Ordinary recovered class-extra storage. */
    CHECK(wow_window_client_try_get_long(FIXTURE_WINDOW, DWL_USER, TRUE, &result));
    CHECK(result == 0x2468 && GetLastError() == 0x1234);
    window.wnd.fnid = FNID_DIALOG;
    SetWF(&window.wnd, WFDIALOGWINDOW);
    CHECK(window.resultWP == -17 && window.unused == 0x2468 && window.pdlg == &data);
    CHECK(wow_window_client_try_set_long(FIXTURE_WINDOW, DWL_MSGRESULT, 81, TRUE, &result));
    CHECK(result == -17 && window.resultWP == 81);
    CHECK(get(DWL_MSGRESULT, TRUE) == 81);
    CHECK(wow_window_client_try_set_long(FIXTURE_WINDOW, DWL_USER, 0x1357, TRUE, &result));
    CHECK(result == 0x2468 && get(DWL_USER, TRUE) == 0x1357);
    caller_ok = FALSE;
    CHECK(wow_window_client_try_set_long(FIXTURE_WINDOW, DWL_USER, 7, TRUE, &result));
    CHECK(!result && window.unused == 0x1357 && GetLastError() == ERROR_INVALID_INDEX);
    caller_ok = TRUE;
    data.lpfnDlg = (WNDPROC)(ULONG_PTR)0x12345678;
    data.flags = DLGF_ANSI;
    CHECK(wow_window_client_try_set_long(FIXTURE_WINDOW, DWL_DLGPROC, 0x23456789, TRUE, &result));
    CHECK(result == 0x12345678 && (ULONG_PTR)data.lpfnDlg == 0x23456789);
    token = 0xffff1234;
    CHECK(wow_window_client_try_set_long(FIXTURE_WINDOW, DWL_DLGPROC, 0x3456789a, FALSE, &result));
    CHECK((DWORD)result == token && !(data.flags & DLGF_ANSI));
    CHECK(conversion == (CPD_DIALOG | CPD_UNICODE_TO_ANSI));
    cpd.pfnClientPrevious = 0x456789ab;
    cpd.wType = CPD_UNICODE_TO_ANSI;
    CHECK(wow_window_client_try_set_long(FIXTURE_WINDOW, DWL_DLGPROC, (LONG)0xffff3456, FALSE, &result));
    CHECK((ULONG_PTR)data.lpfnDlg == 0x456789ab && (data.flags & DLGF_ANSI));
    same_process = FALSE;
    CHECK(wow_window_client_try_set_long(FIXTURE_WINDOW, DWL_DLGPROC, 0, TRUE, &result));
    CHECK(!result && GetLastError() == ERROR_ACCESS_DENIED);
    CHECK((ULONG_PTR)data.lpfnDlg == 0x456789ab);
    same_process = TRUE;
    CHECK(wow_window_client_try_set_long(FIXTURE_WINDOW, DWL_DLGPROC + 1, 0, TRUE, &result));
    CHECK(!result && GetLastError() == ERROR_PRIVATE_DIALOG_INDEX);
    result = 99;
    SetLastError(0x1234);
    CHECK(!wow_window_client_try_set_long(NULL, DWL_USER, 7, TRUE, &result));
    CHECK(result == 99 && GetLastError() == 0x1234);
    verify_preserved_listbox_guard();
    verify_registered_extents();
    verify_original_edit_publication();
    printf("dialog_getset_failures=%u conversions=%u style_writes=%u\n",
        failures, conversions, style_writes);
    return failures != 0;
}
