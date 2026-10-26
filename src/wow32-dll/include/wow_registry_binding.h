#ifndef WOW_REGISTRY_BINDING_H
#define WOW_REGISTRY_BINDING_H

/*
 * Private provider binding for the selected original WOW Registry thunks.
 * It deliberately exposes Win32-shaped calls so wshell.c and wkernel.c keep
 * their original conversion, error and recursive-delete bodies.  Values that
 * reach guest DWORD fields are worker-local tokens, never parent heap
 * pointers or Windows Registry handles.
 */
#include <windows.h>

LONG WINAPI wow_registry_open_key(HKEY root, LPCSTR subkey, PHKEY result);
LONG WINAPI wow_registry_create_key(HKEY root, LPCSTR subkey, PHKEY result);
LONG WINAPI wow_registry_close_key(HKEY key);
LONG WINAPI wow_registry_delete_key(HKEY key, LPCSTR subkey);
LONG WINAPI wow_registry_set_value(HKEY key, LPCSTR subkey, DWORD type,
    LPCSTR data, DWORD data_size);
LONG WINAPI wow_registry_query_value(HKEY key, LPCSTR subkey, LPSTR data,
    PLONG data_size);
LONG WINAPI wow_registry_enum_key(HKEY key, DWORD index, LPSTR name,
    DWORD name_size);
LONG WINAPI wow_registry_open_key_ex(HKEY root, LPCSTR subkey, DWORD options,
    REGSAM access, PHKEY result);
LONG WINAPI wow_registry_query_info_key(HKEY key, LPTSTR class_name,
    LPDWORD class_length, LPDWORD reserved, LPDWORD subkeys,
    LPDWORD maximum_subkey_length, LPDWORD maximum_class_length,
    LPDWORD values, LPDWORD maximum_value_name_length,
    LPDWORD maximum_value_data_length, LPDWORD security_descriptor,
    PFILETIME last_write_time);
LONG WINAPI wow_registry_enum_value(HKEY key, DWORD index, LPSTR value_name,
    LPDWORD value_name_size, LPDWORD reserved, LPDWORD type, LPBYTE data,
    LPDWORD data_size);
VOID WINAPI wow_registry_binding_shutdown(VOID);

/* This header is force-included only for the two selected original caller
 * bodies.  It comes after windows.h here, so a later original winreg.h
 * include is guarded and cannot replace these aliases. */
#undef RegOpenKey
#undef RegCreateKey
#undef RegCloseKey
#undef RegDeleteKey
#undef RegSetValue
#undef RegQueryValue
#undef RegEnumKey
#undef RegOpenKeyEx
#undef RegQueryInfoKey
#undef RegEnumValue
#define RegOpenKey wow_registry_open_key
#define RegCreateKey wow_registry_create_key
#define RegCloseKey wow_registry_close_key
#define RegDeleteKey wow_registry_delete_key
#define RegSetValue wow_registry_set_value
#define RegQueryValue wow_registry_query_value
#define RegEnumKey wow_registry_enum_key
#define RegOpenKeyEx wow_registry_open_key_ex
#define RegQueryInfoKey wow_registry_query_info_key
#define RegEnumValue wow_registry_enum_value

#endif
