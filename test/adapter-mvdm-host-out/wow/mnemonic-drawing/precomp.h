#ifndef WOW_MNEMONIC_DRAWING_TEST_H
#define WOW_MNEMONIC_DRAWING_TEST_H
#include "../../../../src/compat/include/control-drawing/precomp.h"
PVOID WINAPI mnemonic_allocate(SIZE_T);
VOID WINAPI mnemonic_free(PVOID);
BOOL WINAPI mnemonic_text(HDC, int, int, LPCWSTR, int);
BOOL WINAPI mnemonic_metrics(HDC, LPTEXTMETRICW);
BOOL WINAPI mnemonic_extent(HDC, LPCWSTR, int, LPSIZE);
COLORREF WINAPI mnemonic_background(HDC, COLORREF);
COLORREF WINAPI mnemonic_foreground(HDC);
BOOL WINAPI mnemonic_underline(HDC, int, int, UINT, CONST RECT *, LPCWSTR, UINT, CONST INT *);
#undef UserRtlAllocMem
#define UserRtlAllocMem mnemonic_allocate
#undef UserRtlFreeMem
#define UserRtlFreeMem mnemonic_free
#undef UserTextOutW
#define UserTextOutW mnemonic_text
#undef UserGetTextMetricsW
#define UserGetTextMetricsW mnemonic_metrics
#undef UserGetTextExtentPointW
#define UserGetTextExtentPointW mnemonic_extent
#undef UserSetBkColor
#define UserSetBkColor mnemonic_background
#undef UserGetTextColor
#define UserGetTextColor mnemonic_foreground
#undef UserExtTextOutW
#define UserExtTextOutW mnemonic_underline
VOID WINAPI PSMTextOut(HDC, int, int, LPWSTR, int, DWORD);
/* Instrument native leaves used by the actual XP client PSM body. */
#define UserLocalAlloc(flags, bytes) mnemonic_allocate(bytes)
#define UserLocalFree(ptr) mnemonic_free(ptr)
#define TextOutW mnemonic_text
#define GetTextMetricsW mnemonic_metrics
#define GetTextExtentPointW mnemonic_extent
#define SetBkColor mnemonic_background
#define GetTextColor mnemonic_foreground
#define ExtTextOutW mnemonic_underline
#endif
