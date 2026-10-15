#include "precomp.h"
#include "wow_task_order_bindings.h"
#include "wow_user_borrow_scope.h"
#include "wow_window_creation_binding.h"
#include "wow_user_runtime.h"
#include <stdio.h>

LONG WINAPI WU32StdHookProc(INT, LONG, LONG, INT);
static unsigned errors, task_depth, borrow_depth, original_calls;
static BOOL deny_task, deny_borrow, raise_original;
static BOOL deny_enrollment, deny_graph, veto_creation;
static BOOL deny_leave;
static unsigned enrollments, rollbacks, accepted_creations;
static wow_user_runtime_thread runtime;
#define CHECK(x) do { if (!(x)) { ++errors; fprintf(stderr, "FAIL line=%d\n", __LINE__); } } while (0)

/* Ordering/unwind fixture only. The original hook algorithm and actual
 * scheduler are deliberately not replaced in the production DLL. */
BOOL W32GetHookStateData(LPHOOKSTATEDATA hook)
{
    hook->InUse = TRUE;
    hook->iHook = hook->iIndex == 3 ? WH_CBT : WH_KEYBOARD;
    return TRUE;
}
wow_user_runtime_thread *WINAPI wow_user_runtime_current(void)
{
    return &runtime;
}
BOOL WINAPI wow_user_window_refresh_graph(HWND window)
{
    (void)window;
    CHECK(task_depth == borrow_depth && borrow_depth);
    return !deny_graph;
}
/* This fixture checks owner/unwind order only; production operand selection
 * and graph publication are checked by the separate real CCPU fixture. */
BOOL WINAPI wow_user_window_refresh_payload(HWND window, const HWND *roots, ULONG count)
{
    CHECK(!count || roots != NULL);
    return wow_user_window_refresh_graph(window);
}
ULONG WINAPI wow_user_message_window_roots(UINT message, WPARAM wp, LPARAM lp, HWND roots[4])
{
    (void)message; (void)wp; (void)lp; (void)roots;
    CHECK(task_depth == borrow_depth && borrow_depth);
    return 0;
}
BOOL WINAPI wow_menu_native_refresh_message(UINT message, WPARAM wp, LPARAM lp)
{
    (void)message; (void)wp; (void)lp;
    CHECK(task_depth == borrow_depth && borrow_depth);
    return TRUE;
}
BOOL WINAPI wow_window_creation_enroll_internal(wow_task_order_thread *thread,
    HWND window, const CREATESTRUCTA *request, wow_window_creation_enrollment *scope)
{
    CHECK(thread == runtime.thread && window == (HWND)23 && request);
    CHECK(task_depth == borrow_depth && borrow_depth);
    ++enrollments;
    if (deny_enrollment) return FALSE;
    scope->window = window;
    scope->borrow = (wow_window_words_binding *)1;
    return TRUE;
}
void WINAPI wow_window_creation_finish(wow_window_creation_enrollment *scope,
    BOOL accepted)
{
    if (!scope->borrow) return;
    CHECK(task_depth + 1 == borrow_depth && borrow_depth);
    if (accepted) ++accepted_creations;
    else ++rollbacks;
    scope->borrow = NULL;
}
BOOL WINAPI wow_task_callback_enter(wow_task_callback_scope *scope)
{
    (void)scope;
    if (deny_task) return FALSE;
    CHECK(task_depth == borrow_depth); ++task_depth; return TRUE;
}
BOOL WINAPI wow_task_callback_leave(wow_task_callback_scope *scope)
{
    (void)scope;
    CHECK(task_depth == borrow_depth + 1 || task_depth == borrow_depth);
    --task_depth;
    return !deny_leave;
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
    CHECK((task_depth == borrow_depth || task_depth + 1 == borrow_depth) && borrow_depth);
    --borrow_depth; return TRUE;
}
LONG WINAPI WowOriginalStdHookProc(INT code, LONG wp, LONG lp, INT index)
{
    CHECK(task_depth && task_depth == borrow_depth);
    ++original_calls;
    if (raise_original) RaiseException(0xe0447004u, 0, 0, NULL);
    if (index == 3) {
        CHECK(enrollments);
        return veto_creation ? 1 : 0;
    }
    if (index == 2) CHECK(WU32StdHookProc(1, 2, 3, 1) == 7);
    return code + wp + lp + index;
}
int __cdecl main(void)
{
    BOOL caught = FALSE;
    CREATESTRUCTA request = {0};
    CBT_CREATEWNDA creation = {&request, NULL};
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
    CHECK(WU32StdHookProc(HCBT_CREATEWND, 23, (LONG)&creation, 3) == 0);
    CHECK(enrollments == 1 && accepted_creations == 1 && !rollbacks);
    veto_creation = TRUE;
    CHECK(WU32StdHookProc(HCBT_CREATEWND, 23, (LONG)&creation, 3) == 1);
    CHECK(rollbacks == 1);
    veto_creation = FALSE; deny_graph = TRUE;
    CHECK(WU32StdHookProc(HCBT_CREATEWND, 23, (LONG)&creation, 3) == 1);
    CHECK(rollbacks == 2);
    deny_graph = FALSE; deny_enrollment = TRUE;
    CHECK(WU32StdHookProc(HCBT_CREATEWND, 23, (LONG)&creation, 3) == 1);
    CHECK(rollbacks == 2);
    deny_enrollment = FALSE; raise_original = TRUE; caught = FALSE;
    __try { (void)WU32StdHookProc(HCBT_CREATEWND, 23, (LONG)&creation, 3); }
    __except (GetExceptionCode() == 0xe0447004u ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught && rollbacks == 3 && !task_depth && !borrow_depth);
    raise_original = FALSE;
    CHECK(WU32StdHookProc(HCBT_CREATEWND, 2, 3, 1) == HCBT_CREATEWND + 6);
    CHECK(enrollments == 5); /* Same code on a non-CBT hook is not creation. */
    deny_leave = TRUE;
    CHECK(WU32StdHookProc(HCBT_CREATEWND, 23, (LONG)&creation, 3) == 1);
    deny_leave = FALSE;
    CHECK(original_calls == 9 && accepted_creations == 1 && rollbacks == 4);
    CHECK(!task_depth && !borrow_depth);
    fprintf(stderr, "WOW_HOOK_CREATION_GUARD errors=%u enrollment=%u accepted=%u rollback=%u\n",
        errors, enrollments, accepted_creations, rollbacks);
    fprintf(stderr, "WOW_HOOK_CALLBACK_SCOPE errors=%u calls=%u denied/normal/nested/exception checked\n", errors, original_calls);
    return errors ? 1 : 0;
}
