#ifndef COMPAT_WOW_BITMAP_NATIVE_H
#define COMPAT_WOW_BITMAP_NATIVE_H
#include <windows.h>
#include <assert.h>

/* Shared native leaves for the retained original bitmap algorithms. */
#define UserLocalAlloc(flags, bytes) LocalAlloc(flags, bytes)
#define UserLocalFree(p) LocalFree((HLOCAL)(p))
#define UserAssert(condition) assert(condition)
#define BitmapWidth(cx, bpp) (((((cx)*(bpp)) + 31) & ~31) >> 3)
#define RGBX(rgb) RGB(GetBValue(rgb), GetGValue(rgb), GetRValue(rgb))
#endif
