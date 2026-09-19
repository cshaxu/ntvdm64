#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <oemuni.h>
#include <stdio.h>
#include <string.h>

static int fail_stage;
static int borrowed_frees;
static CHAR output[64];
static NTSTATUS NTAPI fail_ansi(PUNICODE_STRING dst, PANSI_STRING src, BOOLEAN allocate)
{
    if (fail_stage == 1) { dst->Buffer = NULL; return (NTSTATUS)0xc0000017; }
    return RtlAnsiStringToUnicodeString(dst, src, allocate);
}
static NTSTATUS NTAPI fail_oem(POEM_STRING dst, PUNICODE_STRING src, BOOLEAN allocate)
{
    if (fail_stage == 2) { dst->Buffer = NULL; return (NTSTATUS)0xc0000017; }
    return RtlUnicodeStringToOemString(dst, src, allocate);
}
static VOID NTAPI checked_free(PANSI_STRING string)
{
    if (string->Buffer == output) { ++borrowed_frees; return; }
    RtlFreeAnsiString(string);
}

/* Compile the actual original-owner function with only two failed conversion
 * endpoints and a checked free. None of these hooks enters the product. */
#define RtlAnsiStringToUnicodeString fail_ansi
#define RtlUnicodeStringToOemString fail_oem
#define RtlFreeAnsiString checked_free
#include "../../../src/mvdm/oemuni/process.c"

int main(void)
{
    DWORD result;
    for (fail_stage = 1; fail_stage <= 2; ++fail_stage) {
        memset(output, 0x5a, sizeof(output));
        borrowed_frees = 0;
        SetLastError(0);
        result = ExpandEnvironmentStringsOem("abc", output, sizeof(output));
        printf("S37_EXPAND_FAILURE stage=%d result=%lu error=%lu borrowed_frees=%d\n",
            fail_stage, result, GetLastError(), borrowed_frees);
        if (result || GetLastError() != ERROR_NOT_ENOUGH_MEMORY || borrowed_frees ||
            (unsigned char)output[sizeof(output)-1] != 0x5a) return fail_stage;
    }
    puts("S37_EXPAND_CONVERSION_FAILURE_ZERO_AND_SAFE_CLEANUP_OK");
    return 0;
}
