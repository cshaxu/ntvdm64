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

VOID WINAPI FreeDDEData(HANDLE hDDE, BOOL fIgnorefRelease,
    BOOL fFreeTruelyGlobalObjects);

#endif
