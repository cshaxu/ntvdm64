#ifndef WOW_MNEMONIC_DRAWING_TEST_H
#define WOW_MNEMONIC_DRAWING_TEST_H
#include <windows.h>
#define CH_PREFIX TEXT('&')
#define ghFontSys ((HFONT)GetStockObject(SYSTEM_FONT))
#define UserGetHFONT(dc) ((HFONT)GetCurrentObject((dc), OBJ_FONT))
#define UserGetMapMode GetMapMode
PVOID WINAPI mnemonic_allocate(SIZE_T);
VOID WINAPI mnemonic_free(PVOID);
BOOL WINAPI mnemonic_text(HDC, int, int, LPCWSTR, int);
BOOL WINAPI mnemonic_metrics(HDC, LPTEXTMETRICW);
BOOL WINAPI mnemonic_extent(HDC, LPCWSTR, int, LPSIZE);
COLORREF WINAPI mnemonic_background(HDC, COLORREF);
COLORREF WINAPI mnemonic_foreground(HDC);
BOOL WINAPI mnemonic_underline(HDC, int, int, UINT, CONST RECT *, LPCWSTR, UINT, CONST INT *);
#define UserRtlAllocMem mnemonic_allocate
#define UserRtlFreeMem mnemonic_free
#define UserTextOutW mnemonic_text
#define UserGetTextMetricsW mnemonic_metrics
#define UserGetTextExtentPointW mnemonic_extent
#define UserSetBkColor mnemonic_background
#define UserGetTextColor mnemonic_foreground
#define UserExtTextOutW mnemonic_underline
VOID WINAPI PSMTextOut(HDC, int, int, LPWSTR, int);
#endif
