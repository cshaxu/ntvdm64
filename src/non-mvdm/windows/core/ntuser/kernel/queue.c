/****************************** Module Header ******************************\
* Module Name: queue.c
*
* Copyright (c) 1985-95, Microsoft Corporation
*
* This module contains the low-level code for working with the Q structure.
*
* History:
* 12-02-90 DavidPe      Created.
* 02-06-91 IanJa        HWND revalidation added
\***************************************************************************/

/* DIVERGENCE(OPENNT-HOST-053): original InitTask, compatibility and
 * start-glass owners. Current thread/process are explicit; native image name
 * comes from the process view, not a kernel ETHREAD/PEB dereference. */
#define WOW_ORIGINAL_TASK_ORDER
#include "wow_task_init_bindings.h"
#include "userk.h"

void SetAppCompatFlags(
    PTHREADINFO pti)
{
    DWORD dwFlags = 0;
    WCHAR szHex[QUERY_VALUE_BUFFER];
    WCHAR szKey[80];
    WCHAR *pchStart, *pchEnd;
    DWORD cb;
    PUNICODE_STRING pstrAppName;

    /*
     * If this process is WOW, every app (and every thread) has its own
     * compat flags. If not WOW, then we only need to do this lookup
     * once per process.
     */
    if (!(pti->TIF_flags & TIF_16BIT)) {
        if (LOWORD(pti->dwExpWinVer) <= VER31) {
            if (pti->ppi->W32PF_Flags & W32PF_HAVECOMPATFLAGS) {
                pti->dwCompatFlags = pti->ppi->dwCompatFlags;
                pti->pClientInfo->dwCompatFlags = pti->dwCompatFlags;
                return;
            }
        }
        goto SACF_GotFlags;     // They're zero; we're a 3.1 (32 bit) app
    }


    /*
     * Find end of app name
     */
    if (pti->pstrAppName != NULL)
        pstrAppName = pti->pstrAppName;
    else
        pstrAppName = &pti->ppi->imagePathName;
    pchStart = pchEnd = pstrAppName->Buffer +
            (pstrAppName->Length / sizeof(WCHAR));

    /*
     * Locate start of extension
     */
    while (TRUE) {
        if (pchEnd == pstrAppName->Buffer) {
            pchEnd = pchStart;
            break;
        }

        if (*pchEnd == TEXT('.'))
            break;

        pchEnd--;
    }

    /*
     * Locate start of filename
     */
    pchStart = pchEnd;

    while (pchStart != pstrAppName->Buffer) {
        if (*pchStart == TEXT('\\') || *pchStart == TEXT(':')) {
            pchStart++;
            break;
        }

        pchStart--;
    }

    /*
     * Get a copy of the filename - make sure it fits and is zero
     * terminated.
     */
    cb = (pchEnd - pchStart) * sizeof(WCHAR);
    if (cb >= sizeof(szKey))
        cb = sizeof(szKey) - sizeof(WCHAR);
    RtlCopyMemory(szKey, pchStart, cb);
    szKey[(cb / sizeof(WCHAR))] = 0;

    /*
     * Find compatiblility flags (if not a 4.0 app)
     */
    if (LOWORD(pti->dwExpWinVer) <= VER31)
        if (UT_FastGetProfileStringW(PMAP_COMPAT, szKey, TEXT(""),
                /* DIVERGENCE(OPENNT-HOST-054): cch is WCHAR count, not bytes. */
                szHex, sizeof(szHex) / sizeof(szHex[0]))) {
            UNICODE_STRING strHex;

            /*
             * Found some flags.  Attempt to convert the hex string
             * into numeric value. Specify base 0, so
             * RtlUnicodeStringToInteger will handle the 0x format
             */
            RtlInitUnicodeString(&strHex, szHex);
            RtlUnicodeStringToInteger(&strHex, 0, (PULONG)&dwFlags);
        }

SACF_GotFlags:

    pti->dwCompatFlags = dwFlags;
    pti->pClientInfo->dwCompatFlags = dwFlags;

    pti->ppi->dwCompatFlags = dwFlags;
    pti->ppi->W32PF_Flags |= W32PF_HAVECOMPATFLAGS;

}

NTSTATUS xxxInitTask(
    UINT dwExpWinVer,
    PUNICODE_STRING pstrAppName,
    DWORD hTaskWow,
    DWORD dwHotkey,
    DWORD idTask,
    DWORD dwX,
    DWORD dwY,
    DWORD dwXSize,
    DWORD dwYSize,
    WORD  wUnusedAndAvailable,
    PTHREADINFO currentThread)
{
    PTHREADINFO pti;
    PTDB ptdb;
    PPROCESSINFO ppi;
    PWOWTHREADINFO pwti;

    UNREFERENCED_PARAMETER(wUnusedAndAvailable);

    pti = currentThread;
    ppi = pti->ppi;

    /*
     * Set the real name of the module.  (Instead of 'NTVDM')
     */
    if (pti->pstrAppName != NULL)
        UserFreePool(pti->pstrAppName);
    pti->pstrAppName = UserAllocPoolWithQuota(sizeof(UNICODE_STRING) +
            pstrAppName->MaximumLength, TAG_TEXT);
    if (pti->pstrAppName != NULL) {
        pti->pstrAppName->Buffer = (PWCHAR)(pti->pstrAppName + 1);
        try {
            RtlCopyMemory(pti->pstrAppName->Buffer, pstrAppName->Buffer,
                    pstrAppName->Length);
        } except (EXCEPTION_EXECUTE_HANDLER) {
            UserFreePool(pti->pstrAppName);
            pti->pstrAppName = NULL;
            return STATUS_OBJECT_NAME_INVALID;
        }
        pti->pstrAppName->MaximumLength = pstrAppName->MaximumLength;
        pti->pstrAppName->Length = pstrAppName->Length;
    } else
        return STATUS_OBJECT_NAME_INVALID;

    /*
     * An app is starting!
     */
    if (!(ppi->W32PF_Flags & W32PF_APPSTARTING)) {
        ppi->W32PF_Flags |= W32PF_APPSTARTING;
        ppi->ppiNext = gppiStarting;
        gppiStarting = ppi;
    }

    /*
     * We never want to use the ShowWindow defaulting mechanism for WOW
     * apps.  If STARTF_USESHOWWINDOW was set in the client-side
     * STARTUPINFO structure, WOW has already picked it up and used
     * it for the first (command-line) app.
     */
    ppi->usi.dwFlags &= ~STARTF_USESHOWWINDOW;

    /*
     * If WOW passed us a hotkey for this app, save it for CreateWindow's use.
     */
    if (dwHotkey != 0) {
        ppi->dwHotkey = dwHotkey;
    }

    /*
     * If WOW passed us a non-default window position use it, otherwise clear it.
     */
    ppi->usi.cb = sizeof(ppi->usi);

    if (dwX == CW_USEDEFAULT || dwX == CW2_USEDEFAULT) {
        ppi->usi.dwFlags &= ~STARTF_USEPOSITION;
    } else {
        ppi->usi.dwFlags |= STARTF_USEPOSITION;
        ppi->usi.dwX = dwX;
        ppi->usi.dwY = dwY;
    }

    /*
     * If WOW passed us a non-default window size use it, otherwise clear it.
     */
    if (dwXSize == CW_USEDEFAULT || dwXSize == CW2_USEDEFAULT) {
        ppi->usi.dwFlags &= ~STARTF_USESIZE;
    } else {
        ppi->usi.dwFlags |= STARTF_USESIZE;
        ppi->usi.dwXSize = dwXSize;
        ppi->usi.dwYSize = dwYSize;
    }

    /*
     * Alloc and Link in new task into the task list
     */
    if ((ptdb = (PTDB)UserAllocPoolWithQuota(sizeof(TDB), TAG_WOW)) == NULL)
        return STATUS_NO_MEMORY;
    pti->ptdb = ptdb;

    /*
     * Set the flags to say this is a 16-bit thread - before attaching
     * queues!
     */
    pti->TIF_flags |= TIF_16BIT | TIF_FIRSTIDLE;

    /*
     * If this task is running in the shared WOW VDM, we handle
     * WaitForInputIdle a little differently than separate WOW
     * VDMs.  This is because CreateProcess returns a real process
     * handle when you start a separate WOW VDM, so the "normal"
     * WaitForInputIdle works.  For the shared WOW VDM, CreateProcess
     * returns an event handle.
     */
    ptdb->pwti = NULL;
    if (idTask) {
        pti->TIF_flags |= TIF_SHAREDWOW;

        /*
         * Look for a matching thread in the WOW thread info list.
         */
        if (idTask != (DWORD)-1) {
            for (pwti = gpwtiFirst; pwti != NULL; pwti = pwti->pwtiNext) {
                if (pwti->idTask == idTask) {
                    ptdb->pwti = pwti;
                    break;
                }
            }
#ifdef DEBUG
            if (pwti == NULL) {
                RIPMSG0(RIP_WARNING, "InitTask couldn't find WOW struct\n");
            }
#endif
        }
    }
    pti->pClientInfo->dwTIFlags |= pti->TIF_flags;

    /*
     * We need this thread to share the queue of other win16 apps.
     * If we're journalling, all apps are sharing a queue, so we wouldn't
     * want to interrupt that - so only cause queue recalculation
     * if we aren't journalling.
     */
    if (!FJOURNALRECORD() && !FJOURNALPLAYBACK())
        ReattachThreads(FALSE);

    /*
     * Save away the 16 bit task handle: we use this later when calling
     * wow back to close a WOW task.
     */
    ptdb->hTaskWow = hTaskWow;

    /*
     * Setup the app start cursor for 5 second timeout.
     */
    CalcStartCursorHide((PW32PROCESS)ppi, 5000);

    /*
     * HIWORD: != 0 if wants proportional font
     * LOWORD: Expected windows version (3.00 [300], 3.10 [30A], etc)
     */
    pti->dwExpWinVer = dwExpWinVer;
    pti->pClientInfo->dwExpWinVer = dwExpWinVer;

    /*
     * Mark this guy and add him to the global task list so he can run.
     */
#define NORMAL_PRIORITY_TASK 10

    /*
     * To be Compatible it super important that the new task run immediately
     * Set its priority accordingly.  No other task should ever be set to
     * CREATION priority
     */
    ptdb->nPriority = NORMAL_PRIORITY_TASK;
    ptdb->nEvents = 0;
    ptdb->pti = pti;
    ptdb->ptdbNext = NULL;

    InsertTask(ppi, ptdb);

    SetAppCompatFlags(pti);

    /*
     * Force this new task to be the active task (WOW will ensure the
     * currently running task does a Yield which will put it into the
     * non preemptive scheduler.
     */
    ppi->pwpi->ptiScheduled = pti;
    ppi->pwpi->CSLockCount = -1;

    EnterWowCritSect(pti, ppi->pwpi);

    /*
     * ensure app gets focus
     */
    _ShowStartGlass(10000, ppi);

    return STATUS_SUCCESS;
}

void _ShowStartGlass(
    DWORD dwTimeout,
    PPROCESSINFO currentProcess)
{
    PPROCESSINFO ppi;

    /*
     * If this is the first call to ShowStartGlass(), then the
     * W32PF_ALLOWFOREGROUNDACTIVATE bit has already been set in the process
     * info - we don't want to set it again because it may have been
     * purposefully cleared when the user hit a key or mouse clicked.
     */
    ppi = currentProcess;
    if (ppi->W32PF_Flags & W32PF_SHOWSTARTGLASSCALLED) {
        /*
         * Allow this wow app to come to the foreground. This'll be cancelled
         * if the user mouse clicks or hits any keys.
         */
        gfAllowForegroundActivate = TRUE;
        ppi->W32PF_Flags |= W32PF_ALLOWFOREGROUNDACTIVATE;
    }
    ppi->W32PF_Flags |= W32PF_SHOWSTARTGLASSCALLED;

    /*
     * Show the start glass cursor for this much longer.
     */
    CalcStartCursorHide((PW32PROCESS)ppi, dwTimeout);
}


