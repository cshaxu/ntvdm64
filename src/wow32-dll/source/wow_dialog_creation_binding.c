#include "wow_dialog_creation_binding.h"
#include "wow_edit_lookaside_bindings.h"
#include "wow_user_task_lifecycle.h"
#include "wow_user_borrow_scope.h"
#include "wow_dialog_context_bindings.h"
#include "opennt-abi/host-compat/include/wow32_provider_private.h"

/* ADAPTER-WOW-063 supersedes ADAPTER-WOW-052's native creator approximation.
 * Original dlgbegin.c owns template parsing, PDLG, WOWDlgInit, children,
 * initialization, revalidation and rollback. Its private native creation
 * calls use the existing explicit HWND enrollment boundary.
 * Select only with the complete dialog/EDIT dependency group. */
HWND WINAPI InternalCreateDialog(HANDLE, LPDLGTEMPLATE, DWORD, HWND,
    DLGPROC, LONG, UINT);
int WINAPI wow_original_DialogBoxIndirectParamAorW(HINSTANCE, LPCDLGTEMPLATEW,
    HWND, DLGPROC, LPARAM, UINT);

typedef struct wow_dialog_call_scope {
    wow_task_callback_scope task;
    wow_user_borrow_scope borrow;
    wow_dialog_context context;
    wow_dialog_context *previous;
    wow_default_context *previous_geometry;
} wow_dialog_call_scope;

/* Both recovered original call shapes reach InternalCreateDialog, whose
 * precompiled gpsi/oemInfo aliases require this provider-local TLS context.
 * This owns only the entered call lease.  Original clres.c and dlgbegin.c
 * retain all dialog policy, parsing, creation and rollback. */
static BOOL wow_dialog_call_scope_enter(wow_dialog_call_scope *scope)
{
    DWORD error;
    wow_user_runtime_thread *runtime = wow_user_runtime_current();

    ZeroMemory(scope, sizeof(*scope));
    scope->previous = wow_dialog_current;
    scope->previous_geometry = wow_default_current;
    if (!runtime || !runtime->thread || !runtime->thread->ppi ||
            !runtime->thread->ppi->objects) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    if (runtime->thread->TIF_flags & WOW_TASK_TIF_INCLEANUP) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    if (!wow_edit_lookaside_ready()) return FALSE;
    if (!wow_task_callback_enter(&scope->task)) return FALSE;
    if (!wow_user_borrow_enter(&scope->borrow)) {
        error = GetLastError();
        (void)wow_task_callback_leave(&scope->task);
        SetLastError(error);
        return FALSE;
    }
    if (!wow_dialog_capture_context(&scope->context, TRUE)) {
        error = GetLastError();
        wow_user_borrow_leave_required(&scope->borrow);
        (void)wow_task_callback_leave(&scope->task);
        SetLastError(error);
        return FALSE;
    }
    wow_dialog_current = &scope->context;
    wow_default_current = &scope->context.geometry;
    return TRUE;
}

static VOID wow_dialog_call_scope_leave(wow_dialog_call_scope *scope)
{
    DWORD error = GetLastError();

    wow_dialog_current = scope->previous;
    wow_default_current = scope->previous_geometry;
    wow_dialog_release_context(&scope->context);
    wow_user_borrow_leave_required(&scope->borrow);
    (void)wow_task_callback_leave(&scope->task);
    SetLastError(error);
}

HWND WINAPI wow_user_server_create_dialog(HANDLE module,
    LPDLGTEMPLATE dialog_template, DWORD bytes, HWND owner, DLGPROC procedure,
    LONG init_parameter, UINT flags)
{
    DWORD error = GetLastError();
    wow_user_runtime_thread *runtime = wow_user_runtime_current();
    wow_dialog_call_scope scope;
    HWND result = NULL;

    if (!dialog_template || !bytes || !runtime || !runtime->thread ||
            !runtime->thread->ppi || !runtime->thread->ppi->objects ||
            (flags & ~(SCDLG_CLIENT | SCDLG_ANSI | SCDLG_NOREVALIDATE |
                SCDLG_16BIT))) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }
    if (!wow_dialog_call_scope_enter(&scope)) return NULL;
    __try {
        SetLastError(error);
        result = InternalCreateDialog(module, dialog_template, bytes, owner,
            procedure, init_parameter, flags);
    } __finally {
        wow_dialog_call_scope_leave(&scope);
    }
    return result;
}

int WINAPI wow_user_modal_dialog_box(HINSTANCE module,
    LPCDLGTEMPLATEW dialog_template, HWND owner, DLGPROC procedure,
    LPARAM init_parameter, UINT flags)
{
    DWORD error = GetLastError();
    wow_dialog_call_scope scope;
    int result = -1;

    if (dialog_template == NULL) return -1;
    if (!wow_dialog_call_scope_enter(&scope)) return -1;
    __try {
        SetLastError(error);
        result = wow_original_DialogBoxIndirectParamAorW(module,
            dialog_template, owner, procedure, init_parameter, flags);
    } __finally {
        wow_dialog_call_scope_leave(&scope);
    }
    return result;
}
