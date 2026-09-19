#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <oemuni.h>
#include <stdio.h>
#include <string.h>

/* Test-only CP932 boundary: never changes the host/system code page. */
static const WCHAR full_path[] = L"C:\\\x8868\\x.txt";
static ULONG NTAPI fixed_path(PCWSTR input, ULONG bytes, PWSTR output, PWSTR *part)
{
    ULONG size = sizeof(full_path);
    (void)input;
    if (bytes < size) return size;
    memcpy(output, full_path, size);
    if (part) *part = output + 5;
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

#define RtlGetFullPathName_U fixed_path
#define RtlUnicodeStringToOemString cp932_oem
#include "../../../src/mvdm/oemuni/file.c"

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
    /* Baseline witness, not a capability PASS: lock the three observed
       violations until a reviewed original-owner correction is admitted. */
    if (actual != 10 || part != output + 5 ||
        (unsigned char)output[11] != 0x5a ||
        memcmp(output, expected, 11)) return 3;
    puts("S37_DBCS_ORIGINAL_LENGTH_COPY_FILEPART_DEFECT_REPRODUCED");
    return 0;
}
