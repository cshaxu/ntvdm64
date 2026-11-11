#include <windows.h>
/* Test-only selection of an actual Windows DBCS converter; production always
 * uses CP_OEMCP. No hand-written substitute encoding algorithm. */
int WINAPI fixture_oem_conversion(UINT, DWORD, LPCWCH, int, LPSTR, int,
    LPCCH, LPBOOL);
#define WideCharToMultiByte fixture_oem_conversion
