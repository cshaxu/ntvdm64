#ifndef WOW_EDIT_NATIVE_BINDINGS_H
#define WOW_EDIT_NATIVE_BINDINGS_H
#include <windows.h>
#include <winternl.h>
#include "wow_native_gui_thread.h"
#define WOW_ORIGINAL_LARGE_STRING_LAYOUT
#include "opennt-host/windows/core/ntuser/inc/user.h"
#undef WOW_ORIGINAL_LARGE_STRING_LAYOUT
#include "opennt-abi/source/public/internal/windows/inc/wingdip.h"

/* ADAPTER-WOW-057. Native leaves for the original EDIT package, not a
 * replacement editor. Callers retain the existing task/native-call scopes.
 * No HWND, text pointer, timer or CallProc identity is retained here. */
BOOL WINAPI wow_edit_native_ready(void);
BOOL WINAPI wow_edit_native_NtUserDefSetText(HWND, const LARGE_STRING *);
PVOID WINAPI wow_edit_native_highest_user_address(void);
int WINAPI wow_edit_native_GdiGetCharDimensions(HDC, TEXTMETRICW *, LPINT);
BOOL WINAPI wow_edit_native_GetCharWidthInfo(HDC, PCHWIDTHINFO);
BOOL WINAPI wow_edit_native_PolyPatBlt(HDC, DWORD, PPOLYPATBLT, DWORD, DWORD);
/* Original signature; the reached EDIT callers always pass NULL callback. */
UINT WINAPI wow_edit_native_NtUserSetSystemTimer(HWND, UINT, DWORD, WNDPROC);
BOOL WINAPI wow_edit_native_NtUserKillSystemTimer(HWND, UINT);
DWORD WINAPI wow_edit_native_NtUserGetCPD(HWND, DWORD, DWORD);
VOID WINAPI wow_edit_native_NtUserSetWindowState(HWND, UINT);
VOID WINAPI wow_edit_native_NtUserClearWindowState(HWND, UINT);
VOID WINAPI wow_edit_native_NtUserAlterWindowStyle(HWND, DWORD, DWORD);
BOOL WINAPI wow_edit_native_NtUserFillWindow(HWND, HWND, HDC, HBRUSH);
HBRUSH WINAPI wow_edit_native_NtUserGetControlColor(HWND, HWND, HDC, UINT);
/* Original public/sdk/inc/ntrtl.h ABI consumed by editec.c ECLock/ECUnlock.
 * NTDLL owns the in-place byte transform; original EDIT retains the seed,
 * counted text extent, lock nesting and encoded-state transitions. */
NTSYSAPI VOID NTAPI RtlRunEncodeUnicodeString(PUCHAR, PUNICODE_STRING);
NTSYSAPI VOID NTAPI RtlRunDecodeUnicodeString(UCHAR, PUNICODE_STRING);
#endif
