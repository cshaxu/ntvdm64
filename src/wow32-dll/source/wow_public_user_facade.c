/*
 * Narrow public USER mappings reached by the selected WOW32 provider.
 *
 * These preserve two original client-side call shapes only.  They neither
 * register WOW callbacks nor recreate a USER/CSRSS/Win32k service.
 */

#include <windows.h>
#include "wow_dialog_creation_binding.h"
#include "compat/include/wow32_provider_private.h"
#include "non-mvdm/public/internal/windows/inc/wingdip.h"

/* ADAPTER-WOW-063 supersedes ADAPTER-WIN32-051's native modal substitution.
 * Preserve the complete input flags so the original entry owns their
 * interpretation. */
int WINAPI wow_user_dialog_box_indirect_param(HINSTANCE module,
    LPCDLGTEMPLATEW dialog_template, HWND owner, DLGPROC dialog_proc,
    LPARAM init_param, UINT flags)
{
    return wow_user_modal_dialog_box(module,dialog_template,
        owner,dialog_proc,init_param,flags);
}
