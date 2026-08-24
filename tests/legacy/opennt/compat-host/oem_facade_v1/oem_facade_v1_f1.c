#include <windows.h>
#include <wchar.h>

#include "oem_facade_v1.h"

#define NTDOS64_OEM_FACADE_V1_PATH_CAPACITY 32768u

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif

static WCHAR ntdos64_oem_facade_v1_root[NTDOS64_OEM_FACADE_V1_PATH_CAPACITY];
static DWORD ntdos64_oem_facade_v1_root_length;

static WCHAR *ntdos64_oem_facade_v1_to_wide(LPCSTR text)
{
    int length;
    WCHAR *result;

    if (text == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }

    length = MultiByteToWideChar(CP_OEMCP, MB_ERR_INVALID_CHARS, text, -1,
                                 NULL, 0);
    if (length == 0) return NULL;
    result = HeapAlloc(GetProcessHeap(), 0, (SIZE_T)length * sizeof(WCHAR));
    if (result == NULL) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }
    if (MultiByteToWideChar(CP_OEMCP, MB_ERR_INVALID_CHARS, text, -1, result,
                            length) == 0) {
        HeapFree(GetProcessHeap(), 0, result);
        return NULL;
    }
    return result;
}

static BOOL ntdos64_oem_facade_v1_is_below_root(LPCWSTR path)
{
    WCHAR next;

    if (ntdos64_oem_facade_v1_root_length == 0 ||
        _wcsnicmp(path, ntdos64_oem_facade_v1_root,
                  ntdos64_oem_facade_v1_root_length) != 0) {
        return FALSE;
    }
    next = path[ntdos64_oem_facade_v1_root_length];
    return next == L'\0' || next == L'\\';
}

static WCHAR *ntdos64_oem_facade_v1_map_path(LPCSTR path)
{
    WCHAR *input;
    WCHAR *result;
    DWORD length;

    input = ntdos64_oem_facade_v1_to_wide(path);
    if (input == NULL) return NULL;
    result = HeapAlloc(GetProcessHeap(), 0,
                       NTDOS64_OEM_FACADE_V1_PATH_CAPACITY * sizeof(WCHAR));
    if (result == NULL) {
        HeapFree(GetProcessHeap(), 0, input);
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }
    length = GetFullPathNameW(input, NTDOS64_OEM_FACADE_V1_PATH_CAPACITY,
                              result, NULL);
    HeapFree(GetProcessHeap(), 0, input);
    if (length == 0 || length >= NTDOS64_OEM_FACADE_V1_PATH_CAPACITY ||
        !ntdos64_oem_facade_v1_is_below_root(result)) {
        HeapFree(GetProcessHeap(), 0, result);
        SetLastError(ERROR_ACCESS_DENIED);
        return NULL;
    }
    return result;
}

BOOL WINAPI ntdos64_oem_facade_v1_configure_resource_root(LPCSTR path)
{
    WCHAR *input;
    DWORD length;
    DWORD attributes;

    ntdos64_oem_facade_v1_reset();
    input = ntdos64_oem_facade_v1_to_wide(path);
    if (input == NULL) return FALSE;
    length = GetFullPathNameW(input, NTDOS64_OEM_FACADE_V1_PATH_CAPACITY,
                              ntdos64_oem_facade_v1_root, NULL);
    HeapFree(GetProcessHeap(), 0, input);
    if (length == 0 || length >= NTDOS64_OEM_FACADE_V1_PATH_CAPACITY) {
        ntdos64_oem_facade_v1_reset();
        return FALSE;
    }
    while (length > 3 && ntdos64_oem_facade_v1_root[length - 1] == L'\\') {
        ntdos64_oem_facade_v1_root[--length] = L'\0';
    }
    attributes = GetFileAttributesW(ntdos64_oem_facade_v1_root);
    if (attributes == INVALID_FILE_ATTRIBUTES ||
        (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
        ntdos64_oem_facade_v1_reset();
        return FALSE;
    }
    ntdos64_oem_facade_v1_root_length = length;
    return TRUE;
}

VOID WINAPI ntdos64_oem_facade_v1_reset(void)
{
    ntdos64_oem_facade_v1_root[0] = L'\0';
    ntdos64_oem_facade_v1_root_length = 0;
}

HANDLE WINAPI CreateFileOem(LPCSTR path, DWORD access, DWORD share,
                            LPSECURITY_ATTRIBUTES security, DWORD disposition,
                            DWORD flags, HANDLE template_file)
{
    WCHAR *mapped = ntdos64_oem_facade_v1_map_path(path);
    HANDLE result;

    if (mapped == NULL) return INVALID_HANDLE_VALUE;
    result = CreateFileW(mapped,
                         access == GENERIC_READ
                             ? access | FILE_WRITE_ATTRIBUTES
                             : access,
                         share, security, disposition, flags, template_file);
    if (result == INVALID_HANDLE_VALUE && access == GENERIC_READ) {
        result = CreateFileW(mapped, access, share, security, disposition,
                             flags, template_file);
    }
    HeapFree(GetProcessHeap(), 0, mapped);
    return result;
}
