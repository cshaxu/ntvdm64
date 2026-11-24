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

static DWORD read_identity(const wchar_t *path)
{
    HMODULE image = LoadLibraryExW(path, NULL,
        LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    HRSRC resource;
    const unsigned char *bytes;
    DWORD error = ERROR_SUCCESS;
    unsigned int index;
    if (!image) return GetLastError();
    resource = FindResourceW(image, L"NTVDM_PACKAGE_ID", MAKEINTRESOURCEW(10));
    if (!resource) error = GetLastError();
    else if (SizeofResource(image, resource) != sizeof(GUID)) error = ERROR_INVALID_DATA;
    else {
        bytes = (const unsigned char *)LockResource(LoadResource(image, resource));
        if (!bytes) error = ERROR_INVALID_DATA;
        else {
            for (index = 0; index < sizeof(GUID); ++index) printf("%02X", bytes[index]);
            putchar('\n');
        }
    }
    FreeLibrary(image);
    return error;
}

int wmain(int argc, wchar_t **argv)
{
    GUID identity;
    static const GUID empty = {0};
    wchar_t text[39];
    int index;
    if (argc == 3 && !wcscmp(argv[1], L"--read")) return (int)read_identity(argv[2]);
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
