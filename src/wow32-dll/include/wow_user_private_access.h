#ifndef WOW_USER_PRIVATE_ACCESS_H
#define WOW_USER_PRIVATE_ACCESS_H
#include <windows.h>
LONG WINAPI wow_user_get_window_longA(HWND, int);
LONG WINAPI wow_user_set_window_longA(HWND, int, LONG);
DWORD WINAPI wow_user_get_class_longA(HWND, int);
DWORD WINAPI wow_user_set_class_longA(HWND, int, LONG);
BOOL WINAPI wow_user_get_class_infoA(HINSTANCE, LPCSTR, LPWNDCLASSA);
LRESULT WINAPI wow_user_call_window_procA(WNDPROC, HWND, UINT, WPARAM, LPARAM);
LRESULT WINAPI wow_user_call_window_procW(WNDPROC, HWND, UINT, WPARAM, LPARAM);
VOID WINAPI wow_user_register_wow_exec(HANDLE);
VOID WINAPI wow_user_notify_startup_noop(ULONG);
BOOL WINAPI wow_user_get_messageA(LPMSG, HWND, UINT, UINT);
BOOL WINAPI wow_user_peek_messageA(LPMSG, HWND, UINT, UINT, UINT);
BOOL WINAPI wow_user_wait_message(void);
BOOL WINAPI wow_native_BringWindowToTop(HWND w);
BOOL WINAPI wow_native_SetWindowPos(HWND w, HWND after, int x, int y, int cx, int cy, UINT flags);
BOOL WINAPI wow_native_MoveWindow(HWND w, int x, int y, int cx, int cy, BOOL paint);
BOOL WINAPI wow_native_ShowWindow(HWND w, int show);
BOOL WINAPI wow_native_EnableWindow(HWND w, BOOL enable);
HWND WINAPI wow_native_SetActiveWindow(HWND w);
HWND WINAPI wow_native_SetFocus(HWND w);
BOOL WINAPI wow_native_DestroyWindow(HWND w);
HWND WINAPI wow_native_SetParent(HWND w, HWND parent);
BOOL WINAPI wow_native_SetWindowPlacement(HWND w, const WINDOWPLACEMENT *placement);
LONG WINAPI wow_native_SetWindowLongA(HWND w, int index, LONG value);
BOOL WINAPI wow_native_EndDeferWindowPos(HDWP positions);
UINT WINAPI wow_native_ArrangeIconicWindows(HWND w);
BOOL WINAPI wow_native_CloseWindow(HWND w);
BOOL WINAPI wow_native_OpenIcon(HWND w);
BOOL WINAPI wow_native_ShowOwnedPopups(HWND w, BOOL show);
BOOL WINAPI wow_native_ScrollWindow(HWND w, int dx, int dy, const RECT *area, const RECT *clip);
int WINAPI wow_native_ScrollWindowEx(HWND w, int dx, int dy, const RECT *area, const RECT *clip, HRGN update, LPRECT bounds, UINT flags);
BOOL WINAPI wow_native_ShowScrollBar(HWND w, int bar, BOOL show);
BOOL WINAPI wow_native_SetScrollRange(HWND w, int bar, int minimum, int maximum, BOOL redraw);
int WINAPI wow_native_SetScrollPos(HWND w, int bar, int position, BOOL redraw);
BOOL WINAPI wow_native_EnableScrollBar(HWND w, UINT bar, UINT arrows);
DWORD WINAPI wow_native_SetClassLongA(HWND w, int index, LONG value);
int WINAPI wow_native_GetWindowTextA(HWND w, LPSTR text, int count);
int WINAPI wow_native_GetWindowTextLengthA(HWND w);
BOOL WINAPI wow_native_SetWindowTextA(HWND w, LPCSTR text);
LRESULT WINAPI wow_native_SendMessageA(HWND w, UINT message, WPARAM wp, LPARAM lp);
LRESULT WINAPI wow_native_SendMessageTimeoutA(HWND w, UINT message, WPARAM wp, LPARAM lp, UINT flags, UINT timeout, PDWORD_PTR value);
LRESULT WINAPI wow_native_DefWindowProcA(HWND, UINT, WPARAM, LPARAM);
LRESULT WINAPI wow_native_DefDlgProcA(HWND, UINT, WPARAM, LPARAM);
LRESULT WINAPI wow_native_DefMDIChildProcA(HWND, UINT, WPARAM, LPARAM);
LRESULT WINAPI wow_native_DefFrameProcA(HWND, HWND, UINT, WPARAM, LPARAM);
LRESULT WINAPI wow_native_DispatchMessageA(const MSG *);
HWND WINAPI wow_native_FindWindowA(LPCSTR, LPCSTR);
HWND WINAPI wow_native_ChildWindowFromPoint(HWND, POINT);
HWND WINAPI wow_native_WindowFromPoint(POINT);
HWND WINAPI wow_native_GetCapture(void);
HWND WINAPI wow_native_GetFocus(void);
HWND WINAPI wow_native_GetActiveWindow(void);
HWND WINAPI wow_native_GetForegroundWindow(void);
HWND WINAPI wow_native_GetLastActivePopup(HWND);
HWND WINAPI wow_native_GetNextDlgGroupItem(HWND, HWND, BOOL);
HWND WINAPI wow_native_GetNextDlgTabItem(HWND, HWND, BOOL);
BOOL WINAPI wow_native_EnumWindows(WNDENUMPROC, LPARAM);
BOOL WINAPI wow_native_EnumChildWindows(HWND, WNDENUMPROC, LPARAM);
BOOL WINAPI wow_native_EnumThreadWindows(DWORD, WNDENUMPROC, LPARAM);
/* Include only after SDK declarations in the selected original provider.
 * The implementation itself must call native USER without these mappings. */
#ifdef WOW_USER_PRIVATE_REDIRECT
#include "wow_menu_native_bindings.h"
#define GetWindowLongA wow_user_get_window_longA
#define SetWindowLongA wow_user_set_window_longA
#define GetClassLongA wow_user_get_class_longA
#define SetClassLongA wow_user_set_class_longA
#define GetClassInfoA wow_user_get_class_infoA
#define CallWindowProcA wow_user_call_window_procA
#define CallWindowProcW wow_user_call_window_procW
#define RegisterWowExec wow_user_register_wow_exec
#define GetMessageA wow_user_get_messageA
#define PeekMessageA wow_user_peek_messageA
#define WaitMessage wow_user_wait_message
#define BringWindowToTop wow_native_BringWindowToTop
#define SetWindowPos wow_native_SetWindowPos
#define MoveWindow wow_native_MoveWindow
#define ShowWindow wow_native_ShowWindow
#define EnableWindow wow_native_EnableWindow
#define SetActiveWindow wow_native_SetActiveWindow
#define SetFocus wow_native_SetFocus
#define DestroyWindow wow_native_DestroyWindow
#define SetParent wow_native_SetParent
#define SetWindowPlacement wow_native_SetWindowPlacement
#define EndDeferWindowPos wow_native_EndDeferWindowPos
#define ArrangeIconicWindows wow_native_ArrangeIconicWindows
#define CloseWindow wow_native_CloseWindow
#define OpenIcon wow_native_OpenIcon
#define ShowOwnedPopups wow_native_ShowOwnedPopups
#define ScrollWindow wow_native_ScrollWindow
#define ScrollWindowEx wow_native_ScrollWindowEx
#define ShowScrollBar wow_native_ShowScrollBar
#define SetScrollRange wow_native_SetScrollRange
#define SetScrollPos wow_native_SetScrollPos
#define EnableScrollBar wow_native_EnableScrollBar
#define GetWindowTextA wow_native_GetWindowTextA
#define GetWindowTextLengthA wow_native_GetWindowTextLengthA
#define SetWindowTextA wow_native_SetWindowTextA
#define SendMessageA wow_native_SendMessageA
#define SendMessageTimeoutA wow_native_SendMessageTimeoutA
#define DefWindowProcA wow_native_DefWindowProcA
#define DefDlgProcA wow_native_DefDlgProcA
#define DefMDIChildProcA wow_native_DefMDIChildProcA
#define DefFrameProcA wow_native_DefFrameProcA
#define DispatchMessageA wow_native_DispatchMessageA
#define FindWindowA wow_native_FindWindowA
#define ChildWindowFromPoint wow_native_ChildWindowFromPoint
#define WindowFromPoint wow_native_WindowFromPoint
#define GetCapture wow_native_GetCapture
#define GetFocus wow_native_GetFocus
#define GetActiveWindow wow_native_GetActiveWindow
#define GetForegroundWindow wow_native_GetForegroundWindow
#define GetLastActivePopup wow_native_GetLastActivePopup
#define GetNextDlgGroupItem wow_native_GetNextDlgGroupItem
#define GetNextDlgTabItem wow_native_GetNextDlgTabItem
#define EnumWindows wow_native_EnumWindows
#define EnumChildWindows wow_native_EnumChildWindows
#define EnumThreadWindows wow_native_EnumThreadWindows
#endif
#endif
