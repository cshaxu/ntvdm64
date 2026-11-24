#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <objbase.h>
#include <stdio.h>
#include <string.h>

static DWORD stamp(const wchar_t *path, const GUID *identity)
{
    HANDLE update = BeginUpdateResourceW(path, FALSE);
    DWORD error;
    if (!update) return GetLastError();
    if (!UpdateResourceW(update, MAKEINTRESOURCEW(10), L"NTVDM_PACKAGE_ID", 0,
            (void *)identity, sizeof(*identity))) {
        error = GetLastError();
        EndUpdateResourceW(update, TRUE);
        return error;
    }
    if (!EndUpdateResourceW(update, FALSE)) return GetLastError();
    return ERROR_SUCCESS;
}

int wmain(int argc, wchar_t **argv)
{
    GUID identity;
    static const GUID empty = {0};
    wchar_t text[39];
    int index;
    if (argc < 3 || wcslen(argv[1]) != 36) {
        fputws(L"Usage: ntvdm-package-id UUID IMAGE [IMAGE...]\n", stderr);
        return ERROR_INVALID_PARAMETER;
    }
    if (swprintf_s(text, 39, L"{%s}", argv[1]) < 0 ||
            FAILED(CLSIDFromString(text, &identity)) ||
            !memcmp(&identity, &empty, sizeof(identity))) return ERROR_INVALID_PARAMETER;
    for (index = 2; index < argc; ++index) {
        DWORD error = stamp(argv[index], &identity);
        if (error) {
            fwprintf(stderr, L"Package identity write failed: %s (%lu)\n", argv[index], error);
            return (int)error;
        }
    }
    return 0;
}
