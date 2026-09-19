#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <oemuni.h>
#include <stdio.h>
#include <string.h>

static int fail_stage;
static int borrowed_frees;
static int oem_calls;
static CHAR output[64];
static NTSTATUS NTAPI fail_ansi(PUNICODE_STRING dst, PANSI_STRING src, BOOLEAN allocate)
{
    if (fail_stage == 1) { dst->Buffer = NULL; return (NTSTATUS)0xc0000017; }
    return RtlAnsiStringToUnicodeString(dst, src, allocate);
}
static NTSTATUS NTAPI fail_oem(POEM_STRING dst, PUNICODE_STRING src, BOOLEAN allocate)
{
    ++oem_calls;
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
    if (!SetEnvironmentVariableW(L"S37_ENV_FAILURE", L"abc")) return 3;
    for (fail_stage = 1; fail_stage <= 2; ++fail_stage) {
        memset(output, 0x5a, sizeof(output));
        oem_calls = 0;
        borrowed_frees = 0;
        SetLastError(0);
        result = GetEnvironmentVariableOem("S37_ENV_FAILURE", output, sizeof(output));
        printf("S37_ENV_FAILURE stage=%d result=%lu error=%lu oem_calls=%d\n",
            fail_stage, result, GetLastError(), oem_calls);
        if (result || GetLastError() != ERROR_NOT_ENOUGH_MEMORY || borrowed_frees ||
            oem_calls != fail_stage - 1 || output[0] != 0x5a) return 4;
    }
    if (!SetEnvironmentVariableW(L"S37_ENV_FAILURE", NULL)) return 5;
    puts("S37_ENV_CONVERSION_FAILURE_ORDER_OK");
    return 0;
}
