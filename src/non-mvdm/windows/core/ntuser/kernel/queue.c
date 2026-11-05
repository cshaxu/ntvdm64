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

#line 30
#ifdef DEBUG
#line 31
#error Excluded unsupported original DEBUG DDE tracing
#line 36
#else
#line 37
#define TraceDdeMsg(m, h1, h2, c)
#endif
#line 39

#line 54
#ifdef DEBUG
#line 55
#error Excluded unsupported original DEBUG queue validation
#line 114
#else
#line 115
#define DebugValidateMLIST(pml)
#define DebugValidateMLISTandQMSG(pml, pqmsg)
#endif
#line 118

#line 526
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
#line 608
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

#line 658
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
#line 669
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
#line 785
#ifdef DEBUG
#line 786
#error DIVERGENCE OPENNT-HOST-146: DEBUG user profile is not selected
#line 789
#endif
#line 790
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

#line 867
void _ShowStartGlass(
    DWORD dwTimeout,
    PPROCESSINFO currentProcess)
{
#line 870
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



#line 43
#if DBG
#line 44
#error Excluded unsupported original DBG branch
#line 49
#endif
#line 50

#line 2962
#ifdef LOCK_MOUSE_CODE
#line 2963
#error Excluded unsupported original LOCK_MOUSE_CODE branch
#endif
#line 2965

#line 1480
#ifdef DEBUG
#line 1481
#error Excluded unsupported original DEBUG branch
#line 1489
#endif
#line 1490

#line 1518
#if DBG
#line 1519
#error Excluded unsupported original DBG branch
#endif
#line 1521

#line 1555
#if DBG
#line 1556
#error Excluded unsupported original DBG branch
#line 1558
#endif
#line 1559

#line 1598
#if DBG
#line 1599
#error Excluded unsupported original DBG branch
#line 1608
#endif
#line 1609

#line 1626
#ifdef DEBUG
#line 1627
#error Excluded unsupported original DEBUG branch
#line 1635
#endif
#line 1636

#line 1784
#if DBG
#line 1785
#error Excluded unsupported original DBG branch
#line 1787
#endif
#line 1788

#line 2275
#if DBG
#line 2276
#error Excluded unsupported original DBG branch
#endif // DBG
#line 2278

#line 2282
#if DBG
#line 2283
#error Excluded unsupported original DBG branch
#line 2288
#endif // DBG
#line 2289

#line 2322
#if DBG
#line 2323
#error Excluded unsupported original DBG branch
#endif // DBG
#line 2325

#line 2355
#if DBG
#line 2356
#error Excluded unsupported original DBG branch
#endif // DBG
#line 2358

#line 2375
#if DBG
#line 2376
#error Excluded unsupported original DBG branch
#endif // DBG
#line 2378

#line 2385
#if DBG
#line 2386
#error Excluded unsupported original DBG branch
#endif // DBG
#line 2388

#line 2649
#ifdef DEBUG
#line 2650
#error Excluded unsupported original DEBUG branch
#line 2657
#endif
#line 2658

#line 3705
#if DBG
#line 3706
#error Excluded unsupported original DBG branch
#line 3727
#endif
#line 3728


#line 992
#ifdef FE_IME
#line 993
#error Unsupported FE_IME original branch
#line 996
#endif
#line 997

#line 1915
#ifdef FE_IME
#line 1916
#error Unsupported FE_IME original branch
#line 1920
#endif
#line 1921

#line 957
#ifdef FE_IME
#line 958
#error Unsupported FE_IME original branch
#else
#line 960
#line 961
#endif
#line 962

#line 963
#ifdef FE_IME
#line 964
#error Unsupported FE_IME original branch
#else
#line 966
#line 967
#endif
#line 968

#line 1203
#ifdef FE_IME
#line 1204
#error Unsupported FE_IME original branch
#line 1208
#else
#line 1209
#line 1210
#endif
#line 1211
