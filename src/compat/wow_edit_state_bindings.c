#include <windows.h>
#include <winternl.h>
#include "wow_edit_entry_bindings.h"
#include "wow_user_borrow_scope.h"
#include "wow_user_task_lifecycle.h"

/* ADAPTER-WOW-074. Original comboini.c owns these CBOX fields and children;
 * modern GetComboBoxInfo queries native USER's unrelated private control.
 * Copy a call-local public view only. No control state, geometry algorithm,
 * hidden HWND, native-private cast or lifetime registry is introduced. */
BOOL WINAPI wow_control_get_combo_info(HWND window, PCOMBOBOXINFO info)
{
    DWORD error=GetLastError();
    wow_window_words_binding *words;
    wow_task_callback_scope task;
    wow_user_borrow_scope borrow;
    struct tagWND *state;
    BOOL original=FALSE,result=FALSE;
    if(!info || info->cbSize!=sizeof(*info)){
        SetLastError(ERROR_INVALID_PARAMETER);return FALSE;
    }
    words=wow_window_words_acquire(window);
    state=wow_window_words_client_state_value(words);
    if(!words || !wow_window_words_owns_client_state(words,state)){
        wow_window_words_release(words);
        SetLastError(error);return GetComboBoxInfo(window,info);
    }
    if(wow_window_words_thread_id(words)!=GetCurrentThreadId()){
        wow_window_words_release(words);
        SetLastError(ERROR_INVALID_THREAD_ID);return FALSE;
    }
    if(!wow_task_callback_enter(&task)){
        error=GetLastError();wow_window_words_release(words);SetLastError(error);return FALSE;
    }
    if(!wow_user_borrow_enter(&borrow)){
        error=GetLastError();wow_window_words_release(words);
        (void)wow_task_callback_leave(&task);SetLastError(error);return FALSE;
    }
    __try {
        result=wow_control_original_provider()->combo_info(window,state,info,&original);
        if(result)SetLastError(error);
    } __finally {
        DWORD final_error=GetLastError();
        wow_window_words_release(words);
        wow_user_borrow_leave_required(&borrow);
        (void)wow_task_callback_leave(&task);
        SetLastError(final_error);
    }
    if(!original){SetLastError(error);return GetComboBoxInfo(window,info);}
    return result;
}

BOOL WINAPI wow_control_finalize_window(HWND window)
{
    return wow_control_original_provider()->finalize(window);
}
