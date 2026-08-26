#include "dos_safe_alias.h"

#include <winioctl.h>
#include <string.h>
#include <wchar.h>

#ifndef SYMBOLIC_LINK_FLAG_DIRECTORY
#define SYMBOLIC_LINK_FLAG_DIRECTORY 0x1u
#endif
#ifndef SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE
#define SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE 0x2u
#endif

/* Keep the mount-point buffer private: the older SDK shapes used by the
 * imported OpenNT headers do not consistently publish REPARSE_DATA_BUFFER. */
struct app_mount_point_reparse {
    DWORD tag;
    WORD data_bytes;
    WORD reserved;
    WORD substitute_offset;
    WORD substitute_bytes;
    WORD print_offset;
    WORD print_bytes;
    wchar_t names[1024];
};

static int create_directory_junction(const wchar_t *link, const wchar_t *target)
{
    struct app_mount_point_reparse data;
    wchar_t substitute[MAX_PATH + 5u];
    HANDLE handle;
    DWORD ignored;
    size_t target_chars, substitute_chars;
    DWORD total_bytes;

    if (link == NULL || target == NULL || wcsncmp(target, L"\\\\?\\", 4u) == 0 ||
        target[1] != L':') return 0;
    target_chars = wcslen(target);
    if (target_chars + 5u >= sizeof(substitute) / sizeof(substitute[0])) return 0;
    if (swprintf(substitute, sizeof(substitute) / sizeof(substitute[0]),
            L"\\??\\%ls", target) < 0) return 0;
    substitute_chars = wcslen(substitute);
    if (substitute_chars + target_chars + 2u >
        sizeof(data.names) / sizeof(data.names[0])) return 0;
    if (!CreateDirectoryW(link, NULL)) return 0;
    memset(&data, 0, sizeof(data));
    data.tag = IO_REPARSE_TAG_MOUNT_POINT;
    data.substitute_offset = 0u;
    data.substitute_bytes = (WORD)(substitute_chars * sizeof(wchar_t));
    data.print_offset = (WORD)((substitute_chars + 1u) * sizeof(wchar_t));
    data.print_bytes = (WORD)(target_chars * sizeof(wchar_t));
    memcpy(data.names, substitute, (substitute_chars + 1u) * sizeof(wchar_t));
    memcpy((unsigned char *)data.names + data.print_offset, target,
        (target_chars + 1u) * sizeof(wchar_t));
    data.data_bytes = (WORD)(8u + data.substitute_bytes + sizeof(wchar_t) +
        data.print_bytes + sizeof(wchar_t));
    total_bytes = 8u + data.data_bytes;
    handle = CreateFileW(link, GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL,
        OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
        NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        (void)RemoveDirectoryW(link);
        return 0;
    }
    if (!DeviceIoControl(handle, FSCTL_SET_REPARSE_POINT, &data, total_bytes,
            NULL, 0u, &ignored, NULL)) {
        CloseHandle(handle);
        (void)RemoveDirectoryW(link);
        return 0;
    }
    CloseHandle(handle);
    return 1;
}

static int dos_component_char(wchar_t value)
{
    return (value >= L'a' && value <= L'z') ||
        (value >= L'A' && value <= L'Z') ||
        (value >= L'0' && value <= L'9') ||
        wcschr(L"$%'-_@~`!(){}^#&", value) != NULL;
}

/* This validates the spelling that the original DOS configuration parser will
 * receive.  It deliberately does not attempt to implement DOS pathname
 * canonicalization; it merely rejects names that cannot survive that parser. */
static int dos_83_path(const wchar_t *path, uint32_t maximum_oem_chars)
{
    const wchar_t *cursor;
    char oem[MAX_PATH];
    BOOL used_default = FALSE;
    int bytes;
    if (path == NULL || wcslen(path) < 3u || path[1] != L':' ||
        (path[2] != L'\\' && path[2] != L'/')) return 0;
    bytes = WideCharToMultiByte(CP_OEMCP, WC_NO_BEST_FIT_CHARS, path, -1,
        oem, (int)sizeof(oem), NULL, &used_default);
    if (bytes <= 1 || used_default || (uint32_t)(bytes - 1) > maximum_oem_chars)
        return 0;
    cursor = path + 3u;
    while (*cursor != L'\0') {
        const wchar_t *end = cursor;
        uint32_t base = 0u, extension = 0u, dots = 0u;
        while (*end != L'\0' && *end != L'\\' && *end != L'/') ++end;
        if (end == cursor) return 0;
        while (cursor != end) {
            if (*cursor == L'.') {
                if (++dots != 1u) return 0;
            } else if (!dos_component_char(*cursor)) return 0;
            else if (dots == 0u) ++base;
            else ++extension;
            ++cursor;
        }
        if (base == 0u || base > 8u || extension > 3u) return 0;
        if (*end == L'\0') break;
        cursor = end + 1u;
        if (*cursor == L'\0') return 0;
    }
    return 1;
}

static int copy_path(wchar_t destination[MAX_PATH], const wchar_t *source)
{
    size_t chars;
    if (destination == NULL || source == NULL || (chars = wcslen(source)) >= MAX_PATH)
        return 0;
    memcpy(destination, source, (chars + 1u) * sizeof(*destination));
    return 1;
}

void app_dos_safe_alias_clear(struct app_dos_safe_alias *value)
{
    if (value != NULL) memset(value, 0, sizeof(*value));
}

void app_dos_safe_alias_release(struct app_dos_safe_alias *value)
{
    if (value == NULL) return;
    /* RemoveDirectoryW removes the link itself, never its target.  A failed
     * removal is retained for normal Windows diagnostics; it is not converted
     * into a guest filesystem operation. */
    if (value->owns_alias != 0u && value->admitted_root[0] != L'\0')
        (void)RemoveDirectoryW(value->admitted_root);
    app_dos_safe_alias_clear(value);
}

int app_dos_safe_alias_admit(const wchar_t *root,
    uint32_t maximum_oem_chars, struct app_dos_safe_alias *value)
{
    wchar_t short_root[MAX_PATH], temporary[MAX_PATH], short_temporary[MAX_PATH];
    wchar_t candidate[MAX_PATH];
    DWORD chars;
    if (root == NULL || value == NULL || maximum_oem_chars == 0u ||
        GetFileAttributesW(root) == INVALID_FILE_ATTRIBUTES) return 0;
    app_dos_safe_alias_clear(value);
    chars = GetShortPathNameW(root, short_root, MAX_PATH);
    if (chars != 0u && chars < MAX_PATH &&
        dos_83_path(short_root, maximum_oem_chars))
        return copy_path(value->admitted_root, short_root);

    /* OpenNT itself had an installed DOS directory with a DOS-consumable
     * spelling.  On a modern volume without 8.3 aliases, the CLI supplies the
     * smallest equivalent session link through public Win32 APIs. */
    chars = GetTempPathW(MAX_PATH, temporary);
    if (chars == 0u || chars >= MAX_PATH) return 0;
    chars = GetShortPathNameW(temporary, short_temporary, MAX_PATH);
    if (chars == 0u || chars >= MAX_PATH) return 0;
    if (chars > 0u && (short_temporary[chars - 1u] == L'\\' ||
        short_temporary[chars - 1u] == L'/')) short_temporary[chars - 1u] = L'\0';
    if (!dos_83_path(short_temporary, maximum_oem_chars)) return 0;
    if (GetTempFileNameW(short_temporary, L"NTD", 0u, candidate) == 0u ||
        !DeleteFileW(candidate)) return 0;
    if (!CreateSymbolicLinkW(candidate, root, SYMBOLIC_LINK_FLAG_DIRECTORY |
            SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE) &&
        !create_directory_junction(candidate, root)) return 0;
    if (!dos_83_path(candidate, maximum_oem_chars) ||
        GetFileAttributesW(candidate) == INVALID_FILE_ATTRIBUTES) {
        (void)RemoveDirectoryW(candidate);
        return 0;
    }
    if (!copy_path(value->admitted_root, candidate)) {
        (void)RemoveDirectoryW(candidate);
        return 0;
    }
    value->owns_alias = 1u;
    return 1;
}
