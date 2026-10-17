#ifndef WOW_EDIT_LOOKASIDE_BINDINGS_H
#define WOW_EDIT_LOOKASIDE_BINDINGS_H
#include <windows.h>
#include <winternl.h>

/* OPENNT-HOST-074: original nthal.h declaration subset (Microsoft).
 * Native heap storage only; no kernel zone service or guest allocation. */
typedef struct _ZONE_SEGMENT_HEADER {
    SINGLE_LIST_ENTRY SegmentList;
    PVOID Reserved;
} ZONE_SEGMENT_HEADER, *PZONE_SEGMENT_HEADER;

typedef struct _ZONE_HEADER {
    SINGLE_LIST_ENTRY FreeList;
    SINGLE_LIST_ENTRY SegmentList;
    ULONG BlockSize;
    ULONG TotalSegmentSize;
} ZONE_HEADER, *PZONE_HEADER;

#define WOW_EDIT_LOOKASIDE_DECLARATIONS_ONLY
#include "opennt-host/windows/core/ntuser/client/usercli.h"
#undef WOW_EDIT_LOOKASIDE_DECLARATIONS_ONLY

/* Original USER initialized its global critical section before controls.
 * The standalone provider establishes that same process-lifetime resource
 * before entering the recovered editor or dialog implementation. The dialog
 * pools remain defined by original editec.c and dlgmgr.c. No allocation policy
 * lives here. */
extern CRITICAL_SECTION gcsLookaside;
BOOL WINAPI wow_edit_lookaside_ready(void);

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0)
#endif
#ifndef STATUS_NO_MEMORY
#define STATUS_NO_MEMORY ((NTSTATUS)0xC0000017L)
#endif
/* Original usercli.h owns the native heap operation macros. */
#define pUserHeap GetProcessHeap()
#define RtlEnterCriticalSection EnterCriticalSection
#define RtlLeaveCriticalSection LeaveCriticalSection
#endif
