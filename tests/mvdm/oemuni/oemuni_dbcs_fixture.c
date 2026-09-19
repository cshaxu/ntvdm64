#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <oemuni.h>
#include <stdio.h>
#include <string.h>

/* Test-only CP932 boundary: never changes the host/system code page. */
static const WCHAR full_path[] = L"C:\\\x8868\\x.txt";
static int path_mode;
static ULONG NTAPI fixed_path(PCWSTR input, ULONG bytes, PWSTR output, PWSTR *part)
{
    ULONG size = sizeof(full_path);
    (void)input;
    if (path_mode == 1) return 0;
    if (path_mode == 2) return (MAX_PATH + 1) * sizeof(WCHAR);
    if (bytes < size) return size;
    memcpy(output, full_path, size);
    if (part) *part = path_mode == 3 ? NULL : output + 5;
    return size - sizeof(WCHAR);
}

static NTSTATUS NTAPI cp932_oem(POEM_STRING dst, PUNICODE_STRING src, BOOLEAN allocate)
{
    int size = WideCharToMultiByte(932, 0, src->Buffer, src->Length / 2,
        NULL, 0, NULL, NULL);
    if (!size) return (NTSTATUS)0xc000000d;
    if (allocate) {
        dst->Buffer = RtlAllocateHeap(RtlProcessHeap(), 0, size + 1);
        if (!dst->Buffer) return (NTSTATUS)0xc0000017;
        dst->MaximumLength = (USHORT)(size + 1);
    }
    if (dst->MaximumLength <= size) return (NTSTATUS)0x80000005;
    WideCharToMultiByte(932, 0, src->Buffer, src->Length / 2,
        dst->Buffer, size, NULL, NULL);
    dst->Buffer[size] = 0;
    dst->Length = (USHORT)size;
    return 0;
}

static ULONG NTAPI cp932_size(PCUNICODE_STRING src)
{
    if (!src->Length) return 1;
    return WideCharToMultiByte(932, 0, src->Buffer, src->Length / 2,
        NULL, 0, NULL, NULL) + 1;
}

static ULONG NTAPI fixed_current(ULONG bytes, PWSTR output)
{
    return fixed_path(L"", bytes, output, NULL);
}

static DWORD WINAPI fixed_search(LPCWSTR path, LPCWSTR name, LPCWSTR extension,
    DWORD chars, LPWSTR output, LPWSTR *part)
{
    (void)path; (void)name; (void)extension;
    return fixed_path(L"", chars * sizeof(WCHAR), output, part) / sizeof(WCHAR);
}

static UINT WINAPI fixed_directory(LPWSTR output, UINT chars)
{
    return fixed_path(L"", chars * sizeof(WCHAR), output, NULL) / sizeof(WCHAR);
}

static DWORD WINAPI fixed_temp(DWORD chars, LPWSTR output)
{
    return fixed_directory(output, chars);
}

#define RtlGetFullPathName_U fixed_path
#define RtlUnicodeStringToOemString cp932_oem
#define RtlUnicodeStringToOemSize cp932_size
#define RtlGetCurrentDirectory_U fixed_current
#define SearchPathW fixed_search
#define GetSystemDirectoryW fixed_directory
#define GetWindowsDirectoryW fixed_directory
#define GetTempPathW fixed_temp
#include "../../../src/mvdm/oemuni/file.c"
#include "../../../src/mvdm/oemuni/process.c"

int main(void)
{
    CHAR output[64], expected[64], *part = NULL;
    DWORD actual;
    int size = WideCharToMultiByte(932, 0, full_path, -1,
        expected, sizeof(expected), NULL, NULL);
    if (size != 12) return 2;
    memset(output, 0x5a, sizeof(output));
    actual = GetFullPathNameOem("x.txt", sizeof(output), output, &part);
    printf("S37_DBCS_FULLPATH actual_length=%lu expected_length=%d part=%ld expected_part=6 terminator=%u\n",
        actual, size - 1, part ? (long)(part - output) : -1,
        (unsigned char)output[size-1]);
    if (actual != 11 || part != output + 6 ||
        memcmp(output, expected, size)) return 3;
    memset(output, 0x5a, sizeof(output));
    actual = GetFullPathNameOem("x.txt", 11, output, NULL);
    if (actual != 12 || output[0] != 0x5a || output[11] != 0x5a) return 4;
    actual = GetFullPathNameOem("x.txt", 12, output, NULL);
    if (actual != 11 || memcmp(output, expected, size) || output[12] != 0x5a) return 5;
    for (path_mode = 1; path_mode <= 2; ++path_mode) {
        memset(output, 0x5a, sizeof(output));
        if (GetFullPathNameOem("x.txt", sizeof(output), output, NULL) ||
            output[0] != 0x5a) return 6;
    }
    path_mode = 3;
    part = output;
    if (GetFullPathNameOem("x.txt", sizeof(output), output, &part) != 11 || part) return 7;
    puts("S37_DBCS_FULLPATH_LENGTH_COPY_FILEPART_SHORT_EXACT_BUFFER_OK");
    path_mode = 0;
    memset(output, 0x5a, sizeof(output));
    actual = GetCurrentDirectoryOem(11, output);
    printf("S37_DBCS_CURRENT_DEFECT length=%lu capacity=11 byte_after=%u\n",
        actual, (unsigned char)output[11]);
    /* Diagnostic assertions: original code advertises n+1 to the converter
       and writes the NUL beyond the caller's declared capacity. */
    if (actual != 11 || output[11] != 0) return 8;
    memset(output, 0x5a, sizeof(output));
    actual = SearchPathOem(NULL, "x.txt", NULL, 11, output, &part);
    printf("S37_DBCS_SEARCH_DEFECT length=%lu expected=11 part=%ld expected_part=6 byte_after=%u\n",
        actual, (long)(part-output), (unsigned char)output[11]);
    if (actual != 10 || part != output+5 || output[11] != 0) return 9;
    puts("S37_DBCS_CURRENT_SEARCH_CAPACITY_DEFECTS_REPRODUCED_NOT_ACCEPTED");
    memset(output, 0x5a, sizeof(output));
    actual = GetSystemDirectoryOem(output, 11);
    if (actual != 11 || output[11] != 0) return 10;
    memset(output, 0x5a, sizeof(output));
    actual = GetWindowsDirectoryOem(output, 11);
    if (actual != 11 || output[11] != 0) return 11;
    memset(output, 0x5a, sizeof(output));
    actual = GetTempPathOem(11, output);
    if (actual != 11 || output[11] != 0) return 12;
    puts("S37_DBCS_SYSTEM_WINDOWS_TEMP_CAPACITY_DEFECTS_REPRODUCED_NOT_ACCEPTED");
    return 0;
}
