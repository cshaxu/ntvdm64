#ifndef NTVDM_CONSOLE_BITMAP_H
#define NTVDM_CONSOLE_BITMAP_H
#include <windows.h>
#include <stdint.h>

/* Worker-local backing for the original Console graphics ABI. No KVM/UI
 * dependency, wire pointer, guest interpretation or display policy. */
typedef struct ntvdm_console_bitmap ntvdm_console_bitmap;
typedef struct ntvdm_bitmap_description {
    uint32_t width, height, stride, depth, bytes;
    RGBQUAD palette[256];
} ntvdm_bitmap_description;

/* Stop callers before destroy. The original painter owns the returned mutex
 * duplicate independently and closes it after closing the graphics surface. */
BOOL ntvdm_console_bitmap_create(const BITMAPINFO *, DWORD, DWORD,
    ntvdm_console_bitmap **, void **, HANDLE *);
void ntvdm_console_bitmap_destroy(ntvdm_console_bitmap *);
DWORD ntvdm_console_bitmap_bytes(const ntvdm_console_bitmap *);
BOOL ntvdm_console_bitmap_palette(ntvdm_console_bitmap *, HPALETTE);
/* Copies packed top-down rows, including original DWORD padding. No external
 * call/IPC occurs while holding the painter mutex. Failure publishes nothing. */
BOOL ntvdm_console_bitmap_copy(ntvdm_console_bitmap *, void *, size_t,
    ntvdm_bitmap_description *, DWORD);
#endif
