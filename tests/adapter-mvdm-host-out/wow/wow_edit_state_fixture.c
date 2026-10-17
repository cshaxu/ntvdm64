#include "wow_edit_state_bindings.h"
#include "wow_dialog_state_bindings.h"
#include "wow_edit_entry_bindings.h"
#include "wow_user_borrow_scope.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"
#include "opennt-abi/source/public/internal/windows/inc/wingdip.h"
#include <stdio.h>
#include <string.h>

/* Supplemental original FNID and host-storage contract. Not real EDIT
 * allocation/teardown or guest acceptance. Link original kernel/ntstubs.c
 * in its scoped EDIT selection, never the native NtUserSetWindowFNID. */
static unsigned failures;
static unsigned final_calls;
static BOOL reject_final;
static unsigned gettext_calls;
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)
/* Boundary double, not an EDIT implementation. Tests finalizer selection
 * and outcome checks only; real original cleanup remains a separate gate. */
LONG WINAPI EditWndProcW(HWND window, UINT message, WPARAM wp, LONG lp)
{
    ++final_calls;
    CHECK(message == 0x0070 && wp == 0 && lp == 0);
    if (!reject_final) CHECK(wow_edit_set_window_fnid(window, FNID_CLEANEDUP_BIT));
    return 0;
}
static WORD final_selected;
LONG WINAPI ComboBoxWndProcW(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    final_selected = FNID_COMBOBOX;
    return EditWndProcW(window, message, wp, lp);
}
LONG WINAPI ListBoxWndProcW(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    final_selected = FNID_LISTBOX;
    return EditWndProcW(window, message, wp, lp);
}
static void terminal_cases(HWND window, PEDITWND edit)
{
    SetLastError(0x4567);
    CHECK(wow_control_finalize_window(window));
    CHECK(GetLastError() == 0x4567 && final_calls == 0);
    edit->wnd.fnid = FNID_BUTTON;
    CHECK(!wow_control_finalize_window(window));
    CHECK(GetLastError() == ERROR_INVALID_STATE && final_calls == 0);
    edit->wnd.fnid = FNID_EDIT | FNID_DELETED_BIT;
    CHECK(!wow_control_finalize_window(window));
    CHECK(final_calls == 0);
    edit->wnd.fnid = FNID_EDIT | FNID_CLEANEDUP_BIT;
    edit->ped = (PED)(ULONG_PTR)1; /* Poison: must never be dereferenced. */
    CHECK(wow_control_finalize_window(window) && final_calls == 0);
    edit->wnd.fnid = FNID_EDIT;
    reject_final = TRUE;
    CHECK(!wow_control_finalize_window(window));
    CHECK(GetLastError() == ERROR_INVALID_STATE && final_calls == 1);
    reject_final = FALSE;
    SetLastError(0x7654);
    CHECK(wow_control_finalize_window(window));
    CHECK(GetLastError() == 0x7654 && final_calls == 2);
    CHECK(wow_control_finalize_window(window) && final_calls == 2);
    edit->wnd.fnid = FNID_COMBOBOX;
    CHECK(wow_control_finalize_window(window));
    CHECK(final_selected == FNID_COMBOBOX && final_calls == 3);
    CHECK(wow_control_finalize_window(window) && final_calls == 3);
    edit->wnd.fnid = FNID_LISTBOX;
    CHECK(wow_control_finalize_window(window));
    CHECK(final_selected == FNID_LISTBOX && final_calls == 4);
    CHECK(wow_control_finalize_window(window) && final_calls == 4);
    /* Reset synthetic state for the independent original FNID cases. */
    edit->wnd.fnid = 0;
    edit->ped = NULL;
}
static LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    if (message == WM_GETTEXT || message == WM_GETTEXTLENGTH) {
        ++gettext_calls;
        return 0; /* Stored-name sampling must not call application getters. */
    }
    if (message == WM_NCDESTROY) wow_window_words_detach(window);
    return DefWindowProcA(window, message, wp, lp);
}
static void name_cases(HWND window, wow_window_words_binding *binding)
{
    WCHAR title[601], *captured = (WCHAR *)(ULONG_PTR)1;
    ULONG length = 17, capacity = 23;
    PWND state;
    unsigned i;
    for (i = 0; i < 600; ++i) title[i] = L'A' + i % 26;
    title[600] = 0;
    CHECK(SetWindowTextW(window, title));
    SetLastError(0x7654);
    CHECK(wow_window_client_refresh(window));
    CHECK(GetLastError() == 0x7654);
    state = wow_window_words_lock_client_name(binding);
    CHECK(state != NULL);
    if (state) {
        CHECK(state->strName.Length == 1200 && !state->strName.bAnsi);
        CHECK(state->strName.MaximumLength >= 1202);
        CHECK(lstrcmpW(state->strName.Buffer, title) == 0);
        wow_window_words_unlock_client_name(binding);
    }
    CHECK(!wow_window_client_capture_name(NULL, &captured, &length, &capacity));
    CHECK(captured == (WCHAR *)(ULONG_PTR)1 && length == 17 && capacity == 23);
    state = wow_window_words_lock_client_name(binding);
    if (state) {
        CHECK(state->strName.Length == 1200);
        wow_window_words_unlock_client_name(binding);
    }
    CHECK(SetWindowTextW(window, L""));
    CHECK(wow_window_client_refresh(window));
    state = wow_window_words_lock_client_name(binding);
    if (state) {
        CHECK(state->strName.Length == 0 && state->strName.Buffer[0] == 0);
        wow_window_words_unlock_client_name(binding);
    }
    CHECK(gettext_calls == 0);
}
typedef struct name_reader_case {
    wow_window_words_binding *binding;
    HANDLE ready;
    volatile LONG stop, reads, errors;
} name_reader_case;

static DWORD WINAPI name_reader_thread(void *parameter)
{
    name_reader_case *test = parameter;
    for (;;) {
        PWND state = wow_window_words_lock_client_name(test->binding);
        BOOL valid = state != NULL;
        if (state) {
            ULONG i, chars = state->strName.Length / sizeof(WCHAR);
            WCHAR expected = chars == 300 ? L'X' : L'Y';
            valid = (chars == 300 || chars == 600) && !state->strName.bAnsi &&
                state->strName.MaximumLength >= (chars + 1) * sizeof(WCHAR);
            if (valid) {
                for (i = 0; i < chars; ++i)
                    if (state->strName.Buffer[i] != expected) valid = FALSE;
                if (state->strName.Buffer[chars] != 0) valid = FALSE;
            }
            wow_window_words_unlock_client_name(test->binding);
        }
        if (!valid) InterlockedIncrement(&test->errors);
        InterlockedIncrement(&test->reads);
        SetEvent(test->ready);
        if (InterlockedCompareExchange(&test->stop, 0, 0)) return 0;
        SwitchToThread();
    }
}

static void name_concurrency_case(HWND window, wow_window_words_binding *binding)
{
    name_reader_case test = {0};
    WCHAR short_name[301], long_name[601];
    HANDLE thread;
    unsigned i;
    for (i = 0; i < 300; ++i) short_name[i] = L'X';
    short_name[300] = 0;
    for (i = 0; i < 600; ++i) long_name[i] = L'Y';
    long_name[600] = 0;
    CHECK(SetWindowTextW(window, short_name));
    CHECK(wow_window_client_refresh(window));
    test.binding = binding; /* Caller's reference outlives the joined reader. */
    test.ready = CreateEventW(NULL, TRUE, FALSE, NULL);
    CHECK(test.ready != NULL);
    if (!test.ready) return;
    thread = CreateThread(NULL, 0, name_reader_thread, &test, 0, NULL);
    CHECK(thread != NULL);
    if (!thread) { CloseHandle(test.ready); return; }
    CHECK(WaitForSingleObject(test.ready, 10000) == WAIT_OBJECT_0);
    for (i = 0; i < 100; ++i) {
        CHECK(SetWindowTextW(window, i & 1 ? short_name : long_name));
        CHECK(wow_window_client_refresh(window));
    }
    InterlockedExchange(&test.stop, 1);
    /* Never return stack storage or release its binding with a live reader. */
    if (WaitForSingleObject(thread, 10000) != WAIT_OBJECT_0) ExitProcess(3);
    CHECK(test.reads > 0 && test.errors == 0);
    CloseHandle(thread);
    CloseHandle(test.ready);
    CHECK(SetWindowTextW(window, L""));
    CHECK(wow_window_client_refresh(window));
    CHECK(gettext_calls == 0);
}
static void creation_case(HINSTANCE instance, BOOL ansi, DWORD version,
    DWORD flags, BOOL win31, BOOL win40, BOOL old_ui, BOOL ncpaint)
{
    HWND window;
    WW words = {0};
    wow_window_words_binding *binding = NULL;
    wow_user_borrow_scope scope;
    BOOL entered = FALSE;
    PEDITWND edit;
    int extra_bytes;
    SIZE_T i, allocated;
    DLG dialog_data = {0};
    PDIALOG dialog_view;
    window = ansi ? CreateWindowExA(0, "WOW_EDIT_STATE_FIXTURE", "", 0,
        0, 0, 32, 32, NULL, NULL, instance, NULL) :
        CreateWindowExW(0, L"WOW_EDIT_STATE_FIXTURE", L"", 0,
        0, 0, 32, 32, NULL, NULL, instance, NULL);
    CHECK(window != NULL);
    if (!window) return;
    CHECK(wow_window_words_attach(window, &words));
    binding = wow_window_words_acquire(window);
    CHECK(binding != NULL);
    if (!binding) goto done;
    CHECK(wow_window_words_set_creation_ansi(binding, ansi));
    entered = wow_user_borrow_enter(&scope);
    CHECK(entered);
    if (!entered) goto done;
    extra_bytes = GetClassLongA(window, GCL_CBWNDEXTRA);
    CHECK(extra_bytes > (int)sizeof(PED));
    CHECK(wow_window_words_client_state_value(binding) == NULL);
    SetLastError(0x7654);
    CHECK(wow_window_client_create_state(window, version, flags, extra_bytes));
    CHECK(GetLastError() == 0x7654);
    edit = (PEDITWND)wow_window_words_client_state_value(binding);
    CHECK(edit != NULL);
    if (!edit) goto done;
    CHECK(wow_window_words_owns_client_state(binding, &edit->wnd));
    CHECK(wow_window_client_validate_hwnd(window) == &edit->wnd);
    CHECK(!wow_window_client_create_state(window, version, flags, extra_bytes));
    CHECK(GetLastError() == ERROR_ALREADY_EXISTS);
    CHECK(wow_window_words_client_state_value(binding) == &edit->wnd);
    CHECK(!!TestWF(&edit->wnd, WFANSICREATOR) == !!ansi);
    CHECK(!!TestWF(&edit->wnd, WFWIN31COMPAT) == !!win31);
    CHECK(!!TestWF(&edit->wnd, WFWIN40COMPAT) == !!win40);
    CHECK(!!TestWF(&edit->wnd, WFOLDUI) == !!old_ui);
    CHECK(!!TestWF(&edit->wnd, WFALWAYSSENDNCPAINT) == !!ncpaint);
    CHECK(edit->wnd.dwExpWinVer == LOWORD(version));
    CHECK(edit->wnd.fnid == 0 && edit->ped == NULL);
    CHECK(edit->wnd.cbwndExtra == extra_bytes);
    allocated = HeapSize(GetProcessHeap(), 0, edit);
    CHECK(allocated != (SIZE_T)-1 && allocated >= sizeof(WND) + extra_bytes);
    if (allocated == (SIZE_T)-1 || allocated < sizeof(WND) + extra_bytes)
        goto done; /* Diagnose a short allocation without overflowing it. */
    for (i = sizeof(PED); i < (SIZE_T)extra_bytes; ++i)
        CHECK(((BYTE *)edit)[sizeof(WND) + i] == 0);
    ((BYTE *)edit)[sizeof(WND) + extra_bytes - 1] = 0x5a;
    CHECK(wow_window_client_refresh(window));
    CHECK(((BYTE *)edit)[sizeof(WND) + extra_bytes - 1] == 0x5a);
    /* Same physical WND/class-extra owner; no EDIT-specific allocation or
     * second dialog identity is needed. Exercise the actual publication
     * boundary and unchanged original SetDialogPointer before retirement. */
    dialog_view = (PDIALOG)&edit->wnd;
    dialog_view->resultWP = 37;
    dialog_view->unused = 91;
    edit->wnd.cbwndExtra = sizeof(PED);
    CHECK(!wow_dialog_set_pointer(window, &dialog_data));
    CHECK(GetLastError() == ERROR_INVALID_INDEX);
    CHECK(!dialog_view->pdlg && edit->wnd.fnid == 0);
    edit->wnd.cbwndExtra = extra_bytes;
    SetLastError(0x1234);
    CHECK(wow_dialog_set_pointer(window, &dialog_data));
    CHECK(GetLastError() == 0x1234);
    CHECK(dialog_view->pdlg == &dialog_data);
    CHECK(edit->wnd.fnid == FNID_DIALOG && TestWF(&edit->wnd, WFDIALOGWINDOW));
    CHECK(wow_window_client_validate_hwnd(window) == &edit->wnd);
    CHECK(wow_dialog_set_pointer(window, NULL));
    CHECK(!dialog_view->pdlg && !TestWF(&edit->wnd, WFDIALOGWINDOW));
    CHECK(edit->wnd.fnid == (FNID_DIALOG | FNID_CLEANEDUP_BIT));
    CHECK(dialog_view->resultWP == 37 && dialog_view->unused == 91);
    CHECK(((BYTE *)edit)[sizeof(WND) + extra_bytes - 1] == 0x5a);
done:
    CHECK(DestroyWindow(window));
    wow_window_words_release(binding);
    if (entered) CHECK(wow_user_borrow_leave(&scope));
}
static void child_id_case(HINSTANCE instance)
{
    HWND parent, child;
    WW words = {0};
    wow_window_words_binding *binding = NULL;
    wow_user_borrow_scope scope;
    BOOL entered = FALSE;
    PWND state;
    parent = CreateWindowExA(0, "WOW_EDIT_STATE_FIXTURE", "", 0,
        0, 0, 32, 32, NULL, NULL, instance, NULL);
    CHECK(parent != NULL);
    if (!parent) return;
    child = CreateWindowExA(0, "WOW_EDIT_STATE_FIXTURE", "", WS_CHILD,
        0, 0, 16, 16, parent, (HMENU)(ULONG_PTR)0xfe01, instance, NULL);
    CHECK(child != NULL);
    if (!child) { CHECK(DestroyWindow(parent)); return; }
    CHECK(wow_window_words_attach(child, &words));
    binding = wow_window_words_acquire(child);
    CHECK(binding != NULL);
    if (!binding) goto done;
    CHECK(wow_window_words_set_creation_ansi(binding, TRUE));
    entered = wow_user_borrow_enter(&scope);
    CHECK(entered);
    if (!entered) goto done;
    SetLastError(0x2345);
    CHECK(wow_window_client_create_state(child, 0x030a, 0, 64));
    CHECK(GetLastError() == 0x2345);
    state = wow_window_words_client_state_value(binding);
    CHECK(state && TestwndChild(state));
    if (!state) goto done;
    CHECK((DWORD)(ULONG_PTR)state->spmenu == 0xfe01);
    CHECK(SetWindowLongA(child, GWL_ID, 0) == 0xfe01);
    CHECK(wow_window_client_ensure_state(child));
    CHECK(state->spmenu == NULL); /* ID zero is valid, not a query failure. */
    SetLastError(ERROR_SUCCESS);
    CHECK(SetWindowLongA(child, GWL_ID, (LONG)0xfffffe02) == 0);
    CHECK(GetLastError() == ERROR_SUCCESS);
    CHECK(wow_window_client_ensure_state(child));
    CHECK((DWORD)(ULONG_PTR)state->spmenu == 0xfffffe02);
    {
        WINDOWINFO info = {sizeof(info)};
        WINDOWINFO unchanged;
        DWORD sampled_id = 0x76543210;
        SetLastError(0x2345);
        CHECK(wow_window_words_refresh_info(child, binding, &info, &sampled_id));
        CHECK(GetLastError() == 0x2345);
        CHECK(sampled_id == 0xfffffe02);
        CHECK(sampled_id == (DWORD)(ULONG_PTR)state->spmenu);
        unchanged = info;
        sampled_id = 0x76543210;
        CHECK(!wow_window_words_refresh_info(NULL, binding, &info, &sampled_id));
        CHECK(sampled_id == 0x76543210);
        CHECK(memcmp(&info, &unchanged, sizeof(info)) == 0);
        CHECK(GetWindowInfo(child, &info));
        /* A committed observation supplies the ID; do not mix it with a
         * newly sampled native value while retaining earlier geometry. */
        CHECK(wow_window_words_refresh_observed(child, &info, 0x4321, 0x76543210));
        CHECK((DWORD)(ULONG_PTR)state->hModule == 0x76543210);
        CHECK((DWORD)(ULONG_PTR)wow_window_words_value(binding)->hInstance == 0x76543210);
        CHECK(wow_window_words_refresh_observed(child, &info, 0x4321, 0));
        CHECK(state->hModule == NULL && wow_window_words_value(binding)->hInstance == NULL);
        CHECK((DWORD)(ULONG_PTR)state->spmenu == 0x4321);
        CHECK((DWORD)GetWindowLongA(child, GWL_ID) == 0xfffffe02);
        CHECK(wow_window_client_ensure_state(child));
        CHECK((DWORD)(ULONG_PTR)state->spmenu == 0xfffffe02);
        CHECK((DWORD)(ULONG_PTR)state->hModule == (DWORD)GetWindowLongA(child, GWL_HINSTANCE));
    }
    CHECK(wow_window_words_client_state_value(binding) == state);
done:
    CHECK(DestroyWindow(child));
    wow_window_words_release(binding);
    if (entered) CHECK(wow_user_borrow_leave(&scope));
    CHECK(DestroyWindow(parent));
}

int __cdecl main(void)
{
    WNDCLASSA wc = {0};
    HWND window;
    WW words = {0};
    wow_window_words_binding *binding;
    wow_user_borrow_scope scope;
    PEDITWND edit;
    WINDOWINFO info = {sizeof(info)};
    WND copied_state;
    wow_window_creation_facts facts = {0x1234030a, 0, 64};
    wow_window_creation_facts observed = {0xfeed, 0xbeef, 99};
    wc.lpfnWndProc = window_proc;
    wc.cbWndExtra = facts.class_extra_bytes;
    wc.hInstance = GetModuleHandleA(NULL);
    wc.lpszClassName = "WOW_EDIT_STATE_FIXTURE";
    CHECK(RegisterClassA(&wc));
    window = CreateWindowExA(0, wc.lpszClassName, "", 0, 0, 0, 32, 32,
        NULL, NULL, wc.hInstance, NULL);
    CHECK(window != NULL);
    if (!window) return 3;
    CHECK(GetWindowInfo(window, &info));
    SetLastError(0x2345);
    CHECK(wow_window_words_refresh_observed(window, &info, 0, (DWORD)(ULONG_PTR)wc.hInstance));
    CHECK(GetLastError() == 0x2345);
    CHECK(!wow_window_words_acquire(window));
    CHECK(GetLastError() == ERROR_NOT_FOUND); /* No implicit enrollment. */
    CHECK(wow_window_words_attach(window, &words));
    binding = wow_window_words_acquire(window);
    CHECK(binding != NULL);
    if (!binding) return 3;
    CHECK(wow_window_client_validate_hwnd(window) == NULL);
    CHECK(GetLastError() == ERROR_INVALID_STATE);
    CHECK(wow_user_borrow_enter(&scope));
    CHECK(wow_window_client_validate_hwnd(window) == NULL);
    CHECK(GetLastError() == ERROR_INVALID_STATE);
    CHECK(!wow_window_client_create_state(window, 0x030a, 0, -1));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    CHECK(!wow_window_client_create_state(window, 0x030a, 0, wc.cbWndExtra));
    CHECK(GetLastError() == ERROR_NOT_FOUND);
    CHECK(wow_window_words_set_creation_ansi(binding, TRUE));
    CHECK(!wow_window_words_creation_facts(binding, &observed));
    CHECK(GetLastError() == ERROR_NOT_FOUND);
    CHECK(observed.module_version == 0xfeed && observed.class_extra_bytes == 99);
    CHECK(!wow_window_client_ensure_state(window));
    CHECK(GetLastError() == ERROR_NOT_FOUND);
    CHECK(wow_window_words_set_creation_facts(binding, &facts));
    CHECK(wow_window_words_set_creation_facts(binding, &facts));
    ++facts.module_version;
    CHECK(!wow_window_words_set_creation_facts(binding, &facts));
    CHECK(GetLastError() == ERROR_INVALID_STATE);
    --facts.module_version;
    CHECK(wow_window_words_creation_facts(binding, &observed));
    CHECK(observed.module_version == facts.module_version &&
        observed.compatibility_flags == facts.compatibility_flags &&
        observed.class_extra_bytes == facts.class_extra_bytes);
    SetLastError(0x1357);
    CHECK(wow_window_client_ensure_state(window));
    CHECK(GetLastError() == 0x1357);
    edit = (PEDITWND)wow_window_words_client_state_value(binding);
    CHECK(edit != NULL);
    if (!edit) return 3;
    CHECK(edit->wnd.head.h == window && edit->wnd.head.pSelf == (PBYTE)edit);
    SetLastError(0x3456);
    CHECK(wow_window_client_owns_state(&edit->wnd));
    CHECK(GetLastError() == 0x3456);
    copied_state = edit->wnd;
    CHECK(!wow_window_client_owns_state(&copied_state));
    CHECK(GetLastError() == 0x3456);
    CHECK(!wow_window_client_owns_state(NULL));
    CHECK(GetLastError() == 0x3456);
    CHECK(TestWF(&edit->wnd, WFANSICREATOR));
    CHECK(TestWF(&edit->wnd, WFWIN31COMPAT));
    CHECK(!TestWF(&edit->wnd, WFWIN40COMPAT));
    CHECK(!TestWF(&edit->wnd, WFOLDUI));
    CHECK(edit->wnd.dwExpWinVer == 0x030a && edit->ped == NULL);
    name_cases(window, binding);
    name_concurrency_case(window, binding);
    terminal_cases(window, edit);
    CHECK(!wow_window_client_create_state(window, 0x0400, 0, wc.cbWndExtra));
    CHECK(GetLastError() == ERROR_ALREADY_EXISTS);
    CHECK(wow_window_words_client_state_value(binding) == &edit->wnd);
    CHECK(wow_window_client_ensure_state(window));
    CHECK(wow_window_words_client_state_value(binding) == &edit->wnd);
    SetLastError(0x1234);
    CHECK(wow_window_client_validate_hwnd(window) == &edit->wnd);
    CHECK(GetLastError() == 0x1234);
    CHECK(wow_edit_set_window_fnid(window, FNID_EDIT));
    CHECK(edit->wnd.fnid == FNID_EDIT);
    CHECK(!wow_edit_set_window_fnid(window, FNID_BUTTON));
    CHECK(edit->wnd.fnid == FNID_EDIT);
    CHECK(!wow_edit_set_window_fnid(window, FNID_EDIT));
    CHECK(wow_window_client_set_window_long(window, 0, 0x12345678, FALSE) == 0);
    CHECK((LONG)edit->ped == 0x12345678);
    edit->wnd.state = 0x11223344;
    edit->wnd.state2 = 0x55667788;
    edit->wnd.dwExpWinVer = 0x030a;
    CHECK(MoveWindow(window, 13, 17, 80, 96, FALSE));
    CHECK(GetWindowInfo(window, &info));
    SetLastError(0x5678);
    /* Exercise the actual callback-entry refresh owner, not only the
     * explicit recovered-state helper. Both views consume one sample. */
    CHECK(wow_window_words_refresh(window, binding));
    CHECK(GetLastError() == 0x5678);
    CHECK(EqualRect(&edit->wnd.rcWindow, &info.rcWindow));
    CHECK(EqualRect(&edit->wnd.rcClient, &info.rcClient));
    CHECK(edit->wnd.style == info.dwStyle && edit->wnd.ExStyle == info.dwExStyle);
    CHECK(edit->wnd.hModule == wc.hInstance);
    CHECK(edit->wnd.state == 0x11223344 && edit->wnd.state2 == 0x55667788);
    CHECK(edit->wnd.dwExpWinVer == 0x030a && edit->wnd.fnid == FNID_EDIT);
    CHECK((LONG)edit->ped == 0x12345678);
    /* Canonical native control dispatch bypasses the WOW-procedure refresh
     * above. Its ordinary entry uses ensure_state on every message, not
     * merely on first allocation. Keep one allocation and private payload. */
    CHECK(MoveWindow(window, 19, 29, 100, 120, FALSE));
    SetWindowLongA(window, GWL_STYLE,
        GetWindowLongA(window, GWL_STYLE) | WS_DISABLED);
    SetWindowLongA(window, GWL_EXSTYLE,
        GetWindowLongA(window, GWL_EXSTYLE) | WS_EX_CLIENTEDGE);
    CHECK(GetWindowInfo(window, &info));
    CHECK((info.dwStyle & WS_DISABLED) != 0);
    CHECK((info.dwExStyle & WS_EX_CLIENTEDGE) != 0);
    SetLastError(0x789a);
    CHECK(wow_window_client_ensure_state(window));
    CHECK(GetLastError() == 0x789a);
    CHECK(wow_window_words_client_state_value(binding) == &edit->wnd);
    CHECK(EqualRect(&edit->wnd.rcWindow, &info.rcWindow));
    CHECK(EqualRect(&edit->wnd.rcClient, &info.rcClient));
    CHECK(edit->wnd.style == info.dwStyle && edit->wnd.ExStyle == info.dwExStyle);
    CHECK(edit->wnd.state == 0x11223344 && edit->wnd.state2 == 0x55667788);
    CHECK(edit->wnd.dwExpWinVer == 0x030a && edit->wnd.fnid == FNID_EDIT);
    CHECK((LONG)edit->ped == 0x12345678);
    /* Original xxxSetWindowLong protects a published private pointer until
     * destruction. Refresh above must not change that private state. */
    ClrWF(&edit->wnd, WFDESTROYED);
    CHECK(MoveWindow(window, 23, 31, 112, 128, FALSE));
    CHECK(GetWindowInfo(window, &info));
    SetLastError(0x6789);
    CHECK(wow_window_words_refresh_observed(window, &info, 0, (DWORD)(ULONG_PTR)wc.hInstance));
    CHECK(GetLastError() == 0x6789);
    CHECK(EqualRect(&edit->wnd.rcWindow, &info.rcWindow));
    CHECK(EqualRect(&edit->wnd.rcClient, &info.rcClient));
    CHECK(edit->wnd.fnid == FNID_EDIT && (LONG)edit->ped == 0x12345678);
    CHECK(!TestWF(&edit->wnd, WFDESTROYED));
    {
        mvdm_softpc_wow_window_observation rows[2] = {0};
        RECT before = edit->wnd.rcWindow;
        WCHAR *old_name = edit->wnd.strName.Buffer;
        HANDLE old_module = edit->wnd.hModule;
        WW *words = wow_window_words_value(binding);
        DWORD old_style = words->dwStyle;
        rows[0].links.window = (ULONG)(ULONG_PTR)window;
        rows[0].process_id = GetCurrentProcessId();
        rows[0].style = info.dwStyle ^ WS_DISABLED;
        rows[0].module = 0x76543210;
        rows[0].window_rect[0] = 1234;
        /* First row prepares a real retained name; the invalid second
         * local identity must abort without publishing that first row. */
        rows[1].process_id = GetCurrentProcessId();
        CHECK(!wow_window_words_project_observed(2, rows));
        CHECK(EqualRect(&edit->wnd.rcWindow, &before));
        CHECK(edit->wnd.strName.Buffer == old_name);
        CHECK(edit->wnd.hModule == old_module && words->dwStyle == old_style);
        /* Invalid graph descriptor (atom zero) also leaves prepared local
         * fields untouched. This fixture does not prove successful CCPU
         * graph commit; that belongs to the page-domain group. */
        CHECK(!wow_window_words_project_observed(1, rows));
        CHECK(EqualRect(&edit->wnd.rcWindow, &before));
        CHECK(edit->wnd.strName.Buffer == old_name);
        CHECK(edit->wnd.hModule == old_module && words->dwStyle == old_style);
    }
    CHECK(!wow_window_words_refresh_observed(window, NULL, 0, 0));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    CHECK(wow_window_client_set_window_long(window, 0, 0x23456789, TRUE) == 0);
    CHECK(GetLastError() == ERROR_INVALID_INDEX && (LONG)edit->ped == 0x12345678);
    CHECK(wow_window_client_set_window_long(window, 1, 0, FALSE) == 0);
    CHECK(GetLastError() == ERROR_INVALID_INDEX && (LONG)edit->ped == 0x12345678);
    edit->wnd.cbwndExtra = 0;
    CHECK(wow_window_client_set_window_long(window, 0, 0, FALSE) == 0);
    CHECK(GetLastError() == ERROR_INVALID_INDEX && (LONG)edit->ped == 0x12345678);
    edit->wnd.cbwndExtra = facts.class_extra_bytes;
    SetWF(&edit->wnd, WFDESTROYED);
    CHECK(wow_window_client_set_window_long(window, 0, 0x23456789, TRUE) == 0x12345678);
    CHECK(wow_window_client_set_window_long(window, 0, 0, FALSE) == 0x23456789);
    CHECK(wow_edit_set_window_fnid(window, FNID_CLEANEDUP_BIT));
    CHECK(edit->wnd.fnid == (FNID_EDIT | FNID_CLEANEDUP_BIT));
    CHECK(wow_edit_set_window_fnid(window, FNID_CLEANEDUP_BIT));
    CHECK(!wow_edit_set_window_fnid(window, FNID_BUTTON));
    CHECK(edit->wnd.fnid == (FNID_EDIT | FNID_CLEANEDUP_BIT));
    CHECK(DestroyWindow(window));
    CHECK(wow_window_client_validate_hwnd(window) == NULL);
    SetLastError(0x3456);
    CHECK(!wow_window_client_owns_state(&edit->wnd));
    CHECK(GetLastError() == 0x3456);
    /* Retained carrier remains valid, but no new HWND lookup may acquire it. */
    {
        PWND retained = wow_window_words_lock_client_name(binding);
        CHECK(retained == &edit->wnd);
        if (retained) {
            CHECK(retained->strName.Length == 0 && retained->strName.Buffer[0] == 0);
            wow_window_words_unlock_client_name(binding);
        }
    }
    CHECK(!wow_window_client_refresh(window));
    CHECK(!wow_window_client_ensure_state(window));
    CHECK(edit->wnd.fnid == (FNID_EDIT | FNID_CLEANEDUP_BIT));
    CHECK(wow_window_words_client_state_value(binding) == &edit->wnd);
    wow_window_words_release(binding);
    CHECK(wow_user_borrow_leave(&scope));
    creation_case(wc.hInstance, TRUE, 0x0300, 0, FALSE, FALSE, FALSE, FALSE);
    creation_case(wc.hInstance, FALSE, 0x0309, GACF_ALWAYSSENDNCPAINT,
        FALSE, FALSE, FALSE, TRUE);
    creation_case(wc.hInstance, TRUE, 0x030a, GACF_ALWAYSSENDNCPAINT,
        TRUE, FALSE, FALSE, FALSE);
    creation_case(wc.hInstance, FALSE, 0x03ff, GACF_RANDOM3XUI,
        TRUE, FALSE, TRUE, FALSE);
    creation_case(wc.hInstance, TRUE, 0x0400,
        GACF_RANDOM3XUI | GACF_ALWAYSSENDNCPAINT, TRUE, TRUE, TRUE, FALSE);
    creation_case(wc.hInstance, FALSE, 0x12340400, 0, TRUE, TRUE, FALSE, FALSE);
    child_id_case(wc.hInstance);
    CHECK(UnregisterClassA(wc.lpszClassName, wc.hInstance));
    printf("WOW_EDIT_ORIGINAL_STATE failures=%u\n", failures);
    return failures ? 3 : 0;
}
