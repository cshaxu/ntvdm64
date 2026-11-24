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

/* Inspect bytes only: package validation must never load executable code. */
static DWORD check_machine(const wchar_t *path, WORD expected)
{
    HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    IMAGE_DOS_HEADER dos;
    IMAGE_FILE_HEADER coff;
    LARGE_INTEGER size, offset;
    DWORD read, signature, error = ERROR_BAD_EXE_FORMAT;
    WORD magic;
    if (file == INVALID_HANDLE_VALUE) return GetLastError();
    if (!GetFileSizeEx(file, &size) ||
        !ReadFile(file, &dos, sizeof(dos), &read, NULL) || read != sizeof(dos) ||
        dos.e_magic != IMAGE_DOS_SIGNATURE || dos.e_lfanew < (LONG)sizeof(dos) ||
        (LONGLONG)dos.e_lfanew + (LONGLONG)(sizeof(signature) + sizeof(coff) + sizeof(magic)) > size.QuadPart)
        goto done;
    offset.QuadPart = dos.e_lfanew;
    if (!SetFilePointerEx(file, offset, NULL, FILE_BEGIN) ||
        !ReadFile(file, &signature, sizeof(signature), &read, NULL) || read != sizeof(signature) ||
        signature != IMAGE_NT_SIGNATURE ||
        !ReadFile(file, &coff, sizeof(coff), &read, NULL) || read != sizeof(coff) ||
        coff.Machine != expected || coff.SizeOfOptionalHeader < sizeof(magic) ||
        (LONGLONG)dos.e_lfanew + (LONGLONG)(sizeof(signature) + sizeof(coff)) + coff.SizeOfOptionalHeader > size.QuadPart ||
        !ReadFile(file, &magic, sizeof(magic), &read, NULL) || read != sizeof(magic) ||
        magic != (expected == IMAGE_FILE_MACHINE_I386 ? IMAGE_NT_OPTIONAL_HDR32_MAGIC : IMAGE_NT_OPTIONAL_HDR64_MAGIC))
        goto done;
    error = ERROR_SUCCESS;
done:
    CloseHandle(file);
    return error;
}
DWORD package_manifest_rows(const wchar_t *path);

int wmain(int argc, wchar_t **argv)
{
    GUID identity;
    static const GUID empty = {0};
    wchar_t text[39];
    int index;
    if (argc == 3 && !wcscmp(argv[1], L"--manifest")) return (int)package_manifest_rows(argv[2]);
    if (argc == 2 && !wcscmp(argv[1], L"--new")) {
        if (FAILED(CoCreateGuid(&identity)) || !memcmp(&identity, &empty, sizeof(identity)) ||
            !StringFromGUID2(&identity, text, ARRAYSIZE(text))) return ERROR_GEN_FAILURE;
        text[37] = 0;
        wprintf(L"%ls\n", text + 1);
        return 0;
    }
    if (argc == 4 && !wcscmp(argv[1], L"--machine")) {
        WORD machine = !wcscmp(argv[2], L"x86") ? IMAGE_FILE_MACHINE_I386 :
            !wcscmp(argv[2], L"x64") ? IMAGE_FILE_MACHINE_AMD64 : 0;
        return machine ? (int)check_machine(argv[3], machine) : ERROR_INVALID_PARAMETER;
    }
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
