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
#endif
#endif
