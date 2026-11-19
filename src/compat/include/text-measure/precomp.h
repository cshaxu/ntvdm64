#ifndef WOW_TEXT_MEASURE_PRECOMP_H
#define WOW_TEXT_MEASURE_PRECOMP_H

#include "compat/include/wow_char_conversion_abi.h"

/* OpenNT windows/core/ntuser/inc/user.h:2596 and rtl/text.c:28-32
 * at 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c:
 * SDK-only declarations; original prefix and measurement bodies stay in C.
 * The product compiles those bodies with /Gz, preserving their x86 ABI. */
LONG WINAPI GetPrefixCount(LPCWSTR, int, LPWSTR, int);
BOOL WINAPI PSMGetTextExtent(HDC, LPWSTR, int, PSIZE);
#define UserGetTextExtentPointW GetTextExtentPointW

#endif
