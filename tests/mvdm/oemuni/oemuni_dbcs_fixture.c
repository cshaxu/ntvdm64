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
    if (path_mode == 4 && bytes) return size + sizeof(WCHAR);
    if (path_mode == 5 && bytes) { SetLastError(ERROR_NOT_READY); return 0; }
    if (path_mode == 6) return 32768 * sizeof(WCHAR);
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
static DWORD WINAPI fixed_short(LPCWSTR input, LPWSTR output, DWORD chars)
{
    return fixed_path(input, chars*sizeof(WCHAR), output, NULL)/sizeof(WCHAR);
}
static BOOL WINAPI fixed_computer(LPWSTR output, LPDWORD chars)
{
    if (*chars < 2) { *chars = 2; SetLastError(ERROR_BUFFER_OVERFLOW); return FALSE; }
    output[0] = 0x8868; output[1] = 0; *chars = 1; return TRUE;
}

static int volume_failure, outstanding, fail_allocation;
static PVOID NTAPI counted_alloc(PVOID heap, ULONG flags, SIZE_T size)
{
    PVOID p;
    if (fail_allocation) return NULL;
    p = RtlAllocateHeap(heap, flags, size);
    if (p) ++outstanding;
    return p;
}
static BOOLEAN NTAPI counted_free(PVOID heap, ULONG flags, PVOID p)
{
    BOOLEAN result = RtlFreeHeap(heap, flags, p);
    if (p && result) --outstanding;
    return result;
}
static BOOL WINAPI fixed_volume(LPCWSTR root, LPWSTR volume, DWORD volume_size,
    LPDWORD serial, LPDWORD component, LPDWORD flags, LPWSTR fs, DWORD fs_size)
{
    (void)root; (void)serial; (void)component; (void)flags;
    if (volume_failure) { SetLastError(ERROR_NOT_READY); return FALSE; }
    if ((volume && volume_size < 2) || (fs && fs_size < 2)) {
        SetLastError(ERROR_MORE_DATA); return FALSE;
    }
    if (volume) { volume[0] = 0x8868; volume[1] = 0; }
    if (fs) { fs[0] = 0x8868; fs[1] = 0; }
    return TRUE;
}

#define RtlGetFullPathName_U fixed_path
#define RtlUnicodeStringToOemString cp932_oem
#define RtlUnicodeStringToOemSize cp932_size
#define RtlGetCurrentDirectory_U fixed_current
#define SearchPathW fixed_search
#define GetSystemDirectoryW fixed_directory
#define GetWindowsDirectoryW fixed_directory
#define GetTempPathW fixed_temp
#define GetShortPathNameW fixed_short
#define GetComputerNameW fixed_computer
#define GetVolumeInformationW fixed_volume
#define RtlAllocateHeap counted_alloc
#define RtlFreeHeap counted_free
#include "../../../src/mvdm/oemuni/file.c"
#include "../../../src/mvdm/oemuni/process.c"

int main(void)
{
    CHAR output[65536], expected[64], *part = NULL;
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
    if (actual != 12 || output[0] != 0x5a || output[11] != 0x5a) return 8;
    if (GetCurrentDirectoryOem(12, output) != 11 || memcmp(output, expected, 12)) return 13;
    memset(output, 0x5a, sizeof(output));
    actual = SearchPathOem(NULL, "x.txt", NULL, 11, output, &part);
    if (actual != 12 || output[0] != 0x5a || output[11] != 0x5a) return 9;
    if (SearchPathOem(NULL, "x.txt", NULL, 12, output, &part) != 11 ||
        part != output+6 || memcmp(output, expected, 12)) return 14;
    puts("S37_DBCS_CURRENT_SEARCH_CAPACITY_OK");
    memset(output, 0x5a, sizeof(output));
    actual = GetSystemDirectoryOem(output, 11);
    if (actual != 12 || output[0] != 0x5a || output[11] != 0x5a) return 10;
    if (GetSystemDirectoryOem(output, 12) != 11 || memcmp(output, expected, 12)) return 15;
    memset(output, 0x5a, sizeof(output));
    actual = GetWindowsDirectoryOem(output, 11);
    if (actual != 12 || output[0] != 0x5a || output[11] != 0x5a) return 11;
    if (GetWindowsDirectoryOem(output, 12) != 11 || memcmp(output, expected, 12)) return 16;
    memset(output, 0x5a, sizeof(output));
    actual = GetTempPathOem(11, output);
    if (actual != 12 || output[0] != 0x5a || output[11] != 0x5a) return 12;
    if (GetTempPathOem(12, output) != 11 || memcmp(output, expected, 12)) return 17;
    puts("S37_DBCS_SYSTEM_WINDOWS_TEMP_CAPACITY_OK");
    {
        CHAR volume[8], fs[8];
        int which;
        for (which = 0; which < 2; ++which) {
            memset(volume, 0x5a, sizeof(volume));
            memset(fs, 0x5a, sizeof(fs));
            SetLastError(0);
            actual = GetVolumeInformationOem("C:\\", volume, which ? 3 : 2,
                NULL, NULL, NULL, fs, which ? 2 : 3);
            printf("S37_VOLUME_BOUNDARY side=%d result=%lu error=%lu canary=%u outstanding=%d\n",
                which, actual, GetLastError(), (unsigned char)(which ? fs[2] : volume[2]), outstanding);
            if (actual || GetLastError() != ERROR_MORE_DATA ||
                (which ? fs[2] : volume[2]) != 0x5a || outstanding) return 18;
        }
        if (!GetVolumeInformationOem("C:\\", volume, 3, NULL, NULL, NULL, fs, 3) ||
            (unsigned char)volume[0] != 0x95 || (unsigned char)volume[1] != 0x5c ||
            volume[2] || memcmp(volume, fs, 3) || outstanding) return 19;
        if (!GetVolumeInformationOem("C:\\", NULL, 0, NULL, NULL, NULL, NULL, 0) || outstanding) return 20;
        volume_failure = 1;
        if (GetVolumeInformationOem("C:\\", volume, 3, NULL, NULL, NULL, fs, 3) ||
            GetLastError() != ERROR_NOT_READY || outstanding) return 21;
    }
    puts("S37_DBCS_VOLUME_BOTH_CAPACITIES_OPTIONAL_FAILURE_CLEANUP_OK");
    {
        static const DWORD capacities[] = {0, 1, 10, 11, 12, 65536};
        static const char *names[] = {"current", "system", "windows", "temp", "search"};
        unsigned i, api, mismatches = 0;
        for (api = 0; api < 5; ++api) {
            for (i = 0; i < sizeof(capacities)/sizeof(capacities[0]); ++i) {
                DWORD cap = capacities[i], wanted = cap < 12 ? 12 : 11;
                int valid;
                memset(output, 0x5a, sizeof(output));
                SetLastError(0);
                switch (api) {
                case 0: actual = GetCurrentDirectoryOem(cap, output); break;
                case 1: actual = GetSystemDirectoryOem(output, cap); break;
                case 2: actual = GetWindowsDirectoryOem(output, cap); break;
                case 3: actual = GetTempPathOem(cap, output); break;
                default: actual = SearchPathOem(NULL, "x.txt", NULL, cap, output, NULL); break;
                }
                valid = actual == wanted && (cap < 12 ? output[0] == 0x5a :
                    memcmp(output, expected, 12) == 0);
                if (!valid) ++mismatches;
                printf("S37_SIZE_MATRIX api=%s cap=%lu actual=%lu expected=%lu error=%lu contract=%s\n",
                    names[api], cap, actual, wanted, GetLastError(), valid ? "PASS" : "FAIL");
            }
        }
        if (mismatches) return 22;
        puts("S37_SIZE_MATRIX_ALL_30_PASS");
        for (path_mode = 4; path_mode <= 7; ++path_mode) {
            for (api = 1; api < 5; ++api) {
                DWORD error = path_mode == 5 ? ERROR_NOT_READY :
                    path_mode == 7 ? ERROR_NOT_ENOUGH_MEMORY : ERROR_MORE_DATA;
                fail_allocation = path_mode == 7;
                memset(output, 0x5a, sizeof(output));
                SetLastError(0);
                switch (api) {
                case 1: actual = GetSystemDirectoryOem(output, 12); break;
                case 2: actual = GetWindowsDirectoryOem(output, 12); break;
                case 3: actual = GetTempPathOem(12, output); break;
                default: actual = SearchPathOem(NULL, "x.txt", NULL, 12, output, NULL); break;
                }
                if (actual || GetLastError() != error || output[0] != 0x5a || outstanding) return 23;
            }
        }
        puts("S37_QUERY_GROWTH_FAILURE_OVERSIZE_ALLOCATION_CLEANUP_OK");
        path_mode = 0;
        fail_allocation = 0;
        memset(output, 0x5a, sizeof(output));
        actual = GetShortPathNameOem("x.txt", output, 11);
        printf("S37_SHORTPATH_OPEN_DEFECT result=%lu expected_required=12 boundary=%u\n",
            actual, (unsigned char)output[11]);
        if (actual != 10 || output[11] != 0) return 24;
        memset(output, 0x5a, sizeof(output));
        actual = 2;
        SetLastError(0);
        if (!GetComputerNameOem(output, &actual) || actual != 0 || output[0] != 0x5a) return 25;
        puts("S37_COMPUTER_CONVERSION_FAILURE_FALSE_SUCCESS_REPRODUCED_NOT_ACCEPTED");
    }
    return 0;
}
