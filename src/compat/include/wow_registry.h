#ifndef COMMON_WOW_REGISTRY_H
#define COMMON_WOW_REGISTRY_H
#include <windows.h>
/* Selected original WOW reads use the worker's existing read-only projection. */
LONG WINAPI ntvdm_shadow_reg_open_key_ex(HKEY, LPCSTR, DWORD, REGSAM, PHKEY);
LONG WINAPI ntvdm_shadow_reg_query_value_ex(HKEY, LPCSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
LONG WINAPI ntvdm_shadow_reg_query_info_key(HKEY, LPTSTR, LPDWORD, LPDWORD,
    LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, LPDWORD, PFILETIME);
LONG WINAPI ntvdm_shadow_reg_enum_value(HKEY, DWORD, LPSTR, LPDWORD, LPDWORD,
    LPDWORD, LPBYTE, LPDWORD);
LONG WINAPI ntvdm_shadow_reg_close_key(HKEY);
#undef RegOpenKeyEx
#undef RegQueryValueEx
#undef RegQueryInfoKey
#undef RegEnumValue
#undef RegCloseKey
#define RegOpenKeyEx ntvdm_shadow_reg_open_key_ex
#define RegQueryValueEx ntvdm_shadow_reg_query_value_ex
#define RegQueryInfoKey ntvdm_shadow_reg_query_info_key
#define RegEnumValue ntvdm_shadow_reg_enum_value
#define RegCloseKey ntvdm_shadow_reg_close_key
#endif
