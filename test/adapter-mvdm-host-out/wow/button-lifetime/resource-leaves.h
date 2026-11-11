#ifndef WOW_BUTTON_LIFETIME_RESOURCE_LEAVES_H
#define WOW_BUTTON_LIFETIME_RESOURCE_LEAVES_H
#include <windows.h>
BOOL WINAPI fixture_button_delete_dc(HDC);
BOOL WINAPI fixture_button_delete_bitmap(HGDIOBJ);
#define DeleteDC fixture_button_delete_dc
#define DeleteObject fixture_button_delete_bitmap
#endif
