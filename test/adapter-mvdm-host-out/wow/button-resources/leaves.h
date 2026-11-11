#ifndef BUTTON_RESOURCE_TEST_LEAVES_H
#define BUTTON_RESOURCE_TEST_LEAVES_H
#include <windows.h>
HDC WINAPI button_test_GetDC(HWND);
int WINAPI button_test_ReleaseDC(HWND, HDC);
HDC WINAPI button_test_CreateCompatibleDC(HDC);
HBITMAP WINAPI button_test_CreateCompatibleBitmap(HDC, int, int);
HGDIOBJ WINAPI button_test_SelectObject(HDC, HGDIOBJ);
BOOL WINAPI button_test_DeleteDC(HDC);
BOOL WINAPI button_test_DeleteObject(HGDIOBJ);
BOOL WINAPI button_test_DrawFrameControl(HDC, LPRECT, UINT, UINT);
#ifdef WOW_BUTTON_RESOURCE_TEST_LEAVES
#define GetDC button_test_GetDC
#define ReleaseDC button_test_ReleaseDC
#define CreateCompatibleDC button_test_CreateCompatibleDC
#define CreateCompatibleBitmap button_test_CreateCompatibleBitmap
#define SelectObject button_test_SelectObject
#define DeleteDC button_test_DeleteDC
#define DeleteObject button_test_DeleteObject
#define DrawFrameControl button_test_DrawFrameControl
#endif
#endif
