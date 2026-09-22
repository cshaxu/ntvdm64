#include "wow_bitmap_bindings.h"

/* ADAPTER-WOW-025: original USER shared scaling scratch remains serialized;
 * native DC/capabilities are scoped to each synchronous invocation. */
__declspec(thread) wow_bitmap_context *wow_bitmap_current;
CRITICAL_SECTION wow_bitmap_lock;
static INIT_ONCE lock_once = INIT_ONCE_STATIC_INIT;
static LONG lock_initialized;

static BOOL CALLBACK initialize_lock(PINIT_ONCE once, PVOID parameter, PVOID *result)
{
    (void)once; (void)parameter; (void)result;
    if (!InitializeCriticalSectionEx(&wow_bitmap_lock, 0, 0)) return FALSE;
    InterlockedExchange(&lock_initialized, 1);
    return TRUE;
}

BOOL WINAPI wow_bitmap_bindings_enter(wow_bitmap_context *context)
{
    HDC screen;
    if (!context) return FALSE;
    ZeroMemory(context, sizeof(*context));
    if (!InitOnceExecuteOnce(&lock_once, initialize_lock, NULL, NULL)) return FALSE;
    EnterCriticalSection(&wow_bitmap_lock);
    screen = GetDC(NULL);
    if (!screen) { LeaveCriticalSection(&wow_bitmap_lock); return FALSE; }
    context->screen.Planes = (BYTE)GetDeviceCaps(screen, PLANES);
    context->screen.BitsPixel = (BYTE)GetDeviceCaps(screen, BITSPIXEL);
    context->screen.BitCount = context->screen.Planes * context->screen.BitsPixel;
    context->server.fPaletteDisplay = (GetDeviceCaps(screen, RASTERCAPS) & RC_PALETTE) != 0;
    context->dc = CreateCompatibleDC(screen);
    ReleaseDC(NULL, screen);
    if (!context->dc) { LeaveCriticalSection(&wow_bitmap_lock); return FALSE; }
    context->previous = wow_bitmap_current;
    wow_bitmap_current = context;
    return TRUE;
}

void WINAPI wow_bitmap_bindings_leave(wow_bitmap_context *context)
{
    assert(context && wow_bitmap_current == context);
    DeleteDC(context->dc);
    wow_bitmap_current = context->previous;
    ZeroMemory(context, sizeof(*context));
    LeaveCriticalSection(&wow_bitmap_lock);
}

HBITMAP WINAPI wow_bitmap_load(HINSTANCE module, LPCSTR name,
    LPBYTE resource, DWORD resource_size)
{
    wow_bitmap_context context;
    HBITMAP result;
    if (!resource) return WOWLoadBitmapA(module, name, NULL, resource_size);
    if (!wow_bitmap_bindings_enter(&context)) return NULL;
    result = WOWLoadBitmapA(module, name, resource, resource_size);
    wow_bitmap_bindings_leave(&context);
    return result;
}

void WINAPI wow_bitmap_bindings_shutdown(void)
{
    assert(wow_bitmap_current == NULL);
    if (InterlockedExchange(&lock_initialized, 0))
        DeleteCriticalSection(&wow_bitmap_lock);
}
