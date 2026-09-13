#include <windows.h>
#include <nt.h>
#include <ntrtl.h>
#include "opennt-host/base/ntos/rtl/environapi.h"

static int expect(int condition, int code)
{
    return condition ? 0 : code;
}

/* The fixture supplies a minimal private PEB because environ.c consults it
 * for cache identity even when callers pass an explicit environment block.
 * The selected worker provides the real private-PEB ingress. */
POPENNT_SUPPORT_PEB NTAPI NtCurrentPeb(void)
{
    static OPENNT_SUPPORT_PROCESS_PARAMETERS parameters;
    static OPENNT_SUPPORT_PEB peb;

    peb.ProcessParameters = &parameters;
    return &peb;
}

static POPENNT_SUPPORT_PROCESS_PARAMETERS fixture_parameters(void)
{
    return NtCurrentPeb()->ProcessParameters;
}

int main(void)
{
    LARGE_INTEGER dividend;
    LARGE_INTEGER quotient;
    LARGE_INTEGER product;
    ULONG remainder = 0;
    ULONG words[2] = { 0x11111111u, 0x22222222u };
    PVOID environment = NULL;
    PVOID clone = NULL;
    UNICODE_STRING name;
    UNICODE_STRING value;
    WCHAR queryBuffer[16];
    NTSTATUS status;
    int result;

    dividend.HighPart = 1;
    dividend.LowPart = 0;
    quotient = RtlExtendedLargeIntegerDivide(dividend, 3u, &remainder);
    result = expect(quotient.QuadPart == 0x55555555ULL, 1);
    if (result != 0) return result;
    result = expect(remainder == 1u, 2);
    if (result != 0) return result;

    dividend.QuadPart = (LONGLONG)0x8000000000000000ULL;
    quotient = RtlExtendedLargeIntegerDivide(dividend, 3u, &remainder);
    result = expect((ULONGLONG)quotient.QuadPart == 0x2AAAAAAAAAAAAAAAULL,
        3);
    if (result != 0) return result;
    result = expect(remainder == 2u, 4);
    if (result != 0) return result;

    dividend.QuadPart = -3;
    product = RtlExtendedIntegerMultiply(dividend, -7);
    result = expect(product.QuadPart == 21, 5);
    if (result != 0) return result;

    RtlFillMemoryUlong(words, 6u, 0xA1B2C3D4u);
    result = expect(words[0] == 0xA1B2C3D4u, 6);
    if (result != 0) return result;
    result = expect(words[1] == 0x22222222u, 7);
    if (result != 0) return result;

    /* D11: exercise the imported original mutable MULTI_SZ implementation,
     * not the native NTDLL export with the same public spellings. */
    status = RtlCreateEnvironment(FALSE, &environment);
    result = expect(NT_SUCCESS(status) && environment != NULL, 8);
    if (result != 0) return result;
    RtlInitUnicodeString(&name, L"RTL_FIXTURE");
    RtlInitUnicodeString(&value, L"one");
    status = RtlSetEnvironmentVariable(&environment, &name, &value);
    result = expect(NT_SUCCESS(status), 9);
    if (result != 0) return result;
    RtlInitUnicodeString(&value, L"two");
    status = RtlSetEnvironmentVariable(&environment, &name, &value);
    result = expect(NT_SUCCESS(status), 10);
    if (result != 0) return result;
    value.Buffer = queryBuffer;
    value.Length = 0;
    value.MaximumLength = (USHORT)sizeof(queryBuffer);
    status = RtlQueryEnvironmentVariable_U(environment, &name, &value);
    result = expect(NT_SUCCESS(status) && value.Length == 6 &&
                    queryBuffer[0] == L't' && queryBuffer[1] == L'w' &&
                    queryBuffer[2] == L'o', 11);
    if (result != 0) return result;
    status = RtlSetEnvironmentVariable(&environment, &name, NULL);
    result = expect(NT_SUCCESS(status), 12);
    if (result != 0) return result;
    status = RtlQueryEnvironmentVariable_U(environment, &name, &value);
    result = expect(status == STATUS_VARIABLE_NOT_FOUND, 13);
    if (result != 0) return result;

    /* The original validates empty names and embedded '=' before it touches
     * the block.  These are source-owned failure outcomes, not adapter
     * policy. */
    RtlInitUnicodeString(&name, L"");
    status = RtlSetEnvironmentVariable(&environment, &name, &value);
    result = expect(status == STATUS_INVALID_PARAMETER, 14);
    if (result != 0) return result;
    RtlInitUnicodeString(&name, L"RTL=INVALID");
    status = RtlSetEnvironmentVariable(&environment, &name, &value);
    result = expect(status == STATUS_INVALID_PARAMETER, 15);
    if (result != 0) return result;
    RtlInitUnicodeString(&name, L"RTL_FIXTURE");
    status = RtlQueryEnvironmentVariable_U(environment, &name, &value);
    result = expect(status == STATUS_VARIABLE_NOT_FOUND, 16);
    if (result != 0) return result;
    status = RtlDestroyEnvironment(environment);
    result = expect(NT_SUCCESS(status), 17);
    if (result != 0) return result;

    /* Exercise the original TRUE clone path through the same private PEB
     * shape used by the standalone adapter. */
    status = RtlCreateEnvironment(FALSE, &environment);
    result = expect(NT_SUCCESS(status), 18);
    if (result != 0) return result;
    RtlInitUnicodeString(&name, L"RTL_CLONE");
    RtlInitUnicodeString(&value, L"present");
    status = RtlSetEnvironmentVariable(&environment, &name, &value);
    result = expect(NT_SUCCESS(status), 19);
    if (result != 0) return result;
    fixture_parameters()->Environment = environment;
    status = RtlCreateEnvironment(TRUE, &clone);
    result = expect(NT_SUCCESS(status) && clone != NULL && clone != environment, 20);
    if (result != 0) return result;
    value.Buffer = queryBuffer;
    value.Length = 0;
    value.MaximumLength = (USHORT)sizeof(queryBuffer);
    status = RtlQueryEnvironmentVariable_U(clone, &name, &value);
    result = expect(NT_SUCCESS(status) && value.Length == 14, 21);
    if (result != 0) return result;
    status = RtlDestroyEnvironment(clone);
    result = expect(NT_SUCCESS(status), 22);
    if (result != 0) return result;
    status = RtlDestroyEnvironment(environment);
    result = expect(NT_SUCCESS(status), 23);
    if (result != 0) return result;
    fixture_parameters()->Environment = NULL;

    __try {
        (void)RtlExtendedLargeIntegerDivide(dividend, 0u, NULL);
        return 24;
    } __except (GetExceptionCode() == STATUS_INTEGER_DIVIDE_BY_ZERO ?
                    EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        return 0;
    }
}
