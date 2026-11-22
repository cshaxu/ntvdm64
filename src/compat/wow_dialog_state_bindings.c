#include <windows.h>
#include <winternl.h>
#include "wow_dialog_state_bindings.h"
#include "wow_dialog_finalization_provider.h"

BOOL WINAPI wow_dialog_set_pointer(HWND window, struct _DLG *dialog)
{
    DWORD error = GetLastError();
    struct tagWND *state = wow_window_client_validate_hwnd(window);
    if (!state) return FALSE;
    /* The source creator/default handler owns ValidateDialogPwnd policy.
     * This boundary only prevents writing outside the retained class extent.
     * Do not send the old PDLG pointer to modern NtUserSetDialogPointer. */
    if (!wow_dialog_finalization_current()->set_pointer(state, dialog))
        return FALSE;
    SetLastError(error);
    return TRUE;
}
