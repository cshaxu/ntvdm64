#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "opennt-abi/host-compat/include/wow32_provider_private.h"

int WINAPI wow_original_DialogBoxIndirectParamAorW(HINSTANCE instance,
    LPCDLGTEMPLATEW template_data, HWND owner, DLGPROC procedure,
    LPARAM parameter, UINT flags)
{
    UNREFERENCED_PARAMETER(instance);
    UNREFERENCED_PARAMETER(template_data);
    UNREFERENCED_PARAMETER(owner);
    UNREFERENCED_PARAMETER(procedure);
    UNREFERENCED_PARAMETER(parameter);
    UNREFERENCED_PARAMETER(flags);
    return -1;
}

typedef struct _OUTLINE_RESULT {
    GLYPHMETRICS metrics;
    DWORD size;
    BYTE *data;
} OUTLINE_RESULT;

static MAT2 fixture_identity = {
    {0, 1}, {0, 0}, {0, 0}, {0, 1}
};

static void fixture_free_outline(OUTLINE_RESULT *result)
{
    if (result->data != NULL) (void)LocalFree(result->data);
    memset(result, 0, sizeof(*result));
}

static BOOL fixture_outline(HDC dc, BOOL wow, OUTLINE_RESULT *result)
{
    DWORD size;

    memset(result, 0, sizeof(*result));
    size = wow ? GetGlyphOutlineWow(dc, 'A', GGO_BITMAP, &result->metrics,
        0, NULL, &fixture_identity) : GetGlyphOutlineA(dc, 'A', GGO_BITMAP,
        &result->metrics, 0, NULL, &fixture_identity);
    if (size == GDI_ERROR) return FALSE;
    result->size = size;
    if (size == 0) return TRUE;
    result->data = (BYTE *)LocalAlloc(LMEM_FIXED, size);
    if (result->data == NULL) return FALSE;
    if ((wow ? GetGlyphOutlineWow(dc, 'A', GGO_BITMAP, &result->metrics,
            size, result->data, &fixture_identity) : GetGlyphOutlineA(dc,
            'A', GGO_BITMAP, &result->metrics, size, result->data,
            &fixture_identity)) != size) {
        fixture_free_outline(result);
        return FALSE;
    }
    return TRUE;
}

static BOOL fixture_equal_outline(const OUTLINE_RESULT *left,
    const OUTLINE_RESULT *right)
{
    return left->size == right->size &&
        memcmp(&left->metrics, &right->metrics, sizeof(left->metrics)) == 0 &&
        (left->size == 0 || memcmp(left->data, right->data, left->size) == 0);
}

int main(void)
{
    HDC dc;
    HFONT rotated, plain, old;
    OUTLINE_RESULT raw, adapted, expected;
    BOOL raw_differs;

    dc = CreateCompatibleDC(NULL);
    if (dc == NULL) return 1;
    rotated = CreateFontA(-48, 0, 900, 900, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_SWISS, "Arial");
    plain = CreateFontA(-48, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_SWISS, "Arial");
    if (rotated == NULL || plain == NULL) return 2;
    old = (HFONT)SelectObject(dc, rotated);
    if (old == NULL || !fixture_outline(dc, FALSE, &raw) ||
        !fixture_outline(dc, TRUE, &adapted) ||
        GetCurrentObject(dc, OBJ_FONT) != rotated) return 3;
    raw_differs = !fixture_equal_outline(&raw, &adapted);
    if (SelectObject(dc, plain) == NULL || !fixture_outline(dc, FALSE, &expected))
        return 4;
    (void)SelectObject(dc, old);
    (void)DeleteObject(rotated);
    (void)DeleteObject(plain);
    (void)DeleteDC(dc);
    if (!raw_differs || !fixture_equal_outline(&adapted, &expected)) return 5;
    fixture_free_outline(&raw);
    fixture_free_outline(&adapted);
    fixture_free_outline(&expected);
    puts("WOW_GLYPH_OUTLINE_IGNORE_ROTATION_OK");
    return 0;
}
