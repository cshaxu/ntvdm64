#include <windows.h>
#include <nt.h>
#include <ntrtl.h>

static int expect(int condition, int code)
{
    return condition ? 0 : code;
}

int main(void)
{
    LARGE_INTEGER dividend;
    LARGE_INTEGER quotient;
    LARGE_INTEGER product;
    ULONG remainder = 0;
    ULONG words[2] = { 0x11111111u, 0x22222222u };
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

    __try {
        (void)RtlExtendedLargeIntegerDivide(dividend, 0u, NULL);
        return 8;
    } __except (GetExceptionCode() == STATUS_INTEGER_DIVIDE_BY_ZERO ?
                    EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        return 0;
    }
}
