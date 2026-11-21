/* Controlled same-image dialog providers for the real DLL entry bridge. */
#include "../../src/compat/include/wow_common_dialog_callback.h"
static unsigned dialog_bindings;
UINT WINAPI WowOriginalDialogProc(HWND w, UINT m, WPARAM p, LPARAM l)
{ (void)w; (void)m; (void)p; (void)l; return 1; }
UINT WINAPI WowOriginalPrintSetupDialogProc(HWND w, UINT m, WPARAM p, LPARAM l)
{ (void)w; (void)m; (void)p; (void)l; return 2; }
UINT WINAPI WowOriginalFindReplaceDialogProc(HWND w, UINT m, WPARAM p, LPARAM l)
{ (void)w; (void)m; (void)p; (void)l; return 3; }
BOOL WINAPI wow_common_dialog_bind(const wow_common_dialog_callbacks *p)
{
    if (!p || p->dialog != WowOriginalDialogProc ||
        p->print_setup != WowOriginalPrintSetupDialogProc ||
        p->find_replace != WowOriginalFindReplaceDialogProc) return FALSE;
    ++dialog_bindings;
    return TRUE;
}
