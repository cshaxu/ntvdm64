#include "wow_dialog_state_bindings.h"
#include <stddef.h>

/* Original miscutil.c, built with the provider's original x86 /Gz ABI. */
VOID WINAPI SetDialogPointer(PWND, LONG);
C_ASSERT(sizeof(void *) == 4);
C_ASSERT(offsetof(DIALOG, resultWP) == sizeof(WND));
C_ASSERT(offsetof(DIALOG, pdlg) == sizeof(WND) + sizeof(LONG));
C_ASSERT(offsetof(DIALOG, unused) == sizeof(WND) + 2 * sizeof(LONG));

BOOL WINAPI wow_dialog_set_pointer(HWND window, PDLG dialog)
{
    DWORD error = GetLastError();
    PWND state = wow_window_client_validate_hwnd(window);
    if (!state) return FALSE;
    /* The source creator/default handler owns ValidateDialogPwnd policy.
     * This boundary only prevents writing outside the retained class extent.
     * Do not send the old PDLG pointer to modern NtUserSetDialogPointer. */
    if (state->cbwndExtra < DLGWINDOWEXTRA) {
        SetLastError(ERROR_INVALID_INDEX);
        return FALSE;
    }
    SetDialogPointer(state, (LONG)dialog);
    SetLastError(error);
    return TRUE;
}
