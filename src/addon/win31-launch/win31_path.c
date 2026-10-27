/* WIN31PATH.EXE: bounded root discovery for tools/win31-path/APPLY.CMD.
 * It does not rewrite settings, PIFs, or Program Manager groups. */
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

static BOOL ordinary_file(const wchar_t *path)
{
    DWORD attributes = GetFileAttributesW(path);
    return attributes != INVALID_FILE_ATTRIBUTES &&
           !(attributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT));
}

static BOOL read_text(const wchar_t *path, char **out)
{
    HANDLE file;
    DWORD length, read = 0;

    *out = NULL;
    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return FALSE;
    length = GetFileSize(file, NULL);
    if (length == INVALID_FILE_SIZE || length > 1024u * 1024u ||
        !(*out = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (SIZE_T)length + 1)) ||
        !ReadFile(file, *out, length, &read, NULL) || read != length) {
        CloseHandle(file);
        if (*out) HeapFree(GetProcessHeap(), 0, *out);
        *out = NULL;
        return FALSE;
    }
    CloseHandle(file);
    return TRUE;
}

static BOOL existing_relative_path(const wchar_t *root, const char *suffix)
{
    char ansi[MAX_PATH];
    wchar_t relative[MAX_PATH], candidate[MAX_PATH];
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
    char *contents = NULL, *line;
    BOOL found = FALSE;

    if (!join_path(program_manager, MAX_PATH, root, L"PROGMAN.INI") ||
        !ordinary_file(program_manager) || !read_text(program_manager, &contents)) return FALSE;
    for (line = contents; line && *line;) {
        char *line_end = strpbrk(line, "\r\n");
        char *equals, *slash;
        char saved = 0;
        if (line_end) { saved = *line_end; *line_end = '\0'; }
        equals = strchr(line, '=');
        if (equals && isalpha((unsigned char)equals[1]) && equals[2] == ':' &&
            (slash = strrchr(equals + 1, '\\')) && existing_relative_path(root, slash)) {
            *slash = '\0';
            if (!found) { strncpy_s(old_root, MAX_PATH, equals + 1, _TRUNCATE); found = TRUE; }
            else if (_stricmp(old_root, equals + 1)) {
                *slash = '\\'; if (line_end) *line_end = saved;
                HeapFree(GetProcessHeap(), 0, contents); return FALSE;
            }
            *slash = '\\';
        }
        if (!line_end) break;
        *line_end = saved;
        line = line_end + 1;
        if (saved == '\r' && *line == '\n') ++line;
    }
    HeapFree(GetProcessHeap(), 0, contents);
    return found;
}

int wmain(int argc, wchar_t **argv)
{
    wchar_t root[MAX_PATH];
    char old_root[MAX_PATH], new_root[MAX_PATH];

    if (argc != 3 || _wcsicmp(argv[1], L"--discover-root") ||
        !GetFullPathNameW(argv[2], MAX_PATH, root, NULL)) {
        fwprintf(stderr, L"Usage: WIN31PATH.EXE --discover-root <installed-win31>\n");
        return 64;
    }
    if (GetFileAttributesW(root) == INVALID_FILE_ATTRIBUTES ||
        (GetFileAttributesW(root) & FILE_ATTRIBUTE_REPARSE_POINT)) return 2;
    {
        wchar_t wincom[MAX_PATH];
        if (!join_path(wincom, MAX_PATH, root, L"WIN.COM") || !ordinary_file(wincom)) return 2;
    }
    if (!WideCharToMultiByte(CP_ACP, 0, root, -1, new_root, MAX_PATH, NULL, NULL) ||
        !discover_old_root(root, old_root)) {
        fwprintf(stderr, L"WIN31PATH: no single proven old root in PROGMAN.INI\n");
        return 3;
    }
    printf("OLD_ROOT=%s\nNEW_ROOT=%s\n", old_root, new_root);
    return 0;
}
