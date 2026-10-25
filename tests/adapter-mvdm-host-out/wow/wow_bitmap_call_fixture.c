#include <windows.h>
#include <stdint.h>
#include <stdio.h>

/* The current WU32LoadBitmap body is extracted at test time.  This observes
 * its protected-byte lease and output-slot order without copying its logic. */
typedef WORD HAND16;
typedef struct { DWORD f4, f3, f2; HAND16 f1; } LOADBITMAP16, *PLOADBITMAP16;
typedef LOADBITMAP16 *PVDMFRAME;
typedef struct { void *lease; uint8_t *bytes; uint32_t guest_address, byte_count, access; } wow_callback_frame_lease;
static BYTE fixture_bytes[12];
static int errors, output_calls, acquire_calls, release_calls, acquire_ok, has_data;
#define FASTCALL
#define GETARGPTR(f,s,a) do {(void)(s);(a)=(f);}while(0)
#define GETPSZIDPTR(v,p) do {(void)(v);(p)="BITMAP";}while(0)
#define GETMISCPTR(v,p) do {(void)(v);(p)=has_data ? fixture_bytes : NULL;}while(0)
#define FETCHDWORD(v) ((DWORD)(v))
#define HINSTRES32(v) ((HANDLE)(ULONG_PTR)(v))
#define GUEST_MEMORY_ACCESS_READ 1
#define GETHBITMAP16(v) ((ULONG)(ULONG_PTR)(v))
#define FREEMISCPTR(p) ((void)(p))
#define FREEPSZIDPTR(p) ((void)(p))
#define FREEARGPTR(p) ((void)(p))
#define RETURN(v) return (v)
static int wow_callback_frame_acquire_protected_vp(uint32_t vp, uint32_t count,
    uint32_t access, wow_callback_frame_lease *view)
{
    ++acquire_calls;
    if (vp != 0x12340000u || count != sizeof(fixture_bytes) || access != GUEST_MEMORY_ACCESS_READ) ++errors;
    if (!acquire_ok) return 0;
    view->lease = view; view->bytes = fixture_bytes;
    return 1;
}
static int wow_callback_frame_release(wow_callback_frame_lease *view, int commit)
{
    ++release_calls;
    if (view->bytes != fixture_bytes || commit) ++errors;
    return 1;
}
static HBITMAP WINAPI output(HINSTANCE module, LPCSTR name, LPBYTE bytes, DWORD count)
{
    ++output_calls;
    if (module != (HINSTANCE)(ULONG_PTR)7 || !name || count != sizeof(fixture_bytes) ||
        bytes != (has_data ? fixture_bytes : NULL)) ++errors;
    return (HBITMAP)(ULONG_PTR)0x77;
}
static struct { HBITMAP (WINAPI *pfnWOWLoadBitmapA)(HINSTANCE, LPCSTR, LPBYTE, DWORD); } pfnOut = { output };
#include "bitmap-call.inc"
static void run(int data, int available)
{
    LOADBITMAP16 frame = {sizeof(fixture_bytes), 0x12340000u, 0, 7};
    ULONG result;
    has_data = data;
    acquire_ok = available;
    result = WU32LoadBitmap(&frame);
    if (data && !available ? result != 0 : result != 0x77) ++errors;
}
int main(void)
{
    run(1, 1); run(1, 0); run(0, 0);
    if (output_calls != 2 || acquire_calls != 2 || release_calls != 1) ++errors;
    printf("WOW_BITMAP_CALL_ORDER errors=%d output=%d acquire=%d release=%d\n",
        errors, output_calls, acquire_calls, release_calls);
    return errors ? 3 : 0;
}
