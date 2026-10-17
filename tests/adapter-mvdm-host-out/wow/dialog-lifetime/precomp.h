#ifndef WOW_DIALOG_LIFETIME_FIXTURE_PRECOMP_H
#define WOW_DIALOG_LIFETIME_FIXTURE_PRECOMP_H
#include <windows.h>

/* Transport/state doubles for the actual original modal/end bodies.
 * This WND is deliberately NOT a production ABI or native USER overlay.
 * DLG/DIALOG below are the retained original declarations. */
typedef struct tagWND {
    HWND handle;
    struct tagWND *spwndParent, *spwndOwner;
    DWORD style;
    WORD fnid;
    BOOL alive;
} WND, *PWND;
typedef struct { PWND held; } TL;
#define WOW_ORIGINAL_DIALOG_LAYOUT
#include "opennt-host/windows/core/ntuser/inc/user.h"
#undef WOW_ORIGINAL_DIALOG_LAYOUT
C_ASSERT(sizeof(void *) == 4);
C_ASSERT(sizeof(DLG) == 40);
#define WFVISIBLE WS_VISIBLE
#define WFDISABLED WS_DISABLED
/* Test-local storage bit; this fixture does not model WND state-byte ABI. */
#undef WFDIALOGWINDOW
#define WFDIALOGWINDOW 1u
#undef WFDESTROYED
#define WFDESTROYED 2u
#define TestWF(window, flag) ((window)->style & (flag))
#define SetWF(window, flag) ((window)->style |= (flag))
#define ClrWF(window, flag) ((window)->style &= ~(flag))
#define TestwndChild(window) TestWF(window, WS_CHILD)
#define GetWindowCreator(window) \
    (TestwndChild(window) ? (window)->spwndParent : (window)->spwndOwner)
#define REBASEPTR(window, value) (value)
#define HWq(window) ((window)->handle)
#define PtoHq(window) HWq(window)
#define SAMEWOWHANDLE(a, b) ((a) == (b))
#define CheckLock(window) ((void)0)
#define UserAssert(condition) fixture_check((condition), __LINE__)
#define RIPMSG0(level, text) ((void)0)
#define SCDLG_CLIENT 1
#define SCDLG_ANSI 2
#define SCDLG_16BIT 8
#define SHOW_OPENWINDOW SW_SHOWNORMAL
#define WM_SYSTIMER 0x0118
/* Test-local selectors: no claim about installed win32u enum values. */
#define WindowIsForegroundThread 41
#define UserThreadStateIsForeground 42
#define SFI_ZAPACTIVEANDFOCUS 43
#define SYSMET(name) fixture_metric(#name)

#define EndDialog fixture_original_EndDialog
#define GetDesktopWindow fixture_desktop
#define GetActiveWindow fixture_active
#define GetCapture fixture_capture
#define GetFocus fixture_focus
#define IsChild fixture_is_child
#define IsWindow fixture_is_window
#undef PeekMessage
#define PeekMessage fixture_peek
#undef SendMessage
#define SendMessage fixture_send
#undef PostMessage
#define PostMessage fixture_post
#undef IsDialogMessage
#define IsDialogMessage fixture_dialog_message
#define TranslateMessage fixture_translate
#undef DispatchMessage
#define DispatchMessage fixture_dispatch
#define PostQuitMessage fixture_quit
#define UpdateWindow fixture_update
#undef LoadCursor
#define LoadCursor fixture_cursor

void fixture_check(BOOL, int);
int fixture_metric(const char *);
PWND ValidateHwnd(HWND);
PWND RevalidateHwnd(HWND);
PWND GetTopLevelWindow(PWND);
BOOL ValidateDialogPwnd(PWND);
void ThreadLock(PWND, TL *);
void ThreadUnlock(TL *);
HWND fixture_desktop(void);
HWND fixture_active(void);
HWND fixture_capture(void);
HWND fixture_focus(void);
BOOL fixture_is_child(HWND, HWND);
BOOL fixture_is_window(HWND);
BOOL fixture_peek(MSG *, HWND, UINT, UINT, UINT);
LRESULT fixture_send(HWND, UINT, WPARAM, LPARAM);
BOOL fixture_post(HWND, UINT, WPARAM, LPARAM);
BOOL fixture_dialog_message(HWND, MSG *);
BOOL fixture_translate(const MSG *);
LRESULT fixture_dispatch(const MSG *);
void fixture_quit(int);
BOOL fixture_update(HWND);
HCURSOR fixture_cursor(HINSTANCE, LPCSTR);
BOOL NtUserEnableWindow(HWND, BOOL);
HWND NtUserSetFocus(HWND);
HWND NtUserSetActiveWindow(HWND);
HCURSOR NtUserSetCursor(HCURSOR);
BOOL NtUserShowWindow(HWND, int);
BOOL NtUserSetWindowPos(HWND, HWND, int, int, int, int, UINT);
BOOL NtUserDestroyWindow(HWND);
BOOL NtUserWaitMessage(void);
BOOL NtUserSetForegroundWindow(HWND);
ULONG_PTR NtUserQueryWindow(HWND, int);
ULONG_PTR NtUserGetThreadState(int);
ULONG_PTR NtUserCallNoParam(int);
HWND InternalCreateDialog(HANDLE, LPDLGTEMPLATE, DWORD, HWND, DLGPROC, LONG, UINT);
int InternalDialogBox(HANDLE, LPDLGTEMPLATE, DWORD, HWND, DLGPROC, LONG, UINT);
int PASCAL DialogBox2(HWND, HWND, BOOL, BOOL);
BOOL EndDialog(HWND, int);
VOID SetDialogPointer(PWND, LONG);
#endif
