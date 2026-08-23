#include "byob_identity.h"

#include <windows.h>
#include <bcrypt.h>
#include <wchar.h>

int byob_component_name_safe(const wchar_t *name)
{
    const wchar_t *p;
    if (name == NULL || *name == L'\0' || wcscmp(name, L".") == 0 || wcscmp(name, L"..") == 0) return 0;
    for (p = name; *p; ++p) {
        if (*p == L'\\' || *p == L'/' || *p == L':' || *p == L'*' || *p == L'?' || *p == L'"' || *p == L'<' || *p == L'>' || *p == L'|') return 0;
    }
    return wcsstr(name, L"..") == NULL;
}

byob_identity_result byob_verify_component(const wchar_t *root,
    const wchar_t *name, uint64_t expected_bytes, const wchar_t expected_sha256[65])
{
    wchar_t path[MAX_PATH]; HANDLE root_handle = INVALID_HANDLE_VALUE, file = INVALID_HANDLE_VALUE; BCRYPT_ALG_HANDLE alg = NULL;
    BCRYPT_HASH_HANDLE hash = NULL; BYTE digest[32], buf[4096], *obj = NULL; DWORD obj_bytes = 0, got = 0, used;
    BY_HANDLE_FILE_INFORMATION info; uint64_t size; wchar_t hex[65]; size_t i;
    if (root == NULL || *root == L'\0' || !byob_component_name_safe(name) || expected_sha256 == NULL ||
        wcslen(root) + wcslen(name) + 2u > MAX_PATH) return BYOB_IDENTITY_NAME_UNSAFE;
    root_handle = CreateFileW(root, FILE_READ_ATTRIBUTES,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
    if (root_handle == INVALID_HANDLE_VALUE || !GetFileInformationByHandle(root_handle, &info) ||
        (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0u ||
        (info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0u) goto rejected;
    swprintf(path, MAX_PATH, L"%ls%ls%ls", root, root[wcslen(root) - 1u] == L'\\' ? L"" : L"\\", name);
    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_FLAG_OPEN_REPARSE_POINT, NULL);
    if (file == INVALID_HANDLE_VALUE || !GetFileInformationByHandle(file, &info) ||
        (info.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) != 0u) goto rejected;
    size = ((uint64_t)info.nFileSizeHigh << 32) | info.nFileSizeLow;
    if (size == 0u) goto rejected;
    if (size != expected_bytes) goto mismatch;
    if (BCryptOpenAlgorithmProvider(&alg, BCRYPT_SHA256_ALGORITHM, NULL, 0) < 0 ||
        BCryptGetProperty(alg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&obj_bytes, sizeof(obj_bytes), &used, 0) < 0 ||
        (obj = HeapAlloc(GetProcessHeap(), 0, obj_bytes)) == NULL ||
        BCryptCreateHash(alg, &hash, obj, obj_bytes, NULL, 0, 0) < 0) goto rejected;
    for (;;) {
        got = 0u;
        if (!ReadFile(file, buf, sizeof(buf), &got, NULL)) goto rejected;
        if (got == 0u) break;
        if (BCryptHashData(hash, buf, got, 0) < 0) goto rejected;
    }
    if (BCryptFinishHash(hash, digest, sizeof(digest), 0) < 0) goto rejected;
    for (i = 0; i < 32; ++i) {
        swprintf(hex + i * 2u, 3u, L"%02x", digest[i]);
    }
    hex[64] = 0;
    if (_wcsicmp(hex, expected_sha256) != 0) goto mismatch;
    BCryptDestroyHash(hash); HeapFree(GetProcessHeap(), 0, obj); BCryptCloseAlgorithmProvider(alg, 0); CloseHandle(file); CloseHandle(root_handle); return BYOB_IDENTITY_OK;
mismatch: if (hash) BCryptDestroyHash(hash); if (obj) HeapFree(GetProcessHeap(),0,obj); if (alg) BCryptCloseAlgorithmProvider(alg,0); if (file != INVALID_HANDLE_VALUE) CloseHandle(file); if (root_handle != INVALID_HANDLE_VALUE) CloseHandle(root_handle); return BYOB_IDENTITY_MISMATCH;
rejected: if (hash) BCryptDestroyHash(hash); if (obj) HeapFree(GetProcessHeap(),0,obj); if (alg) BCryptCloseAlgorithmProvider(alg,0); if (file != INVALID_HANDLE_VALUE) CloseHandle(file); if (root_handle != INVALID_HANDLE_VALUE) CloseHandle(root_handle); return BYOB_IDENTITY_OPEN_REJECTED;
}
