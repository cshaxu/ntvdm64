/* Full dlgmgr object is linked so DlgSetFocus is the real production body.
 * Non-exercised creation/modal/resource boundaries fail immediately. They
 * never supply a successful result and are never linked into the product. */
#define WOW_ORIGINAL_EDIT_CLIENT
#define WOW_ORIGINAL_DIALOG_CLIENT
#define WOW_ORIGINAL_DIALOG_MANAGER
#include "control-client/precomp.h"
#include <stdio.h>
#pragma warning(disable:4100)
__declspec(noreturn) static void unreached(const char *name)
{ fprintf(stderr, "UNEXPECTED keyboard dependency: %s\n", name); ExitProcess(99); }
#define REJECT() unreached(__FUNCTION__)
/* The original dlgmgr object must coexist with the native ownership
 * adapter's distinct symbol. A precomp alias regression fails this link. */
LRESULT WINAPI wow_native_DefDlgProcA(HWND w, UINT m, WPARAM wp, LPARAM lp) { REJECT(); }
LOOKASIDE DialogLookaside;
__declspec(thread) wow_dialog_context *wow_dialog_current;
NTSTATUS WINAPI InitLookaside(PLOOKASIDE p, DWORD s, DWORD n) { REJECT(); }
PVOID WINAPI AllocLookasideEntry(PLOOKASIDE p) { REJECT(); }
BOOL WINAPI wow_edit_FillWindow(HWND p, HWND w, HDC d, HBRUSH b) { REJECT(); }
const wow_task_client_info *WINAPI wow_edit_client_info(void) { REJECT(); }
DWORD WINAPI wow_edit_control_extent(WORD f) { REJECT(); }
LONG WINAPI wow_window_client_set_window_long(HWND w, int i, LONG v, BOOL a) { REJECT(); }
const struct tagAPFNWOWHANDLERSIN *WINAPI wow_user_registered_callbacks(void) { REJECT(); }
VOID WINAPI ReleaseEditDS(HANDLE h) { REJECT(); }
VOID WINAPI SetWindowState(PWND w, UINT f) { REJECT(); }
VOID WINAPI ClearWindowState(PWND w, UINT f) { REJECT(); }
HWND WINAPI InternalCreateDialog(HANDLE h, LPDLGTEMPLATE t, DWORD c, HWND w, DLGPROC p, LONG l, UINT f) { REJECT(); }
PWND WINAPI GetTopLevelWindow(PWND w) { REJECT(); }
BOOL WINAPI wow_edit_set_window_fnid(HWND w, WORD f) { REJECT(); }
BOOL WINAPI wow_dialog_messageW(HWND w, LPMSG m) { REJECT(); }
BOOL WINAPI wow_user_peek_messageW(LPMSG m, HWND w, UINT a, UINT b, UINT f) { REJECT(); }
BOOL WINAPI wow_user_wait_message(void) { REJECT(); }
BOOL WINAPI wow_native_SetWindowPos(HWND w, HWND a, int x, int y, int cx, int cy, UINT f) { REJECT(); }
BOOL WINAPI wow_native_ShowWindow(HWND w, int s) { REJECT(); }
BOOL WINAPI wow_native_EnableWindow(HWND w, BOOL e) { REJECT(); }
BOOL WINAPI wow_native_SetForegroundWindow(HWND w) { REJECT(); }
BOOL WINAPI wow_native_SetCursorPos(int x, int y) { REJECT(); }
int WINAPI wow_native_ShowCursor(BOOL s) { REJECT(); }
BOOL WINAPI wow_native_DestroyWindow(HWND w) { REJECT(); }
BOOL WINAPI wow_native_UpdateWindow(HWND w) { REJECT(); }
HCURSOR WINAPI wow_native_SetCursor(HCURSOR c) { REJECT(); }
HWND WINAPI wow_native_GetCapture(void) { REJECT(); }
HWND WINAPI wow_native_GetActiveWindow(void) { REJECT(); }
BOOL WINAPI wow_dialog_current_foreground(void) { REJECT(); }
BOOL WINAPI wow_dialog_window_foreground(HWND w) { REJECT(); }
VOID WINAPI wow_dialog_set_thread_state(DWORD f, DWORD m) { REJECT(); }
VOID WINAPI wow_dialog_set_pointer_word(HWND w, UINT p) { REJECT(); }
