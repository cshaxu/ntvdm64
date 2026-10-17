#include "wow_control_extent_bindings.h"
#include <stddef.h>
#include "wow_edit_state_bindings.h"
#define WOW_ORIGINAL_DIALOG_GETSET_LAYOUT
#define WOW_ORIGINAL_CONTROL_EXTENT_LAYOUT
#define WOW_ORIGINAL_COMBO_STATE_LAYOUT
#include "opennt-host/windows/core/ntuser/client/usercli.h"
#include "opennt-host/windows/core/ntuser/inc/user.h"
#undef WOW_ORIGINAL_CONTROL_EXTENT_LAYOUT
#undef WOW_ORIGINAL_COMBO_STATE_LAYOUT
#undef WOW_ORIGINAL_DIALOG_GETSET_LAYOUT

C_ASSERT(sizeof(void *) == 4);
C_ASSERT(sizeof(CBOX) == 76);
C_ASSERT(offsetof(COMBOWND, pcbox) == sizeof(WND));
C_ASSERT(offsetof(LBWND, pLBIV) == sizeof(WND));

BOOL WINAPI wow_control_registered_extent(WORD fnid, DWORD *extent)
{
    /* ADAPTER-WOW-063: RW_RegisterControls' canonical registration values.
     * Full registration would replace native USER classes and is not a
     * composable dependency. Preserve the exact layout expressions instead;
     * original validation/getset continue to own policy over these sizes. */
    static const struct { WORD fnid; DWORD extra; } classes[] = {
        { FNID_BUTTON, sizeof(BUTNWND) - sizeof(WND) },
        { FNID_COMBOBOX, sizeof(COMBOWND) - sizeof(WND) },
        { FNID_COMBOLISTBOX, sizeof(LBWND) - sizeof(WND) },
        { FNID_DIALOG, DLGWINDOWEXTRA },
        { FNID_EDIT, CBEDITEXTRA },
        { FNID_LISTBOX, sizeof(LBWND) - sizeof(WND) },
        { FNID_MDICLIENT, sizeof(MDIWND) - sizeof(WND) },
        { FNID_STATIC, sizeof(STATWND) - sizeof(WND) }
    };
    unsigned i;
    if (!extent) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    fnid &= ~FNID_STATUS_BITS;
    for (i = 0; i < sizeof(classes) / sizeof(classes[0]); ++i) {
        if (classes[i].fnid == fnid) {
            *extent = sizeof(WND) + classes[i].extra;
            return TRUE;
        }
    }
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
}
