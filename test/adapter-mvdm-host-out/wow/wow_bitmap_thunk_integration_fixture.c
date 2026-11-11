#include "wow_bitmap_bindings.h"
#include "wow_callback_frame_lease.h"
#include <stdio.h>
#include <string.h>

/* Actual WU32LoadBitmap, actual lease, original USER bitmap algorithms and
 * public GDI. Guest descriptor resolution/handle aliasing alone are mocked. */
typedef struct { DWORD f4, f3, f2; WORD f1; } LOADBITMAP16, *PLOADBITMAP16;
typedef LOADBITMAP16 *PVDMFRAME;
typedef char *PSZ;
static BYTE backing[98304];
static guest_memory_lease_context memory;
static unsigned current_calls, protected_calls, writes;
static int read_bytes(void *context, uint32_t address, uint8_t *bytes, uint32_t count)
{
    (void)context;
    if (address != 0x1000 || count > sizeof(backing)) return 0;
    memcpy(bytes, backing, count);
    return 1;
}
static int write_bytes(void *context, uint32_t address, const uint8_t *bytes, uint32_t count)
{
    (void)context; (void)address; (void)bytes; (void)count;
    ++writes;
    return 0;
}
int __cdecl mvdm_softpc_effective_address(uint16_t selector, uint32_t offset, uint32_t *out)
{
    (void)selector; (void)offset; (void)out;
    ++current_calls;
    return 0;
}
int __cdecl mvdm_softpc_protected_address(uint16_t selector, uint32_t offset, uint32_t *out)
{
    ++protected_calls;
    if (selector != 0x1234 || offset) return 0;
    *out = 0x1000;
    return 1;
}
int __cdecl mvdm_softpc_guest_memory_acquire(uint32_t address, uint32_t count,
    uint32_t access, guest_memory_lease **lease, uint8_t **bytes)
{
    return guest_memory_lease_acquire(&memory, address, count, access, lease, bytes);
}
int __cdecl mvdm_softpc_guest_memory_release(guest_memory_lease *lease, int commit)
{
    return guest_memory_lease_release(&memory, lease, commit);
}
#define FASTCALL
#define GETARGPTR(frame,size,arg) ((arg)=(frame))
#define GETPSZIDPTR(vp,p) ((p)=NULL)
#define GETMISCPTR(vp,p) ((p)=backing)
#define FETCHDWORD(value) (value)
#define HINSTRES32(value) ((HINSTANCE)(ULONG_PTR)(value))
#define GETHBITMAP16(value) ((ULONG)(ULONG_PTR)(value))
#define FREEMISCPTR(p) ((void)0)
#define FREEPSZIDPTR(p) ((void)0)
#define FREEARGPTR(p) ((void)0)
#define RETURN(value) return value
static struct { HBITMAP (WINAPI *pfnWOWLoadBitmapA)(HINSTANCE,LPCSTR,LPBYTE,DWORD); }
    pfnOut = { wow_bitmap_load };
#include "bitmap-call.inc"
#define CHECK(x) do { if (!(x)) { printf("FAIL line %d\n", __LINE__); return 3; } } while (0)
int __cdecl main(void)
{
    LOADBITMAP16 frame = {sizeof(backing), 0x12340000, 0, 0};
    BITMAPINFOHEADER *header = (BITMAPINFOHEADER *)backing;
    BITMAP object;
    HBITMAP bitmap, old;
    HDC dc;
    unsigned i;
    DWORD before = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
    header->biSize = sizeof(*header);
    header->biWidth = 128; header->biHeight = 255;
    header->biPlanes = 1; header->biBitCount = 24;
    memset(backing + sizeof(*header), 255, 128 * 3 * 255);
    CHECK(guest_memory_lease_begin(&memory, NULL, read_bytes, write_bytes));
    bitmap = (HBITMAP)(ULONG_PTR)WU32LoadBitmap(&frame);
    CHECK(bitmap && !wow_bitmap_current);
    CHECK(GetObject(bitmap, sizeof(object), &object) == sizeof(object));
    CHECK(object.bmWidth == 128 && object.bmHeight == 255);
    dc = CreateCompatibleDC(NULL); CHECK(dc);
    old = (HBITMAP)SelectObject(dc, bitmap); CHECK(old);
    CHECK(GetPixel(dc, 0, 0) == RGB(255,255,255));
    CHECK(GetPixel(dc, 127, 254) == RGB(255,255,255));
    SelectObject(dc, old);
    CHECK(DeleteDC(dc) && DeleteObject(bitmap));
    frame.f4 = sizeof(backing) + 1;
    CHECK(WU32LoadBitmap(&frame) == 0 && !wow_bitmap_current);
    for (i = 0; i < GUEST_MEMORY_LEASE_MAXIMUM; ++i)
        CHECK(!memory.leases[i].active && !memory.leases[i].bounce);
    CHECK(!writes && !current_calls && protected_calls == 2);
    guest_memory_lease_end(&memory);
    wow_bitmap_bindings_shutdown();
    GdiFlush();
    CHECK(GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS) == before);
    puts("WOW_BITMAP_THUNK_NATIVE_CHAIN_OK bytes=98304 pixels=2 rejected=1 leaks=0");
    return 0;
}
