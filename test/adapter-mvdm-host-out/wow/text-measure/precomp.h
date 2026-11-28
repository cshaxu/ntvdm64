#ifndef WOW_TEXT_MEASURE_FIXTURE_PRECOMP_H
#define WOW_TEXT_MEASURE_FIXTURE_PRECOMP_H
#include <windows.h>
LONG WINAPI GetPrefixCount(LPCWSTR, int, LPWSTR, int);
PVOID WINAPI measure_allocate(UINT, SIZE_T);
VOID WINAPI measure_free(PVOID);
BOOL WINAPI measure_extent(HDC, LPCWSTR, int, PSIZE);
#define UserLocalAlloc measure_allocate
#define UserLocalFree measure_free
#define UserGetTextExtentPointW measure_extent
BOOL WINAPI PSMGetTextExtent(HDC, LPCWSTR, int, PSIZE);
#endif
