/* HASH.EXE: intentionally narrow SHA-256 printer for one file. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>
#include <stdio.h>

#pragma comment(lib, "bcrypt.lib")

int wmain(int argc, wchar_t **argv)
{
    HANDLE file = INVALID_HANDLE_VALUE;
    BCRYPT_ALG_HANDLE algorithm = NULL;
    BCRYPT_HASH_HANDLE hash = NULL;
    BYTE object[4096], buffer[65536], digest[32];
    DWORD read = 0, property_bytes = 0, object_bytes = 0, digest_bytes = 0, i;
    NTSTATUS status;
    int result = 1;

    if (argc != 2) {
        fwprintf(stderr, L"Usage: HASH.EXE <file>\n");
        return 64;
    }
    file = CreateFileW(argv[1], GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) goto done;
    status = BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, NULL, 0);
    if (!BCRYPT_SUCCESS(status) ||
        !BCRYPT_SUCCESS(BCryptGetProperty(algorithm, BCRYPT_OBJECT_LENGTH,
                                          (BYTE *)&object_bytes, sizeof(object_bytes), &property_bytes, 0)) ||
        !BCRYPT_SUCCESS(BCryptGetProperty(algorithm, BCRYPT_HASH_LENGTH,
                                          (BYTE *)&digest_bytes, sizeof(digest_bytes), &property_bytes, 0)) ||
        object_bytes > sizeof(object) || digest_bytes != sizeof(digest) ||
        !BCRYPT_SUCCESS(BCryptCreateHash(algorithm, &hash, object, object_bytes,
                                         NULL, 0, 0))) goto done;
    for (;;) {
        if (!ReadFile(file, buffer, sizeof(buffer), &read, NULL)) goto done;
        if (read == 0) break;
        if (!BCRYPT_SUCCESS(BCryptHashData(hash, buffer, read, 0))) goto done;
    }
    if (!BCRYPT_SUCCESS(BCryptFinishHash(hash, digest, sizeof(digest), 0))) goto done;
    for (i = 0; i < sizeof(digest); ++i) wprintf(L"%02X", digest[i]);
    wprintf(L"\n");
    result = 0;
done:
    if (result != 0) fwprintf(stderr, L"HASH.EXE: could not hash %ls (error %lu)\n", argv[1], GetLastError());
    if (hash) BCryptDestroyHash(hash);
    if (algorithm) BCryptCloseAlgorithmProvider(algorithm, 0);
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    return result;
}
