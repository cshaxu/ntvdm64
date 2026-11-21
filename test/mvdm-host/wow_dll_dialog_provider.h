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

#include "../../src/compat/include/wow_idle_hook_binding.h"
BOOL WINAPI wow_original_idle_registered(DWORD id) { (void)id; return FALSE; }
BOOL WINAPI wow_original_separate_launch(void) { return FALSE; }
LRESULT CALLBACK WowOriginalForegroundIdleHook(int c, WPARAM w, LPARAM l)
{ (void)c; (void)w; (void)l; return 0; }
BOOL WINAPI wow_task_state_bind(const wow_task_state_callbacks *p)
{
    return p && p->idle_registered == wow_original_idle_registered &&
        p->separate_launch == wow_original_separate_launch &&
        p->idle == WowOriginalForegroundIdleHook;
}

#include "../../src/compat/include/wow_standard_class_provider.h"
INT WINAPI GetStdClassNumber(LPSTR name) { (void)name; return 0; }
LPCSTR WINAPI wow_original_class_name(UINT index) { (void)index; return NULL; }
ATOM WINAPI wow_original_class_atom(UINT index) { (void)index; return 0; }
BOOL WINAPI wow_standard_class_bind(const wow_standard_class_provider *p)
{
    return p && p->classify == GetStdClassNumber &&
        p->name == wow_original_class_name && p->atom == wow_original_class_atom;
}

#include "../../src/compat/include/wow_task_cleanup_entry.h"
BOOL WINAPI wow_original_task_cleanup_uninitialized(DWORD id) { (void)id; return FALSE; }
VOID WINAPI wow_original_task_retire_async(void) { }
VOID WINAPI WowOriginalFreeTask(PVOID task) { (void)task; }
BOOL WINAPI wow_task_cleanup_bind(const wow_task_cleanup_provider *p)
{
    return p && p->uninitialized == wow_original_task_cleanup_uninitialized &&
        p->retire_async == wow_original_task_retire_async && p->free_task == WowOriginalFreeTask;
}

#include "../../src/compat/include/wow_timer_callback.h"
VOID WINAPI WowOriginalTimerFunc(UINT i, HWND w, UINT e, DWORD t)
{ (void)i; (void)w; (void)e; (void)t; }
BOOL WINAPI wow_timer_bind(wow_timer_callback p) { return p == WowOriginalTimerFunc; }
