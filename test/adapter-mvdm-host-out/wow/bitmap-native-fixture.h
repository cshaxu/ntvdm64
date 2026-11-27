#ifndef WOW_BITMAP_NATIVE_FIXTURE_H
#define WOW_BITMAP_NATIVE_FIXTURE_H
#include "wow_bitmap_bindings.h"
/* This native bitmap fixture has no registered guest resource module.
 * Bind the unused resource-cover entry to actual native providers, not stubs. */
#define FINDRESOURCEEXA(module, name, type, language) \
    FindResourceExA(module, type, name, language)
int WCSToMBEx(WORD, LPCWSTR, int, LPSTR *, int, BOOL);
#define WCSToMB(text, count, output, capacity, allocate) \
    WCSToMBEx(0, text, count, output, capacity, allocate)
#endif
