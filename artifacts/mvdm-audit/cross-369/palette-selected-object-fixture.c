/* Test-only selected-object probe: real x86 GDI, deterministic guest/heap seams. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static HPALETTE palette;
static PALETTEENTRY guest[4];
static unsigned copied_entries, allocations, releases;

void * __stdcall malloc_w(unsigned n) {
    void *p = malloc(n);
    if (p) { memset(p, 0xa5, n); ++allocations; }
    return p;
}
void __stdcall free_w(void *p) { if (p) ++releases; free(p); }
HANDLE __cdecl mvdm_wow_gdi_handle_to_native(WORD alias) {
    (void)alias;
    return palette;
}
void __stdcall putpaletteentry16(DWORD guest_ptr, int count, PALETTEENTRY *p) {
    if (guest_ptr != 1 || count < 0 || count > 4) ExitProcess(20);
    copied_entries = (unsigned)count;
    memcpy(guest, p, (size_t)count * sizeof(*p));
}
extern ULONG __stdcall WG32GetPaletteEntries(void *frame);

int main(void) {
    struct { WORD version, count; PALETTEENTRY entries[2]; } logical = {
        0x300, 2, {{1,2,3,0},{4,5,6,0}}
    };
    unsigned char frame[52] = {0};
    DWORD vp = 1; WORD count = 4, start = 1, alias = 1;
    ULONG result;
    memcpy(frame + 42, &vp, 4); memcpy(frame + 46, &count, 2);
    memcpy(frame + 48, &start, 2); memcpy(frame + 50, &alias, 2);
    palette = CreatePalette((LOGPALETTE *)&logical);
    if (!palette) return 2;
    memset(guest, 0xee, sizeof guest);
    result = WG32GetPaletteEntries(frame);
    printf("short result=%lu copied=%u first=%u,%u,%u tail=%02x alloc=%u free=%u\n", result, copied_entries, guest[0].peRed, guest[0].peGreen, guest[0].peBlue, guest[1].peRed, allocations, releases);
    if (result != 1 || copied_entries != 4 || guest[0].peRed != 4 || guest[1].peRed != 0xa5 || allocations != 1 || releases != 1) return 3;
    DeleteObject(palette); palette = NULL; copied_entries = 0;
    memset(guest, 0xee, sizeof guest);
    result = WG32GetPaletteEntries(frame);
    printf("invalid result=%lu copied=%u first=%02x alloc=%u free=%u\n", result, copied_entries, guest[0].peRed, allocations, releases);
    if (result != 0 || copied_entries != 4 || guest[0].peRed != 0xa5 || allocations != 2 || releases != 2) return 4;
    return 0;
}
