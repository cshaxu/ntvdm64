#ifndef WOW_DIALOG_GETSET_FIXTURE_PRECOMP_H
#define WOW_DIALOG_GETSET_FIXTURE_PRECOMP_H
#include "wow_dialog_state_bindings.h"
#include "wow_edit_state_bindings.h"
#define WOW_ORIGINAL_DIALOG_GETSET_LAYOUT
#define WOW_EDIT_LOOKASIDE_DECLARATIONS_ONLY
#include "opennt-host/windows/core/ntuser/client/usercli.h"
#define WOW_ORIGINAL_CLIENT_HANDLES
#include "opennt-host/windows/core/ntuser/inc/user.h"
#undef WOW_ORIGINAL_CLIENT_HANDLES
#undef WOW_EDIT_LOOKASIDE_DECLARATIONS_ONLY
#undef WOW_ORIGINAL_DIALOG_GETSET_LAYOUT
#include "opennt-host/windows/core/ntuser/inc/ntsend.h"

/* Actual original layouts; only unavailable transports are test doubles.
 * CBFNID's adapter calls the production canonical registration query, not
 * sizeof(EDITWND) or the tested window's allocation capacity. */
BOOL fixture_same_process(PWND);
DWORD WINAPI GetCPD(PVOID, DWORD, DWORD);
DWORD WINAPI wow_edit_native_NtUserGetCPD(HWND, DWORD, DWORD);
DWORD fixture_control_size(WORD);
BOOL fixture_caller_ok(PWND);
PCALLPROCDATA fixture_validate_cpd(HANDLE, BYTE);
void fixture_check(BOOL, int);
#define ValidateHwnd wow_window_client_validate_hwnd
#define NtUserSetWindowLong wow_window_client_set_window_long
#define FCallerOk fixture_caller_ok
#define HMValidateHandleNoRip fixture_validate_cpd
#define UserAssert(c) fixture_check((c), __LINE__)
#define TestWindowProcess fixture_same_process
#define NtUserGetCPD wow_edit_native_NtUserGetCPD
#define CBFNID fixture_control_size
#define RIPERR0(error, level, text) SetLastError(error)
#define RIPERR1(error, level, text, a) SetLastError(error)
#define RIPERR3(error, level, text, a, b, c) SetLastError(error)
#define RIPMSG0(level, text) ((void)0)
DWORD WINAPI _GetWindowLong(PWND, int, BOOL);
DWORD WINAPI GetWindowData(PWND, int, BOOL);
LONG WINAPI _SetWindowLong(HWND, int, LONG, BOOL);
DWORD WINAPI xxxSetWindowLong(PWND, int, DWORD, BOOL);
DWORD WINAPI xxxSetWindowData(PWND, int, DWORD, BOOL);
#endif
