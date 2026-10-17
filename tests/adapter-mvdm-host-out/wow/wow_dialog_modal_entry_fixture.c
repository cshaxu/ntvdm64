#include "wow_title_policy.h"
#include "wow_dialog_context_bindings.h"
#include "wow_user_task_lifecycle.h"
#include "wow_user_borrow_scope.h"
#include "opennt-abi/host-compat/include/wow32_provider_private.h"
#include "common/protocol/window_style.h"
#include <stdio.h>

/* Actual modal entry; original manager and execution transport are doubles.
 * No original parsing, modal scheduling or guest acceptance is claimed. */
static unsigned errors, tasks, borrows, calls, contexts;
static BOOL reject_capture, raise_manager, nest;
static DLGTEMPLATE resource;
static wow_dialog_context outer;
__declspec(thread) wow_dialog_context *wow_dialog_current;
__declspec(thread) wow_default_context *wow_default_current;
#define TEST_EXCEPTION ((DWORD)0xe0447341)
#define CHECK(c) do { if (!(c)) { ++errors; printf("FAIL line=%d\n", __LINE__); } } while (0)
wow_user_runtime_thread *WINAPI wow_user_runtime_current(void) { return NULL; }
BOOL WINAPI wow_user_task_lifecycle_style(wow_user_task_lifecycle *p, DWORD *v)
{ (void)p; (void)v; CHECK(FALSE); return FALSE; }
BOOL WINAPI wow_edit_lookaside_ready(void) { return TRUE; }
BOOL WINAPI wow_task_callback_enter(wow_task_callback_scope *p)
{ (void)p; ++tasks; return TRUE; }
BOOL WINAPI wow_task_callback_leave(wow_task_callback_scope *p)
{ (void)p; CHECK(tasks); --tasks; SetLastError(0xdead); return TRUE; }
BOOL WINAPI wow_user_borrow_enter(wow_user_borrow_scope *p)
{ (void)p; ++borrows; return TRUE; }
VOID WINAPI wow_user_borrow_leave_required(wow_user_borrow_scope *p)
{ (void)p; CHECK(borrows); --borrows; SetLastError(0xbeef); }
BOOL WINAPI wow_dialog_capture_context(wow_dialog_context *p, BOOL font)
{
    CHECK(font && tasks && borrows);
    if (reject_capture) { SetLastError(ERROR_NOT_READY); return FALSE; }
    ZeroMemory(p, sizeof(*p));
    p->server.hMsgFont = (HFONT)(ULONG_PTR)++contexts;
    return TRUE;
}
VOID WINAPI wow_dialog_release_context(wow_dialog_context *p)
{
    if (p->server.hMsgFont) { CHECK(contexts); --contexts; p->server.hMsgFont = NULL; }
    SetLastError(0xcafe);
}
static INT_PTR invoke(void)
{
    return wow_title_modal((HINSTANCE)0x1234, &resource, (HWND)0x5678,
        NULL, 91, SCDLG_16BIT | SCDLG_ANSI, WINDOW_STYLE_OFF);
}
int WINAPI wow_original_DialogBoxIndirectParamAorW(HINSTANCE module, LPCDLGTEMPLATEW data,
    HWND owner, DLGPROC procedure, LPARAM param, UINT flags)
{
    wow_dialog_context *current = wow_dialog_current;
    CHECK(module == (HINSTANCE)0x1234 && data == &resource && owner == (HWND)0x5678);
    CHECK(!procedure && param == 91 && flags == (SCDLG_16BIT | SCDLG_ANSI));
    CHECK(tasks && tasks == borrows && contexts == tasks);
    CHECK(current && current != &outer && current->server.hMsgFont);
    CHECK(wow_default_current == &current->geometry);
    ++calls;
    if (nest) {
        nest = FALSE;
        CHECK(invoke() == 47);
        CHECK(wow_dialog_current == current && contexts == 1 && tasks == 1);
        CHECK(wow_default_current == &current->geometry);
    }
    SetLastError(0x4473);
    if (raise_manager) RaiseException(TEST_EXCEPTION, 0, 0, NULL);
    return 47;
}
int main(void)
{
    unsigned before;
    BOOL caught = FALSE;
    outer.server.hMsgFont = (HFONT)0x1111;
    wow_dialog_current = &outer;
    wow_default_current = &outer.geometry;
    nest = TRUE;
    CHECK(invoke() == 47 && GetLastError() == 0x4473 && calls == 2);
    CHECK(wow_dialog_current == &outer && !tasks && !borrows && !contexts);
    before = calls; reject_capture = TRUE;
    CHECK(invoke() == -1 && GetLastError() == ERROR_NOT_READY && calls == before);
    CHECK(wow_dialog_current == &outer && !tasks && !borrows && !contexts);
    reject_capture = FALSE; raise_manager = TRUE;
    __try { (void)invoke(); CHECK(FALSE); }
    __except (GetExceptionCode() == TEST_EXCEPTION ?
        EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught && GetLastError() == 0x4473);
    CHECK(wow_dialog_current == &outer && !tasks && !borrows && !contexts);
    CHECK(outer.server.hMsgFont == (HFONT)0x1111);
    CHECK(wow_default_current == &outer.geometry);
    wow_dialog_current = NULL;
    wow_default_current = NULL;
    printf("dialog-modal-entry failures=%u\n", errors);
    return errors ? 1 : 0;
}
