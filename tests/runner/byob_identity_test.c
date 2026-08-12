#include "byob_identity.h"

#include <windows.h>
#include <stdio.h>

int wmain(void)
{
    wchar_t root[MAX_PATH], file[MAX_PATH];
    HANDLE handle;
    DWORD written;
    const char payload[] = "abc";
    const wchar_t sha[] = L"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";

    if (GetTempPathW(MAX_PATH, root) == 0 ||
        GetTempFileNameW(root, L"n64", 0, file) == 0 || !DeleteFileW(file) ||
        !CreateDirectoryW(file, NULL)) return 1;
    if (swprintf(root, MAX_PATH, L"%ls", file) < 0 ||
        swprintf(file, MAX_PATH, L"%ls\\NTIO.SYS", root) < 0) return 1;
    handle = CreateFileW(file, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (handle == INVALID_HANDLE_VALUE || !WriteFile(handle, payload, 3, &written, NULL) || written != 3) return 1;
    CloseHandle(handle);
    if (byob_verify_component(root, L"NTIO.SYS", 3, sha) != BYOB_IDENTITY_OK ||
        byob_verify_component(root, L"NTIO.SYS", 3, L"0000000000000000000000000000000000000000000000000000000000000000") != BYOB_IDENTITY_MISMATCH ||
        byob_verify_component(root, L"..\\NTIO.SYS", 3, sha) != BYOB_IDENTITY_NAME_UNSAFE) return 1;
    DeleteFileW(file);
    RemoveDirectoryW(root);
    wprintf(L"byob-identity-test: owned rooted identity checks verified\n");
    return 0;
}
