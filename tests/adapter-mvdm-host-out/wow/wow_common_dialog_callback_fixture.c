#include "precomp.h"
#include "wow_task_cleanup_entry.h"
#include "wow_user_runtime.h"
#include "wow_task_order_bindings.h"
#include "wow_user_borrow_scope.h"
#include <stdio.h>

UINT WINAPI WCD32DialogProc(HWND, UINT, WPARAM, LPARAM);
UINT WINAPI WCD32PrintSetupDialogProc(HWND, UINT, WPARAM, LPARAM);
UINT WINAPI WCD32FindReplaceDialogProc(HWND, UINT, WPARAM, LPARAM);
VOID W32FreeTask(PVOID);
static unsigned errors, depth, borrowed, app_entries, resource_entries, calls;
static unsigned prepared, freed;
static BOOL deny_task, deny_borrow, deny_graph, exceptional, nested, deny_prepare;
PTD gptdTaskHead;
CRITICAL_SECTION gcsWOW;
static wow_user_runtime_thread startup_binding;
static wow_user_runtime startup_runtime;
static OPENNT_SUPPORT_TEB startup_teb;
POPENNT_SUPPORT_TEB NTAPI opennt_support_current_teb(VOID)
{ return &startup_teb; }
static BOOL unbound;
wow_user_runtime_thread *WINAPI wow_user_runtime_current(void)
{ return unbound ? NULL : &startup_binding; }
#define CHECK(x) do { if (!(x)) { ++errors; fprintf(stderr, "FAIL line=%d\n", __LINE__); } } while (0)

/* Ordering-only test: real production wrappers, explicitly mocked original
 * bodies and execution owners. Real guest/provider tests remain mandatory. */
static BOOL enter(wow_task_callback_scope *scope)
{
    ZeroMemory(scope, sizeof(*scope));
    if (deny_task) return FALSE;
    ++depth;
    return TRUE;
}
BOOL WINAPI wow_task_callback_enter(wow_task_callback_scope *scope)
{
    ++app_entries;
    return enter(scope);
}
BOOL WINAPI wow_task_resource_cleanup_enter(wow_task_callback_scope *scope)
{
    ++resource_entries;
    return enter(scope);
}
BOOL WINAPI wow_task_callback_leave(wow_task_callback_scope *scope)
{
    (void)scope;
    CHECK(depth == borrowed + 1);
    --depth;
    return TRUE;
}
BOOL WINAPI wow_user_borrow_enter(wow_user_borrow_scope *scope)
{
    (void)scope;
    CHECK(depth == borrowed + 1);
    if (deny_borrow) return FALSE;
    ++borrowed;
    return TRUE;
}
VOID WINAPI wow_user_borrow_leave_required(wow_user_borrow_scope *scope)
{
    (void)scope;
    CHECK(depth && depth == borrowed);
    --borrowed;
}
BOOL WINAPI wow_user_window_refresh_message(HWND window, UINT message,
    WPARAM wp, LPARAM lp)
{
    CHECK(depth && depth == borrowed);
    CHECK(window == (HWND)0x1234 && wp == 17 && lp == 23);
    (void)message;
    return !deny_graph;
}
static UINT original(HWND window, UINT message, WPARAM wp, LPARAM lp, UINT result)
{
    ++calls;
    CHECK(window == (HWND)0x1234 && wp == 17 && lp == 23);
    CHECK(depth && depth == borrowed);
    if (nested && depth == 1)
        CHECK(WCD32DialogProc(window, message, wp, lp) == 41);
    if (exceptional) RaiseException(0xe0447035, 0, 0, NULL);
    SetLastError(0x447335);
    return result;
}
UINT WINAPI WowOriginalDialogProc(HWND w, UINT m, WPARAM p, LPARAM l)
{ return original(w, m, p, l, 41); }
UINT WINAPI WowOriginalPrintSetupDialogProc(HWND w, UINT m, WPARAM p, LPARAM l)
{ return original(w, m, p, l, 42); }
UINT WINAPI WowOriginalFindReplaceDialogProc(HWND w, UINT m, WPARAM p, LPARAM l)
{ return original(w, m, p, l, 43); }
BOOL WINAPI wow_user_task_prepare_exit(void)
{
    ++prepared;
    return !deny_prepare;
}
VOID WowOriginalFreeTask(PVOID task)
{
    CHECK(task == (PVOID)0x5678 && prepared == freed + 1);
    ++freed;
}
__declspec(noreturn) void WINAPI wow_user_native_call_failed(const char *where, DWORD error)
{
    (void)where; (void)error;
    RaiseException(0xe0447034, 0, 0, NULL);
    ExitProcess(99);
}
int __cdecl main(void)
{
    BOOL caught = FALSE;
    unsigned failure;
    {
        TD td = {0};
        PVOID previous = NtCurrentTeb()->WOW32Reserved;
        InitializeCriticalSection(&gcsWOW);
        startup_binding.runtime = &startup_runtime;
        gptdTaskHead = &td;
        NtCurrentTeb()->WOW32Reserved = &td;
        td.htask16 = 0x1234; td.dwThreadID = GetCurrentThreadId();
        SetLastError(0x447337);
        CHECK(wow_user_task_cleanup_uninitialized(0x1234));
        CHECK(GetLastError() == 0x447337);
        CHECK(!wow_user_task_cleanup_uninitialized(0));
        CHECK(!wow_user_task_cleanup_uninitialized(0x1235));
        CHECK(!wow_user_task_cleanup_uninitialized(0x11234));
        td.hThread = (HANDLE)1;
        CHECK(!wow_user_task_cleanup_uninitialized(0x1234));
        td.hThread = NULL; ++td.dwThreadID;
        CHECK(!wow_user_task_cleanup_uninitialized(0x1234));
        td.dwThreadID = GetCurrentThreadId();
        startup_binding.thread = (void *)1;
        CHECK(!wow_user_task_cleanup_uninitialized(0x1234));
        startup_binding.thread = NULL; unbound = TRUE;
        CHECK(!wow_user_task_cleanup_uninitialized(0x1234));
        unbound = FALSE;
        NtCurrentTeb()->WOW32Reserved = (PVOID)1;
        CHECK(!wow_user_task_cleanup_uninitialized(0x1234));
        NtCurrentTeb()->WOW32Reserved = &td; gptdTaskHead = NULL;
        CHECK(!wow_user_task_cleanup_uninitialized(0x1234));
        NtCurrentTeb()->WOW32Reserved = previous;
        DeleteCriticalSection(&gcsWOW);
        fprintf(stderr,"WOW_UNINITIALIZED_CLEANUP errors=%u exact_td=1 foreign_rejected=1 committed_rejected=1 retired_rejected=1\n",errors);
    }
    CHECK(WCD32DialogProc((HWND)0x1234, WM_INITDIALOG, 17, 23) == 41);
    CHECK(GetLastError() == 0x447335);
    CHECK(WCD32PrintSetupDialogProc((HWND)0x1234, WM_DESTROY, 17, 23) == 42);
    CHECK(WCD32FindReplaceDialogProc((HWND)0x1234, WM_INITDIALOG, 17, 23) == 43);
    CHECK(app_entries == 3 && resource_entries == 0);
    CHECK(WCD32FindReplaceDialogProc((HWND)0x1234, WM_DESTROY, 17, 23) == 43);
    CHECK(resource_entries == 1 && !depth && !borrowed && calls == 4);
    nested = TRUE;
    CHECK(WCD32PrintSetupDialogProc((HWND)0x1234, WM_INITDIALOG, 17, 23) == 42);
    nested = FALSE;
    exceptional = TRUE;
    __try { WCD32DialogProc((HWND)0x1234, WM_INITDIALOG, 17, 23); }
    __except (GetExceptionCode() == 0xe0447035 ?
        EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught && calls == 7 && !depth && !borrowed);
    exceptional = FALSE;
    deny_task = TRUE;
    CHECK(!WCD32DialogProc(NULL, 0, 0, 0));
    deny_task = FALSE; deny_borrow = TRUE;
    CHECK(!WCD32DialogProc(NULL, 0, 0, 0));
    deny_borrow = FALSE; deny_graph = TRUE;
    CHECK(!WCD32DialogProc((HWND)0x1234, WM_INITDIALOG, 17, 23));
    CHECK(calls == 7 && !depth && !borrowed);
    deny_graph = FALSE;
    for (failure = 0; failure != 3; ++failure) {
        deny_task = failure == 0;
        deny_borrow = failure == 1;
        deny_graph = failure == 2;
        caught = FALSE;
        __try { WCD32FindReplaceDialogProc((HWND)0x1234, WM_DESTROY, 17, 23); }
        __except (GetExceptionCode() == 0xe0447034 ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
        CHECK(caught && calls == 7 && !depth && !borrowed);
    }
    deny_task = deny_borrow = deny_graph = FALSE;
    fprintf(stderr, "WOW_COMMDLG_RESOURCE_FAILURE errors=%u fatal_edges=3 skipped_free=0\n", errors);
    W32FreeTask((PVOID)0x5678);
    CHECK(prepared == 1 && freed == 1);
    deny_prepare = TRUE; caught = FALSE;
    __try { W32FreeTask((PVOID)0x5678); }
    __except (GetExceptionCode() == 0xe0447034 ?
        EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught && prepared == 2 && freed == 1);
    fprintf(stderr, "WOW_COMMDLG_EXIT_GUARD errors=%u entries=3 resource_branch=1 nested=1 exception=1 denied=3 early_order=1 rejected_free=1\n", errors);
    return errors ? 1 : 0;
}
