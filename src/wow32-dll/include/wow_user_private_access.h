#ifndef WOW_USER_PRIVATE_ACCESS_H
#define WOW_USER_PRIVATE_ACCESS_H
#include <windows.h>
LONG WINAPI wow_user_get_window_longA(HWND, int);
LONG WINAPI wow_user_set_window_longA(HWND, int, LONG);
DWORD WINAPI wow_user_get_class_longA(HWND, int);
DWORD WINAPI wow_user_set_class_longA(HWND, int, LONG);
LRESULT WINAPI wow_user_call_window_procA(WNDPROC, HWND, UINT, WPARAM, LPARAM);
LRESULT WINAPI wow_user_call_window_procW(WNDPROC, HWND, UINT, WPARAM, LPARAM);
VOID WINAPI wow_user_register_wow_exec(HANDLE);
BOOL WINAPI wow_user_get_messageA(LPMSG, HWND, UINT, UINT);
BOOL WINAPI wow_user_peek_messageA(LPMSG, HWND, UINT, UINT, UINT);
BOOL WINAPI wow_user_wait_message(void);
BOOL WINAPI wow_native_BringWindowToTop(HWND);
BOOL WINAPI wow_native_SetWindowPos(HWND, HWND, int, int, int, int, UINT);
BOOL WINAPI wow_native_MoveWindow(HWND, int, int, int, int, BOOL);
BOOL WINAPI wow_native_ShowWindow(HWND, int);
BOOL WINAPI wow_native_EnableWindow(HWND, BOOL);
HWND WINAPI wow_native_SetActiveWindow(HWND);
HWND WINAPI wow_native_SetFocus(HWND);
BOOL WINAPI wow_native_DestroyWindow(HWND);
HWND WINAPI wow_native_SetParent(HWND, HWND);
BOOL WINAPI wow_native_SetWindowPlacement(HWND, const WINDOWPLACEMENT *);
int WINAPI wow_native_GetWindowTextA(HWND, LPSTR, int);
int WINAPI wow_native_GetWindowTextLengthA(HWND);
BOOL WINAPI wow_native_SetWindowTextA(HWND, LPCSTR);
LRESULT WINAPI wow_native_SendMessageA(HWND, UINT, WPARAM, LPARAM);
LRESULT WINAPI wow_native_SendMessageTimeoutA(HWND, UINT, WPARAM, LPARAM, UINT, UINT, PDWORD_PTR);
BOOL WINAPI wow_native_ReplyMessage(LRESULT);
/* Include only after SDK declarations in the selected original provider.
 * The implementation itself must call native USER without these mappings. */
#ifdef WOW_USER_PRIVATE_REDIRECT
#define GetWindowLongA wow_user_get_window_longA
#define SetWindowLongA wow_user_set_window_longA
#define GetClassLongA wow_user_get_class_longA
#define SetClassLongA wow_user_set_class_longA
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
#define GetWindowTextA wow_native_GetWindowTextA
#define GetWindowTextLengthA wow_native_GetWindowTextLengthA
#define SetWindowTextA wow_native_SetWindowTextA
#define SendMessageA wow_native_SendMessageA
#define SendMessageTimeoutA wow_native_SendMessageTimeoutA
#define ReplyMessage wow_native_ReplyMessage
#endif
#endif
