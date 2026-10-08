/*
 * Build-only writer for the Microsoft SZDD single-file format used by the
 * Windows 3.1 retail SETUP media.  This intentionally emits literal blocks
 * only: it does not infer, modify, or optimize guest code.  The resulting
 * stream is larger than a retail-compressed file but is a conforming SZDD
 * source that legacy SETUP expands exactly to the supplied input bytes.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ctype.h>
#include <stdio.h>
#include <wchar.h>

static int fail(const wchar_t *message)
{
    fwprintf(stderr, L"SZDDPACK: %ls (error %lu)\n", message, GetLastError());
    return 1;
}

static BOOL read_input(const wchar_t *path, BYTE **bytes, DWORD *length)
{
    HANDLE file = INVALID_HANDLE_VALUE;
    LARGE_INTEGER size;
    DWORD read = 0;
    BYTE *buffer = NULL;
    BOOL ok = FALSE;

    *bytes = NULL;
    *length = 0;
    file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE || !GetFileSizeEx(file, &size) ||
        size.QuadPart < 0 || size.QuadPart > 0x7fffffffu) goto done;
    buffer = HeapAlloc(GetProcessHeap(), 0, (SIZE_T)size.QuadPart);
    if (size.QuadPart && buffer == NULL) goto done;
    if (size.QuadPart && (!ReadFile(file, buffer, (DWORD)size.QuadPart, &read, NULL) ||
                          read != (DWORD)size.QuadPart)) goto done;
    *bytes = buffer;
    *length = (DWORD)size.QuadPart;
    buffer = NULL;
    ok = TRUE;
done:
    if (buffer) HeapFree(GetProcessHeap(), 0, buffer);
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    return ok;
}

static BOOL write_exact(HANDLE file, const void *bytes, DWORD length)
{
    DWORD written = 0;
    return WriteFile(file, bytes, length, &written, NULL) && written == length;
}

static wchar_t extension_last_character(const wchar_t *path)
{
    const wchar_t *dot = wcsrchr(path, L'.');
    const wchar_t *end = path + wcslen(path);
    if (dot == NULL || dot[1] == L'\0' || end[-1] > 0x7f) return L' ';
    return (wchar_t)tolower((unsigned char)end[-1]);
}

int wmain(int argc, wchar_t **argv)
{
    static const BYTE prefix[] = { 'S', 'Z', 'D', 'D', 0x88, 0xf0, 0x27, 0x33, 'A' };
    BYTE *input = NULL;
    DWORD input_length = 0, offset = 0;
    HANDLE output = INVALID_HANDLE_VALUE;
    BYTE header[14];
    int result = 1;

    if (argc != 3) {
        fwprintf(stderr, L"Usage: SZDDPACK.EXE input.exe output.ex_\n");
        return 64;
    }
    if (!read_input(argv[1], &input, &input_length)) return fail(L"read input");
    output = CreateFileW(argv[2], GENERIC_WRITE, 0, NULL, CREATE_NEW,
                         FILE_ATTRIBUTE_NORMAL, NULL);
    if (output == INVALID_HANDLE_VALUE) goto done;
    CopyMemory(header, prefix, sizeof(prefix));
    header[9] = (BYTE)extension_last_character(argv[1]);
    header[10] = (BYTE)(input_length & 0xffu);
    header[11] = (BYTE)((input_length >> 8) & 0xffu);
    header[12] = (BYTE)((input_length >> 16) & 0xffu);
    header[13] = (BYTE)((input_length >> 24) & 0xffu);
    if (!write_exact(output, header, sizeof(header))) goto done;
    while (offset < input_length) {
        DWORD count = input_length - offset;
        BYTE flags;
        if (count > 8u) count = 8u;
        flags = (BYTE)((1u << count) - 1u); /* low bit first: each is literal */
        if (!write_exact(output, &flags, 1u) || !write_exact(output, input + offset, count))
            goto done;
        offset += count;
    }
    if (!CloseHandle(output)) { output = INVALID_HANDLE_VALUE; goto done; }
    output = INVALID_HANDLE_VALUE;
    result = 0;
done:
    if (output != INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        CloseHandle(output);
        DeleteFileW(argv[2]);
        SetLastError(error);
    }
    if (input) HeapFree(GetProcessHeap(), 0, input);
    if (result) return fail(L"write output");
    return 0;
}
