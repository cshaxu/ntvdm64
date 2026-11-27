/* Explicit non-exercised boundaries for the original button callback fixture.
 * These are NOT providers and may never return. Any accidental expansion of
 * the exercised call graph fails the process rather than faking behavior.
 * Production includes neither this file nor these definitions. */
#define WOW_ORIGINAL_EDIT_CLIENT
#define WOW_ORIGINAL_BUTTON_CLIENT
#include "control-client/precomp.h"
#include <stdio.h>
#pragma warning(disable:4100)
__declspec(noreturn) static void unreached(const char *name)
{
    fprintf(stderr, "UNEXPECTED button DC dependency: %s\n", name);
    ExitProcess(99);
}
#define REJECT() unreached(__FUNCTION__)

__declspec(thread) wow_edit_context *wow_edit_current;
SHAREDINFO wow_default_message_state;
HBRUSH wow_edit_system_brushes[WOW_EDIT_COLOR_COUNT];
WCHAR szOneChar[] = L"0";

VOID WINAPI wow_edit_alter_window_style(HWND w, DWORD a, DWORD b) { REJECT(); }
DWORD WINAPI wow_edit_control_extent(WORD f) { REJECT(); }
PWND WINAPI wow_window_client_validate_window(HWND w) { REJECT(); }
LONG WINAPI wow_window_client_set_window_long(HWND w, int i, LONG v, BOOL a) { REJECT(); }
NTSTATUS WINAPI InitLookaside(PLOOKASIDE p, DWORD s, DWORD n) { REJECT(); }
PVOID WINAPI AllocLookasideEntry(PLOOKASIDE p) { REJECT(); }
PWND WINAPI _GetNextDlgGroupItem(PWND d, PWND w, BOOL p) { REJECT(); }
VOID WINAPI wow_control_set_window_state(PWND w, UINT f) { REJECT(); }
VOID WINAPI wow_control_clear_window_state(PWND w, UINT f) { REJECT(); }
BOOL WINAPI wow_native_DestroyWindow(HWND w) { REJECT(); }
HDC WINAPI wow_native_BeginPaint(HWND w, LPPAINTSTRUCT p) { REJECT(); }
BOOL WINAPI wow_native_EndPaint(HWND w, const PAINTSTRUCT *p) { REJECT(); }
BOOL WINAPI wow_control_draw_state(HDC d, HBRUSH b, DRAWSTATEPROC p,
    LPARAM l, WPARAM w, int x, int y, int cx, int cy, UINT f) { REJECT(); }
BOOL WINAPI wow_control_icon_size(HICON i, UINT f, int *x, int *y) { REJECT(); }
BOOL WINAPI PaintRect(HWND p, HWND w, HDC d, HBRUSH b, LPRECT r) { REJECT(); }
BOOL WINAPI PSMGetTextExtent(HDC d, LPWSTR t, int c, PSIZE s) { REJECT(); }
wow_button_resources *WINAPI wow_button_current_resources(void) { REJECT(); }
wow_edit_context *WINAPI wow_button_current_context(void) { REJECT(); }
BOOL WINAPI wow_button_atlas_blit(HDC d, int x, int y, int cx, int cy,
    int sx, int sy, DWORD o) { REJECT(); }
BOOL WINAPI wow_button_BitBltSysBmp(HDC d, int x, int y, UINT i) { REJECT(); }
VOID WINAPI DrawPushButton(HDC d, LPRECT r, UINT s, UINT f) { REJECT(); }
