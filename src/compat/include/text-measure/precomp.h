#ifndef WOW_TEXT_MEASURE_PRECOMP_H
#define WOW_TEXT_MEASURE_PRECOMP_H

#include "compat/include/wow_char_conversion_abi.h"

/* XP SP1 windows/core/ntuser/rtl/text.c client branch:
 * SDK-only declarations; original prefix and measurement bodies stay in C.
 * The product compiles those bodies with /Gz, preserving their x86 ABI. */
LONG WINAPI GetPrefixCount(LPCWSTR, int, LPWSTR, int);
BOOL WINAPI PSMGetTextExtent(HDC, LPCWSTR, int, PSIZE);
#define UserLocalAlloc(flags, bytes) HeapAlloc(GetProcessHeap(), (flags), (bytes))
#define UserLocalFree(value) HeapFree(GetProcessHeap(), 0, (value))
#define UserGetTextExtentPointW GetTextExtentPointW

#endif
