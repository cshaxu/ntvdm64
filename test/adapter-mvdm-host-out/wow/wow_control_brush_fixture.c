#include "wow_control_brush_bindings.h"
#include "wow_edit_call_bindings.h"
#include "wow_user_task_lifecycle.h"
#include <stdio.h>

/* Actual original client/server bodies and adapter, with explicit lifetime
 * and color-service doubles. Not a real HWND/guest lifetime acceptance. */
static WND control, parent, owner;
static HWND expected;
static unsigned failures, depth, calls, retains, leaves;
static BOOL deny_enter, deny_validate, deny_retain, raise_color;
static PWND rejected_state;
#define TEST_EXCEPTION ((DWORD)0xe0424473)
#define CHECK(c) do { if (!(c)) { ++failures; printf("FAIL %d\n", __LINE__); } } while (0)

BOOL WINAPI wow_user_borrow_enter(wow_user_borrow_scope *scope)
{
    (void)scope;
    if (deny_enter) { SetLastError(701); return FALSE; }
    ++depth;
    return TRUE;
}
VOID WINAPI wow_user_borrow_leave_required(wow_user_borrow_scope *scope)
{
    (void)scope; CHECK(depth == 1); --depth; ++leaves;
}
PWND WINAPI wow_window_client_validate_hwnd(HWND window)
{
    CHECK(depth == 1 && window == control.head.h);
    if (deny_validate) { SetLastError(702); return NULL; }
    return &control;
}
wow_window_words_binding *WINAPI wow_user_borrow_client_state(HWND window, PWND state)
{
    CHECK(depth == 1 && window == state->head.h);
    CHECK(state == &control || state == &parent || state == &owner);
    ++retains;
    if (deny_retain || state == rejected_state) { SetLastError(703); return NULL; }
    return (wow_window_words_binding *)(ULONG_PTR)1;
}
__declspec(noreturn) VOID WINAPI wow_user_native_call_failed(const char *stage, DWORD error)
{
    CHECK(stage != NULL && error == ERROR_INVALID_STATE);
    RaiseException(TEST_EXCEPTION, 0, 0, NULL);
    ExitProcess(99);
}
HBRUSH WINAPI wow_edit_NtUserGetControlColor(HWND receiver, HWND source, HDC dc, UINT message)
{
    CHECK(depth == 1 && receiver == expected && source == control.head.h);
    CHECK(dc == (HDC)(ULONG_PTR)4 && message == WM_CTLCOLORLISTBOX);
    ++calls;
    SetLastError(704);
    if (raise_color) RaiseException(TEST_EXCEPTION, 0, 0, NULL);
    return (HBRUSH)(ULONG_PTR)5;
}
static HBRUSH invoke(void)
{
    return GetControlBrush(control.head.h, (HDC)(ULONG_PTR)4, WM_CTLCOLORLISTBOX);
}
static void check_selection(DWORD style, PWND related_parent, PWND related_owner, HWND recipient)
{
    unsigned before = calls, held = retains;
    control.style = style; control.spwndParent = related_parent; control.spwndOwner = related_owner;
    expected = recipient;
    CHECK(invoke() == (HBRUSH)(ULONG_PTR)5);
    CHECK(depth == 0 && calls == before + 1 && retains == held + 2 && GetLastError() == 704);
}
int __cdecl main(void)
{
    unsigned before;
    BOOL caught;
    control.head.h = (HWND)(ULONG_PTR)1;
    parent.head.h = (HWND)(ULONG_PTR)2;
    owner.head.h = (HWND)(ULONG_PTR)3;
    check_selection(WS_CHILD, &parent, &owner, parent.head.h);
    check_selection(WS_POPUP, &parent, &owner, owner.head.h);
    check_selection(WS_CHILD | WS_POPUP, &parent, &owner, owner.head.h);
    check_selection(0, &parent, &owner, parent.head.h);
    check_selection(WS_POPUP, &parent, NULL, control.head.h);
    check_selection(WS_CHILD, NULL, &owner, control.head.h);
    before = calls;
    CHECK(!GetControlBrush(control.head.h, NULL, WM_CTLCOLORLISTBOX));
    CHECK(calls == before && depth == 0);
    deny_enter = TRUE; CHECK(!invoke() && GetLastError() == 701); deny_enter = FALSE;
    deny_validate = TRUE; CHECK(!invoke() && GetLastError() == 702); deny_validate = FALSE;
    CHECK(calls == before && depth == 0);
    deny_retain = TRUE; caught = FALSE;
    __try { (void)invoke(); }
    __except(GetExceptionCode() == TEST_EXCEPTION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught && depth == 0 && calls == before); deny_retain = FALSE;
    control.style = WS_CHILD; control.spwndParent = &parent; expected = parent.head.h;
    rejected_state = &parent; caught = FALSE;
    __try { (void)invoke(); }
    __except(GetExceptionCode() == TEST_EXCEPTION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught && depth == 0 && calls == before); rejected_state = NULL;
    raise_color = TRUE; caught = FALSE;
    __try { (void)invoke(); }
    __except(GetExceptionCode() == TEST_EXCEPTION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught && depth == 0 && calls == before + 1); raise_color = FALSE;
    check_selection(WS_CHILD, &parent, NULL, parent.head.h);
    CHECK(leaves == 11);
    printf("control_brush_failures=%u calls=%u\n", failures, calls);
    return failures != 0;
}
