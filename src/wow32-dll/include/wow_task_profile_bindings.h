#ifndef WOW_TASK_PROFILE_BINDINGS_H
#define WOW_TASK_PROFILE_BINDINGS_H
#include "wow_task_order_bindings.h"
#include <stddef.h>
#include <wchar.h>
/* The older declaration carrier supplies this enum member as a macro.
 * Use the original complete value-query enum within this composition. */
#undef KeyValueFullInformation
#include "opennt-host/public/sdk/inc/ntregapi.h"
UINT WINAPI wow_task_profile(wow_task_order_process *, UINT, LPCWSTR, LPCWSTR, LPWSTR, DWORD);
HANDLE WINAPI wow_profile_open_key(UINT, ACCESS_MASK);
DWORD WINAPI FastGetProfileStringW(UINT, LPCWSTR, LPCWSTR, LPWSTR, DWORD);
NTSYSAPI NTSTATUS NTAPI NtQueryValueKey(HANDLE, PUNICODE_STRING,
    KEY_VALUE_INFORMATION_CLASS, PVOID, ULONG, PULONG);
#ifdef WOW_ORIGINAL_PROFILE
#define OpenCacheKeyEx wow_profile_open_key
#define UserAllocPoolWithQuota(bytes, tag) HeapAlloc(GetProcessHeap(),0,bytes)
#define UserFreePool(pointer) HeapFree(GetProcessHeap(),0,pointer)
#define ZwQueryValueKey NtQueryValueKey
#define ZwClose(key) RegCloseKey((HKEY)(key))
#define UserAssert(expression) ((void)0)
#define RIPMSG0(level,text) ((void)0)
#define RIPMSG1(level,text,arg) ((void)0)
#undef TEXT
#define TEXT(value) L##value
#ifndef STATUS_BUFFER_OVERFLOW
#define STATUS_BUFFER_OVERFLOW ((NTSTATUS)0x80000005)
#endif
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0)
#endif
#endif
#endif
