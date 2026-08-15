#include "dos_namespace.h"

#include <wchar.h>

static int is_reserved_device(const wchar_t *component)
{
    size_t length = 0u;

    while (component[length] != L'\0' && component[length] != L'.') ++length;
    if (length == 3u && (_wcsnicmp(component, L"CON", 3u) == 0 ||
        _wcsnicmp(component, L"PRN", 3u) == 0 ||
        _wcsnicmp(component, L"AUX", 3u) == 0 ||
        _wcsnicmp(component, L"NUL", 3u) == 0)) return 1;
    return length == 4u && (_wcsnicmp(component, L"COM", 3u) == 0 ||
        _wcsnicmp(component, L"LPT", 3u) == 0) && component[3] >= L'1' &&
        component[3] <= L'9';
}

static int append_component(wchar_t output[MAX_PATH], const wchar_t *component,
    size_t length)
{
    wchar_t local[MAX_PATH];
    size_t current = wcslen(output);

    if (length == 0u || length >= ARRAYSIZE(local) ||
        current + 1u + length + 1u > MAX_PATH) return 0;
    memcpy(local, component, length * sizeof(*component));
    local[length] = L'\0';
    if ((length == 1u && local[0] == L'.') ||
        (length == 2u && local[0] == L'.' && local[1] == L'.') ||
        wcschr(local, L':') != NULL || local[length - 1u] == L'.' ||
        local[length - 1u] == L' ' || is_reserved_device(local)) return 0;
    if (current != 0u && output[current - 1u] != L'\\') wcscat_s(output, MAX_PATH, L"\\");
    wcsncat_s(output, MAX_PATH, local, length);
    return 1;
}

int nvtdm_resolve_dos_path(const wchar_t *root, const wchar_t *dos_path,
    wchar_t output[MAX_PATH])
{
    wchar_t full_root[MAX_PATH];
    const wchar_t *cursor = dos_path;
    DWORD root_length;

    if (root == NULL || dos_path == NULL || output == NULL || dos_path[0] == L'\0') return 0;
    root_length = GetFullPathNameW(root, ARRAYSIZE(full_root), full_root, NULL);
    {
        const DWORD root_attributes = GetFileAttributesW(full_root);
        if (root_length == 0u || root_length >= ARRAYSIZE(full_root) ||
            root_attributes == INVALID_FILE_ATTRIBUTES ||
            (root_attributes & (FILE_ATTRIBUTE_DIRECTORY |
            FILE_ATTRIBUTE_REPARSE_POINT)) != FILE_ATTRIBUTE_DIRECTORY) return 0;
    }
    wcscpy_s(output, MAX_PATH, full_root);
    if (cursor[0] != L'\0' && cursor[1] == L':') cursor += 2;
    while (*cursor == L'\\' || *cursor == L'/') ++cursor;
    while (*cursor != L'\0') {
        const wchar_t *start = cursor;

        while (*cursor != L'\0' && *cursor != L'\\' && *cursor != L'/') ++cursor;
        if (!append_component(output, start, (size_t)(cursor - start))) return 0;
        while (*cursor == L'\\' || *cursor == L'/') ++cursor;
    }
    return 1;
}

HANDLE nvtdm_open_rooted_readonly(const wchar_t *root, const wchar_t *dos_path,
    wchar_t resolved[MAX_PATH])
{
    wchar_t root_path[MAX_PATH];
    wchar_t prefix[MAX_PATH];
    DWORD root_length;
    size_t index;

    if (!nvtdm_resolve_dos_path(root, dos_path, resolved)) return INVALID_HANDLE_VALUE;
    root_length = GetFullPathNameW(root, ARRAYSIZE(root_path), root_path, NULL);
    if (root_length == 0u || root_length >= ARRAYSIZE(root_path) ||
        _wcsnicmp(resolved, root_path, root_length) != 0) return INVALID_HANDLE_VALUE;
    wcscpy_s(prefix, ARRAYSIZE(prefix), resolved);
    for (index = root_length; prefix[index] != L'\0'; ++index) {
        if (prefix[index] == L'\\') {
            const wchar_t saved = prefix[index];
            DWORD attributes;
            prefix[index] = L'\0';
            attributes = GetFileAttributesW(prefix);
            prefix[index] = saved;
            if (attributes == INVALID_FILE_ATTRIBUTES ||
                (attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0u) {
                return INVALID_HANDLE_VALUE;
            }
        }
    }
    {
        const DWORD attributes = GetFileAttributesW(resolved);
        if (attributes == INVALID_FILE_ATTRIBUTES ||
            (attributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) != 0u) {
            return INVALID_HANDLE_VALUE;
        }
    }
    return CreateFileW(resolved, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
}
