#include "config.h"

#include <windows.h>
#include <wchar.h>

static int child_path(const wchar_t *root, const wchar_t *name,
    wchar_t output[MAX_PATH])
{
    return root != NULL && name != NULL && output != NULL &&
        swprintf(output, MAX_PATH, L"%ls\\%ls", root, name) >= 0 &&
        GetFileAttributesW(output) != INVALID_FILE_ATTRIBUTES;
}

int app_bundle_load_roots(const wchar_t *dos_root,
    wchar_t config_source[MAX_PATH], wchar_t autoexec_source[MAX_PATH])
{
    wchar_t required[MAX_PATH];
    static const wchar_t *const names[] = {
        L"NTIO.SYS", L"NTDOS.SYS", L"COMMAND.COM", L"CONFIG.NT", L"AUTOEXEC.NT"
    };
    uint32_t index;
    if (dos_root == NULL || *dos_root == L'\0' ||
        GetFileAttributesW(dos_root) == INVALID_FILE_ATTRIBUTES) return 0;
    for (index = 0u; index < sizeof(names) / sizeof(names[0]); ++index)
        if (!child_path(dos_root, names[index], required)) return 0;
    return child_path(dos_root, L"CONFIG.NT", config_source) &&
        child_path(dos_root, L"AUTOEXEC.NT", autoexec_source);
}

int app_bundle_load_sibling(wchar_t product_root[MAX_PATH],
    wchar_t dos_root[MAX_PATH], wchar_t wow16_root[MAX_PATH],
    wchar_t config_source[MAX_PATH], wchar_t autoexec_source[MAX_PATH])
{
    DWORD length;
    wchar_t *slash;
    if (product_root == NULL || dos_root == NULL || wow16_root == NULL ||
        config_source == NULL || autoexec_source == NULL) return 0;
    length = GetModuleFileNameW(NULL, product_root, MAX_PATH);
    if (length == 0u || length >= MAX_PATH ||
        (slash = wcsrchr(product_root, L'\\')) == NULL) return 0;
    *slash = L'\0';
    if (swprintf(dos_root, MAX_PATH, L"%ls\\dos", product_root) < 0 ||
        swprintf(wow16_root, MAX_PATH, L"%ls\\wow16", product_root) < 0 ||
        GetFileAttributesW(wow16_root) == INVALID_FILE_ATTRIBUTES) return 0;
    return app_bundle_load_roots(dos_root, config_source, autoexec_source);
}
