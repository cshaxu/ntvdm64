#include "wow_task_order_bindings.h"
#include "wow_user_borrow_scope.h"
#include <stdio.h>

VOID W32TimerFunc(UINT, HWND, UINT, DWORD);
static unsigned errors, task_depth, borrow_depth, calls;
static BOOL deny_task, deny_borrow, nested, exceptional;
#define CHECK(x) do { if (!(x)) { ++errors; fprintf(stderr, "FAIL line=%d\n", __LINE__); } } while (0)

/* Only the production wrapper's argument/order/unwind contract is tested
 * here. Original timer policy and real task ownership require guest cases. */
BOOL WINAPI wow_task_callback_enter(wow_task_callback_scope *scope)
{
    ZeroMemory(scope, sizeof(*scope));
    if (deny_task) return FALSE;
    ++task_depth;
    return TRUE;
}
BOOL WINAPI wow_task_callback_leave(wow_task_callback_scope *scope)
{
    (void)scope;
    CHECK(task_depth && task_depth == borrow_depth + 1);
    --task_depth;
    return TRUE;
}
BOOL WINAPI wow_user_borrow_enter(wow_user_borrow_scope *scope)
{
    (void)scope;
    CHECK(task_depth == borrow_depth + 1);
    if (deny_borrow) return FALSE;
    ++borrow_depth;
    return TRUE;
}
VOID WINAPI wow_user_borrow_leave_required(wow_user_borrow_scope *scope)
{
    (void)scope;
    CHECK(task_depth == borrow_depth && borrow_depth);
    --borrow_depth;
}
VOID WowOriginalTimerFunc(UINT index, HWND window, UINT event, DWORD time)
{
    ++calls;
    CHECK(index >= 1 && index <= 27);
    CHECK(window == (HWND)(ULONG_PTR)0x1234 && event == 71 && time == 123456);
    CHECK(task_depth && task_depth == borrow_depth);
    if (nested && task_depth == 1)
        W32TimerFunc(index, window, event, time);
    if (exceptional) RaiseException(0xe0447012, 0, 0, NULL);
    SetLastError(0xe0447013);
}
int __cdecl main(void)
{
    UINT index;
    BOOL caught = FALSE;
    for (index = 1; index <= 27; ++index) {
        W32TimerFunc(index, (HWND)(ULONG_PTR)0x1234, 71, 123456);
        CHECK(GetLastError() == 0xe0447013);
        CHECK(!task_depth && !borrow_depth);
    }
    CHECK(calls == 27);
    nested = TRUE;
    W32TimerFunc(1, (HWND)(ULONG_PTR)0x1234, 71, 123456);
    CHECK(calls == 29 && !task_depth && !borrow_depth);
    nested = FALSE;
    exceptional = TRUE;
    __try { W32TimerFunc(2, (HWND)(ULONG_PTR)0x1234, 71, 123456); }
    __except (GetExceptionCode() == 0xe0447012 ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        caught = TRUE;
    }
    CHECK(caught && calls == 30 && !task_depth && !borrow_depth);
    exceptional = FALSE;
    deny_task = TRUE;
    W32TimerFunc(3, NULL, 0, 0);
    deny_task = FALSE;
    deny_borrow = TRUE;
    W32TimerFunc(4, NULL, 0, 0);
    CHECK(calls == 30 && !task_depth && !borrow_depth);
    fprintf(stderr, "WOW_TIMER_CALLBACK_GUARD errors=%u calls=%u nested=1 exception=1 denied=2\n",
        errors, calls);
    return errors ? 1 : 0;
}
