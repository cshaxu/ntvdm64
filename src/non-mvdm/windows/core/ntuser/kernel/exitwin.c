/**************************** Module Header ********************************\
* Module Name: exitwin.c
*
* Copyright 1985-92, Microsoft Corporation
*
* NT: Logoff user
* DOS: Exit windows
*
* History:
* 07-23-92 ScottLu      Created.
\***************************************************************************/

/* DIVERGENCE(OPENNT-HOST-052): true subset; original process lookup is an
 * explicit view. Heap/event operations are bounded host bindings. */
#define WOW_ORIGINAL_TASK_ORDER
#include "wow_task_order_bindings.h"
#define gpwpiFirstWow (currentProcess->shared->pwpiFirstWow)
#define UserAllocPoolWithQuota(bytes, tag) HeapAlloc(GetProcessHeap(), 0, bytes)
#define ObReferenceObjectByHandle(handle, access, type, mode, out, info) \
    (wow_task_reference_event(handle, out) ? (NTSTATUS)0 : (NTSTATUS)0xC0000001)

BOOL xxxRegisterUserHungAppHandlers(
    PFNW32ET pfnW32EndTask,
    HANDLE   hEventWowExec,
    PPROCESSINFO currentProcess)
{
    BOOL   bRetVal;
    PPROCESSINFO    ppi;
    PWOWPROCESSINFO pwpi;

    //
    //  Allocate the per wow process info stuff
    //  ensuring the memory is Zero init.
    //
    pwpi = (PWOWPROCESSINFO) UserAllocPoolWithQuota(sizeof(WOWPROCESSINFO), TAG_WOW);
    if (!pwpi)
        return FALSE;
    RtlZeroMemory(pwpi, sizeof(*pwpi));

    //
    // Reference the WowExec event for kernel access
    //
    bRetVal = NT_SUCCESS(ObReferenceObjectByHandle(
                 hEventWowExec,
                 EVENT_ALL_ACCESS,
                 NULL,
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
        pwpi->lpfnWowExitTask = (DWORD)pfnW32EndTask;
        ppi = currentProcess;
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
