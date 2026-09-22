#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "wow_callback_frame_lease.h"
/* Compile the extracted current WU32LoadBitmap body, not a copied algorithm.
 * All historical macros below are observation seams, not runtime providers. */
typedef struct { DWORD f4, f3, f2; WORD f1; } LOADBITMAP16, *PLOADBITMAP16;
typedef LOADBITMAP16 *PVDMFRAME;
typedef char *PSZ;
static unsigned errors, step, reject, has_data;
static BYTE borrowed, leased;
static const char name[] = "BITMAP";
static void event(unsigned expected) { if (++step != expected) ++errors; }
#define FASTCALL
#define GETARGPTR(frame,size,arg) do { event(1); arg=frame; } while (0)
#define GETPSZIDPTR(vp,p) do { event(2); p=(PSZ)name; } while (0)
#define GETMISCPTR(vp,p) do { event(3); p=has_data ? &borrowed : NULL; } while (0)
#define FETCHDWORD(value) (value)
#define HINSTRES32(value) ((HANDLE)(ULONG_PTR)(value))
#define GETHBITMAP16(value) ((ULONG)(ULONG_PTR)(value))
#define FREEMISCPTR(p) event(has_data ? (reject ? 5 : 7) : 5)
#define FREEPSZIDPTR(p) event(has_data ? (reject ? 6 : 8) : 6)
#define FREEARGPTR(p) event(has_data ? (reject ? 7 : 9) : 7)
#define RETURN(value) return value

int wow_callback_frame_acquire_protected_vp(uint32_t vp, uint32_t count,
    uint32_t access, wow_callback_frame_lease *view)
{
    event(4);
    if (vp != 0x12340020 || count != 98304 || access != GUEST_MEMORY_ACCESS_READ)
        ++errors;
    if (reject) return 0;
    view->lease = (guest_memory_lease *)&leased;
    view->bytes = &leased;
    return 1;
}
int wow_callback_frame_release(wow_callback_frame_lease *view, int commit)
{
    event(6);
    if (view->bytes != &leased || commit) ++errors;
    ZeroMemory(view, sizeof(*view));
    return 1;
}
static HBITMAP WINAPI output(HANDLE module, LPCSTR resource_name, LPBYTE bytes, DWORD size)
{
    event(has_data ? 5 : 4);
    if ((ULONG_PTR)module != 42 || resource_name != name ||
        bytes != (has_data ? &leased : NULL) || size != (has_data ? 98304u : 0u)) ++errors;
    return (HBITMAP)7;
}
static struct { HBITMAP (WINAPI *pfnWOWLoadBitmapA)(HANDLE,LPCSTR,LPBYTE,DWORD); }
    pfnOut = { output };
#include "bitmap-call.inc"

int __cdecl main(void)
{
    LOADBITMAP16 frame = {98304, 0x12340020, 1, 42};
    has_data = 1;
    if (WU32LoadBitmap(&frame) != 7 || step != 9) ++errors;
    step = 0; reject = 1;
    if (WU32LoadBitmap(&frame) != 0 || step != 7) ++errors;
    step = 0; reject = 0; has_data = 0; frame.f3 = frame.f4 = 0;
    if (WU32LoadBitmap(&frame) != 7 || step != 7) ++errors;
    printf("WOW_BITMAP_CALL_ORDER errors=%u cases=3\n", errors);
    return errors ? 3 : 0;
}
