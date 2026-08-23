#include "byob_image.h"

#include "byob_identity.h"

#include <windows.h>
#include <bcrypt.h>
#include <wchar.h>

#define BYOB_IMAGE_MAX_BYTES (64u * 1024u * 1024u)

void byob_image_release(byob_image *image)
{
    if (image != NULL) {
        if (image->bytes != NULL) HeapFree(GetProcessHeap(), 0, image->bytes);
        image->bytes = NULL;
        image->byte_count = 0u;
    }
}

byob_image_result byob_image_load_named(const wchar_t *root, const wchar_t *file_name,
    byob_image *out_image)
{
    wchar_t path[MAX_PATH]; HANDLE file = INVALID_HANDLE_VALUE; LARGE_INTEGER size;
    DWORD read = 0u; byob_image_result result = BYOB_IMAGE_OPEN_REJECTED;
    if (!root || !file_name || !out_image || !byob_component_name_safe(file_name) ||
        swprintf(path, MAX_PATH, L"%ls\\%ls", root, file_name) < 0) return BYOB_IMAGE_INVALID_ARGUMENT;
    memset(out_image, 0, sizeof(*out_image));
    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE || !GetFileSizeEx(file, &size) || size.QuadPart <= 0 ||
        (uint64_t)size.QuadPart > BYOB_IMAGE_MAX_BYTES) goto done;
    out_image->bytes = (uint8_t *)HeapAlloc(GetProcessHeap(), 0u, (SIZE_T)size.QuadPart);
    if (!out_image->bytes) { result = BYOB_IMAGE_NO_MEMORY; goto done; }
    if (!ReadFile(file, out_image->bytes, (DWORD)size.QuadPart, &read, NULL) || read != (DWORD)size.QuadPart) goto done;
    out_image->byte_count = (size_t)size.QuadPart; result = BYOB_IMAGE_OK;
done:
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    if (result != BYOB_IMAGE_OK) byob_image_release(out_image);
    return result;
}

byob_image_result byob_image_load_exact(const wchar_t *root,
    const byob_component_descriptor *descriptor, byob_image *out_image)
{
    wchar_t path[MAX_PATH], actual[65];
    HANDLE root_handle = INVALID_HANDLE_VALUE, file = INVALID_HANDLE_VALUE;
    BY_HANDLE_FILE_INFORMATION info;
    BCRYPT_ALG_HANDLE algorithm = NULL;
    BCRYPT_HASH_HANDLE hash = NULL;
    BYTE digest[32], *hash_object = NULL;
    DWORD hash_object_bytes = 0u, used = 0u, read = 0u;
    uint64_t bytes;
    size_t offset = 0u, index;
    byob_image_result result = BYOB_IMAGE_OPEN_REJECTED;

    if (out_image == NULL || root == NULL || *root == L'\0' || descriptor == NULL ||
        !byob_component_name_safe(descriptor->file_name) || descriptor->bytes == 0u ||
        descriptor->bytes > BYOB_IMAGE_MAX_BYTES || wcslen(root) + wcslen(descriptor->file_name) + 2u > MAX_PATH) {
        return BYOB_IMAGE_INVALID_ARGUMENT;
    }
    out_image->bytes = NULL;
    out_image->byte_count = 0u;
    root_handle = CreateFileW(root, FILE_READ_ATTRIBUTES,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
    if (root_handle == INVALID_HANDLE_VALUE || !GetFileInformationByHandle(root_handle, &info) ||
        (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0u ||
        (info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0u) goto done;
    swprintf(path, MAX_PATH, L"%ls%ls%ls", root,
        root[wcslen(root) - 1u] == L'\\' ? L"" : L"\\", descriptor->file_name);
    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_FLAG_OPEN_REPARSE_POINT, NULL);
    if (file == INVALID_HANDLE_VALUE || !GetFileInformationByHandle(file, &info) ||
        (info.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) != 0u) goto done;
    bytes = ((uint64_t)info.nFileSizeHigh << 32) | info.nFileSizeLow;
    if (bytes != descriptor->bytes || bytes > SIZE_MAX) { result = BYOB_IMAGE_IDENTITY_MISMATCH; goto done; }
    out_image->bytes = (uint8_t *)HeapAlloc(GetProcessHeap(), 0, (SIZE_T)bytes);
    if (out_image->bytes == NULL) { result = BYOB_IMAGE_NO_MEMORY; goto done; }
    if (BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, NULL, 0) < 0 ||
        BCryptGetProperty(algorithm, BCRYPT_OBJECT_LENGTH, (PUCHAR)&hash_object_bytes,
            sizeof(hash_object_bytes), &used, 0) < 0 ||
        (hash_object = HeapAlloc(GetProcessHeap(), 0, hash_object_bytes)) == NULL ||
        BCryptCreateHash(algorithm, &hash, hash_object, hash_object_bytes, NULL, 0, 0) < 0) goto done;
    while (offset < (size_t)bytes) {
        DWORD chunk = (DWORD)(((size_t)bytes - offset) > 65536u ? 65536u : (size_t)bytes - offset);
        if (!ReadFile(file, out_image->bytes + offset, chunk, &read, NULL) || read != chunk ||
            BCryptHashData(hash, out_image->bytes + offset, read, 0) < 0) goto done;
        offset += read;
    }
    if (BCryptFinishHash(hash, digest, sizeof(digest), 0) < 0) goto done;
    for (index = 0u; index < 32u; ++index) swprintf(actual + index * 2u, 3u, L"%02x", digest[index]);
    actual[64] = L'\0';
    if (_wcsicmp(actual, descriptor->sha256) != 0) { result = BYOB_IMAGE_IDENTITY_MISMATCH; goto done; }
    out_image->byte_count = (size_t)bytes;
    result = BYOB_IMAGE_OK;
done:
    if (hash != NULL) BCryptDestroyHash(hash);
    if (hash_object != NULL) HeapFree(GetProcessHeap(), 0, hash_object);
    if (algorithm != NULL) BCryptCloseAlgorithmProvider(algorithm, 0);
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    if (root_handle != INVALID_HANDLE_VALUE) CloseHandle(root_handle);
    if (result != BYOB_IMAGE_OK) byob_image_release(out_image);
    return result;
}
