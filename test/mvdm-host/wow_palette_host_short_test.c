#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    struct { WORD version, count; PALETTEENTRY entries[2]; } logical = {
        0x300, 2, {{1,2,3,0}, {4,5,6,0}}
    };
    PALETTEENTRY result[4];
    HPALETTE palette = CreatePalette((const LOGPALETTE *)&logical);
    UINT n;
    if (!palette) return 2;
    memset(result, 0xa5, sizeof(result));
    n = GetPaletteEntries(palette, 1, 4, result);
    printf("short count=%u tail=%02x%02x%02x%02x\n", n,
        result[1].peRed, result[1].peGreen, result[1].peBlue, result[1].peFlags);
    if (n != 1 || result[1].peRed != 0xa5) return 3;
    memset(result, 0xa5, sizeof(result));
    n = GetPaletteEntries(NULL, 0, 4, result);
    printf("invalid count=%u first=%02x%02x%02x%02x\n", n,
        result[0].peRed, result[0].peGreen, result[0].peBlue, result[0].peFlags);
    if (n != 0 || result[0].peRed != 0xa5) return 4;
    DeleteObject(palette);
    return 0;
}
