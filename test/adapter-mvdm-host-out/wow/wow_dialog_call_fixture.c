#include "wow_dialog_native_bindings.h"
#include "wow_dialog_state_bindings.h"
#include "wow_user_task_lifecycle.h"
#include <stdio.h>

/* Supplemental actual call-boundary test with explicit native/scope doubles.
 * Real no-notification behavior is tested separately by the native fixture. */
static unsigned failures, depth, begins, ends, calls, fatal_calls;
static BOOL reject_begin, reject_end, reject_service, raise_service;
static DWORD fatal_error;
static DWORD queue_flags;
static unsigned queue_calls;
static HWND expected_target;
static unsigned menu_calls;
static BOOL reject_ready;
static PDLG retained_dialog;
static unsigned pointer_calls;
#define TEST_EXCEPTION ((DWORD)0xe0424473)
#define CHECK(c) do { if (!(c)) { ++failures; printf("FAIL %d\n", __LINE__); } } while (0)

BOOL WINAPI wow_user_native_call_begin(wow_user_native_call *call, HWND target)
{
    (void)call; ++begins; CHECK(target == expected_target);
    if (reject_begin) { SetLastError(701); return FALSE; }
    ++depth; SetLastError(702); return TRUE;
}
BOOL WINAPI wow_user_native_call_end(wow_user_native_call *call)
{
    (void)call; CHECK(depth == 1); --depth; ++ends;
    SetLastError(reject_end ? 703 : 704);
    return !reject_end;
}
BOOL WINAPI wow_dialog_native_zap_active_and_focus(void)
{
    CHECK(depth == 1 && GetLastError() == 456); ++calls;
    SetLastError(705);
    if (raise_service) RaiseException(TEST_EXCEPTION, 0, 0, NULL);
    return !reject_service;
}
BOOL WINAPI wow_dialog_native_ready(void)
{
    if (reject_ready) { SetLastError(706); return FALSE; }
    return TRUE;
}
BOOL WINAPI wow_dialog_set_pointer(HWND window, PDLG dialog)
{
    CHECK(window == expected_target && depth == 0);
    ++pointer_calls;
    if (reject_service) { SetLastError(708); return FALSE; }
    retained_dialog = dialog;
    return TRUE;
}
BOOL WINAPI wow_dialog_native_set_system_menu(HWND window)
{
    CHECK(window == expected_target && depth == 1 && GetLastError() == 456);
    ++menu_calls;
    SetLastError(707);
    if (raise_service) RaiseException(TEST_EXCEPTION, 0, 0, NULL);
    return !reject_service;
}
BOOL WINAPI wow_dialog_native_set_thread_state(DWORD flags, DWORD mask)
{
    CHECK(depth == 0 && mask == 0x4000 && (flags == 0 || flags == 0x4000));
    ++queue_calls;
    SetLastError(705);
    if (reject_service) return FALSE;
    queue_flags = (queue_flags & ~mask) | (flags & mask);
    return TRUE;
}
__declspec(noreturn) void WINAPI wow_user_native_call_failed(const char *stage, DWORD error)
{
    CHECK(stage != NULL); ++fatal_calls; fatal_error = error;
    RaiseException(TEST_EXCEPTION, 0, 0, NULL);
    ExitProcess(99);
}
static void invoke(BOOL should_raise)
{
    BOOL caught = FALSE;
    SetLastError(456);
    __try { wow_dialog_zap_active_and_focus(); }
    __except (GetExceptionCode() == TEST_EXCEPTION ?
        EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught == should_raise && depth == 0);
}
static void invoke_menu(BOOL should_raise, BOOL expected_result)
{
    BOOL caught = FALSE, result = FALSE;
    SetLastError(456);
    __try { result = wow_dialog_set_system_menu(expected_target); }
    __except (GetExceptionCode() == TEST_EXCEPTION ?
        EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught == should_raise && depth == 0);
    if (!caught) CHECK(result == expected_result && GetLastError() == 707);
}
int main(void)
{
    BOOL caught = FALSE;
    expected_target = (HWND)(ULONG_PTR)0x1234;
    SetLastError(456);
    wow_dialog_set_pointer_word(expected_target, 0x4560);
    CHECK(retained_dialog == (PDLG)(ULONG_PTR)0x4560 && GetLastError() == 456);
    wow_dialog_set_pointer_word(expected_target, 0);
    CHECK(retained_dialog == NULL && pointer_calls == 2);
    reject_service = TRUE;
    __try { wow_dialog_set_pointer_word(expected_target, 0x4560); }
    __except (GetExceptionCode() == TEST_EXCEPTION ?
        EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught && retained_dialog == NULL && fatal_error == 708);
    CHECK(begins == 0 && ends == 0);
    expected_target = NULL;
    reject_service = caught = FALSE;
    fatal_calls = 0;
    wow_dialog_set_thread_state(0x4000, 0x4000);
    CHECK(queue_flags == 0x4000 && queue_calls == 1 && GetLastError() == 705);
    wow_dialog_set_thread_state(0, 0x4000);
    CHECK(queue_flags == 0 && queue_calls == 2);
    CHECK(begins == 0 && ends == 0);
    reject_service = TRUE;
    __try { wow_dialog_set_thread_state(0x4000, 0x4000); }
    __except (GetExceptionCode() == TEST_EXCEPTION ?
        EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught && queue_flags == 0 && queue_calls == 3 && fatal_error == 705);
    CHECK(fatal_calls == 1 && begins == 0 && ends == 0);
    reject_service = FALSE;
    fatal_calls = 0; /* The following independent reset-path assertions. */
    invoke(FALSE);
    CHECK(begins == 1 && ends == 1 && calls == 1 && fatal_calls == 0);
    CHECK(GetLastError() == 705);
    reject_begin = TRUE; invoke(TRUE); reject_begin = FALSE;
    CHECK(ends == 1 && calls == 1 && fatal_calls == 1 && fatal_error == 701);
    reject_service = TRUE; invoke(TRUE); reject_service = FALSE;
    CHECK(ends == 2 && calls == 2 && fatal_calls == 2 && fatal_error == 705);
    raise_service = TRUE; invoke(TRUE); raise_service = FALSE;
    CHECK(ends == 3 && calls == 3 && fatal_calls == 2);
    reject_end = TRUE; invoke(TRUE); reject_end = FALSE;
    CHECK(ends == 4 && calls == 4 && fatal_calls == 3 && fatal_error == 703);
    invoke(FALSE);
    CHECK(ends == 5 && calls == 5 && fatal_calls == 3);
    begins = ends = fatal_calls = 0;
    expected_target = (HWND)(ULONG_PTR)0x1234;
    invoke_menu(FALSE, TRUE);
    CHECK(begins == 1 && ends == 1 && menu_calls == 1);
    reject_service = TRUE; invoke_menu(FALSE, FALSE); reject_service = FALSE;
    CHECK(ends == 2 && menu_calls == 2 && fatal_calls == 0);
    raise_service = TRUE; invoke_menu(TRUE, FALSE); raise_service = FALSE;
    CHECK(ends == 3 && menu_calls == 3 && fatal_calls == 0);
    reject_ready = TRUE; invoke_menu(TRUE, FALSE); reject_ready = FALSE;
    CHECK(begins == 3 && ends == 3 && menu_calls == 3 && fatal_error == 706);
    reject_begin = TRUE; invoke_menu(TRUE, FALSE); reject_begin = FALSE;
    CHECK(ends == 3 && menu_calls == 3 && fatal_error == 701);
    reject_end = TRUE; invoke_menu(TRUE, FALSE); reject_end = FALSE;
    CHECK(ends == 4 && menu_calls == 4 && fatal_error == 703);
    invoke_menu(FALSE, TRUE);
    CHECK(ends == 5 && menu_calls == 5 && fatal_calls == 3);
    printf("dialog-call failures=%u\n", failures);
    return failures ? 1 : 0;
}
