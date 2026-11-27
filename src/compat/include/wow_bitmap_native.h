#ifndef COMPAT_WOW_BITMAP_NATIVE_H
#define COMPAT_WOW_BITMAP_NATIVE_H
#include <windows.h>
#include <assert.h>

/* XP windows/core/w32inc/w32err.h retail FALSE filter catches without
 * setting LastError. Reject a new TRUE consumer until its contract is bound. */
#ifndef try
#define try __try
#endif
#ifndef except
#define except __except
#endif
#define W32ExceptionHandler(setError, flags) \
    ((void)sizeof(char[(setError) ? -1 : 1]), EXCEPTION_EXECUTE_HANDLER)

/* Shared native leaves for the retained original bitmap algorithms. */
#define UserLocalAlloc(flags, bytes) LocalAlloc(flags, bytes)
#define UserLocalFree(p) LocalFree((HLOCAL)(p))
#define UserAssert(condition) assert(condition)
#define BitmapWidth(cx, bpp) (((((cx)*(bpp)) + 31) & ~31) >> 3)
#define RGBX(rgb) RGB(GetBValue(rgb), GetGValue(rgb), GetRValue(rgb))
#endif
