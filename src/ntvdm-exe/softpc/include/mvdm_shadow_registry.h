#ifndef MVDM_SHADOW_REGISTRY_H
#define MVDM_SHADOW_REGISTRY_H

#include <windows.h>
#include <winternl.h>
#include "common/wow/registry.h"

/* `NTVDM.REG` is the product overlay. Selected original reads use its values
 * first, then a bounded, read-only snapshot of their admitted HKLM key. The
 * original callers never receive a native Registry handle. */
BOOL ntvdm_shadow_registry_initialize(VOID);
VOID ntvdm_shadow_registry_shutdown(VOID);

LONG WINAPI ntvdm_shadow_reg_open_key(HKEY root, LPCSTR subkey, PHKEY result);
LONG WINAPI ntvdm_shadow_reg_create_key(HKEY root, LPCSTR subkey, PHKEY result);
LONG WINAPI ntvdm_shadow_reg_delete_key(HKEY key, LPCSTR subkey);
LONG WINAPI ntvdm_shadow_reg_set_value(HKEY key, LPCSTR subkey, DWORD type,
    LPCSTR data, DWORD data_size);
LONG WINAPI ntvdm_shadow_reg_set_value_ex(HKEY key, LPCSTR value, DWORD reserved,
    DWORD type, const BYTE *data, DWORD data_size);
LONG WINAPI ntvdm_shadow_reg_query_value(HKEY key, LPCSTR subkey, LPSTR data,
    PLONG data_size);
LONG WINAPI ntvdm_shadow_reg_enum_key(HKEY key, DWORD index, LPSTR name,
    DWORD name_size);

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
#define NtOpenKey ntvdm_shadow_nt_open_key
#define NtEnumerateValueKey ntvdm_shadow_nt_enumerate_value_key
#define NtQueryValueKey ntvdm_shadow_nt_query_value_key
#define NtClose ntvdm_shadow_nt_close

#endif
