#ifndef WOW_USER_PRIVATE_ACCESS_H
#define WOW_USER_PRIVATE_ACCESS_H
#include <windows.h>
LONG WINAPI wow_user_get_window_longA(HWND, int);
LONG WINAPI wow_user_set_window_longA(HWND, int, LONG);
DWORD WINAPI wow_user_get_class_longA(HWND, int);
DWORD WINAPI wow_user_set_class_longA(HWND, int, LONG);
/* Include only after SDK declarations in the selected original provider.
 * The implementation itself must call native USER without these mappings. */
#ifdef WOW_USER_PRIVATE_REDIRECT
#define GetWindowLongA wow_user_get_window_longA
#define SetWindowLongA wow_user_set_window_longA
#define GetClassLongA wow_user_get_class_longA
#define SetClassLongA wow_user_set_class_longA
#endif
#endif
