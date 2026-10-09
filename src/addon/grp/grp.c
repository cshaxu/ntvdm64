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
    WORD group_end;

    if (length < 0x22 || memcmp(bytes, "PMCC", 4) ||
        (*item_count = read_u16(bytes, 0x20)) > 255 ||
        0x22u + *item_count * 2u > length) return FALSE;
    /* Win3.1's cbGroup ends the pre-tag group payload.  Tag data follows it. */
    group_end = read_u16(bytes, 6);
    if (group_end < 0x22u + *item_count * 2u || group_end > length) return FALSE;
    for (i = 0; i < *item_count; ++i) {
        WORD item = read_u16(bytes, 0x22 + i * 2u);
        WORD executable;
        DWORD ignored;
        if ((DWORD)item + 24u > length) return FALSE;
        executable = read_u16(bytes, (DWORD)item + 22u);
        if (!string_length(bytes, length, executable, &ignored)) return FALSE;
    }
    for (i = group_end; i < length;) {
        WORD id, item, size;
        if (length - i < 6u) return FALSE;
        id = read_u16(bytes, i);
        item = read_u16(bytes, i + 2u);
        size = read_u16(bytes, i + 4u);
        if (id == 0xffffu && item == 0xffffu && !size)
            return i + 6u == length;
        if (size < 6u || size > length - i) return FALSE;
        /* 0x8101 is the documented Program Item working-directory tag. */
        if (id == 0x8101u && bytes[i + size - 1u]) return FALSE;
        i += size;
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
    DWORD input_cursor;
    WORD tag_offset;
    BOOL item_changed[256] = { FALSE };
    LONG tag_adjustment = 0;
    size_t new_length;
    size_t new_directory_length;
    size_t output_length;
    BOOL ok = FALSE;

    *changed = 0;
    if (!root_oem(root, new_root) || !read_file(path, &input, &input_length) ||
        !group_valid(input, input_length, &item_count)) goto done;
    tag_offset = read_u16(input, 6);
    new_length = strlen(new_root);
    new_directory_length = new_length +
        (new_root[new_length - 1u] == '\\' ? 0u : 1u);
    for (i = 0; i < item_count; ++i) {
        WORD item = read_u16(input, 0x22 + i * 2u);
        WORD executable = read_u16(input, (DWORD)item + 22u);
        DWORD length;
        const char *target = (const char *)input + executable;
        if (!string_length(input, input_length, executable, &length)) goto done;
        const char *suffix;
        if (length > 3 && target_in_root(root, target, &suffix)) {
            additions += (DWORD)new_length + (DWORD)strlen(suffix) + 1u;
            item_changed[i] = TRUE;
            ++replacements;
        }
    }
    if (!replacements) { ok = TRUE; goto done; }
    for (input_cursor = tag_offset; input_cursor < input_length;) {
        WORD id = read_u16(input, input_cursor);
        WORD item = read_u16(input, input_cursor + 2u);
        WORD size = read_u16(input, input_cursor + 4u);
        if (id == 0xffffu && item == 0xffffu && !size) {
            input_cursor += 6u;
            break;
        }
        if (id == 0x8101u && item < item_count && item_changed[item]) {
            LONG new_size = (LONG)(6u + new_directory_length + 1u);
            tag_adjustment += new_size - (LONG)size;
        }
        input_cursor += size;
    }
    if (input_cursor != input_length) goto done;
    {
        LONG total_length = (LONG)input_length + (LONG)additions + tag_adjustment;
        if (total_length <= 0 || total_length > 0xffff) goto done;
        output_length = (size_t)total_length;
    }
    if (additions > 0xffffu - read_u16(input, 6)) goto done;
    output = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, output_length);
    if (!output) goto done;
    /*
     * cbGroup is the byte offset at which the Win3.1 tag records begin, not
     * the physical end of the file.  Program Manager requires item strings
     * to remain before that boundary.  Insert the replacement strings there,
     * then preserve the tag suffix verbatim after the new boundary.
     */
    CopyMemory(output, input, tag_offset);
    cursor = tag_offset;
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
    for (input_cursor = tag_offset; input_cursor < input_length;) {
        WORD id = read_u16(input, input_cursor);
        WORD item = read_u16(input, input_cursor + 2u);
        WORD size = read_u16(input, input_cursor + 4u);
        if (id == 0xffffu && item == 0xffffu && !size) {
            if (cursor + 6u > output_length) goto done;
            CopyMemory(output + cursor, input + input_cursor, 6u);
            cursor += 6u;
            input_cursor += 6u;
            break;
        }
        if (id == 0x8101u && item < item_count && item_changed[item]) {
            DWORD size_out = (DWORD)(6u + new_directory_length + 1u);
            if (cursor + size_out > output_length) goto done;
            write_u16(output, cursor, id);
            write_u16(output, cursor + 2u, item);
            write_u16(output, cursor + 4u, (WORD)size_out);
            CopyMemory(output + cursor + 6u, new_root, new_length);
            if (new_directory_length != new_length)
                output[cursor + 6u + new_length] = '\\';
            output[cursor + 6u + new_directory_length] = 0;
            cursor += size_out;
            ++*changed;
        } else {
            if (cursor + size > output_length) goto done;
            CopyMemory(output + cursor, input + input_cursor, size);
            cursor += size;
        }
        input_cursor += size;
    }
    if (input_cursor != input_length || cursor != output_length) goto done;
    write_u16(output, 6, (WORD)(tag_offset + additions));
    write_u16(output, 4, 0);
    {
        WORD checksum = 0;
        for (i = 0; i + 1u < output_length; i += 2u)
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
