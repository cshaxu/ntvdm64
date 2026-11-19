#ifndef WOW_NATIVE_CONTROLS_H
#define WOW_NATIVE_CONTROLS_H
#include <windows.h>
#include <stddef.h>

/* Native GDI accepts these records opaquely; their original field layouts
 * remain declared and checked by the calling component. */
struct _CHWIDTHINFO;
struct _POLYPATBLT;

/* OPENNT-HOST-118: inspected x86 USER counted-input layout, not NT4
 * LARGE_STRING. Borrowed text remains live only through the native call. */
typedef struct wow_native_text_descriptor {
    ULONG length;
    ULONG maximum_length_and_ansi;
    ULONG buffer;
    ULONG buffer_upper;
} wow_native_text_descriptor;
C_ASSERT(sizeof(void *) == 4);
C_ASSERT(sizeof(wow_native_text_descriptor) == 16);
C_ASSERT(offsetof(wow_native_text_descriptor, buffer) == 8);
C_ASSERT(offsetof(wow_native_text_descriptor, buffer_upper) == 12);

/* ADAPTER-WOW-057. Native leaves for the original EDIT package, not a
 * replacement editor. Callers retain the existing task/native-call scopes.
 * No HWND, text pointer, timer or CallProc identity is retained here. */
BOOL WINAPI wow_edit_native_ready(void);
int WINAPI wow_edit_native_GdiGetCharDimensions(HDC, TEXTMETRICW *, LPINT);
BOOL WINAPI wow_edit_native_GetCharWidthInfo(HDC, struct _CHWIDTHINFO *);
BOOL WINAPI wow_edit_native_PolyPatBlt(HDC, DWORD, struct _POLYPATBLT *, DWORD, DWORD);
/* Original signature; the reached EDIT callers always pass NULL callback. */
UINT WINAPI wow_edit_native_NtUserSetSystemTimer(HWND, UINT, DWORD, WNDPROC);
BOOL WINAPI wow_edit_native_NtUserKillSystemTimer(HWND, UINT);
DWORD WINAPI wow_edit_native_NtUserGetCPD(HWND, DWORD, DWORD);
VOID WINAPI wow_edit_native_NtUserSetWindowState(HWND, UINT);
VOID WINAPI wow_edit_native_NtUserClearWindowState(HWND, UINT);
VOID WINAPI wow_edit_native_NtUserAlterWindowStyle(HWND, DWORD, DWORD);
BOOL WINAPI wow_edit_native_NtUserFillWindow(HWND, HWND, HDC, HBRUSH);
HBRUSH WINAPI wow_edit_native_NtUserGetControlColor(HWND, HWND, HDC, UINT);
BOOL WINAPI wow_native_set_window_text(HWND, const wow_native_text_descriptor *);
#endif
