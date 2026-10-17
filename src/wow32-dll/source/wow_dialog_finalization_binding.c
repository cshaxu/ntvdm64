#include "wow_dialog_state_bindings.h"
#include "wow_edit_lookaside_bindings.h"
#include "wow_user_task_lifecycle.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_private_access.h"
#include "wow_dialog_context_bindings.h"
#include "wow_dialog_procedure_bindings.h"

/* ADAPTER-WOW-063: original createw.c's canonical final-destruction edge.
 * DefDlgProcWorker retains the resource policy; never duplicate its EDIT DS,
 * font, lookaside or SetDialogPointer cleanup in this host boundary.
 * Selected only with the complete original dialog group. */
LONG WINAPI DefDlgProcWorker(PWND, UINT, WPARAM, LPARAM, DWORD);
/* Private-precomp name binding for retained dlgend.c; not native EndDialog. */
BOOL WINAPI wow_original_EndDialog(HWND, int);

enum dialog_operation { dialog_message, dialog_finalize, dialog_end,
    dialog_map, dialog_dir_list, dialog_dir_combo_list,
    dialog_dir_select, dialog_dir_combo_select };
typedef struct dialog_api_args {
    void *buffer;
    int first, second;
    UINT attributes;
} dialog_api_args;

static LRESULT original_api(HWND w, enum dialog_operation operation,
    const dialog_api_args *args)
{
    switch (operation) {
    case dialog_map: return wow_original_MapDialogRect(w, args->buffer);
    case dialog_dir_list: return wow_original_DlgDirListA(w, args->buffer,
        args->first, args->second, args->attributes);
    case dialog_dir_combo_list: return wow_original_DlgDirListComboBoxA(w,
        args->buffer, args->first, args->second, args->attributes);
    case dialog_dir_select: return wow_original_DlgDirSelectExA(w,
        args->buffer, args->first, args->second);
    case dialog_dir_combo_select: return wow_original_DlgDirSelectComboBoxExA(w,
        args->buffer, args->first, args->second);
    default: SetLastError(ERROR_INVALID_PARAMETER); return 0;
    }
}

static LRESULT dialog_entry(HWND window, UINT message, WPARAM wp, LPARAM lp,
    BOOL ansi, enum dialog_operation operation)
{
    DWORD error = GetLastError();
    wow_user_runtime_thread *runtime = wow_user_runtime_current();
    wow_window_words_binding *words;
    wow_cleanup_window *owner;
    wow_task_callback_scope task;
    wow_user_borrow_scope borrow;
    PWND state;
    BOOL cleanup;
    BOOL finalize = operation == dialog_finalize;
    LRESULT result = 0;
    wow_dialog_context context = {0}, *previous = wow_dialog_current;
    BOOL needs_context = operation >= dialog_map ||
        (operation == dialog_message && message != WM_NCDESTROY && message != 0x0070);

    if (!runtime || !runtime->thread || !runtime->thread->ptdb ||
            !runtime->thread->pClientInfo) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    /* A finalizer must not initialize a pool for a dialog whose allocation
     * failed. An existing PDLG already implies successful pool setup. */
    if (!finalize && !wow_edit_lookaside_ready()) return FALSE;
    words = wow_window_words_acquire(window);
    if (!words) return FALSE;
    owner = wow_window_words_cleanup_value(words);
    cleanup = (runtime->thread->TIF_flags & WOW_TASK_TIF_INCLEANUP) != 0;
    if (!owner || (cleanup && (owner->thread != runtime->thread || !finalize))) {
        wow_window_words_release(words);
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    if (!(cleanup ? wow_task_resource_cleanup_enter(&task) :
            wow_task_callback_enter(&task))) {
        error = GetLastError();
        wow_window_words_release(words);
        SetLastError(error);
        return FALSE;
    }
    if (!wow_user_borrow_enter(&borrow)) {
        error = GetLastError();
        wow_window_words_release(words);
        (void)wow_task_callback_leave(&task);
        SetLastError(error);
        return FALSE;
    }
    __try {
        if (needs_context) {
            if (!wow_dialog_capture_context(&context, FALSE)) __leave;
            wow_dialog_current = &context;
        }
        if (!finalize && !wow_window_client_ensure_state(window)) __leave;
        if (!finalize && !wow_window_client_prepare_graph(window)) __leave;
        if (operation == dialog_message && !wow_user_borrow_set_callback(window)) __leave;
        state = wow_window_client_validate_hwnd(window);
        if (!state) __leave;
        if (operation >= dialog_map) {
            SetLastError(error);
            result = original_api(window, operation, (const dialog_api_args *)wp);
            __leave;
        }
        if (operation == dialog_end) {
            SetLastError(error);
            result = wow_original_EndDialog(window, (int)wp);
            __leave;
        }
        if (!finalize) {
            /* Original ValidateDialogPwnd owns promotion/allocation and fake
             * dialog notification. The same state serves get/set and final
             * cleanup. Native/guest calls inside the worker retain their
             * existing scoped boundaries; this is not a native-call wrapper. */
            SetLastError(error);
            result = DefDlgProcWorker(state, message, wp, lp, ansi);
            __leave;
        }
        if (GETFNID(state) != FNID_DIALOG ||
                state->cbwndExtra < DLGWINDOWEXTRA) {
            SetLastError(ERROR_INVALID_STATE);
            __leave;
        }
        if (state->fnid & FNID_CLEANEDUP_BIT) {
            /* A completed original cleanup is idempotent. */
            result = !TestWF(state, WFDIALOGWINDOW) && !((PDIALOG)state)->pdlg;
        } else if (!(state->fnid & FNID_DELETED_BIT) &&
                !TestWF(state, WFDIALOGWINDOW) && !((PDIALOG)state)->pdlg) {
            /* VALIDATECLASSANDSIZE sets FNID before ValidateDialogPwnd
             * allocates PDLG. Original createw.c xxxFreeWindow completes
             * FNID_CLEANEDUP_BIT even when that allocation failed. There is
             * no resource to release and no reason to allocate on teardown. */
            state->fnid |= FNID_CLEANEDUP_BIT;
            result = TRUE;
        } else if (!(state->fnid & FNID_DELETED_BIT) &&
                TestWF(state, WFDIALOGWINDOW) && ((PDIALOG)state)->pdlg) {
            /* Original WM_FINALDESTROY jumps past lpfnDlg directly to
             * DoCleanup. Do not create a PDLG just to destroy it or send
             * this private message through an application subclass. */
            (void)DefDlgProcWorker(state, 0x0070, 0, 0, FALSE);
            result = (state->fnid & FNID_CLEANEDUP_BIT) &&
                !TestWF(state, WFDIALOGWINDOW) && !((PDIALOG)state)->pdlg;
        }
        SetLastError(result ? error : ERROR_INVALID_STATE);
    } __finally {
        DWORD final_error = GetLastError();
        /* Actual WM_NCDESTROY only, while HWND is valid. The later private
         * original finalizer must never re-enter native USER after retirement. */
        if (operation == dialog_message && message == WM_NCDESTROY)
            wow_dialog_host_retire(window);
        wow_dialog_current = previous;
        wow_dialog_release_context(&context);
        wow_window_words_release(words);
        wow_user_borrow_leave_required(&borrow);
        (void)wow_task_callback_leave(&task);
        SetLastError(final_error);
    }
    return result;
}

BOOL WINAPI wow_dialog_finalize_bound(HWND window)
{
    return (BOOL)dialog_entry(window, 0x0070, 0, 0, FALSE, dialog_finalize);
}

/* Only retained original storage can be supplied to original directory and
 * dialog algorithms. Unowned native windows keep their native implementation,
 * just as EndDialog below; never reinterpret native PCBOX/PDLG memory. */
static LRESULT dialog_api(HWND w, enum dialog_operation operation,
    const dialog_api_args *args)
{
    DWORD error = GetLastError();
    wow_window_words_binding *words = wow_window_words_acquire(w);
    PWND state = wow_window_words_client_state_value(words);
    BOOL recovered = words && wow_window_words_owns_client_state(words, state);
    LRESULT result = 0;
    wow_user_native_call call;
    __try {
        SetLastError(error);
        if (recovered) {
            result = dialog_entry(w, 0, (WPARAM)args, 0, TRUE, operation);
            __leave;
        }
        if (!wow_user_native_call_begin(&call, w)) __leave;
        __try {
            SetLastError(error);
            switch (operation) {
            case dialog_map: result = MapDialogRect(w, args->buffer); break;
            case dialog_dir_list: result = DlgDirListA(w, args->buffer,
                args->first, args->second, args->attributes); break;
            case dialog_dir_combo_list: result = DlgDirListComboBoxA(w,
                args->buffer, args->first, args->second, args->attributes); break;
            case dialog_dir_select: result = DlgDirSelectExA(w, args->buffer,
                args->first, args->second); break;
            case dialog_dir_combo_select: result = DlgDirSelectComboBoxExA(w,
                args->buffer, args->first, args->second); break;
            default: SetLastError(ERROR_INVALID_PARAMETER); break;
            }
        } __finally {
            error = GetLastError();
            if (!wow_user_native_call_end(&call))
                wow_user_native_call_failed("dialog-api-return", GetLastError());
            SetLastError(error);
        }
    } __finally {
        error = GetLastError();
        wow_window_words_release(words);
        SetLastError(error);
    }
    return result;
}

BOOL WINAPI wow_user_map_dialog_rect(HWND w, LPRECT rect)
{ dialog_api_args a = {rect,0,0,0}; return (BOOL)dialog_api(w,dialog_map,&a); }
int WINAPI wow_user_dlg_dir_listA(HWND w, LPSTR path, int list, int text, UINT flags)
{ dialog_api_args a = {path,list,text,flags}; return (int)dialog_api(w,dialog_dir_list,&a); }
int WINAPI wow_user_dlg_dir_list_comboA(HWND w, LPSTR path, int list, int text, UINT flags)
{ dialog_api_args a = {path,list,text,flags}; return (int)dialog_api(w,dialog_dir_combo_list,&a); }
BOOL WINAPI wow_user_dlg_dir_selectA(HWND w, LPSTR out, int count, int list)
{ dialog_api_args a = {out,count,list,0}; return (BOOL)dialog_api(w,dialog_dir_select,&a); }
BOOL WINAPI wow_user_dlg_dir_select_comboA(HWND w, LPSTR out, int count, int list)
{ dialog_api_args a = {out,count,list,0}; return (BOOL)dialog_api(w,dialog_dir_combo_select,&a); }

LRESULT CALLBACK wow_dialog_window_procA(HWND window, UINT message,
    WPARAM wp, LPARAM lp)
{
    return dialog_entry(window, message, wp, lp, TRUE, dialog_message);
}

LRESULT CALLBACK wow_dialog_window_procW(HWND window, UINT message,
    WPARAM wp, LPARAM lp)
{
    return dialog_entry(window, message, wp, lp, FALSE, dialog_message);
}

BOOL WINAPI wow_dialog_default_message(HWND window, UINT message, WPARAM wp,
    LPARAM lp, LONG *result)
{
    DWORD error = GetLastError();
    wow_window_words_binding *words;
    PWND state;
    BOOL handled;
    if (!result) return FALSE;
    words = wow_window_words_acquire(window);
    if (!words) { SetLastError(error); return FALSE; }
    state = wow_window_words_client_state_value(words);
    handled = state && (GETFNID(state) == FNID_DIALOG ||
        TestWF(state, WFDIALOGWINDOW));
    __try {
        SetLastError(error);
        if (handled) *result = wow_dialog_window_procA(window, message, wp, lp);
    } __finally {
        error = GetLastError();
        wow_window_words_release(words);
        SetLastError(error);
    }
    return handled;
}

BOOL WINAPI wow_user_end_dialog(HWND window, INT_PTR result)
{
    DWORD error = GetLastError();
    wow_window_words_binding *words = wow_window_words_acquire(window);
    PWND state = wow_window_words_client_state_value(words);
    BOOL recovered = state && (GETFNID(state) == FNID_DIALOG ||
        TestWF(state, WFDIALOGWINDOW));
    BOOL ended = FALSE;
    __try {
        SetLastError(error);
        if (recovered)
            ended = (BOOL)dialog_entry(window, 0, (WPARAM)result, 0,
                FALSE, dialog_end);
        else
            ended = wow_native_EndDialog(window, result);
    } __finally {
        error = GetLastError();
        wow_window_words_release(words);
        SetLastError(error);
    }
    return ended;
}
