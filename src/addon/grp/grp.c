/* GRP.EXE: bounded reader/editor for Windows 3.x PMCC Program Manager groups. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdio.h>
#include <string.h>
#include <wchar.h>

static WORD read_u16(const BYTE *bytes, DWORD offset)
{
    return (WORD)(bytes[offset] | ((WORD)bytes[offset + 1] << 8));
}

static void write_u16(BYTE *bytes, DWORD offset, WORD value)
{
    bytes[offset] = (BYTE)value;
    bytes[offset + 1] = (BYTE)(value >> 8);
}

static BOOL read_file(const wchar_t *path, BYTE **out, DWORD *length)
{
    HANDLE file;
    DWORD read = 0;

    *out = NULL;
    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return FALSE;
    *length = GetFileSize(file, NULL);
    if (*length == INVALID_FILE_SIZE || *length < 0x22 || *length > 0xffffu ||
        !(*out = HeapAlloc(GetProcessHeap(), 0, *length)) ||
        !ReadFile(file, *out, *length, &read, NULL) || read != *length) {
        CloseHandle(file);
        if (*out) HeapFree(GetProcessHeap(), 0, *out);
        *out = NULL;
        return FALSE;
    }
    CloseHandle(file);
    return TRUE;
}

static BOOL write_atomic(const wchar_t *path, const BYTE *bytes, DWORD length)
{
    wchar_t temporary[MAX_PATH];
    HANDLE file;
    DWORD written = 0;
    BOOL ok;

    if (swprintf_s(temporary, MAX_PATH, L"%ls.T438", path) < 0) return FALSE;
    file = CreateFileW(temporary, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    ok = file != INVALID_HANDLE_VALUE &&
         WriteFile(file, bytes, length, &written, NULL) && written == length;
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    if (!ok || !MoveFileExW(temporary, path,
                            MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        DeleteFileW(temporary);
        return FALSE;
    }
    return TRUE;
}

static BOOL string_length(const BYTE *bytes, DWORD length, WORD offset,
                          DWORD *value)
{
    DWORD cursor;

    if (offset >= length) return FALSE;
    for (cursor = offset; cursor < length; ++cursor) {
        if (!bytes[cursor]) {
            *value = cursor - offset;
            return TRUE;
        }
    }
    return FALSE;
}

static BOOL group_valid(const BYTE *bytes, DWORD length, DWORD *item_count)
{
    DWORD i;

    if (length < 0x22 || memcmp(bytes, "PMCC", 4) ||
        (*item_count = read_u16(bytes, 0x20)) > 255 ||
        0x22u + *item_count * 2u > length) return FALSE;
    for (i = 0; i < *item_count; ++i) {
        WORD item = read_u16(bytes, 0x22 + i * 2u);
        WORD executable;
        DWORD ignored;
        if ((DWORD)item + 24u > length) return FALSE;
        executable = read_u16(bytes, (DWORD)item + 22u);
        if (!string_length(bytes, length, executable, &ignored)) return FALSE;
    }
    return TRUE;
}

static BOOL root_oem(const wchar_t *wide, char out[MAX_PATH])
{
    int length = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, wide, -1,
                                     out, MAX_PATH, NULL, NULL);
    return length > 1 && length < MAX_PATH;
}

static BOOL target_in_root(const wchar_t *root, const char *target,
                           const char **suffix_out)
{
    const char *suffix;
    wchar_t relative[MAX_PATH], candidate[MAX_PATH];
    size_t suffix_length;

    if (!(target[0] && target[1] == ':' && target[2] == '\\') ||
        !(suffix = strrchr(target, '\\'))) return FALSE;
    suffix_length = strlen(suffix + 1);
    if (!suffix_length || suffix_length >= MAX_PATH ||
        !MultiByteToWideChar(CP_ACP, 0, suffix + 1, -1, relative, MAX_PATH) ||
        swprintf_s(candidate, MAX_PATH, L"%ls%ls%ls", root,
                   root[wcslen(root) - 1] == L'\\' ? L"" : L"\\", relative) < 0 ||
        GetFileAttributesW(candidate) == INVALID_FILE_ATTRIBUTES) return FALSE;
    *suffix_out = suffix;
    return TRUE;
}

static BOOL replace_root(const wchar_t *path, const wchar_t *root,
                         DWORD *changed)
{
    BYTE *input = NULL;
    BYTE *output = NULL;
    char new_root[MAX_PATH];
    DWORD input_length, item_count, replacements = 0, cursor, i;
    DWORD additions = 0;
    size_t new_length;
    size_t output_length;
    BOOL ok = FALSE;

    *changed = 0;
    if (!root_oem(root, new_root) || !read_file(path, &input, &input_length) ||
        !group_valid(input, input_length, &item_count)) goto done;
    new_length = strlen(new_root);
    for (i = 0; i < item_count; ++i) {
        WORD item = read_u16(input, 0x22 + i * 2u);
        WORD executable = read_u16(input, (DWORD)item + 22u);
        DWORD length;
        const char *target = (const char *)input + executable;
        if (!string_length(input, input_length, executable, &length)) goto done;
        const char *suffix;
        if (length > 3 && target_in_root(root, target, &suffix)) {
            additions += (DWORD)new_length + (DWORD)strlen(suffix) + 1u;
            ++replacements;
        }
    }
    if (!replacements) { ok = TRUE; goto done; }
    output_length = input_length + additions;
    if (output_length > 0xffffu) goto done;
    if (additions > 0xffffu - read_u16(input, 6)) goto done;
    output = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, output_length);
    if (!output) goto done;
    CopyMemory(output, input, input_length);
    cursor = input_length;
    for (i = 0; i < item_count; ++i) {
        WORD item = read_u16(input, 0x22 + i * 2u);
        WORD executable = read_u16(input, (DWORD)item + 22u);
        DWORD length;
        const char *target = (const char *)input + executable;
        size_t suffix_length;
        const char *suffix;
        if (!string_length(input, input_length, executable, &length)) goto done;
        if (!(length > 3 && target_in_root(root, target, &suffix))) continue;
        suffix_length = strlen(suffix);
        if (cursor + new_length + suffix_length + 1u > output_length) goto done;
        CopyMemory(output + cursor, new_root, new_length);
        CopyMemory(output + cursor + new_length, suffix, suffix_length + 1u);
        write_u16(output, (DWORD)item + 22u, (WORD)cursor);
        cursor += (DWORD)new_length + (DWORD)suffix_length + 1u;
        ++*changed;
    }
    if (cursor != output_length) goto done;
    write_u16(output, 6, (WORD)(read_u16(input, 6) + additions));
    write_u16(output, 4, 0);
    {
        WORD checksum = 0;
        for (i = 0; i + 1u < cursor; i += 2u)
            checksum = (WORD)(checksum + read_u16(output, i));
        write_u16(output, 4, (WORD)(0u - checksum));
    }
    ok = write_atomic(path, output, (DWORD)output_length);
done:
    if (output) HeapFree(GetProcessHeap(), 0, output);
    if (input) HeapFree(GetProcessHeap(), 0, input);
    return ok;
}

static BOOL show(const wchar_t *path)
{
    BYTE *input = NULL;
    DWORD length, item_count, i;
    BOOL ok = FALSE;

    if (!read_file(path, &input, &length) || !group_valid(input, length, &item_count)) goto done;
    for (i = 0; i < item_count; ++i) {
        WORD item = read_u16(input, 0x22 + i * 2u);
        WORD executable = read_u16(input, (DWORD)item + 22u);
        wprintf(L"ITEM%lu=", (unsigned long)(i + 1));
        printf("%s\n", (const char *)input + executable);
    }
    ok = TRUE;
done:
    if (input) HeapFree(GetProcessHeap(), 0, input);
    return ok;
}

int wmain(int argc, wchar_t **argv)
{
    DWORD changed;
    BOOL ok;

    if (argc == 3 && !_wcsicmp(argv[1], L"show")) {
        ok = show(argv[2]);
        if (!ok) fwprintf(stderr, L"GRP.EXE: invalid group %ls\n", argv[2]);
        return ok ? 0 : 1;
    }
    if (argc == 5 && !_wcsicmp(argv[1], L"replace-root") &&
        !_wcsicmp(argv[3], L"--root")) {
        if (!replace_root(argv[2], argv[4], &changed)) {
            fwprintf(stderr, L"GRP.EXE: could not update %ls\n", argv[2]);
            return 1;
        }
        wprintf(L"CHANGED=%lu\n", (unsigned long)changed);
        return 0;
    }
    fwprintf(stderr, L"Usage:\n  GRP.EXE show <file>\n  GRP.EXE replace-root <file> --root INSTALLED_ROOT\n");
    return 64;
}
