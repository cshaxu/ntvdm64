#include <stdio.h>
#include <stdint.h>
#include <windows.h>

#include "ntdos64_dos_safe_alias_v1.h"

int main(void)
{
    wchar_t target[MAX_PATH], probe[MAX_PATH], admitted_root[MAX_PATH];
    struct ntdos64_dos_safe_alias_v1 alias;
    HANDLE file;
    DWORD bytes;
    char byte;
    uint32_t owned_alias;

    ntdos64_dos_safe_alias_v1_clear(&alias);
    if (!GetFullPathNameW(L"build\\output\\dos", MAX_PATH, target, NULL) ||
        GetFileAttributesW(target) == INVALID_FILE_ATTRIBUTES) return 1;
    {
        int admitted = ntdos64_dos_safe_alias_v1_admit(target, 51u, &alias);
        if (!admitted || alias.admitted_root[0] == L'\0') {
            wprintf(L"alias admission=%d owns=%u root=%ls error=%lu\n", admitted,
                alias.owns_alias, alias.admitted_root, (unsigned long)GetLastError());
        return 2;
        }
    }
    if (swprintf(probe, MAX_PATH, L"%ls\\COMMAND.COM", alias.admitted_root) < 0)
        return 3;
    file = CreateFileW(probe, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, 0u, NULL);
    if (file == INVALID_HANDLE_VALUE || !ReadFile(file, &byte, 1u, &bytes, NULL) ||
        !CloseHandle(file)) return 4;
    owned_alias = alias.owns_alias;
    if (wcslen(alias.admitted_root) >= MAX_PATH) return 5;
    wcscpy_s(admitted_root, MAX_PATH, alias.admitted_root);
    ntdos64_dos_safe_alias_v1_release(&alias);
    if (alias.owns_alias != 0u || GetFileAttributesW(target) == INVALID_FILE_ATTRIBUTES ||
        (owned_alias != 0u && GetFileAttributesW(admitted_root) != INVALID_FILE_ATTRIBUTES))
        return 6;
    printf("T245 S22 DOS-safe root admission verified (session-link=%u)\n",
        owned_alias);
    return 0;
}
