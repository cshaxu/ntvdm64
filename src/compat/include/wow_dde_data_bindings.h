#ifndef WOW_DDE_DATA_BINDINGS_H
#define WOW_DDE_DATA_BINDINGS_H

#include <windows.h>
#include <dde.h>
#include <ddeml.h>

/* Original ntuser/inc/ddetrack.h data layout and client/usercli.h retail
 * macros. No DDE ownership policy is implemented by this binding. */
typedef struct tagDDE_DATA {
    WORD wStatus;
    WORD wFmt;
    DWORD Data;
} DDE_DATA, *PDDE_DATA;

#define USERGLOBALLOCK(h, p) p = GlobalLock((HANDLE)(h))
#define USERGLOBALUNLOCK(h) GlobalUnlock((HANDLE)(h))
#define WOWGLOBALFREE(pmem) GlobalFree(pmem)

/* Original ddemlcli.h/usercli.h allocation macros; public DC boundaries. */
#define DDEMLAlloc(cb) LocalAlloc(LPTR, cb)
#define DDEMLFree(p) LocalFree(p);
#define UserGlobalAlloc(flags, size) GlobalAlloc(flags, size)
#define UserGlobalSize(pmem) GlobalSize(pmem)
#define UserGlobalFree(h) GlobalFree((HANDLE)(h))
#define NtUserGetDC GetDC
#define NtUserReleaseDC ReleaseDC

HBITMAP WINAPI CopyBitmap(HBITMAP bitmap);
HPALETTE WINAPI CopyPalette(HPALETTE palette);
HANDLE WINAPI CopyDDEData(HANDLE data, BOOL is_execute);

VOID WINAPI FreeDDEData(HANDLE hDDE, BOOL fIgnorefRelease,
    BOOL fFreeTruelyGlobalObjects);

#endif
