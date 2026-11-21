#ifndef WOW_TIMER_CALLBACK_H
#define WOW_TIMER_CALLBACK_H
#include <windows.h>
/* Original timer slot owner, bound before DLL attach for image lifetime. */
typedef VOID (WINAPI *wow_timer_callback)(UINT, HWND, UINT, DWORD);
BOOL WINAPI wow_timer_bind(wow_timer_callback);
#endif
