#ifndef WOW_CONTROL_FNID_PROVIDER_H
#define WOW_CONTROL_FNID_PROVIDER_H
#include <windows.h>
typedef BOOL (WINAPI *wow_control_fnid_provider)(HWND, WORD);
BOOL WINAPI wow_control_fnid_bind(wow_control_fnid_provider);
BOOL WINAPI wow_control_set_fnid(HWND, WORD);
#endif
