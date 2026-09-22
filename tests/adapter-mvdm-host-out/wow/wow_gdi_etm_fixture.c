#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "opennt-abi/host-compat/include/wow32_provider_private.h"

#ifndef WOW_ETM_NATIVE
static int escape_result;
static unsigned escape_calls, kerning_calls;
static BOOL arguments_valid;

static int WINAPI fixture_ext_escape(HDC dc, int escape, int input_size,
    LPCSTR input, int output_size, LPSTR output)
{
    EXTTEXTMETRIC *metrics = (EXTTEXTMETRIC *)output;
    ++escape_calls;
    arguments_valid = dc == (HDC)(ULONG_PTR)1 &&
        escape == GETEXTENDEDTEXTMETRICS && input_size == 0 && input == NULL &&
        output_size == sizeof(*metrics) && output != NULL;
    if (escape_result > 0) metrics->etmSize = sizeof(*metrics);
    return escape_result;
}

static DWORD WINAPI fixture_kerning(HDC dc, DWORD count, LPKERNINGPAIR pairs)
{
    ++kerning_calls;
    if (dc != (HDC)(ULONG_PTR)1 || count != 0 || pairs != NULL)
        arguments_valid = FALSE;
    return 7;
}

/* Compile the actual adapter body with only its public ETM dependencies
 * controlled. This is an error-contract fixture, not a font-driver test. */
#define ExtEscape fixture_ext_escape
#define GetKerningPairsA fixture_kerning
#endif
#include "../../../src/wow32-dll/source/wow_public_user_facade.c"
#ifndef WOW_ETM_NATIVE
#undef GetKerningPairsA
#undef ExtEscape

int main(void)
{
    const int results[] = {-1, -1234, 0, 1, 17};
    unsigned index;
    for (index = 0; index < ARRAYSIZE(results); ++index) {
        EXTTEXTMETRIC metrics, before;
        BOOL actual;
        BOOL expected = results[index] > 0;
        memset(&metrics, 0x5a, sizeof(metrics));
        before = metrics;
        escape_result = results[index];
        escape_calls = kerning_calls = 0;
        arguments_valid = FALSE;
        actual = GetETM((HDC)(ULONG_PTR)1, &metrics);
        printf("escape=%d accepted=%d expected=%d kerning_calls=%u\n",
            escape_result, actual, expected, kerning_calls);
        if (actual != expected || !arguments_valid || escape_calls != 1 ||
            kerning_calls != (unsigned)expected) return 1;
        if (!expected && memcmp(&metrics, &before, sizeof(metrics)) != 0)
            return 2;
        if (expected && (metrics.etmSize != sizeof(metrics) ||
                metrics.etmNKernPairs != 7)) return 3;
    }
    escape_calls = 0;
    if (GetETM((HDC)(ULONG_PTR)1, NULL) || escape_calls != 0) return 4;
    puts("WOW_GDI_ETM_RESULT_CONTRACT_OK");
    return 0;
}
#else
int main(void)
{
    HDC dc = CreateCompatibleDC(NULL);
    HFONT font, old;
    EXTTEXTMETRIC direct, adapted;
    int result;
    BOOL actual;
    CHAR face[80];
    if (!dc) return 1;
    font = CreateFontA(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH, "Arial");
    if (!font) { DeleteDC(dc); return 2; }
    old = (HFONT)SelectObject(dc, font);
    memset(&direct, 0, sizeof(direct));
    memset(&adapted, 0, sizeof(adapted));
    face[0] = 0;
    GetTextFaceA(dc, sizeof(face), face);
    result = ExtEscape(dc, GETEXTENDEDTEXTMETRICS, 0, NULL,
        sizeof(direct), (LPSTR)&direct);
    actual = GetETM(dc, &adapted);
    printf("native_face=%s escape=%d adapted=%d\n", face, result, actual);
    if (result > 0)
        direct.etmNKernPairs = (WORD)GetKerningPairsA(dc, 0, NULL);
    SelectObject(dc, old);
    DeleteObject(font);
    DeleteDC(dc);
    if (actual != (result > 0)) return 3;
    if (actual && memcmp(&direct, &adapted, sizeof(direct)) != 0) return 4;
    puts("WOW_GDI_ETM_NATIVE_BOUNDARY_OK");
    return 0;
}
#endif
