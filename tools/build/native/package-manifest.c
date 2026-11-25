#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <objbase.h>
#include <stdio.h>
#include <string.h>

/* Fixed product-package schema, not a general JSON command processor.
 * No output is emitted until the entire manifest has been validated. */
static void space(const char **p)
{
    while (**p == ' ' || **p == '\t' || **p == '\r' || **p == '\n') ++*p;
}
static int take(const char **p, char value)
{
    space(p);
    if (**p != value) return 0;
    ++*p;
    return 1;
}
static int hex(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}
static int text(const char **p, char *out, size_t size)
{
    size_t count = 0;
    if (!take(p, '"')) return 0;
    while (**p && **p != '"') {
        unsigned char c = (unsigned char)*(*p)++;
        if (c == '\\') {
            c = (unsigned char)*(*p)++;
            if (c == 'u') {
                unsigned int value = 0, i;
                for (i = 0; i < 4; ++i) {
                    int digit = hex(**p);
                    if (digit < 0) return 0;
                    ++*p;
                    value = value * 16 + (unsigned int)digit;
                }
                if (value < 32 || value > 126) return 0;
                c = (unsigned char)value;
            } else if (c != '\\' && c != '/' && c != '"') return 0;
        }
        if (c < 32 || c > 126 || count + 1 >= size) return 0;
        out[count++] = (char)c;
    }
    if (!take(p, '"')) return 0;
    out[count] = 0;
    return 1;
}
static int digest(const char *s)
{
    size_t i;
    if (strlen(s) != 64) return 0;
    for (i = 0; i < 64; ++i) if (hex(s[i]) < 0) return 0;
    return 1;
}
static int uuid(const char *s)
{
    size_t i;
    int nonzero = 0;
    if (strlen(s) != 36) return 0;
    for (i = 0; i < 36; ++i) {
        if (i == 8 || i == 13 || i == 18 || i == 23) {
            if (s[i] != '-') return 0;
        } else {
            int digit = hex(s[i]);
            if (digit < 0) return 0;
            nonzero |= digit;
        }
    }
    return nonzero != 0;
}
DWORD package_manifest_rows(const wchar_t *path)
{
    static const char *names[] = {"run16.exe", "ntsrv.exe", "ntcon.exe",
        "ntvdm.exe", "ntvwm.exe", "ntmon.exe", "WOW32.DLL", "VDMREDIR.DLL",
        "nthook32.dll", "nthook64.dll", "VCDEX.DLL", "NTVDMD.DLL", "VJOY.DLL"};
    static const char *keys[] = {"Name", "RelativePath", "Sha256", "Machine",
        "PackageId", "InputSha256"};
    char input[16385], rows[ARRAYSIZE(names)][6][128], key[32], expected[128];
    HANDLE file;
    LARGE_INTEGER length;
    DWORD size, error = ERROR_INVALID_DATA;
    const char *p;
    unsigned int count = 0, seen = 0, i, j;
    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return GetLastError();
    if (!GetFileSizeEx(file, &length) || length.QuadPart < 2 ||
        length.QuadPart >= (LONGLONG)sizeof(input) ||
        !ReadFile(file, input, (DWORD)length.QuadPart, &size, NULL) ||
        size != (DWORD)length.QuadPart) goto close;
    input[size] = 0;
    if (memchr(input, 0, size)) goto close;
    p = input;
    if (size >= 3 && !memcmp(p, "\xEF\xBB\xBF", 3)) p += 3;
    if (!take(&p, '[')) goto close;
    do {
        unsigned int fields = 0;
        if (count == ARRAYSIZE(names) || !take(&p, '{')) goto close;
        do {
            if (!text(&p, key, sizeof(key)) || !take(&p, ':')) goto close;
            for (j = 0; j < 6 && strcmp(key, keys[j]); ++j) {}
            if (j == 6 || (fields & (1u << j))) goto close;
            fields |= 1u << j;
            if (j == 3) {
                size_t n = 0;
                space(&p);
                while (*p >= '0' && *p <= '9') {
                    if (n + 1 >= sizeof(rows[0][0])) goto close;
                    rows[count][j][n++] = *p++;
                }
                rows[count][j][n] = 0;
                if (strcmp(rows[count][j], "332") && strcmp(rows[count][j], "34404")) goto close;
            } else if (!text(&p, rows[count][j], sizeof(rows[0][0]))) goto close;
        } while (take(&p, ','));
        if (fields != 63 || !take(&p, '}')) goto close;
        for (i = 0; i < ARRAYSIZE(names) && _stricmp(rows[count][0], names[i]); ++i) {}
        if (i == ARRAYSIZE(names) || (seen & (1u << i))) goto close;
        seen |= 1u << i;
        if (sprintf_s(expected, sizeof(expected), "system32\\%s", rows[count][0]) < 0 ||
            _stricmp(expected, rows[count][1]) || !digest(rows[count][2]) ||
            !digest(rows[count][5]) || !uuid(rows[count][4])) goto close;
        if (count && _stricmp(rows[0][4], rows[count][4])) goto close;
        ++count;
    } while (take(&p, ','));
    if (!take(&p, ']')) goto close;
    space(&p);
    if (*p || (seen != 1023 && seen != 2047 && seen != 4095 && seen != ((1u << ARRAYSIZE(names)) - 1u))) goto close;
    for (i = 0; i < count; ++i)
        printf("%s %s %s %s %s\n", rows[i][0], rows[i][3], rows[i][4], rows[i][2], rows[i][5]);
    error = ferror(stdout) ? ERROR_WRITE_FAULT : ERROR_SUCCESS;
close:
    CloseHandle(file);
    return error;
}
