#ifndef WOW_EDIT_VIEW_FIXTURE_PRECOMP_H
#define WOW_EDIT_VIEW_FIXTURE_PRECOMP_H
#include "wow_user_client_view_layout.h"
#ifdef WOW
#error Original host client view must use native RECT, not guest WORD rectangles
#endif
/* Test-owned original WNDs have ordinary scoped parent pointers. */
#define REBASEPWND(p, member) ((p)->member)
#define SYSMET(index) view_fixture_##index
extern int view_fixture_CXMINIMIZED, view_fixture_CYMINIMIZED;
PWND WINAPI view_fixture_validate(HWND);
#define ValidateHwnd view_fixture_validate
BOOL WINAPI _IsWindowVisible(PWND);
BOOL WINAPI _GetClientRect(PWND, LPRECT);
BOOL WINAPI _FChildVisible(PWND);
BOOL WINAPI FChildVisible(HWND);
BOOL WINAPI _IsChild(PWND, PWND);
BOOL WINAPI _ClientToScreen(PWND, PPOINT);
BOOL WINAPI _ScreenToClient(PWND, PPOINT);
BOOL WINAPI IsVisible(PWND);
PWND WINAPI GetTopLevelWindow(PWND);
#endif
