#include "console_bitmap.h"
#include <limits.h>
#include <string.h>

/* Original owner: ntcon/server/bitmap.c::CreateConsoleBitmap. Private CSR
 * client/server section mapping cannot compose in standalone. Retain its
 * bitmap/mutex boundary and original painters, not its presentation server.
 * Bounded allocation/palette mechanics reviewed from reference 286d54a30
 * graphics_buffer.c; KVM frame conversion and controller dependencies removed. */
struct ntvdm_console_bitmap {
    HANDLE mutex;
    BYTE *pixels;
    ntvdm_bitmap_description description;
    WORD indices[256];
    DWORD colors, usage;
    BOOL palette_ready, abandoned;
};

static BOOL reject(DWORD error) { SetLastError(error); return FALSE; }

static BOOL acquire(ntvdm_console_bitmap *buffer, DWORD timeout)
{
    DWORD result = WaitForSingleObject(buffer->mutex, timeout);
    if (result == WAIT_OBJECT_0) {
        if (!buffer->abandoned) return TRUE;
        ReleaseMutex(buffer->mutex);
        return reject(ERROR_ABANDONED_WAIT_0);
    }
    if (result == WAIT_TIMEOUT) return reject(ERROR_BUSY);
    if (result == WAIT_ABANDONED) {
        buffer->abandoned = TRUE;
        ReleaseMutex(buffer->mutex);
        return reject(ERROR_ABANDONED_WAIT_0);
    }
    return FALSE;
}

void ntvdm_console_bitmap_destroy(ntvdm_console_bitmap *buffer)
{
    if (!buffer) return;
    if (buffer->mutex) CloseHandle(buffer->mutex);
    if (buffer->pixels) HeapFree(GetProcessHeap(), 0, buffer->pixels);
    HeapFree(GetProcessHeap(), 0, buffer);
}

DWORD ntvdm_console_bitmap_bytes(const ntvdm_console_bitmap *buffer)
{ return buffer ? buffer->description.bytes : 0; }

BOOL ntvdm_console_bitmap_create(const BITMAPINFO *info, DWORD info_bytes,
    DWORD usage, ntvdm_console_bitmap **out, void **pixels, HANDLE *mutex)
{
    const BITMAPINFOHEADER *h;
    ntvdm_console_bitmap *buffer;
    DWORD height, colors, entry_bytes, i, error;
    uint64_t stride, bytes;
    HANDLE duplicate = NULL;
    if (out) *out = NULL;
    if (pixels) *pixels = NULL;
    if (mutex) *mutex = NULL;
    if (!out || !pixels || !mutex || !info || info_bytes < sizeof(*h))
        return reject(ERROR_INVALID_PARAMETER);
    h = &info->bmiHeader;
    if (h->biSize != sizeof(*h) || h->biPlanes != 1 ||
        h->biCompression != BI_RGB || h->biWidth <= 0 ||
        h->biWidth > SHRT_MAX || !h->biHeight || h->biHeight == LONG_MIN)
        return reject(ERROR_INVALID_PARAMETER);
    if ((h->biBitCount != 1 && h->biBitCount != 8) ||
        (usage != DIB_RGB_COLORS && usage != DIB_PAL_COLORS))
        return reject(ERROR_NOT_SUPPORTED);
    height = (DWORD)(h->biHeight < 0 ? -h->biHeight : h->biHeight);
    if (height > SHRT_MAX) return reject(ERROR_INVALID_PARAMETER);
    colors = h->biClrUsed ? h->biClrUsed : 1u << h->biBitCount;
    entry_bytes = usage == DIB_RGB_COLORS ? sizeof(RGBQUAD) : sizeof(WORD);
    if (colors > (1u << h->biBitCount) ||
        info_bytes - sizeof(*h) < colors * entry_bytes)
        return reject(ERROR_INVALID_PARAMETER);
    stride = (((uint64_t)h->biWidth * h->biBitCount + 31) / 32) * 4;
    bytes = stride * height;
    if (bytes > UINT32_MAX || bytes > SIZE_MAX)
        return reject(ERROR_ARITHMETIC_OVERFLOW);
    buffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*buffer));
    if (!buffer) return reject(ERROR_NOT_ENOUGH_MEMORY);
    buffer->description.width = (DWORD)h->biWidth;
    buffer->description.height = height;
    buffer->description.depth = h->biBitCount;
    buffer->description.stride = (DWORD)stride;
    buffer->description.bytes = (DWORD)bytes;
    buffer->colors = colors;
    buffer->usage = usage;
    buffer->palette_ready = usage == DIB_RGB_COLORS;
    for (i = 0; i < colors; ++i) {
        if (usage == DIB_PAL_COLORS) {
            memcpy(&buffer->indices[i], (const BYTE *)info->bmiColors + i * sizeof(WORD), sizeof(WORD));
            if (buffer->indices[i] >= 256) {
                ntvdm_console_bitmap_destroy(buffer);
                return reject(ERROR_INVALID_PARAMETER);
            }
        } else {
            buffer->description.palette[i] = info->bmiColors[i];
            buffer->description.palette[i].rgbReserved = 0;
        }
    }
    /* Original server normalizes either height sign to top-down. SizeImage
     * cannot size an untrusted allocation: use checked DWORD-aligned rows. */
    buffer->pixels = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (SIZE_T)bytes);
    if (!buffer->pixels) { ntvdm_console_bitmap_destroy(buffer); return reject(ERROR_NOT_ENOUGH_MEMORY); }
    buffer->mutex = CreateMutexW(NULL, FALSE, NULL);
    if (!buffer->mutex || !DuplicateHandle(GetCurrentProcess(), buffer->mutex,
        GetCurrentProcess(), &duplicate, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
        error = GetLastError(); ntvdm_console_bitmap_destroy(buffer); return reject(error);
    }
    *out = buffer; *pixels = buffer->pixels; *mutex = duplicate;
    return TRUE;
}

BOOL ntvdm_console_bitmap_palette(ntvdm_console_bitmap *buffer, HPALETTE palette)
{
    PALETTEENTRY entries[256];
    RGBQUAD resolved[256] = {0};
    UINT count, i;
    if (!buffer || !palette) return reject(ERROR_INVALID_PARAMETER);
    if (buffer->usage == DIB_RGB_COLORS) return TRUE;
    count = GetPaletteEntries(palette, 0, 256, entries);
    if (!count) return reject(ERROR_INVALID_HANDLE);
    for (i = 0; i < buffer->colors; ++i) {
        const PALETTEENTRY *entry;
        if (buffer->indices[i] >= count) return reject(ERROR_INVALID_PARAMETER);
        entry = &entries[buffer->indices[i]];
        resolved[i].rgbRed = entry->peRed;
        resolved[i].rgbGreen = entry->peGreen;
        resolved[i].rgbBlue = entry->peBlue;
    }
    if (!acquire(buffer, INFINITE)) return FALSE;
    memcpy(buffer->description.palette, resolved, sizeof(resolved));
    buffer->palette_ready = TRUE;
    return ReleaseMutex(buffer->mutex);
}

BOOL ntvdm_console_bitmap_copy(ntvdm_console_bitmap *buffer, void *pixels,
    size_t capacity, ntvdm_bitmap_description *description, DWORD timeout)
{
    if (!buffer || !pixels || !description) return reject(ERROR_INVALID_PARAMETER);
    if (capacity < buffer->description.bytes) return reject(ERROR_INSUFFICIENT_BUFFER);
    if (!acquire(buffer, timeout)) return FALSE;
    if (!buffer->palette_ready) {
        ReleaseMutex(buffer->mutex); return reject(ERROR_NOT_READY);
    }
    memcpy(pixels, buffer->pixels, buffer->description.bytes);
    *description = buffer->description;
    return ReleaseMutex(buffer->mutex);
}
