#include "wow_dialog_creation_binding.h"
#include "wow_dialog_context_bindings.h"
#include "wow_user_task_lifecycle.h"
#include "wow_user_borrow_scope.h"
#include <stdio.h>

/* Entry mechanics only: original algorithm is explicitly doubled here. */
static unsigned failures, tasks, borrows, calls;
static BOOL fail_task, fail_borrow, fail_context, raise_body, false_body, nested;
static wow_dialog_context outer;
__declspec(thread) wow_dialog_context *wow_dialog_current;
#define FAULT ((DWORD)0xe0447119)
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL line=%d\n", __LINE__); } } while (0)
BOOL WINAPI wow_task_callback_enter(wow_task_callback_scope *scope)
{
    (void)scope;
    if (fail_task) { SetLastError(ERROR_BUSY); return FALSE; }
    ++tasks; return TRUE;
}
BOOL WINAPI wow_task_callback_leave(wow_task_callback_scope *scope)
{ (void)scope; CHECK(tasks); --tasks; SetLastError(1); return TRUE; }
BOOL WINAPI wow_user_borrow_enter(wow_user_borrow_scope *scope)
{
    (void)scope;
    if (fail_borrow) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    ++borrows; return TRUE;
}
VOID WINAPI wow_user_borrow_leave_required(wow_user_borrow_scope *scope)
{ (void)scope; CHECK(borrows); --borrows; SetLastError(2); }
BOOL WINAPI wow_dialog_capture_context(wow_dialog_context *context, BOOL font)
{
    CHECK(!font && tasks && borrows);
    if (fail_context) { SetLastError(ERROR_NOT_READY); return FALSE; }
    ZeroMemory(context, sizeof(*context)); return TRUE;
}
VOID WINAPI wow_dialog_release_context(wow_dialog_context *context)
{ (void)context; SetLastError(3); }
static BOOL original(HWND window, LPMSG message, BOOL ansi)
{
    wow_dialog_context *saved = wow_dialog_current;
    CHECK(tasks == borrows && tasks && saved && saved != &outer);
    CHECK(window == (HWND)(ULONG_PTR)19 && message->message == WM_CHAR);
    ++calls;
    if (nested) {
        CHECK(ansi && tasks == 1);
        nested = FALSE;
        CHECK(wow_dialog_messageW(window, message));
        CHECK(wow_dialog_current == saved && tasks == 1 && borrows == 1);
    }
    SetLastError(0x447119);
    if (raise_body) RaiseException(FAULT, 0, 0, NULL);
    return !false_body;
}
BOOL WINAPI wow_original_IsDialogMessageA(HWND w, LPMSG m) { return original(w, m, TRUE); }
BOOL WINAPI wow_original_IsDialogMessageW(HWND w, LPMSG m) { return original(w, m, FALSE); }
int main(void)
{
    MSG message = {0};
    HWND window = (HWND)(ULONG_PTR)19;
    unsigned before;
    BOOL caught = FALSE;
    message.message = WM_CHAR;
    wow_dialog_current = &outer;
    nested = TRUE;
    CHECK(wow_dialog_messageA(window, &message));
    CHECK(calls == 2 && GetLastError() == 0x447119);
    CHECK(!tasks && !borrows && wow_dialog_current == &outer);
    false_body = TRUE;
    CHECK(!wow_dialog_messageW(window, &message) && GetLastError() == 0x447119);
    false_body = FALSE; raise_body = TRUE;
    __try { (void)wow_dialog_messageA(window, &message); }
    __except (GetExceptionCode() == FAULT ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught && GetLastError() == 0x447119);
    CHECK(!tasks && !borrows && wow_dialog_current == &outer);
    raise_body = FALSE; before = calls;
    fail_task = TRUE;
    CHECK(!wow_dialog_messageW(window, &message) && GetLastError() == ERROR_BUSY);
    fail_task = FALSE; fail_borrow = TRUE;
    CHECK(!wow_dialog_messageW(window, &message) && GetLastError() == ERROR_NOT_ENOUGH_MEMORY);
    fail_borrow = FALSE; fail_context = TRUE;
    CHECK(!wow_dialog_messageW(window, &message) && GetLastError() == ERROR_NOT_READY);
    fail_context = FALSE;
    CHECK(!wow_dialog_messageW(window, NULL) && GetLastError() == ERROR_INVALID_PARAMETER);
    CHECK(calls == before && !tasks && !borrows && wow_dialog_current == &outer);
    CHECK(wow_dialog_messageW(window, &message));
    CHECK(!tasks && !borrows && wow_dialog_current == &outer);
    printf("dialog-keyboard-entry failures=%u\n", failures);
    return failures ? 1 : 0;
}
