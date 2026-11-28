/**************************** Module Header ********************************\
* Module Name: exitwin.c
*
* Copyright (c) 1985 - 1999, Microsoft Corporation
*
* History:
* 07-23-92 ScottLu      Created.
\***************************************************************************/

#define WOW_ORIGINAL_TASK_ORDER
#include "wow_task_order_bindings.h"
#line 11
#pragma hdrstop

#define OPTIONMASK (EWX_SHUTDOWN | EWX_REBOOT | EWX_FORCE)

/*
 * Globals local to this file only
 */
PWINDOWSTATION  gpwinstaLogoff;
DWORD           gdwLocks;
DWORD           gdwShutdownFlags;
HANDLE          gpidEndSession;

#ifdef PRERELEASE
DWORD gdwllParamCopy, gdwStatusCopy, gdwFlagsCopy;
BOOL  gfNotifiedCopy;
#endif // PRERELEASE

#define gpwpiFirstWow (currentProcess->shared->pwpiFirstWow)
#define PpiCurrent() (currentProcess)
#define UserAllocPoolWithQuotaZInit(bytes, tag) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bytes)
#define ObReferenceObjectByHandle(handle, access, type, mode, out, info) \
    (wow_task_reference_event(handle, out) ? (NTSTATUS)0 : (NTSTATUS)0xC0000001)

#line 189
#ifndef LATER
#line 191
#else
#error
#line 197
#endif

#line 563
/***************************************************************************\
* xxxRegisterUserHungAppHandlers
*
* This routine simply records the WOW callback address for notification of
* "hung" wow apps.
*
* History:
* 01-Apr-1992 jonpa      Created.
* Added saving and duping of wowexc event handle
\***************************************************************************/

BOOL xxxRegisterUserHungAppHandlers(
    PFNW32ET pfnW32EndTask,
    HANDLE   hEventWowExec, PPROCESSINFO currentProcess)
{
    BOOL   bRetVal;
    PPROCESSINFO    ppi;
    PWOWPROCESSINFO pwpi;

    //
    //  Allocate the per wow process info stuff
    //  ensuring the memory is Zero init.
    //
    pwpi = (PWOWPROCESSINFO) UserAllocPoolWithQuotaZInit(
            sizeof(WOWPROCESSINFO), TAG_WOWPROCESSINFO);

    if (!pwpi)
        return FALSE;

    //
    // Reference the WowExec event for kernel access
    //
    bRetVal = NT_SUCCESS(ObReferenceObjectByHandle(
                 hEventWowExec,
                 EVENT_ALL_ACCESS,
                 *ExEventObjectType,
                 UserMode,
                 &pwpi->pEventWowExec,
                 NULL
                 ));

    //
    //  if sucess then intialize the pwpi, ppi structs
    //  else free allocated memory
    //
    if (bRetVal) {
        pwpi->hEventWowExecClient = hEventWowExec;
        pwpi->lpfnWowExitTask = pfnW32EndTask;
        ppi = PpiCurrent();
        ppi->pwpi = pwpi;

        // add to the list, order doesn't matter
        pwpi->pwpiNext = gpwpiFirstWow;
        gpwpiFirstWow  = pwpi;

        }
    else {
        UserFreePool(pwpi);
        }

   return bRetVal;
}
