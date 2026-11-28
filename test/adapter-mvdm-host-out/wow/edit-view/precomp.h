#ifndef WOW_EDIT_VIEW_FIXTURE_PRECOMP_H
#define WOW_EDIT_VIEW_FIXTURE_PRECOMP_H
#define WOW_ORIGINAL_EDIT_CLIENT
#include "control-client/precomp.h"
#ifdef WOW
#error Original host client view must use native RECT, not guest WORD rectangles
#endif
/* This native-view fixture has no guest dispatch owner. */
#undef SendMessageW
#undef SendMessage
#define SendMessage SendMessageW
/* Test-owned original WNDs have ordinary scoped parent pointers. */
#define REBASEPWND(p, member) ((p)->member)
/* Use the production declarations for restored, unreferenced helpers.
 * Only minimized dimensions and HWND lookup are owned by this fixture. */
#undef SYSMET
#define SYSMET(index) (SM_##index == SM_CXMINIMIZED ? view_fixture_CXMINIMIZED : \
    SM_##index == SM_CYMINIMIZED ? view_fixture_CYMINIMIZED : GetSystemMetrics(SM_##index))
extern int view_fixture_CXMINIMIZED, view_fixture_CYMINIMIZED;
PWND WINAPI view_fixture_validate(HWND);
#undef ValidateHwnd
#define ValidateHwnd view_fixture_validate
BOOL WINAPI _IsWindowVisible(PWND);
VOID WINAPI _GetClientRect(PWND, LPRECT);
VOID WINAPI _GetWindowRect(PWND, LPRECT);
BOOL WINAPI _FChildVisible(PWND);
BOOL WINAPI FChildVisible(HWND);
BOOL WINAPI _IsChild(PWND, PWND);
VOID WINAPI _ClientToScreen(PWND, PPOINT);
VOID WINAPI _ScreenToClient(PWND, PPOINT);
BOOL WINAPI IsVisible(PWND);
PWND WINAPI GetTopLevelWindow(PWND);
#endif
