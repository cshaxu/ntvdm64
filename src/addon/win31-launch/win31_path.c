/* Conservative installed-tree relocation for textual Windows 3.1 settings.
 * Binary .GRP files are deliberately excluded until their record format is
 * parsed; raw byte substitution would corrupt variable-length records. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

static BOOL join_path(wchar_t *out, DWORD capacity, const wchar_t *left,
                      const wchar_t *right)
{
    return swprintf_s(out, capacity, L"%ls%ls%ls", left,
                      left[wcslen(left) - 1] == L'\\' ? L"" : L"\\", right) >= 0;
}

static BOOL read_text(const wchar_t *path, char **out, DWORD *length)
{
    HANDLE file;
    DWORD read = 0;

    *out = NULL;
    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                       0, NULL);
    if (file == INVALID_HANDLE_VALUE) return FALSE;
    *length = GetFileSize(file, NULL);
    if (*length == INVALID_FILE_SIZE || *length > 1024u * 1024u ||
        !(*out = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                   (SIZE_T)*length + 1)) ||
        !ReadFile(file, *out, *length, &read, NULL) || read != *length) {
        CloseHandle(file);
        if (*out) HeapFree(GetProcessHeap(), 0, *out);
        *out = NULL;
        return FALSE;
    }
    CloseHandle(file);
    return TRUE;
}

static BOOL write_file(const wchar_t *path, const char *bytes, DWORD length)
{
    HANDLE file = CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    DWORD written = 0;
    BOOL ok = file != INVALID_HANDLE_VALUE &&
              WriteFile(file, bytes, length, &written, NULL) && written == length;
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    return ok;
}

/* Preserve the preimage beside the file being replaced.  An existing .BAK
 * belongs to an earlier repair and is never overwritten. */
static BOOL backup_original(const wchar_t *path)
{
    wchar_t backup[MAX_PATH];

    if (swprintf_s(backup, MAX_PATH, L"%ls.BAK", path) < 0 ||
        GetFileAttributesW(backup) != INVALID_FILE_ATTRIBUTES) return FALSE;
    return CopyFileW(path, backup, TRUE);
}

static BOOL ordinary_file(const wchar_t *path)
{
    DWORD attributes = GetFileAttributesW(path);
    return attributes != INVALID_FILE_ATTRIBUTES &&
           !(attributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT));
}

static char *find_ci(char *text, const char *needle)
{
    size_t length = strlen(needle);
    for (; *text; ++text) {
        if (!_strnicmp(text, needle, length)) return text;
    }
    return NULL;
}

static BOOL existing_relative_path(const wchar_t *root, const char *suffix)
{
    char ansi[MAX_PATH];
    wchar_t relative[MAX_PATH];
    wchar_t candidate[MAX_PATH];
    size_t length = 0;

    if (*suffix != '\\') return FALSE;
    while (suffix[length] && suffix[length] != '\r' && suffix[length] != '\n' &&
           suffix[length] != '"' && suffix[length] != ';') {
        if (length >= sizeof(ansi) - 1) return FALSE;
        ++length;
    }
    if (length <= 1) return FALSE;
    CopyMemory(ansi, suffix + 1, length - 1);
    ansi[length - 1] = '\0';
    if (!MultiByteToWideChar(CP_ACP, 0, ansi, -1, relative, MAX_PATH) ||
        !join_path(candidate, MAX_PATH, root, relative)) return FALSE;
    return GetFileAttributesW(candidate) != INVALID_FILE_ATTRIBUTES;
}

static BOOL discover_old_root(const wchar_t *root, char old_root[MAX_PATH])
{
    wchar_t program_manager[MAX_PATH];
    char *contents = NULL;
    char *line;
    DWORD length;
    BOOL found = FALSE;

    if (!join_path(program_manager, MAX_PATH, root, L"PROGMAN.INI") ||
        !ordinary_file(program_manager) ||
        !read_text(program_manager, &contents, &length)) return FALSE;

    for (line = contents; line && *line;) {
        char *line_end = strpbrk(line, "\r\n");
        char *equals;
        char *slash;
        char saved = 0;

        if (line_end) {
            saved = *line_end;
            *line_end = '\0';
        }
        equals = strchr(line, '=');
        if (equals && isalpha((unsigned char)equals[1]) && equals[2] == ':' &&
            (slash = strrchr(equals + 1, '\\')) != NULL &&
            existing_relative_path(root, slash)) {
            *slash = '\0';
            if (!found) {
                strncpy_s(old_root, MAX_PATH, equals + 1, _TRUNCATE);
                found = TRUE;
            } else if (_stricmp(old_root, equals + 1)) {
                *slash = '\\';
                if (line_end) *line_end = saved;
                HeapFree(GetProcessHeap(), 0, contents);
                return FALSE;
            }
            *slash = '\\';
        }
        if (line_end) {
            *line_end = saved;
            line = line_end + 1;
            if (saved == '\r' && *line == '\n') ++line;
        } else {
            break;
        }
    }
    HeapFree(GetProcessHeap(), 0, contents);
    return found;
}

static DWORD count_replacements(const wchar_t *root, char *contents,
                                const char *old_root)
{
    const size_t old_length = strlen(old_root);
    char *cursor = contents;
    char *match;
    DWORD count = 0;

    while ((match = find_ci(cursor, old_root)) != NULL) {
        if (existing_relative_path(root, match + old_length)) ++count;
        cursor = match + old_length;
    }
    return count;
}

static BOOL replace_references(const wchar_t *root, const wchar_t *path,
                               const char *old_root, const char *new_root,
                               DWORD *changed)
{
    char *contents = NULL;
    char *output = NULL;
    char *cursor;
    char *writer;
    char *match;
    wchar_t temporary[MAX_PATH];
    DWORD length;
    DWORD replacements;
    size_t old_length = strlen(old_root);
    size_t new_length = strlen(new_root);
    size_t output_length;

    *changed = 0;
    if (!read_text(path, &contents, &length)) return TRUE;
    replacements = count_replacements(root, contents, old_root);
    if (!replacements) {
        HeapFree(GetProcessHeap(), 0, contents);
        return TRUE;
    }
    if (new_length >= old_length) {
        output_length = (size_t)length + (new_length - old_length) * replacements + 1;
    } else {
        output_length = (size_t)length - (old_length - new_length) * replacements + 1;
    }
    output = (char *)HeapAlloc(GetProcessHeap(), 0, output_length);
    if (!output) goto fail;

    cursor = contents;
    writer = output;
    while ((match = find_ci(cursor, old_root)) != NULL) {
        size_t prefix = (size_t)(match - cursor);
        CopyMemory(writer, cursor, prefix);
        writer += prefix;
        if (existing_relative_path(root, match + old_length)) {
            CopyMemory(writer, new_root, new_length);
            writer += new_length;
            ++*changed;
        } else {
            CopyMemory(writer, match, old_length);
            writer += old_length;
        }
        cursor = match + old_length;
    }
    strcpy_s(writer, output_length - (size_t)(writer - output), cursor);
    if (swprintf_s(temporary, MAX_PATH, L"%ls.T438", path) < 0 ||
        !write_file(temporary, output, (DWORD)strlen(output)) ||
        !backup_original(path) ||
        !MoveFileExW(temporary, path, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        DeleteFileW(temporary);
        goto fail;
    }
    HeapFree(GetProcessHeap(), 0, output);
    HeapFree(GetProcessHeap(), 0, contents);
    return TRUE;

fail:
    if (output) HeapFree(GetProcessHeap(), 0, output);
    if (contents) HeapFree(GetProcessHeap(), 0, contents);
    return FALSE;
}

static WORD read_u16(const BYTE *bytes, DWORD offset)
{
    return (WORD)(bytes[offset] | ((WORD)bytes[offset + 1] << 8));
}

static void write_u16(BYTE *bytes, DWORD offset, WORD value)
{
    bytes[offset] = (BYTE)value;
    bytes[offset + 1] = (BYTE)(value >> 8);
}

static BOOL terminated_string_length(const BYTE *bytes, DWORD length,
                                     WORD offset, DWORD *string_length)
{
    DWORD cursor;

    if (offset >= length) return FALSE;
    for (cursor = offset; cursor < length; ++cursor) {
        if (!bytes[cursor]) {
            *string_length = cursor - offset;
            return TRUE;
        }
    }
    return FALSE;
}

/* Program Manager 3.x uses PMCC group files.  Its 0x22-byte container is
 * followed by a group header: item count at 0x20 and a WORD item-offset table
 * at 0x22.  Each verified ITEMDATA record carries its executable path pointer
 * at byte +22.  Appending replacement strings preserves every existing record
 * offset and resource payload; only that pointer, the group byte count, and
 * the documented 16-bit checksum change. */
static BOOL repair_group_file(const wchar_t *root, const wchar_t *path,
                              DWORD *changed)
{
    BYTE *input = NULL;
    BYTE *output = NULL;
    DWORD input_length;
    DWORD item_count;
    DWORD additions = 0;
    DWORD cursor;
    DWORD i;
    wchar_t temporary[MAX_PATH];

    *changed = 0;
    if (!read_text(path, (char **)&input, &input_length)) return TRUE;
    if (input_length < 0x22 || memcmp(input, "PMCC", 4) ||
        (item_count = read_u16(input, 0x20)) > 255 ||
        0x22u + item_count * 2u > input_length) goto fail;

    for (i = 0; i < item_count; ++i) {
        WORD item = read_u16(input, 0x22 + i * 2u);
        WORD executable;
        DWORD string_length;
        if ((DWORD)item + 24u > input_length) goto fail;
        executable = read_u16(input, (DWORD)item + 22u);
        if (!terminated_string_length(input, input_length, executable, &string_length)) goto fail;
        if (string_length >= 3 && isalpha(input[executable]) && input[executable + 1] == ':') {
            const char *suffix = strrchr((const char *)input + executable, '\\');
            wchar_t ignored[MAX_PATH];
            int wide;
            int root_bytes = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS,
                                                  root, -1, NULL, 0, NULL, NULL);
            int suffix_bytes;
            if (!suffix || !existing_relative_path(root, suffix)) continue;
            wide = MultiByteToWideChar(CP_ACP, 0, suffix, -1, ignored, MAX_PATH);
            if (!wide || wide > MAX_PATH || !root_bytes) goto fail;
            suffix_bytes = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS,
                                                ignored, -1, NULL, 0, NULL, NULL);
            if (!suffix_bytes) goto fail;
            additions += (DWORD)root_bytes - 1u + (DWORD)suffix_bytes;
        }
    }
    if (!additions) {
        HeapFree(GetProcessHeap(), 0, input);
        return TRUE;
    }
    if (input_length + additions > 0xffffu ||
        read_u16(input, 6) + additions > 0xffffu) goto fail;
    output = (BYTE *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                               (SIZE_T)input_length + additions);
    if (!output) goto fail;
    CopyMemory(output, input, input_length);
    cursor = input_length;

    for (i = 0; i < item_count; ++i) {
        WORD item = read_u16(input, 0x22 + i * 2u);
        WORD executable = read_u16(input, (DWORD)item + 22u);
        DWORD string_length;
        wchar_t suffix[MAX_PATH];
        int wide;
        int written;
        DWORD replacement;

        if (!terminated_string_length(input, input_length, executable, &string_length)) goto fail;
        if (string_length < 3 || !isalpha(input[executable]) ||
            input[executable + 1] != ':') continue;
        {
            const char *path_suffix = strrchr((const char *)input + executable, '\\');
            if (!path_suffix || !existing_relative_path(root, path_suffix)) continue;
            wide = MultiByteToWideChar(CP_ACP, 0, path_suffix, -1, suffix, MAX_PATH);
        }
        if (!wide || wide > MAX_PATH) goto fail;
        replacement = cursor;
        written = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, root, -1,
                                      (char *)output + cursor,
                                      input_length + additions - cursor, NULL, NULL);
        if (!written || cursor + (DWORD)written - 1u >= input_length + additions) goto fail;
        cursor += (DWORD)written;
        --cursor;
        written = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, suffix, -1,
                                      (char *)output + cursor,
                                      input_length + additions - cursor, NULL, NULL);
        if (!written) goto fail;
        cursor += (DWORD)written;
        write_u16(output, (DWORD)item + 22u, (WORD)replacement);
        ++*changed;
    }
    if (cursor != input_length + additions) {
        goto fail;
    }

    write_u16(output, 6, (WORD)(read_u16(input, 6) + additions));
    write_u16(output, 4, 0);
    {
        WORD checksum = 0;
        for (i = 0; i + 1u < cursor; i += 2u) checksum = (WORD)(checksum + read_u16(output, i));
        write_u16(output, 4, (WORD)(0u - checksum));
    }
    if (swprintf_s(temporary, MAX_PATH, L"%ls.T438", path) < 0 ||
        !write_file(temporary, (const char *)output, cursor) ||
        !backup_original(path) ||
        !MoveFileExW(temporary, path, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        DeleteFileW(temporary);
        goto fail;
    }
    HeapFree(GetProcessHeap(), 0, output);
    HeapFree(GetProcessHeap(), 0, input);
    return TRUE;

fail:
    fwprintf(stderr, L"WIN31PATH: could not safely repair Program Manager group %ls\n", path);
    if (output) HeapFree(GetProcessHeap(), 0, output);
    if (input) HeapFree(GetProcessHeap(), 0, input);
    return FALSE;
}

static BOOL validate_group_file(const wchar_t *root, const wchar_t *path)
{
    BYTE *input = NULL;
    DWORD input_length;
    DWORD item_count;
    DWORD additions = 0;
    DWORD i;
    BOOL valid = FALSE;

    if (!read_text(path, (char **)&input, &input_length) || input_length < 0x22 ||
        memcmp(input, "PMCC", 4) || (item_count = read_u16(input, 0x20)) > 255 ||
        0x22u + item_count * 2u > input_length) goto done;
    for (i = 0; i < item_count; ++i) {
        WORD item = read_u16(input, 0x22 + i * 2u);
        WORD executable;
        DWORD string_length;
        const char *suffix;
        wchar_t ignored[MAX_PATH];
        int wide;
        int root_bytes;
        int suffix_bytes;

        if ((DWORD)item + 24u > input_length) goto done;
        executable = read_u16(input, (DWORD)item + 22u);
        if (!terminated_string_length(input, input_length, executable, &string_length)) goto done;
        if (string_length < 3 || !isalpha(input[executable]) || input[executable + 1] != ':') continue;
        suffix = strrchr((const char *)input + executable, '\\');
        if (!suffix || !existing_relative_path(root, suffix)) continue;
        wide = MultiByteToWideChar(CP_ACP, 0, suffix, -1, ignored, MAX_PATH);
        root_bytes = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS,
                                         root, -1, NULL, 0, NULL, NULL);
        suffix_bytes = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS,
                                           ignored, -1, NULL, 0, NULL, NULL);
        if (!wide || wide > MAX_PATH || !root_bytes || !suffix_bytes) goto done;
        additions += (DWORD)root_bytes - 1u + (DWORD)suffix_bytes;
    }
    valid = input_length + additions <= 0xffffu &&
            read_u16(input, 6) + additions <= 0xffffu;
done:
    if (input) HeapFree(GetProcessHeap(), 0, input);
    return valid;
}

static BOOL validate_program_groups(const wchar_t *root)
{
    WIN32_FIND_DATAW entry;
    HANDLE search;
    wchar_t pattern[MAX_PATH];
    wchar_t path[MAX_PATH];
    BOOL valid = TRUE;

    if (!join_path(pattern, MAX_PATH, root, L"*.GRP")) return FALSE;
    search = FindFirstFileW(pattern, &entry);
    if (search == INVALID_HANDLE_VALUE) return GetLastError() == ERROR_FILE_NOT_FOUND;
    do {
        if (entry.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) continue;
        if (!join_path(path, MAX_PATH, root, entry.cFileName) ||
            !validate_group_file(root, path)) {
            valid = FALSE;
            break;
        }
    } while (FindNextFileW(search, &entry));
    if (valid && GetLastError() != ERROR_NO_MORE_FILES) valid = FALSE;
    FindClose(search);
    return valid;
}

int wmain(int argc, wchar_t **argv)
{
    static const wchar_t *const names[] = {
        L"PROGMAN.INI", L"WIN.INI", L"SYSTEM.INI", L"CONTROL.INI",
        L"DOSAPP.INI", L"WINFILE.INI"
    };
    wchar_t root[MAX_PATH];
    wchar_t path[MAX_PATH];
    wchar_t patch[MAX_PATH];
    wchar_t report[MAX_PATH];
    char old_root[MAX_PATH];
    char new_root[MAX_PATH];
    DWORD total = 0;
    FILE *log;
    DWORD i;

    if (argc != 3 || _wcsicmp(argv[1], L"--root") ||
        !GetFullPathNameW(argv[2], MAX_PATH, root, NULL)) {
        fwprintf(stderr, L"Usage: WIN31PATH.EXE --root <installed-win31>\n");
        return 64;
    }
    if (GetFileAttributesW(root) == INVALID_FILE_ATTRIBUTES ||
        (GetFileAttributesW(root) & FILE_ATTRIBUTE_REPARSE_POINT) ||
        !join_path(path, MAX_PATH, root, L"WIN.COM") ||
        !ordinary_file(path)) return 2;
    if (!WideCharToMultiByte(CP_ACP, 0, root, -1, new_root, MAX_PATH, NULL, NULL) ||
        !discover_old_root(root, old_root)) {
        fwprintf(stderr, L"WIN31PATH: no single proven old root in PROGMAN.INI; no changes made\n");
        return 3;
    }
    if (!_stricmp(old_root, new_root)) {
        wprintf(L"WIN31PATH: paths already match %ls\n", root);
        return 0;
    }
    if (!validate_program_groups(root)) {
        fwprintf(stderr, L"WIN31PATH: unsupported Program Manager group; no changes made\n");
        return 3;
    }
    if (!join_path(patch, MAX_PATH, root, L"PATCH") ||
        (!CreateDirectoryW(patch, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) ||
        !join_path(report, MAX_PATH, patch, L"PATH-REPAIR.TXT") ||
        _wfopen_s(&log, report, L"wb") || !log) return 4;

    fprintf(log, "old=%s\r\nnew=%s\r\n", old_root, new_root);
    for (i = 0; i < ARRAYSIZE(names); ++i) {
        DWORD changed;
        if (!join_path(path, MAX_PATH, root, names[i]) ||
            !replace_references(root, path, old_root, new_root, &changed)) {
            fclose(log);
            return 5;
        }
        if (changed) fprintf(log, "%ls: %lu replacement(s)\r\n", names[i], changed);
        total += changed;
    }
    {
        WIN32_FIND_DATAW entry;
        HANDLE search;

        if (!join_path(path, MAX_PATH, root, L"*.GRP")) {
            fclose(log);
            return 5;
        }
        search = FindFirstFileW(path, &entry);
        if (search != INVALID_HANDLE_VALUE) {
            do {
                DWORD changed;
                if (entry.dwFileAttributes &
                    (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) continue;
                if (!join_path(path, MAX_PATH, root, entry.cFileName) ||
                    !repair_group_file(root, path, &changed)) {
                    FindClose(search);
                    fclose(log);
                    return 5;
                }
                if (changed) {
                    fprintf(log, "%ls: %lu program-target replacement(s)\r\n",
                            entry.cFileName, changed);
                }
                total += changed;
            } while (FindNextFileW(search, &entry));
            if (GetLastError() != ERROR_NO_MORE_FILES) {
                FindClose(search);
                fclose(log);
                return 5;
            }
            FindClose(search);
        } else if (GetLastError() != ERROR_FILE_NOT_FOUND) {
            fclose(log);
            return 5;
        }
    }
    fprintf(log, "total=%lu\r\n", total);
    fclose(log);
    wprintf(L"WIN31PATH: repaired %lu textual reference(s)\n", total);
    return 0;
}
