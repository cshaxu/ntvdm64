#include "wow_edit_state_bindings.h"
#include "wow_edit_entry_bindings.h"
#include "wow_user_borrow_scope.h"
#include "wow_button_lifetime.h"
#include "opennt-abi/source/public/internal/windows/inc/wingdip.h"
#include <stddef.h>

C_ASSERT(sizeof(void *) == 4);
C_ASSERT(offsetof(EDITWND, ped) == sizeof(WND));
C_ASSERT(sizeof(EDITWND) == sizeof(WND) + sizeof(PED));
LONG WINAPI EditWndProcW(HWND, UINT, WPARAM, LONG);
LONG WINAPI ComboBoxWndProcW(HWND, UINT, WPARAM, LPARAM);
LONG WINAPI ListBoxWndProcW(HWND, UINT, WPARAM, LPARAM);
LONG WINAPI ButtonWndProcW(HWND, UINT, WPARAM, LPARAM);

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
