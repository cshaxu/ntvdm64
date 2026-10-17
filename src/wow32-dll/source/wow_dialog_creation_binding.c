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

HWND WINAPI wow_user_server_create_dialog(HANDLE module,
    LPDLGTEMPLATE dialog_template, DWORD bytes, HWND owner, DLGPROC procedure,
    LONG init_parameter, UINT flags)
{
    DWORD error = GetLastError();
    wow_user_runtime_thread *runtime = wow_user_runtime_current();
    wow_task_callback_scope task;
    wow_user_borrow_scope borrow;
    HWND result = NULL;
    wow_dialog_context context = {0}, *previous = wow_dialog_current;
    wow_default_context *previous_geometry = wow_default_current;

    if (!dialog_template || !bytes || !runtime || !runtime->thread ||
            !runtime->thread->ppi || !runtime->thread->ppi->objects ||
            (flags & ~(SCDLG_CLIENT | SCDLG_ANSI | SCDLG_NOREVALIDATE |
                SCDLG_16BIT))) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }
    if (runtime->thread->TIF_flags & WOW_TASK_TIF_INCLEANUP) {
        SetLastError(ERROR_INVALID_STATE);
        return NULL;
    }
    if (!wow_edit_lookaside_ready()) return NULL;
    if (!wow_task_callback_enter(&task)) return NULL;
    if (!wow_user_borrow_enter(&borrow)) {
        error = GetLastError();
        (void)wow_task_callback_leave(&task);
        SetLastError(error);
        return NULL;
    }
    __try {
        if (!wow_dialog_capture_context(&context, TRUE)) __leave;
        wow_dialog_current = &context;
        wow_default_current = &context.geometry;
        SetLastError(error);
        result = InternalCreateDialog(module, dialog_template, bytes, owner,
            procedure, init_parameter, flags);
    } __finally {
        error = GetLastError();
        wow_dialog_current = previous;
        wow_default_current = previous_geometry;
        wow_dialog_release_context(&context);
        wow_user_borrow_leave_required(&borrow);
        (void)wow_task_callback_leave(&task);
        SetLastError(error);
    }
    return result;
}
