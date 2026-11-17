/****************************** Module Header ******************************\
* Module Name: handtabl.c
*
* Copyright (c) 1985-95, Microsoft Corporation
*
* Implements the USER handle table.
*
* 01-13-92 ScottLu      Created.
\***************************************************************************/

#include <windows.h>
#include "compat/include/opennt_user_multipass.h"
#define WOW_USER_OBJECT_FLAGS_ONLY
#include "userk.h"
#include "winerrp.h"
#line 12
#pragma hdrstop

#line 14
/*
 * Turning this variable on results in lock tracking, for debugging
 * purposes. This is FALSE by default.
 */
#ifdef DEBUG_LOCKS
#error
#else
BOOL gfTrackLocks = FALSE;
#endif

/*
 * Handle table allocation globals.  The purpose of keeping per-page free
 * lists is to keep the table as small as is practical and to minimize
 * the number of pages touched while performing handle table operations.
 */
#define CPAGEENTRIESINIT    4

typedef struct _HANDLEPAGE {
    DWORD iheLimit; /* first handle index past the end of the page */
    DWORD iheFree;  /* first free handle in the page */
} HANDLEPAGE, *PHANDLEPAGE;

DWORD gcHandlePages;
PHANDLEPAGE gpHandlePages;

#line 39
CONST BYTE gabObjectCreateFlags[TYPE_CTYPES] = {
    0,                                                  /* free */
    OCF_THREADOWNED | OCF_MARKTHREAD | OCF_USEQUOTA,    /* window */
    OCF_PROCESSOWNED,                                   /* menu */
    OCF_PROCESSOWNED | OCF_USEQUOTA,                    /* cursor/icon */
    OCF_THREADOWNED | OCF_USEQUOTA,                     /* hswpi (SetWindowPos Information) */
    OCF_THREADOWNED | OCF_MARKTHREAD,                   /* hook */
    OCF_THREADOWNED | OCF_USEQUOTA,                     /* thread info object (internal) */
    0,                                                  /* clipboard data (internal) */
    OCF_THREADOWNED,                                    /* CALLPROCDATA */
    OCF_PROCESSOWNED | OCF_USEQUOTA,                    /* accel table */
    OCF_THREADOWNED | OCF_USEQUOTA,                     /* dde access */
    OCF_THREADOWNED | OCF_MARKTHREAD | OCF_USEQUOTA,    /* dde conversation */
    OCF_THREADOWNED | OCF_MARKTHREAD | OCF_USEQUOTA,    /* ddex */
    OCF_PROCESSOWNED,                                   /* zombie */
    OCF_PROCESSOWNED,                                   /* keyboard layout */
    OCF_PROCESSOWNED,                                   /* keyboard file */
#line 56
#ifdef FE_IME
#line 57
#error
#endif
#line 59
};

#line 60

/*
 * Tag array for objects allocated from pool
 */
CONST DWORD gdwAllocTag[TYPE_CTYPES] = {
    0,              /* free */
    0,              /* window */
    0,              /* menu */
    TAG_CURSOR,     /* cursor/icon */
    TAG_SWP,        /* hswpi (SetWindowPos Information) */
    0,              /* hook */
    TAG_THREADINFO, /* thread info object (internal) */
    TAG_CLIPBOARD,  /* clipboard data (internal) */
    0,              /* CALLPROCDATA */
    TAG_ACCEL,      /* accel table */
    TAG_DDE9,       /* dde access */
    TAG_DDEa,       /* dde conversation */
    TAG_DDEb,       /* ddex */
    0,              /* zombie */
    TAG_KBDLAYOUT,  /* keyboard layout */
    TAG_KBDFILE,    /* keyboard file */
#ifdef FE_IME
#error
#line 83
#endif
};

#ifdef DEBUG
#error
#line 98
#endif

void HMDestroyUnlockedObject(PHE phe);
void HMRecordLock(PVOID ppobj, PVOID pobj, DWORD cLockObj, PVOID pfn);
BOOL HMUnrecordLock(PVOID ppobj, PVOID pobj);
VOID ShowLocks(PHE);
BOOL HMRelocateLockRecord(PVOID ppobjNew, int cbDelta);

/***************************************************************************\
* HMInitHandleTable
*
* Initialize the handle table. Unused entries are linked together.
*
* 01-13-92 ScottLu      Created.
\***************************************************************************/

#define CHANDLEENTRIESINIT 200
#define CLOCKENTRIESINIT   100


#line 544
#if 0
#line 545
#error
#line 563
#endif
#line 564


#line 852
#ifdef DEBUG
#line 853
#error
#line 952
#endif // DEBUG
#line 953

#line 963
#ifdef DEBUG
#line 964
#error
#line 994
#endif // DEBUG
#line 995

#line 1169
#ifdef DEBUG
#line 1170
#error
#line 1188
#endif
#line 1189

#line 1198
#ifdef DEBUG
#line 1199
#error
#line 1239
#endif
#line 1240

#line 1250
#ifdef DEBUG
#line 1251
#error
#line 1301
#endif
#line 1302

#line 1321
#ifdef DEBUG
#line 1322
#error
#line 1325
#else
#line 1326
#line 1329
#endif
#line 1330

#line 1394
#ifdef DEBUG
#line 1395
#error
#line 1423
#endif
#line 1424

#line 1691
#ifdef DEBUG
#line 1692
#error
#line 1712
#endif
#line 1713

#line 1714
#ifdef DEBUG
#line 1715
#error
#line 1794
#endif
#line 1795

#line 1933
#ifdef DEBUG
#line 1934
#error
#line 2013
#endif // DEBUG
#line 2014

#line 283
#define TRACE_OBJECT_ALLOCS  0
#define TYPE_MAXTYPES       20

#line 286
#if (DBG || TRACE_OBJECT_ALLOCS)
#line 287
#error
#line 302
#endif // TRACE_OBJECT_ALLOCS
#line 303

#line 134
#ifdef DEBUG
#line 135
#error
#line 137
#endif
#line 138

#line 456
#ifdef FE_IME
#line 457
#error
#endif
#line 459

#line 585
#ifdef DEBUG
#line 586
#error
#endif
#line 588

#line 764
#ifdef DEBUG
#line 765
#error
#line 776
#endif
#line 777

#line 797
#ifdef DEBUG
#line 798
#error
#line 803
#endif
#line 804

#line 1056
#ifdef DEBUG
#line 1057
#error
#line 1069
#endif
#line 1070

#line 1084
#ifdef DEBUG
#line 1085
#error
#line 1098
#endif
#line 1099

#line 1139
#ifdef DEBUG
#line 1140
#error
#line 1152
#endif
#line 1153

#line 1353
#ifdef DEBUG
#line 1354
#error
#line 1361
#endif
#line 1362

#line 1519
#ifdef FE_IME
#line 1520
#error
#line 1523
#endif
#line 1524

#line 1886
#ifdef DEBUG
#line 1887
#error
#line 1911
#endif // DEBUG
#line 1912


#line 594
#ifndef DEBUG
#line 595
#line 600
#ifdef FE_IME
#line 601
#error
#endif
#line 603
#line 622
#else // DEBUG
#line 623
#error
#line 712
#endif
#line 713

#line 2137
/***************************************************************************\
* KernelPtoH
*
* This function is called from the client to convert pool-based object
* pointers, such as a cursor pointer, to a handle.
*
* HISTORY:
* 11/22/95       BradG            Created
\***************************************************************************/
HANDLE KernelPtoH( PVOID pObj ) {
    HANDLE h;

    UserAssert( pObj != NULL );

    try {
        h = PtoHq( pObj );
    } except (EXCEPTION_EXECUTE_HANDLER) {
        h = NULL;
    }
    return h;
}
