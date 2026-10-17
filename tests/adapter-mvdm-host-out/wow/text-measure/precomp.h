#ifndef WOW_TEXT_MEASURE_FIXTURE_PRECOMP_H
#define WOW_TEXT_MEASURE_FIXTURE_PRECOMP_H
#include <windows.h>
LONG WINAPI GetPrefixCount(LPCWSTR, int, LPWSTR, int);
PVOID WINAPI measure_allocate(SIZE_T);
VOID WINAPI measure_free(PVOID);
BOOL WINAPI measure_extent(HDC, LPCWSTR, int, PSIZE);
#define UserRtlAllocMem measure_allocate
#define UserRtlFreeMem measure_free
#define UserGetTextExtentPointW measure_extent
BOOL WINAPI PSMGetTextExtent(HDC, LPWSTR, int, PSIZE);
#endif
