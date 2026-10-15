#include "wow_task_order_bindings.h"
#include "wow_user_borrow_scope.h"
#include <stdio.h>

LONG WINAPI WU32StdHookProc(INT, LONG, LONG, INT);
static unsigned errors, task_depth, borrow_depth, original_calls;
static BOOL deny_task, deny_borrow, raise_original;
#define CHECK(x) do { if (!(x)) { ++errors; fprintf(stderr, "FAIL line=%d\n", __LINE__); } } while (0)

/* Ordering/unwind fixture only. The original hook algorithm and actual
 * scheduler are deliberately not replaced in the production DLL. */
BOOL WINAPI wow_task_callback_enter(wow_task_callback_scope *scope)
{
    (void)scope;
    if (deny_task) return FALSE;
    CHECK(task_depth == borrow_depth); ++task_depth; return TRUE;
}
BOOL WINAPI wow_task_callback_leave(wow_task_callback_scope *scope)
{
    (void)scope;
    CHECK(task_depth == borrow_depth + 1); --task_depth; return TRUE;
}
BOOL WINAPI wow_user_borrow_enter(wow_user_borrow_scope *scope)
{
    (void)scope;
    if (deny_borrow) return FALSE;
    CHECK(task_depth == borrow_depth + 1); ++borrow_depth; return TRUE;
}
BOOL WINAPI wow_user_borrow_leave(wow_user_borrow_scope *scope)
{
    (void)scope;
    CHECK(task_depth == borrow_depth && borrow_depth); --borrow_depth; return TRUE;
}
LONG WINAPI WowOriginalStdHookProc(INT code, LONG wp, LONG lp, INT index)
{
    CHECK(task_depth && task_depth == borrow_depth);
    ++original_calls;
    if (raise_original) RaiseException(0xe0447004u, 0, 0, NULL);
    if (index == 2) CHECK(WU32StdHookProc(1, 2, 3, 1) == 7);
    return code + wp + lp + index;
}
int __cdecl main(void)
{
    BOOL caught = FALSE;
    deny_task = TRUE;
    CHECK(!WU32StdHookProc(1, 2, 3, 1) && !original_calls && !task_depth);
    deny_task = FALSE; deny_borrow = TRUE;
    CHECK(!WU32StdHookProc(1, 2, 3, 1) && !original_calls && !task_depth);
    deny_borrow = FALSE;
    CHECK(WU32StdHookProc(1, 2, 3, 1) == 7 && original_calls == 1);
    raise_original = TRUE;
    __try { (void)WU32StdHookProc(1, 2, 3, 1); }
    __except (GetExceptionCode() == 0xe0447004u ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught && !task_depth && !borrow_depth);
    raise_original = FALSE;
    CHECK(WU32StdHookProc(1, 2, 3, 2) == 8 && original_calls == 4);
    CHECK(!task_depth && !borrow_depth);
    fprintf(stderr, "WOW_HOOK_CALLBACK_SCOPE errors=%u calls=%u denied/normal/nested/exception checked\n", errors, original_calls);
    return errors ? 1 : 0;
}
