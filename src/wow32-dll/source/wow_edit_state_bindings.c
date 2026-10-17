#include "wow_edit_state_bindings.h"
#define WOW_ORIGINAL_COMBO_STATE_LAYOUT
#define WOW_ORIGINAL_CONTROL_EXTENT_LAYOUT
#include "opennt-host/windows/core/ntuser/client/usercli.h"
#undef WOW_ORIGINAL_CONTROL_EXTENT_LAYOUT
#undef WOW_ORIGINAL_COMBO_STATE_LAYOUT
#include "wow_edit_entry_bindings.h"
#include "wow_user_borrow_scope.h"
#include "wow_button_lifetime.h"
#include "wow_user_task_lifecycle.h"
#include "opennt-abi/source/public/internal/windows/inc/wingdip.h"
#include <stddef.h>

C_ASSERT(sizeof(void *) == 4);
C_ASSERT(offsetof(EDITWND, ped) == sizeof(WND));
C_ASSERT(sizeof(EDITWND) == sizeof(WND) + sizeof(PED));
LONG WINAPI EditWndProcW(HWND, UINT, WPARAM, LONG);
LONG WINAPI ComboBoxWndProcW(HWND, UINT, WPARAM, LPARAM);
LONG WINAPI ListBoxWndProcW(HWND, UINT, WPARAM, LPARAM);
LONG WINAPI ButtonWndProcW(HWND, UINT, WPARAM, LPARAM);

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
    PWND state;
    PCBOX combo;
    COMBOBOXINFO value={sizeof(value)};
    BOOL original=FALSE,result=FALSE;
    /* Public oleacc.h scalar values. Do not import its COM/RPC declarations
     * into the retained NT4 control ABI merely to name three state bits. */
    enum { combo_unavailable=0x1,combo_pressed=0x8,combo_invisible=0x8000 };
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
        original=GETFNID(state)==FNID_COMBOBOX;
        if(!original)__leave;
        if((state->fnid&(FNID_CLEANEDUP_BIT|FNID_DELETED_BIT)) ||
                !wow_user_borrow_client_state(window,state) ||
                !(combo=((PCOMBOWND)state)->pcbox) || combo->spwnd!=state ||
                !combo->spwndEdit || !combo->spwndList){
            SetLastError(ERROR_INVALID_STATE);__leave;
        }
        if(!wow_user_borrow_client_state(combo->spwndEdit->head.h,combo->spwndEdit) ||
                !wow_user_borrow_client_state(combo->spwndList->head.h,combo->spwndList)){
            SetLastError(ERROR_INVALID_STATE);__leave;
        }
        value.rcItem=combo->editrc;value.rcButton=combo->buttonrc;
        value.hwndCombo=window;
        value.hwndItem=combo->spwndEdit->head.h;
        value.hwndList=combo->spwndList->head.h;
        if(combo->fButtonPressed)value.stateButton|=combo_pressed;
        if(!(combo->CBoxStyle&SDROPPABLE))value.stateButton|=combo_invisible;
        if(!IsWindowEnabled(window))value.stateButton|=combo_unavailable;
        *info=value;result=TRUE;SetLastError(error);
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
    DWORD error = GetLastError();
    PWND state = wow_window_client_validate_hwnd(window);
    WORD fnid;
    if (!state) return FALSE;
    /* ADAPTER-WOW-059: original xxxFreeWindow's canonical control fallback,
     * for the recovered EDIT/ComboBox/ListBox collaboration group. Do not
     * allocate private control data solely to free an uninitialized control. */
    if (!state->fnid && !((PEDITWND)state)->ped) {
        SetLastError(error);
        return TRUE;
    }
    fnid = GETFNID(state);
    if (fnid != FNID_EDIT && fnid != FNID_COMBOBOX && fnid != FNID_LISTBOX &&
            fnid != FNID_BUTTON) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    /* Failed creation/normal cleanup may leave a freed, nonzero PED slot. */
    if (state->fnid & FNID_CLEANEDUP_BIT) {
        SetLastError(error);
        return TRUE;
    }
    if (state->fnid & FNID_DELETED_BIT) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    if (!wow_window_client_prepare_graph(window)) return FALSE;
    SetLastError(error);
    /* Original winuserp.h WM_FINALDESTROY. The enclosing finalize_bound
     * already established task/borrow and early-resource permission. Enter
     * the canonical body directly, not another application/dispatch scope. */
    if (fnid == FNID_EDIT) (void)EditWndProcW(window, 0x0070, 0, 0);
    else if (fnid == FNID_COMBOBOX) (void)ComboBoxWndProcW(window, 0x0070, 0, 0);
    else if (fnid == FNID_LISTBOX) (void)ListBoxWndProcW(window, 0x0070, 0, 0);
    else (void)wow_button_invoke(window, 0x0070, 0, 0, ButtonWndProcW);
    /* Original handler returns zero for success and rejection alike. The
     * caller's borrow keeps state alive across nested combo notification. */
    if (!(state->fnid & FNID_CLEANEDUP_BIT)) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    SetLastError(error);
    return TRUE;
}
