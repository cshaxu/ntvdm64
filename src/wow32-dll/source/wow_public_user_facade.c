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

/* DIVERGENCE(ADAPTER-WIN32-039): the original private USER request changes
 * only NT4's server-managed start-glass presentation.  This selected product
 * has no CSRSS GUI presentation owner; retain the void call/order without
 * synthesizing a cursor, dialog or task result. */
void ShowStartGlass(DWORD timeout)
{
    UNREFERENCED_PARAMETER(timeout);
}

/* DIVERGENCE(ADAPTER-WIN32-056): OpenNT's GdiCleanCacheDC validates a private
 * local-DC tag, then returns TRUE without mutating the DC; its own source
 * calls out that a server-side cleanup would be preferable.  WOW calls it
 * only before the existing public GdiFlush.  Modern public HDCs do not expose
 * that private tag, so preserve the reached successful no-state direction. */
BOOL GdiCleanCacheDC(HDC device_context)
{
    UNREFERENCED_PARAMETER(device_context);
    return TRUE;
}
