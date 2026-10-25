#include "wow_dialog_creation_binding.h"
#include "wow_dialog_context_bindings.h"
#include "wow_user_task_lifecycle.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_object_bindings.h"
#include "opennt-abi/host-compat/include/wow32_provider_private.h"
#include <stdio.h>

/* Actual thin entry, with original creator/task/borrow transport doubles.
 * Tests exact forwarding and scope restoration, not template parsing,
 * native HWNDs, original allocation/rollback or guest callback behavior. */
static wow_user_runtime_thread runtime;
static wow_task_order_thread thread;
static wow_task_order_process process;
static wow_user_object_table objects;
static DLGTEMPLATE resource;
static unsigned errors, tasks, borrows, calls, modal_calls;
static BOOL reject_task, reject_borrow, fail_create, raise_create, fail_ready;
static unsigned ready_calls;
static UINT expected_flags;
static BOOL fail_context;
static wow_dialog_context outer_context;
__declspec(thread) wow_dialog_context *wow_dialog_current;
__declspec(thread) wow_default_context *wow_default_current;
#define EXCEPTION_CODE ((DWORD)0xe0447303)
#define CHECK(c) do { if (!(c)) { ++errors; printf("FAIL line=%d\n", __LINE__); } } while (0)
static INT_PTR CALLBACK procedure(HWND w, UINT m, WPARAM wp, LPARAM lp)
{ (void)w; (void)m; (void)wp; (void)lp; CHECK(FALSE); return 0; }
BOOL WINAPI wow_dialog_capture_context(wow_dialog_context *context, BOOL font)
{
    CHECK(font && tasks && borrows);
    if (fail_context) { SetLastError(ERROR_NOT_READY); return FALSE; }
    ZeroMemory(context, sizeof(*context));
    context->server.hMsgFont = (HFONT)(ULONG_PTR)0x123;
    return TRUE;
}
VOID WINAPI wow_dialog_release_context(wow_dialog_context *context)
{ context->server.hMsgFont = NULL; }
wow_user_runtime_thread *WINAPI wow_user_runtime_current(void) { return &runtime; }
BOOL WINAPI wow_edit_lookaside_ready(void)
{
    CHECK(!tasks && !borrows);
    ++ready_calls;
    if (fail_ready) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    return TRUE;
}
BOOL WINAPI wow_task_callback_enter(wow_task_callback_scope *scope)
{
    (void)scope;
    if (reject_task) { SetLastError(ERROR_BUSY); return FALSE; }
    ++tasks; return TRUE;
}
BOOL WINAPI wow_task_callback_leave(wow_task_callback_scope *scope)
{ (void)scope; CHECK(tasks); --tasks; return TRUE; }
BOOL WINAPI wow_user_borrow_enter(wow_user_borrow_scope *scope)
{
    (void)scope;
    if (reject_borrow) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    ++borrows; return TRUE;
}
VOID WINAPI wow_user_borrow_leave_required(wow_user_borrow_scope *scope)
{ (void)scope; CHECK(borrows); --borrows; }
HWND WINAPI InternalCreateDialog(HANDLE module, LPDLGTEMPLATE data, DWORD bytes,
    HWND owner, DLGPROC proc, LONG parameter, UINT flags)
{
    CHECK(ready_calls && tasks == 1 && borrows == 1);
    CHECK(wow_dialog_current && wow_dialog_current->server.hMsgFont == (HFONT)(ULONG_PTR)0x123);
    CHECK(wow_dialog_current != &outer_context);
    CHECK(wow_default_current == &wow_dialog_current->geometry);
    CHECK(module == (HANDLE)0x1235 && data == &resource && bytes == sizeof(resource));
    CHECK(owner == (HWND)0x2468 && proc == procedure && parameter == 0x76543210);
    CHECK(flags == expected_flags);
    ++calls;
    if (raise_create) RaiseException(EXCEPTION_CODE, 0, 0, NULL);
    SetLastError(fail_create ? ERROR_INVALID_DATA : 0x4473);
    return fail_create ? NULL : (HWND)0x3456;
}
int WINAPI wow_original_DialogBoxIndirectParamAorW(HINSTANCE module,
    LPCDLGTEMPLATEW data, HWND owner, DLGPROC proc, LPARAM parameter,
    UINT flags)
{
    CHECK(ready_calls && tasks == 1 && borrows == 1);
    CHECK(wow_dialog_current &&
        wow_dialog_current->server.hMsgFont == (HFONT)(ULONG_PTR)0x123);
    CHECK(wow_dialog_current != &outer_context);
    CHECK(wow_default_current == &wow_dialog_current->geometry);
    CHECK(module == (HINSTANCE)0x1235 && data == (LPCDLGTEMPLATEW)&resource);
    CHECK(owner == (HWND)0x2468 && proc == procedure && parameter == 0x76543210);
    CHECK(flags == expected_flags);
    ++modal_calls;
    SetLastError(fail_create ? ERROR_INVALID_DATA : 0x4473);
    return fail_create ? -1 : 49;
}
static HWND create(void)
{
    return wow_user_server_create_dialog((HANDLE)0x1235, &resource,
        sizeof(resource), (HWND)0x2468, procedure, 0x76543210, expected_flags);
}
static int create_modal(void)
{
    return wow_user_modal_dialog_box((HINSTANCE)0x1235,
        (LPCDLGTEMPLATEW)&resource, (HWND)0x2468, procedure, 0x76543210,
        expected_flags);
}
int main(void)
{
    unsigned before, ansi;
    runtime.thread = &thread; thread.ppi = &process; process.objects = &objects;
    /* An inner entry must restore the caller's context on success, ordinary
     * failure and SEH unwind, without releasing the caller's font. */
    outer_context.server.hMsgFont = (HFONT)(ULONG_PTR)0x456;
    wow_dialog_current = &outer_context;
    wow_default_current = &outer_context.geometry;
    for (ansi = 0; ansi != 2; ++ansi) {
        expected_flags = SCDLG_CLIENT | SCDLG_16BIT | SCDLG_NOREVALIDATE |
            (ansi ? SCDLG_ANSI : 0);
        CHECK(create() == (HWND)0x3456 && GetLastError() == 0x4473);
        CHECK(!tasks && !borrows);
        CHECK(wow_dialog_current == &outer_context);
    }
    expected_flags = SCDLG_ANSI;
    CHECK(create_modal() == 49 && GetLastError() == 0x4473);
    CHECK(modal_calls == 1 && !tasks && !borrows);
    CHECK(wow_dialog_current == &outer_context);
    fail_create = TRUE;
    CHECK(create_modal() == -1 && GetLastError() == ERROR_INVALID_DATA);
    CHECK(modal_calls == 2 && !tasks && !borrows);
    fail_create = FALSE;
    fail_create = TRUE;
    CHECK(!create() && GetLastError() == ERROR_INVALID_DATA);
    CHECK(wow_dialog_current == &outer_context);
    fail_create = FALSE; raise_create = TRUE;
    __try { (void)create(); CHECK(FALSE); }
    __except (GetExceptionCode() == EXCEPTION_CODE ?
        EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { }
    raise_create = FALSE; CHECK(!tasks && !borrows);
    CHECK(wow_dialog_current == &outer_context);
    before = calls; fail_ready = TRUE;
    CHECK(!create() && GetLastError() == ERROR_NOT_ENOUGH_MEMORY);
    CHECK(calls == before && !tasks && !borrows);
    fail_ready = FALSE; reject_task = TRUE;
    CHECK(!create() && GetLastError() == ERROR_BUSY);
    reject_task = FALSE; reject_borrow = TRUE;
    CHECK(!create() && GetLastError() == ERROR_NOT_ENOUGH_MEMORY);
    reject_borrow = FALSE; thread.TIF_flags = WOW_TASK_TIF_INCLEANUP;
    CHECK(!create() && GetLastError() == ERROR_INVALID_STATE);
    thread.TIF_flags = 0; expected_flags = 0x80000000;
    CHECK(!create() && GetLastError() == ERROR_INVALID_PARAMETER);
    CHECK(calls == before && !tasks && !borrows);
    expected_flags = SCDLG_CLIENT;
    fail_context = TRUE;
    CHECK(!create() && GetLastError() == ERROR_NOT_READY);
    CHECK(calls == before && !tasks && !borrows);
    CHECK(wow_dialog_current == &outer_context);
    CHECK(outer_context.server.hMsgFont == (HFONT)(ULONG_PTR)0x456);
    CHECK(wow_default_current == &outer_context.geometry);
    wow_dialog_current = NULL;
    wow_default_current = NULL;
    printf("dialog-creation-entry failures=%u\n", errors);
    return errors ? 1 : 0;
}
