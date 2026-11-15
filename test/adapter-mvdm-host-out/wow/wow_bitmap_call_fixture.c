#include <windows.h>
#include <stdint.h>
#include <stdio.h>

/* The current WU32LoadBitmap body is extracted at test time.  This observes
 * original resource pointer and output-slot order without copying its logic. */
typedef WORD HAND16;
typedef struct { DWORD f4, f3, f2; HAND16 f1; } LOADBITMAP16, *PLOADBITMAP16;
typedef LOADBITMAP16 *PVDMFRAME;
static BYTE fixture_bytes[12];
static int errors, output_calls, has_data;
#define FASTCALL
#define GETARGPTR(f,s,a) do {(void)(s);(a)=(f);}while(0)
#define GETPSZIDPTR(v,p) do {(void)(v);(p)="BITMAP";}while(0)
#define GETMISCPTR(v,p) do {(void)(v);(p)=has_data ? fixture_bytes : NULL;}while(0)
#define FETCHDWORD(v) ((DWORD)(v))
#define HINSTRES32(v) ((HANDLE)(ULONG_PTR)(v))
#define GETHBITMAP16(v) ((ULONG)(ULONG_PTR)(v))
#define FREEMISCPTR(p) ((void)(p))
#define FREEPSZIDPTR(p) ((void)(p))
#define FREEARGPTR(p) ((void)(p))
#define RETURN(v) return (v)
static HBITMAP WINAPI output(HINSTANCE module, LPCSTR name, LPBYTE bytes, DWORD count)
{
    ++output_calls;
    if (module != (HINSTANCE)(ULONG_PTR)7 || !name || count != sizeof(fixture_bytes) ||
        bytes != (has_data ? fixture_bytes : NULL)) ++errors;
    return (HBITMAP)(ULONG_PTR)0x77;
}
static struct { HBITMAP (WINAPI *pfnWOWLoadBitmapA)(HINSTANCE, LPCSTR, LPBYTE, DWORD); } pfnOut = { output };
#include "bitmap-call.inc"
static void run(int data)
{
    LOADBITMAP16 frame = {sizeof(fixture_bytes), 0x12340000u, 0, 7};
    ULONG result;
    has_data = data;
    result = WU32LoadBitmap(&frame);
    if (result != 0x77) ++errors;
}
int main(void)
{
    run(1); run(0);
    if (output_calls != 2) ++errors;
    printf("WOW_BITMAP_CALL_ORDER errors=%d output=%d\n", errors, output_calls);
    return errors ? 3 : 0;
}
