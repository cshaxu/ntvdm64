#ifndef MVDM_SHADOW_REGISTRY_H
#define MVDM_SHADOW_REGISTRY_H

#include <windows.h>
#include <winternl.h>

/* `NTVDM.REG` is the product overlay. Selected original reads use its values
 * first, then a bounded, read-only snapshot of their admitted HKLM key. The
 * original callers never receive a native Registry handle. */
BOOL ntvdm_shadow_registry_initialize(VOID);
VOID ntvdm_shadow_registry_shutdown(VOID);

LONG WINAPI ntvdm_shadow_reg_open_key_ex(HKEY root, LPCSTR subkey, DWORD options,
    REGSAM access, PHKEY result);
LONG WINAPI ntvdm_shadow_reg_query_value_ex(HKEY key, LPCSTR value, LPDWORD reserved,
    LPDWORD type, LPBYTE data, LPDWORD data_size);
LONG WINAPI ntvdm_shadow_reg_query_info_key(HKEY key, LPTSTR class_name,
    LPDWORD class_length, LPDWORD reserved, LPDWORD subkeys,
    LPDWORD maximum_subkey_length, LPDWORD maximum_class_length,
    LPDWORD values, LPDWORD maximum_value_name_length,
    LPDWORD maximum_value_data_length, LPDWORD security_descriptor,
    PFILETIME last_write_time);
LONG WINAPI ntvdm_shadow_reg_enum_value(HKEY key, DWORD index, LPSTR value_name,
    LPDWORD value_name_size, LPDWORD reserved, LPDWORD type, LPBYTE data,
    LPDWORD data_size);
LONG WINAPI ntvdm_shadow_reg_close_key(HKEY key);

/* The two original native callers use this shape rather than ADVAPI32.  It
 * remains a read-only facade over the same worker-local source. */
NTSTATUS NTAPI ntvdm_shadow_nt_open_key(PHANDLE key, ULONG desired_access,
    POBJECT_ATTRIBUTES attributes);
NTSTATUS NTAPI ntvdm_shadow_nt_enumerate_value_key(HANDLE key, ULONG index,
    ULONG information_class, PVOID information,
    ULONG information_length, PULONG result_length);
NTSTATUS NTAPI ntvdm_shadow_nt_query_value_key(HANDLE key, PVOID value_name,
    ULONG information_class, PVOID information, ULONG information_length,
    PULONG result_length);
NTSTATUS NTAPI ntvdm_shadow_nt_close(HANDLE key);

/* Include only in selected original static-reader translation units. The
 * original body keeps its call sequence, fallback and failure branches. */
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

#define NtOpenKey ntvdm_shadow_nt_open_key
#define NtEnumerateValueKey ntvdm_shadow_nt_enumerate_value_key
#define NtQueryValueKey ntvdm_shadow_nt_query_value_key
#define NtClose ntvdm_shadow_nt_close

#endif
