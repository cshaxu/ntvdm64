#ifndef WOW_USER_PRIVATE_ACCESS_H
#define WOW_USER_PRIVATE_ACCESS_H
#include <windows.h>
#include "wow_dialog_creation_binding.h"
#include "wow_dialog_api_bindings.h"
#include "wow_hook_native_binding.h"
LONG WINAPI wow_user_get_window_longA(HWND, int);
LONG WINAPI wow_user_set_window_longA(HWND, int, LONG);
DWORD WINAPI wow_user_get_class_longA(HWND, int);
DWORD WINAPI wow_user_set_class_longA(HWND, int, LONG);
/* ADAPTER-WOW-072: original IsThunkWindowProc supplies input origin. Native
 * USER owns the real WND slot and its CPD decoding; only our immutable entry
 * is decoded on return. Original CLS retains its class behavior and the
 * original address-only native normalization. Origin survives FFFF collisions. */
DWORD WINAPI wow_user_get_procedure(HWND, BOOL class_procedure, BOOL *guest);
DWORD WINAPI wow_user_set_procedure(HWND, BOOL class_procedure, DWORD,
    BOOL guest, BOOL *previous_guest);
BOOL WINAPI wow_user_get_class_infoA(HINSTANCE, LPCSTR, LPWNDCLASSA);
BOOL WINAPI wow_user_unregister_classA(LPCSTR, HINSTANCE);
DWORD WINAPI wow_native_GetWindowThreadProcessId(HWND, LPDWORD);
LRESULT WINAPI wow_user_call_window_procA(WNDPROC, HWND, UINT, WPARAM, LPARAM);
LRESULT WINAPI wow_user_call_window_procW(WNDPROC, HWND, UINT, WPARAM, LPARAM);
VOID WINAPI wow_user_register_wow_exec(HANDLE);
VOID WINAPI wow_user_notify_startup_noop(ULONG);
BOOL WINAPI wow_user_get_messageA(LPMSG, HWND, UINT, UINT);
BOOL WINAPI wow_user_peek_messageA(LPMSG, HWND, UINT, UINT, UINT);
BOOL WINAPI wow_user_get_messageW(LPMSG, HWND, UINT, UINT);
BOOL WINAPI wow_user_peek_messageW(LPMSG, HWND, UINT, UINT, UINT);
BOOL WINAPI wow_user_wait_message(void);
LRESULT WINAPI wow_native_CallNextHookEx(HHOOK, int, WPARAM, LPARAM);
BOOL WINAPI wow_native_BringWindowToTop(HWND w);
BOOL WINAPI wow_native_SetWindowPos(HWND w, HWND after, int x, int y, int cx, int cy, UINT flags);
BOOL WINAPI wow_native_MoveWindow(HWND w, int x, int y, int cx, int cy, BOOL paint);
BOOL WINAPI wow_native_ShowWindow(HWND w, int show);
BOOL WINAPI wow_native_EnableWindow(HWND w, BOOL enable);
HWND WINAPI wow_native_SetActiveWindow(HWND w);
BOOL WINAPI wow_native_SetForegroundWindow(HWND);
BOOL WINAPI wow_native_SetWindowContextHelpId(HWND, DWORD);
BOOL WINAPI wow_native_SetCursorPos(int, int);
int WINAPI wow_native_ShowCursor(BOOL);
BOOL WINAPI wow_native_DragDetect(HWND, POINT);
HWND WINAPI wow_native_SetFocus(HWND w);
HWND WINAPI wow_native_SetCapture(HWND);
BOOL WINAPI wow_native_ReleaseCapture(void);
LONG WINAPI wow_native_GetWindowLongA(HWND, int);
BOOL WINAPI wow_native_DestroyWindow(HWND w);
BOOL WINAPI wow_native_EndDialog(HWND w, INT_PTR result);
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
/* Recovered EDIT native leaves share the existing callout/return owner. */
HDC WINAPI wow_native_BeginPaint(HWND, LPPAINTSTRUCT);
BOOL WINAPI wow_native_EndPaint(HWND, const PAINTSTRUCT *);
int WINAPI wow_native_SetScrollInfo(HWND, int, LPCSCROLLINFO, BOOL);
BOOL WINAPI wow_native_CreateCaret(HWND, HBITMAP, int, int);
BOOL WINAPI wow_native_DestroyCaret(void);
BOOL WINAPI wow_native_SetCaretPos(int, int);
BOOL WINAPI wow_native_HideCaret(HWND);
BOOL WINAPI wow_native_ShowCaret(HWND);
BOOL WINAPI wow_native_InvalidateRect(HWND, const RECT *, BOOL);
BOOL WINAPI wow_native_UpdateWindow(HWND);
BOOL WINAPI wow_native_RedrawWindow(HWND, const RECT *, HRGN, UINT);
HCURSOR WINAPI wow_native_SetCursor(HCURSOR);
DWORD WINAPI wow_native_SetClassLongA(HWND w, int index, LONG value);
int WINAPI wow_native_GetWindowTextA(HWND w, LPSTR text, int count);
int WINAPI wow_native_GetWindowTextW(HWND w, LPWSTR text, int count);
/* ADAPTER-WOW-103: original WUMSG is ANSI; controls also use Unicode. */
BOOL WINAPI wow_native_CallMsgFilterA(LPMSG message, int code);
BOOL WINAPI wow_native_CallMsgFilterW(LPMSG message, int code);
int WINAPI wow_native_GetWindowTextLengthA(HWND w);
BOOL WINAPI wow_native_SetWindowTextA(HWND w, LPCSTR text);
BOOL WINAPI wow_native_SetWindowTextW(HWND w, LPCWSTR text);
BOOL WINAPI wow_native_SetDlgItemTextW(HWND w, int id, LPCWSTR text);
LRESULT WINAPI wow_native_SendMessageA(HWND w, UINT message, WPARAM wp, LPARAM lp);
LRESULT WINAPI wow_native_SendMessageW(HWND w, UINT message, WPARAM wp, LPARAM lp);
LRESULT WINAPI wow_native_SendMessageTimeoutA(HWND w, UINT message, WPARAM wp, LPARAM lp, UINT flags, UINT timeout, PDWORD_PTR value);
LRESULT WINAPI wow_native_DefWindowProcA(HWND, UINT, WPARAM, LPARAM);
LRESULT WINAPI wow_native_DefWindowProcW(HWND, UINT, WPARAM, LPARAM);
LRESULT WINAPI wow_native_DefDlgProcA(HWND, UINT, WPARAM, LPARAM);
int WINAPI wow_user_dialog_box_indirect_param(HINSTANCE, LPCDLGTEMPLATEW,
    HWND, DLGPROC, LPARAM, UINT);
BOOL WINAPI wow_user_end_dialog(HWND, INT_PTR);
LRESULT WINAPI wow_native_DefMDIChildProcA(HWND, UINT, WPARAM, LPARAM);
LRESULT WINAPI wow_native_DefFrameProcA(HWND, HWND, UINT, WPARAM, LPARAM);
LRESULT WINAPI wow_native_DispatchMessageA(const MSG *);
LRESULT WINAPI wow_native_DispatchMessageW(const MSG *);
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
int WINAPI wow_native_EnumPropsA(HWND, PROPENUMPROCA);
/* Include only after SDK declarations in the selected original provider.
 * The implementation itself must call native USER without these mappings. */
#if defined(WOW_USER_PRIVATE_REDIRECT) || defined(WOW_ORIGINAL_EDIT_CLIENT)
#include "wow_menu_native_bindings.h"
#define GetWindowLongA wow_user_get_window_longA
#define SetWindowLongA wow_user_set_window_longA
#define MapDialogRect wow_user_map_dialog_rect
#define DlgDirListA wow_user_dlg_dir_listA
#define DlgDirListComboBoxA wow_user_dlg_dir_list_comboA
#define DlgDirSelectExA wow_user_dlg_dir_selectA
#define DlgDirSelectComboBoxExA wow_user_dlg_dir_select_comboA
#define GetClassLongA wow_user_get_class_longA
#define SetClassLongA wow_user_set_class_longA
#define GetClassInfoA wow_user_get_class_infoA
#define UnregisterClassA wow_user_unregister_classA
#define GetWindowThreadProcessId wow_native_GetWindowThreadProcessId
#define CallWindowProcA wow_user_call_window_procA
#define CallWindowProcW wow_user_call_window_procW
#define RegisterWowExec wow_user_register_wow_exec
#define GetMessageA wow_user_get_messageA
#define PeekMessageA wow_user_peek_messageA
#define GetMessageW wow_user_get_messageW
#define PeekMessageW wow_user_peek_messageW
#define WaitMessage wow_user_wait_message
#define BringWindowToTop wow_native_BringWindowToTop
#define SetWindowPos wow_native_SetWindowPos
#define MoveWindow wow_native_MoveWindow
#define ShowWindow wow_native_ShowWindow
#define EnableWindow wow_native_EnableWindow
#define SetActiveWindow wow_native_SetActiveWindow
#define SetFocus wow_native_SetFocus
#define SetCapture wow_native_SetCapture
#define ReleaseCapture wow_native_ReleaseCapture
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
#define UpdateWindow wow_native_UpdateWindow
#define RedrawWindow wow_native_RedrawWindow
#define GetWindowTextA wow_native_GetWindowTextA
#define GetWindowTextW wow_native_GetWindowTextW
#define CallMsgFilterA wow_native_CallMsgFilterA
#define CallMsgFilterW wow_native_CallMsgFilterW
#define GetWindowTextLengthA wow_native_GetWindowTextLengthA
#define SetWindowTextA wow_native_SetWindowTextA
#define SetWindowTextW wow_native_SetWindowTextW
#define SetDlgItemTextW wow_native_SetDlgItemTextW
#define SendMessageA wow_native_SendMessageA
#define SendMessageW wow_native_SendMessageW
#define SendMessageTimeoutA wow_native_SendMessageTimeoutA
#define CallNextHookEx wow_native_CallNextHookEx
#define SetWindowsHookExA wow_native_SetWindowsHookExA
#define DefWindowProcA wow_native_DefWindowProcA
#define DefDlgProcA wow_native_DefDlgProcA
#define DialogBoxIndirectParamAorW wow_user_dialog_box_indirect_param
#define EndDialog wow_user_end_dialog
#define IsDialogMessageA wow_dialog_messageA
#define IsDialogMessageW wow_dialog_messageW
#define DefMDIChildProcA wow_native_DefMDIChildProcA
#define DefFrameProcA wow_native_DefFrameProcA
#define DispatchMessageA wow_native_DispatchMessageA
#define DispatchMessageW wow_native_DispatchMessageW
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
#define EnumPropsA wow_native_EnumPropsA
#endif
#endif
