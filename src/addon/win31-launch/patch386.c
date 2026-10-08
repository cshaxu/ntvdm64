/* PATCH386.EXE applies only the two approved, identity-bound Win3.1 deltas.
 * APPLY.CMD owns backup, mouse, configuration, and launch-profile policy. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "bcrypt.lib")

#define KRNL_RETAIL "FBEAF672EE9E917318CE8FCD1D560DB805DBFADF4777039ABBAAFF1A9C75B980"
#define KRNL_CANDIDATE "88E095A7C39C6294E8E3BE71CBD2EDC1E4DCECBB7101D1581021EA7BE7B33181"
#define WIN_RETAIL "6006860AE1003114583D70A1C6447247E1280A99633202912092A5F06A0C22A5"
#define WIN_CANDIDATE "C67E667E25E91C65EFB5ACDDEC437DA8B586037DA7DE502228CA8FBD806CF3F8"

static BOOL sha256(const wchar_t *path, char output[65])
{
    BCRYPT_ALG_HANDLE algorithm = NULL; BCRYPT_HASH_HANDLE hash = NULL;
    BYTE digest[32], buffer[8192], *object = NULL;
    DWORD object_length = 0, returned, read = 0; HANDLE file = INVALID_HANDLE_VALUE;
    BOOL result = FALSE; unsigned int index;
    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (file == INVALID_HANDLE_VALUE || BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, NULL, 0) < 0 ||
        BCryptGetProperty(algorithm, BCRYPT_OBJECT_LENGTH, (PUCHAR)&object_length, sizeof(object_length), &returned, 0) < 0 ||
        !(object = (BYTE *)HeapAlloc(GetProcessHeap(), 0, object_length)) ||
        BCryptCreateHash(algorithm, &hash, object, object_length, NULL, 0, 0) < 0) goto done;
    for (;;) { if (!ReadFile(file, buffer, sizeof(buffer), &read, NULL)) goto done; if (!read) break; if (BCryptHashData(hash, buffer, read, 0) < 0) goto done; }
    if (BCryptFinishHash(hash, digest, sizeof(digest), 0) < 0) goto done;
    for (index = 0; index < ARRAYSIZE(digest); ++index) sprintf_s(output + index * 2, 65 - index * 2, "%02X", digest[index]);
    result = TRUE;
done:
    if (hash) BCryptDestroyHash(hash); if (algorithm) BCryptCloseAlgorithmProvider(algorithm, 0);
    if (object) HeapFree(GetProcessHeap(), 0, object); if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    return result;
}

static BOOL read_all(const wchar_t *path, BYTE **output, DWORD *length)
{
    HANDLE file; DWORD read = 0; *output = NULL;
    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (file == INVALID_HANDLE_VALUE) return FALSE;
    *length = GetFileSize(file, NULL);
    if (*length == INVALID_FILE_SIZE || !(*output = (BYTE *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (SIZE_T)*length + 1)) ||
        !ReadFile(file, *output, *length, &read, NULL) || read != *length) { if (*output) HeapFree(GetProcessHeap(), 0, *output); *output = NULL; CloseHandle(file); return FALSE; }
    CloseHandle(file); return TRUE;
}

static BOOL replace_candidate(const wchar_t *target, const BYTE *bytes, DWORD length, const char *expected)
{
    wchar_t temporary[MAX_PATH]; HANDLE file = INVALID_HANDLE_VALUE; DWORD written; char actual[65] = {0}; BOOL ok = FALSE;
    if (swprintf_s(temporary, ARRAYSIZE(temporary), L"%ls.PATCH386", target) < 0 ||
        (file = CreateFileW(temporary, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL)) == INVALID_HANDLE_VALUE ||
        !WriteFile(file, bytes, length, &written, NULL) || written != length) goto done;
    CloseHandle(file); file = INVALID_HANDLE_VALUE;
    if (!sha256(temporary, actual) || _stricmp(actual, expected) ||
        !MoveFileExW(temporary, target, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) goto done;
    ok = TRUE;
done:
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    if (!ok) DeleteFileW(temporary);
    return ok;
}

static BOOL patch_krnl386(const wchar_t *target)
{
    static const BYTE before[] = {0x33,0xdb,0x3c,0x0a,0x73,0x22,0x3c,0x03,0x77,0x12,0xbb,0x38,0,0x80,0xfc,0,0x74,0x16,0xbb,0x35,0,0x80,0xfc,0x1f,0x76,0x0e,0xeb,0,0x50,0xe8,0x23,0,0x8b,0xd8,0x58,0x83,0xfb,0xff,0x74,0x0b};
    BYTE after[ARRAYSIZE(before)], *image = NULL; DWORD length; char hash[65]; BOOL result = FALSE;
    if (!sha256(target, hash)) { fwprintf(stderr, L"PATCH386.EXE: cannot hash KRNL386.EXE\n"); goto done; }
    if (!_stricmp(hash, KRNL_CANDIDATE)) { result = TRUE; goto done; }
    if (_stricmp(hash, KRNL_RETAIL)) { fwprintf(stderr, L"PATCH386.EXE: KRNL386 identity mismatch\n"); goto done; }
    if (!read_all(target, &image, &length)) { fwprintf(stderr, L"PATCH386.EXE: cannot read KRNL386.EXE\n"); goto done; }
    if (length < 0xcfedu + ARRAYSIZE(before) || memcmp(image + 0xcfed, before, sizeof(before))) { fwprintf(stderr, L"PATCH386.EXE: KRNL386 byte contract mismatch\n"); goto done; }
    memset(after, 0x90, sizeof(after));
    after[0] = 0xbb; after[1] = 0x21; after[2] = 0;
    after[3] = 0xe9; after[4] = 0x22; after[5] = 0;
    result = replace_candidate(target, (memcpy(image + 0xcfed, after, sizeof(after)), image), length, KRNL_CANDIDATE);
done:
    if (image) HeapFree(GetProcessHeap(), 0, image); return result;
}

static BOOL patch_win386(const wchar_t *target)
{
    static const BYTE before[] = {0x6a,0,0x6a,4,0xcd,0x20,0xa9,0,1,0};
    static const BYTE after[] = {0xba,0x21,0,0,0,0xe9,0x23,1,0,0};
    BYTE *image = NULL; DWORD length; char hash[65]; BOOL result = FALSE;
    if (!sha256(target, hash)) { fwprintf(stderr, L"PATCH386.EXE: cannot hash WIN386.EXE\n"); goto done; }
    if (!_stricmp(hash, WIN_CANDIDATE)) { result = TRUE; goto done; }
    if (_stricmp(hash, WIN_RETAIL)) { fwprintf(stderr, L"PATCH386.EXE: WIN386 identity mismatch\n"); goto done; }
    if (!read_all(target, &image, &length)) { fwprintf(stderr, L"PATCH386.EXE: cannot read WIN386.EXE\n"); goto done; }
    if (length < 407671u || memcmp(image + 407661u, before, sizeof(before))) { fwprintf(stderr, L"PATCH386.EXE: WIN386 byte contract mismatch\n"); goto done; }
    memcpy(image + 407661u, after, sizeof(after)); result = replace_candidate(target, image, length, WIN_CANDIDATE);
done:
    if (image) HeapFree(GetProcessHeap(), 0, image); return result;
}

int wmain(int argc, wchar_t **argv)
{
    if (argc != 6 || _wcsicmp(argv[1], L"--apply") || _wcsicmp(argv[2], L"--krnl") || _wcsicmp(argv[4], L"--win386")) {
        fwprintf(stderr, L"Usage: PATCH386.EXE --apply --krnl <KRNL386.EXE> --win386 <WIN386.EXE>\n"); return 64;
    }
    if (!patch_krnl386(argv[3])) { fwprintf(stderr, L"PATCH386.EXE: unsupported KRNL386.EXE\n"); return 2; }
    if (!patch_win386(argv[5])) { fwprintf(stderr, L"PATCH386.EXE: unsupported WIN386.EXE\n"); return 3; }
    return 0;
}
