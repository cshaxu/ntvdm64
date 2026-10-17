#include "wow_dialog_state_bindings.h"
#include "wow_dialog_context_bindings.h"
#include "wow_user_task_lifecycle.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_private_access.h"
#include <stdio.h>

/* Actual finalization boundary with explicit scope/worker doubles. This
 * does not prove original resource release, native destruction or guest
 * lifetime; the full dialog-group tests must establish those independently. */
static DIALOG dialog;
static DLG data;
static wow_user_runtime_thread runtime;
static wow_task_order_thread thread, other;
static wow_task_order_entry tdb;
static wow_task_client_info client;
static wow_cleanup_window owner;
static unsigned errors, references, tasks, borrows, calls, cleanup_entries;
static unsigned ensures;
static unsigned native_retires;
static unsigned api_calls;
static BOOL api_raise;
static DWORD last_ansi;
static UINT last_message;
static unsigned end_calls, native_begins;
static int end_result;
static BOOL fail_borrow, fail_worker, fail_ready;
static BOOL native_success, native_raise;
static BOOL fail_context;
static BOOL fail_graph;
static unsigned graph_calls;
static unsigned callback_publications;
static unsigned captures;
static wow_dialog_context outer_context;
__declspec(thread) wow_dialog_context *wow_dialog_current;
#define NATIVE_EXCEPTION ((DWORD)0xe0424475)
#define WINDOW ((HWND)(ULONG_PTR)0x1234)
#define WORDS ((wow_window_words_binding *)(ULONG_PTR)0x5678)
#define CHECK(c) do { if (!(c)) { ++errors; printf("FAIL line=%d\n", __LINE__); } } while (0)

wow_user_runtime_thread *WINAPI wow_user_runtime_current(void) { return &runtime; }
VOID WINAPI wow_dialog_host_retire(HWND window)
{
    CHECK(window == WINDOW && references && tasks && borrows);
    ++native_retires;
}
BOOL WINAPI wow_dialog_capture_context(wow_dialog_context *context, BOOL font)
{
    CHECK(!font && tasks && borrows);
    ++captures;
    if (fail_context) { SetLastError(ERROR_NOT_READY); return FALSE; }
    ZeroMemory(context, sizeof(*context));
    return TRUE;
}
VOID WINAPI wow_dialog_release_context(wow_dialog_context *context)
{ CHECK(!context->server.hMsgFont); }
BOOL WINAPI wow_edit_lookaside_ready(void)
{
    CHECK(!tasks && !borrows);
    if (fail_ready) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    return TRUE;
}
wow_window_words_binding *WINAPI wow_window_words_acquire(HWND window)
{ CHECK(window == WINDOW); ++references; return WORDS; }
void WINAPI wow_window_words_release(wow_window_words_binding *words)
{ CHECK(words == WORDS && references); --references; }
wow_cleanup_window *WINAPI wow_window_words_cleanup_value(wow_window_words_binding *words)
{ CHECK(words == WORDS); return &owner; }
struct tagWND *WINAPI wow_window_words_client_state_value(wow_window_words_binding *words)
{ CHECK(words == WORDS && references); return &dialog.wnd; }
BOOL WINAPI wow_window_words_owns_client_state(wow_window_words_binding *words, PWND state)
{ CHECK(words == WORDS && references); return state == &dialog.wnd; }
BOOL WINAPI wow_user_native_call_begin(wow_user_native_call *call, HWND window)
{ (void)call; (void)window; CHECK(FALSE); return FALSE; }
BOOL WINAPI wow_user_native_call_end(wow_user_native_call *call)
{ (void)call; CHECK(FALSE); return FALSE; }
__declspec(noreturn) void WINAPI wow_user_native_call_failed(const char *reason, DWORD error)
{ printf("unexpected native failure %s %lu\n", reason,error); ExitProcess(2); }
static int api_entry(HWND window, void *buffer)
{
    CHECK(window == WINDOW && buffer && references && tasks && borrows && wow_dialog_current);
    ++api_calls;
    SetLastError(0x4474);
    if (api_raise) RaiseException(NATIVE_EXCEPTION,0,0,NULL);
    return 37;
}
BOOL WINAPI wow_original_MapDialogRect(HWND w, LPRECT r) { return api_entry(w,r); }
int WINAPI wow_original_DlgDirListA(HWND w, LPSTR p, int a, int b, UINT f)
{ CHECK(a == 3 && b == 5 && f == 7); return api_entry(w,p); }
int WINAPI wow_original_DlgDirListComboBoxA(HWND w, LPSTR p, int a, int b, UINT f)
{ CHECK(a == 3 && b == 5 && f == 7); return api_entry(w,p); }
BOOL WINAPI wow_original_DlgDirSelectExA(HWND w, LPSTR p, int a, int b)
{ CHECK(a == 3 && b == 5); return api_entry(w,p); }
BOOL WINAPI wow_original_DlgDirSelectComboBoxExA(HWND w, LPSTR p, int a, int b)
{ CHECK(a == 3 && b == 5); return api_entry(w,p); }
BOOL WINAPI wow_window_client_ensure_state(HWND window)
{ CHECK(window == WINDOW && tasks && borrows); ++ensures; return TRUE; }
/* Entry-order double only; graph generation/lifetime is not modeled here. */
BOOL WINAPI wow_window_client_prepare_graph(HWND window)
{
    CHECK(window == WINDOW && tasks && borrows && references);
    ++graph_calls;
    if (fail_graph) { SetLastError(ERROR_INVALID_STATE); return FALSE; }
    return TRUE;
}
BOOL WINAPI wow_original_EndDialog(HWND window, int result)
{
    CHECK(window == WINDOW && tasks && borrows && references);
    ++end_calls; end_result = result;
    SetLastError(fail_worker ? ERROR_INVALID_DATA : 0x4473);
    return !fail_worker;
}
BOOL WINAPI wow_user_borrow_set_callback(HWND window)
{
    CHECK(window == WINDOW && tasks && borrows && references && graph_calls);
    ++callback_publications;
    return TRUE;
}
BOOL WINAPI wow_native_EndDialog(HWND window, INT_PTR result)
{
    CHECK(window == WINDOW && result == 7 && references);
    ++native_begins;
    SetLastError(native_success ? 0x4473 : ERROR_BUSY);
    if (native_raise) RaiseException(NATIVE_EXCEPTION, 0, 0, NULL);
    return native_success;
}
BOOL WINAPI wow_task_callback_enter(wow_task_callback_scope *scope)
{ (void)scope; ++tasks; return TRUE; }
BOOL WINAPI wow_task_resource_cleanup_enter(wow_task_callback_scope *scope)
{ ++cleanup_entries; return wow_task_callback_enter(scope); }
BOOL WINAPI wow_task_callback_leave(wow_task_callback_scope *scope)
{ (void)scope; CHECK(tasks); --tasks; return TRUE; }
BOOL WINAPI wow_user_borrow_enter(wow_user_borrow_scope *scope)
{
    (void)scope;
    if (fail_borrow) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    ++borrows; return TRUE;
}
VOID WINAPI wow_user_borrow_leave_required(wow_user_borrow_scope *scope)
{ (void)scope; CHECK(borrows); --borrows; }
PWND WINAPI wow_window_client_validate_hwnd(HWND window)
{ CHECK(window == WINDOW && references && tasks && borrows); return &dialog.wnd; }
LONG WINAPI DefDlgProcWorker(PWND state, UINT message, WPARAM wp, LPARAM lp, DWORD ansi)
{
    CHECK(state == &dialog.wnd);
    CHECK(references && tasks && borrows);
    ++calls;
    last_message = message; last_ansi = ansi;
    if (message != 0x70) {
        if (message != WM_NCDESTROY) CHECK(wow_dialog_current != NULL);
        CHECK(wp == 17 && lp == 29);
        return fail_worker ? 0 : 0x1234;
    }
    CHECK(!wp && !lp && !ansi);
    if (!fail_worker) {
        dialog.pdlg = NULL;
        ClrWF(state, WFDIALOGWINDOW);
        state->fnid |= FNID_CLEANEDUP_BIT;
    }
    return 0; /* Original success and rejection both can return zero. */
}
static void reset(void)
{
    CHECK(!references && !tasks && !borrows);
    ZeroMemory(&dialog, sizeof(dialog));
    dialog.wnd.fnid = FNID_DIALOG;
    dialog.wnd.cbwndExtra = DLGWINDOWEXTRA;
    SetWF(&dialog.wnd, WFDIALOGWINDOW);
    dialog.pdlg = &data;
    runtime.thread = &thread;
    thread.ptdb = &tdb; thread.pClientInfo = &client; thread.TIF_flags = 0;
    owner.thread = &thread;
    fail_borrow = fail_worker = fail_ready = FALSE;
    native_success = native_raise = FALSE;
    fail_context = FALSE; captures = 0;
    calls = cleanup_entries = ensures = end_calls = native_begins = 0;
}
int main(void)
{
    LONG result;
    BOOL caught = FALSE;
    reset(); fail_ready = TRUE;
    CHECK(!wow_dialog_window_procA(WINDOW, WM_USER, 17, 29));
    CHECK(GetLastError() == ERROR_NOT_ENOUGH_MEMORY && !calls && !ensures &&
        !references && !tasks && !borrows);
    reset(); SetLastError(0x4473);
    CHECK(wow_dialog_finalize_bound(WINDOW));
    CHECK(calls == 1 && GetLastError() == 0x4473);
    CHECK(wow_dialog_finalize_bound(WINDOW) && calls == 1);
    reset(); fail_worker = TRUE;
    CHECK(!wow_dialog_finalize_bound(WINDOW));
    CHECK(calls == 1 && GetLastError() == ERROR_INVALID_STATE && dialog.pdlg == &data);
    reset(); fail_borrow = TRUE;
    CHECK(!wow_dialog_finalize_bound(WINDOW));
    CHECK(!calls && GetLastError() == ERROR_NOT_ENOUGH_MEMORY);
    reset(); dialog.pdlg = NULL; ClrWF(&dialog.wnd, WFDIALOGWINDOW);
    fail_ready = TRUE; /* Release needs no new lookaside initialization. */
    SetLastError(0x4473);
    CHECK(wow_dialog_finalize_bound(WINDOW) && !calls);
    CHECK((dialog.wnd.fnid & FNID_CLEANEDUP_BIT) != 0);
    CHECK(GetLastError() == 0x4473 && !ensures);
    CHECK(wow_dialog_finalize_bound(WINDOW) && !calls);
    reset(); dialog.pdlg = NULL; /* Flag/pointer mismatch is not empty state. */
    CHECK(!wow_dialog_finalize_bound(WINDOW) && !calls);
    reset(); ClrWF(&dialog.wnd, WFDIALOGWINDOW);
    CHECK(!wow_dialog_finalize_bound(WINDOW) && !calls);
    reset(); dialog.pdlg = NULL; ClrWF(&dialog.wnd, WFDIALOGWINDOW);
    dialog.wnd.fnid |= FNID_DELETED_BIT;
    CHECK(!wow_dialog_finalize_bound(WINDOW) && !calls);
    reset(); dialog.wnd.cbwndExtra = 0;
    CHECK(!wow_dialog_finalize_bound(WINDOW) && !calls);
    reset(); thread.TIF_flags = WOW_TASK_TIF_INCLEANUP;
    CHECK(wow_dialog_finalize_bound(WINDOW) && calls == 1 && cleanup_entries == 1);
    reset(); thread.TIF_flags = WOW_TASK_TIF_INCLEANUP; owner.thread = &other;
    CHECK(!wow_dialog_finalize_bound(WINDOW) && !calls && !cleanup_entries);
    reset();
    CHECK(wow_dialog_window_procA(WINDOW, WM_USER, 17, 29) == 0x1234);
    CHECK(calls == 1 && ensures == 1 && last_ansi && last_message == WM_USER);
    CHECK(graph_calls != 0 && callback_publications != 0);
    CHECK(wow_dialog_window_procW(WINDOW, WM_USER + 1, 17, 29) == 0x1234);
    CHECK(calls == 2 && ensures == 2 && !last_ansi);
    reset(); fail_worker = TRUE; result = -1;
    CHECK(wow_dialog_default_message(WINDOW, WM_USER, 17, 29, &result));
    CHECK(result == 0 && calls == 1); /* Handled zero, no native retry. */
    reset(); dialog.wnd.fnid = 0; ClrWF(&dialog.wnd, WFDIALOGWINDOW);
    result = -1; SetLastError(0x4473);
    CHECK(!wow_dialog_default_message(WINDOW, WM_USER, 17, 29, &result));
    CHECK(result == -1 && !calls && !ensures && GetLastError() == 0x4473);
    reset(); thread.TIF_flags = WOW_TASK_TIF_INCLEANUP;
    CHECK(!wow_dialog_window_procA(WINDOW, WM_USER, 17, 29));
    CHECK(!calls && !ensures && !cleanup_entries);
    reset();
    CHECK(wow_user_end_dialog(WINDOW, -49));
    CHECK(end_calls == 1 && end_result == -49 && !native_begins &&
        GetLastError() == 0x4473);
    fail_worker = TRUE;
    CHECK(!wow_user_end_dialog(WINDOW, 7));
    CHECK(end_calls == 2 && end_result == 7 && !native_begins &&
        GetLastError() == ERROR_INVALID_DATA);
    reset(); dialog.wnd.fnid = 0; ClrWF(&dialog.wnd, WFDIALOGWINDOW);
    CHECK(!wow_user_end_dialog(WINDOW, 7));
    CHECK(!end_calls && native_begins == 1 && GetLastError() == ERROR_BUSY);
    native_success = TRUE;
    CHECK(wow_user_end_dialog(WINDOW, 7));
    CHECK(!end_calls && native_begins == 2 && GetLastError() == 0x4473);
    native_raise = TRUE;
    __try { (void)wow_user_end_dialog(WINDOW, 7); }
    __except (GetExceptionCode() == NATIVE_EXCEPTION ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught && native_begins == 3 && !end_calls && GetLastError() == 0x4473);
    CHECK(!references && !tasks && !borrows);
    CHECK(!wow_dialog_current);
    reset(); wow_dialog_current = &outer_context; fail_context = TRUE;
    CHECK(!wow_dialog_window_procA(WINDOW, WM_USER, 17, 29));
    CHECK(GetLastError() == ERROR_NOT_READY && captures == 1 && !calls && !ensures);
    CHECK(wow_dialog_current == &outer_context && !references && !tasks && !borrows);
    /* Destruction must not depend on acquiring preferences. */
    CHECK(wow_dialog_window_procA(WINDOW, WM_NCDESTROY, 17, 29) == 0x1234);
    CHECK(native_retires == 1);
    CHECK(captures == 1 && wow_dialog_current == &outer_context);
    CHECK(wow_dialog_finalize_bound(WINDOW));
    CHECK(captures == 1 && wow_dialog_current == &outer_context);
    CHECK(native_retires == 1); /* Private finalization must not touch retired HWND. */
    CHECK(!references && !tasks && !borrows);
    wow_dialog_current = NULL;
    reset(); fail_graph = TRUE;
    CHECK(!wow_dialog_window_procA(WINDOW, WM_USER, 17, 29));
    CHECK(!calls && GetLastError() == ERROR_INVALID_STATE);
    CHECK(!references && !tasks && !borrows);
    /* Canonical dialog final cleanup has no relationship traversal and must
     * remain independent of fresh observation/allocation failure. */
    CHECK(wow_dialog_finalize_bound(WINDOW));
    fail_graph = FALSE;
    reset();
    {
        RECT rect = {0};
        char path[16] = "*.*";
        CHECK(wow_user_map_dialog_rect(WINDOW,&rect) == 37);
        CHECK(wow_user_dlg_dir_listA(WINDOW,path,3,5,7) == 37);
        CHECK(wow_user_dlg_dir_list_comboA(WINDOW,path,3,5,7) == 37);
        CHECK(wow_user_dlg_dir_selectA(WINDOW,path,3,5) == 37);
        CHECK(wow_user_dlg_dir_select_comboA(WINDOW,path,3,5) == 37);
        CHECK(api_calls == 5 && GetLastError() == 0x4474);
        CHECK(!references && !tasks && !borrows && !wow_dialog_current);
        fail_context = TRUE;
        CHECK(!wow_user_map_dialog_rect(WINDOW,&rect));
        CHECK(api_calls == 5 && GetLastError() == ERROR_NOT_READY);
        fail_context = FALSE; api_raise = TRUE; caught = FALSE;
        __try { (void)wow_user_map_dialog_rect(WINDOW,&rect); }
        __except(GetExceptionCode() == NATIVE_EXCEPTION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
        CHECK(caught && api_calls == 6 && GetLastError() == 0x4474);
        CHECK(!references && !tasks && !borrows && !wow_dialog_current);
    }
    printf("dialog-finalization failures=%u\n", errors);
    return errors ? 1 : 0;
}
