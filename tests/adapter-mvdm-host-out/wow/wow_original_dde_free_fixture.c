#include "wow_dde_data_bindings.h"
#include <stdio.h>

#define CHECK(x) do { if (!(x)) { printf("FAIL line %d\n", __LINE__); return 1; } } while (0)
static HGLOBAL data(WORD format, WORD status, HANDLE nested)
{
    HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(DDE_DATA));
    PDDE_DATA p;
    if (!h) return NULL;
    p = GlobalLock(h);
    if (!p) { GlobalFree(h); return NULL; }
    p->wStatus = status; p->wFmt = format; p->Data = (DWORD)nested;
    GlobalUnlock(h);
    return h;
}

int __cdecl main(void)
{
    HGLOBAL outer, inner;
    HBITMAP bitmap;
    BITMAP bitmap_info;
    unsigned mode;
    outer = data(CF_TEXT, 0, NULL);
    CHECK(outer);
    FreeDDEData(outer, FALSE, TRUE);
    CHECK(GlobalFlags(outer) != GMEM_INVALID_HANDLE);
    CHECK((GlobalFlags(outer) & GMEM_LOCKCOUNT) == 0);
    FreeDDEData(outer, TRUE, TRUE);
    CHECK(GlobalFlags(outer) == GMEM_INVALID_HANDLE);
    for (mode = 0; mode < 2; ++mode) {
        bitmap = CreateBitmap(2, 2, 1, 1, NULL);
        CHECK(bitmap);
        outer = data(CF_BITMAP, DDE_FRELEASE, bitmap);
        CHECK(outer);
        FreeDDEData(outer, FALSE, mode);
        CHECK(GlobalFlags(outer) == GMEM_INVALID_HANDLE);
        if (mode) { GdiFlush(); CHECK(GetObject(bitmap, sizeof(bitmap_info), &bitmap_info) == 0); }
        else { CHECK(GetObjectType(bitmap) == OBJ_BITMAP); CHECK(DeleteObject(bitmap)); }
    }
    inner = GlobalAlloc(GMEM_MOVEABLE, 32);
    CHECK(inner);
    outer = data(CF_DIB, DDE_FRELEASE, inner);
    CHECK(outer);
    FreeDDEData(outer, FALSE, FALSE);
    CHECK(GlobalFlags(outer) == GMEM_INVALID_HANDLE);
    CHECK(GlobalFlags(inner) == GMEM_INVALID_HANDLE);
    outer = data(CF_DIB, DDE_FRELEASE, NULL);
    CHECK(outer);
    FreeDDEData(outer, FALSE, TRUE);
    CHECK(GlobalFlags(outer) == GMEM_INVALID_HANDLE);
    FreeDDEData(NULL, FALSE, FALSE);
    for (mode = 0; mode < 2; ++mode) {
        HDC dc = CreateMetaFile(NULL);
        HMETAFILE metafile;
        LPMETAFILEPICT picture;
        HENHMETAFILE enhanced;
        CHECK(dc);
        CHECK(MoveToEx(dc, 0, 0, NULL) && LineTo(dc, 2, 2));
        metafile = CloseMetaFile(dc);
        CHECK(metafile && GetMetaFileBitsEx(metafile, 0, NULL) > 0);
        inner = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(METAFILEPICT));
        CHECK(inner);
        picture = GlobalLock(inner);
        CHECK(picture);
        picture->hMF = metafile;
        GlobalUnlock(inner);
        outer = data(mode ? CF_DSPMETAFILEPICT : CF_METAFILEPICT, DDE_FRELEASE, inner);
        CHECK(outer);
        FreeDDEData(outer, FALSE, TRUE);
        CHECK(GlobalFlags(outer) == GMEM_INVALID_HANDLE);
        CHECK(GlobalFlags(inner) == GMEM_INVALID_HANDLE);
        CHECK(GetMetaFileBitsEx(metafile, 0, NULL) == 0);
        dc = CreateEnhMetaFile(NULL, NULL, NULL, NULL);
        CHECK(dc);
        CHECK(MoveToEx(dc, 0, 0, NULL) && LineTo(dc, 2, 2));
        enhanced = CloseEnhMetaFile(dc);
        CHECK(enhanced && GetEnhMetaFileBits(enhanced, 0, NULL) > 0);
        outer = data(mode ? CF_DSPENHMETAFILE : CF_ENHMETAFILE, DDE_FRELEASE, enhanced);
        CHECK(outer);
        FreeDDEData(outer, FALSE, TRUE);
        CHECK(GlobalFlags(outer) == GMEM_INVALID_HANDLE);
        CHECK(GetEnhMetaFileBits(enhanced, 0, NULL) == 0);
    }
    for (mode = 0; mode < 2; ++mode) {
        struct { WORD version, count; PALETTEENTRY entries[2]; } pal =
            {0x300, 2, {{0,0,0,0}, {255,255,255,0}}};
        HPALETTE palette = CreatePalette((LOGPALETTE *)&pal);
        PALETTEENTRY entries[2];
        CHECK(palette && GetPaletteEntries(palette, 0, 2, entries) == 2);
        outer = data(CF_PALETTE, DDE_FRELEASE, palette);
        CHECK(outer);
        FreeDDEData(outer, FALSE, mode);
        GdiFlush();
        CHECK(GlobalFlags(outer) == GMEM_INVALID_HANDLE);
        CHECK(GetPaletteEntries(palette, 0, 2, entries) == (mode ? 0u : 2u));
        if (!mode) CHECK(DeleteObject(palette));
    }
    bitmap = CreateBitmap(2, 2, 1, 1, NULL);
    CHECK(bitmap);
    outer = data(CF_DSPBITMAP, DDE_FRELEASE, bitmap);
    CHECK(outer);
    FreeDDEData(outer, FALSE, TRUE);
    GdiFlush();
    CHECK(GlobalFlags(outer) == GMEM_INVALID_HANDLE);
    CHECK(GetObject(bitmap, sizeof(bitmap_info), &bitmap_info) == 0);
    puts("WOW_ORIGINAL_DDE_FREE_OK release override bitmap palette dib metafile enhanced display-formats null");
    return 0;
}
