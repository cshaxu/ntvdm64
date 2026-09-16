#include "insignia.h"
#include "host_def.h"
#include "cfpu_def.h"

#include <assert.h>
#include <stddef.h>
#include <string.h>

int main(void)
{
    const float singles[] = { 1.0f, -1.5f, 0.0f };
    const double doubles[] = { 1.0, -1.5, 0.0 };
    const unsigned signs[] = { 0, 1, 0 };
    const unsigned single_exponents[] = { 127, 127, 0 };
    const unsigned double_exponents[] = { 1023, 1023, 0 };
    const unsigned single_mantissas[] = { 0, 0x400000, 0 };
    const unsigned double_mantissas[] = { 0, 0x80000, 0 };
    const unsigned char extended[] = { 0,0,0,0,0,0,0,0xc0,0xff,0xbf }; /* -1.5 */
    const unsigned char integer[] = { 0x78,0x56,0x34,0x12,0xfe,0xff,0xff,0xff };
    FP80 fp80 = {0};
    FPU_I64 i64;
    unsigned i;
    assert(sizeof(FP32) == 4 && sizeof(FP64HI) == 4 && sizeof(FP64) == 8);
    assert(sizeof(FP80SE) == 2 && sizeof(FPU_I64) == 8);
    assert(offsetof(FP64, mant_lo) == 0 && offsetof(FP64, hiword) == 4);
    assert(offsetof(FP80, mant_lo) == 0 && offsetof(FP80, mant_hi) == 4);
    assert(offsetof(FP80, sign_exp) == 8);
    assert(offsetof(FPU_I64, low_word) == 0 && offsetof(FPU_I64, high_word) == 4);
    for (i = 0; i < sizeof(singles) / sizeof(singles[0]); ++i) {
        FP32 fp32;
        FP64 fp64;
        memcpy(&fp32, &singles[i], sizeof(fp32));
        memcpy(&fp64, &doubles[i], sizeof(fp64));
        assert(fp32.sign == signs[i] && fp32.exp == single_exponents[i]);
        assert(fp32.mant == single_mantissas[i]);
        assert(fp64.hiword.sign == signs[i] && fp64.hiword.exp == double_exponents[i]);
        assert(fp64.hiword.mant_hi == double_mantissas[i] && fp64.mant_lo == 0);
    }
    memcpy(&fp80, extended, sizeof(extended));
    assert(fp80.sign_exp.sign == 1 && fp80.sign_exp.exp == 16383);
    assert(fp80.mant_hi == 0xc0000000u && fp80.mant_lo == 0);
    memcpy(&i64, integer, sizeof(i64));
    assert(i64.low_word == 0x12345678u && i64.high_word == -2);
    return 0;
}
