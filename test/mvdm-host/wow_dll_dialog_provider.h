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

#include "../../src/compat/include/wow_hook_callback.h"
void WINAPI wow_hook_capture_context(wow_hook_context *p) { (void)p; }
void WINAPI wow_hook_restore_context(const wow_hook_context *p) { (void)p; }
BOOL WINAPI wow_hook_query_type(INT i, INT *p) { (void)i; (void)p; return FALSE; }
LONG WINAPI WowOriginalStdHookProc(INT c, LONG w, LONG l, INT i)
{ (void)c; (void)w; (void)l; (void)i; return 0; }
BOOL WINAPI wow_hook_bind(const wow_hook_callbacks *p)
{
    return p && p->capture == wow_hook_capture_context &&
        p->restore == wow_hook_restore_context && p->type == wow_hook_query_type &&
        p->invoke == WowOriginalStdHookProc;
}
