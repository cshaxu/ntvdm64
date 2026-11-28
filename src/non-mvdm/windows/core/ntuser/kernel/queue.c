/****************************** Module Header ******************************\
* Module Name: queue.c
*
* Copyright (c) 1985 - 1999, Microsoft Corporation
*
* This module contains the low-level code for working with the Q structure.
*
* History:
* 12-02-90 DavidPe      Created.
* 02-06-91 IanJa        HWND revalidation added
\***************************************************************************/

#define WOW_ORIGINAL_TASK_ORDER
#include "wow_task_init_bindings.h"
#include "wow_user_exception_filter.h"
#define GetAppCompatFlags wow_kernel_GetAppCompatFlags
#define GetAppCompatFlags2 wow_kernel_GetAppCompatFlags2
#define GetAppImeCompatFlags wow_kernel_GetAppImeCompatFlags
#undef InitiateShutdown
#include "userk.h"
#line 14
#pragma hdrstop
#line 16
VOID DestroyProcessesObjects(PPROCESSINFO ppi);
VOID DestroyThreadsMessages(PQ pq, PTHREADINFO pti);
VOID CheckProcessForeground(PTHREADINFO pti);
DWORD xxxPollAndWaitForSingleObject(PKEVENT pEvent, PVOID pExecObject,
                                    DWORD dwMilliseconds);

NTSTATUS InitiateShutdown(PETHREAD Thread, PULONG lpdwFlags);
NTSTATUS EndShutdown(PETHREAD Thread, NTSTATUS StatusShutdown);
VOID SetVDMCursorBounds(LPRECT lprc);
NTSTATUS InitQEntryLookaside(VOID);
VOID SetAppStarting(PPROCESSINFO ppi);

#line 28
#if defined(_WIN64) 
/* 
 * For Win64 ask winlogon to play sounds for accessibility events
 * (IA64 machines do not have internal speakers so we have to go
 * thru the sound card).  Post a message to winlogon with an lParam
 * whose high word is ACCESS_SOUND_RANGE and low word is the index 
 * of the sound to make.  If a new RITSOUND_xx macro is added then
 * winlogon has to be updated too.
 */
#define ACCESS_SOUND_RANGE 1
#endif
#line 42
PW32PROCESS gpwpCalcFirst;

PPAGED_LOOKASIDE_LIST QLookaside;
PPAGED_LOOKASIDE_LIST QEntryLookaside;

#line 685
#define QUERY_VALUE_BUFFER 80
#line 695
PUNICODE_STRING gpastrSetupExe;    // These are initialized in the routine
int giSetupExe;                    // CreateSetupNameArray in setup.c


#line 709
BOOL SetAppImeCompatFlags(
    PTHREADINFO pti,
    PUNICODE_STRING pstrModName,
    PUNICODE_STRING pstrBaseFileName)
{
    DWORD dwImeFlags = 0;
    WCHAR szHex[QUERY_VALUE_BUFFER];
    WORD wPrimaryLangID;
    LCID lcid;
    int iSetup;
    BOOL fSetup = FALSE;
    int iAppName;
    int cAppNames;
    PUNICODE_STRING rgpstrAppNames[2];
    UNICODE_STRING strHex;

    /*
     * Because can't access pClientInfo of another process
     */
    UserAssert(pti->ppi == PpiCurrent());

    /*
     * Because it is used as a zero-terminated profile key name.
     */
    UserAssert(pstrModName->Buffer[pstrModName->Length / sizeof(WCHAR)] == 0);

    if (FastGetProfileStringW(
                NULL,
                PMAP_IMECOMPAT,
                pstrModName->Buffer,
                NULL,
                szHex,
                ARRAY_SIZE(szHex),
                0)) {

        /*
         * Found some flags. Attempt to convert the hex string
         * into numeric value. Specify base 0, so
         * RtlUnicodeStringToInteger will handle the 0x format.
         */
        RtlInitUnicodeString(&strHex, szHex);
        RtlUnicodeStringToInteger(&strHex, 0, (PULONG)&dwImeFlags);
    }

    /*
     * If current layout is not IME layout, we don't need to get
     * compatible flags for IME. But now, we don't have any scheme
     * to get this flags when the keyboard layout is switched. Then
     * we get it here, even this flags are not nessesary for non-IME
     * keyboard layouts.
     */
    ZwQueryDefaultLocale(FALSE, &lcid);
    wPrimaryLangID = PRIMARYLANGID(lcid);

    if ((wPrimaryLangID == LANG_KOREAN || wPrimaryLangID == LANG_JAPANESE) &&
            (LOWORD(pti->dwExpWinVer) <= VER31)) {
        /*
         * IME compatibility flags are needed even if it's a 32 bit app.
         */
        pti->ppi->dwImeCompatFlags = dwImeFlags;
    } else {
        pti->ppi->dwImeCompatFlags = dwImeFlags & (IMECOMPAT_NOFINALIZECOMPSTR | IMECOMPAT_HYDRACLIENT);
        if (dwImeFlags & IMECOMPAT_NOFINALIZECOMPSTR) {
            RIPMSG1(RIP_WARNING, "IMECOMPAT_NOFINALIZECOMPSTR is set to ppi=%#p", pti->ppi);
        }
        if (dwImeFlags & IMECOMPAT_HYDRACLIENT) {
            RIPMSG1(RIP_WARNING, "IMECOMPAT_HYDRACLIENT is set to ppi=%#p", pti->ppi);
        }
    }


    if (gpastrSetupExe == NULL) {
        return fSetup;
    }

    rgpstrAppNames[0] = pstrModName;
    cAppNames = 1;
    if (pstrBaseFileName) {
        rgpstrAppNames[1] = pstrBaseFileName;
        cAppNames = 2;
    }

    for (iAppName = 0;
         iAppName < cAppNames && !fSetup;
         iAppName++) {

        iSetup = 0;
        while (iSetup < giSetupExe) {
            if (RtlCompareUnicodeString(rgpstrAppNames[iAppName], &(gpastrSetupExe[iSetup]), TRUE) == 0) {
                fSetup = TRUE;
                break;
            }
            iSetup++;
        }
    }

    return fSetup;
}

#line 974
DWORD GetAppCompatFlags(
    PTHREADINFO pti)
{
    // From GRE with pti = NULL
    // We got to use PtiCurrentShared()
    if (pti == NULL)
        pti = PtiCurrentShared();

    return pti->dwCompatFlags;
}
/***************************************************************************\
* GetAppCompatFlags2
*
* Compatibility flags for < wVer apps
*
* History:
* 07-01-98 MCostea      Created.
\***************************************************************************/
DWORD GetAppCompatFlags2(
    WORD wVer)
{
    return GetAppCompatFlags2ForPti(PtiCurrentShared(), wVer);
}

DWORD GetAppImeCompatFlags(
    PTHREADINFO pti)
{
    if (pti == NULL) {
        pti = PtiCurrentShared();
    }

    return pti->ppi->dwImeCompatFlags;
}


#line 1057
VOID SetAppStarting(
    PPROCESSINFO ppi)
{
    static UINT_PTR guAppStartingId = 0;

    /*
     * This ppi had better not be in the list already, or we will be creating
     * a loop (as seen in stress).
     */
    UserAssert((ppi->W32PF_Flags & W32PF_APPSTARTING) == 0);

    /*
     * If we add this to the gppiStartingList without this bit set, we will
     * skip removing it from the list in DestroyProcessInfo(), but continue
     * to free it in FreeW32Process called by W32pProcessCallout.
     */
    UserAssert((ppi->W32PF_Flags & W32PF_PROCESSCONNECTED));

    ppi->W32PF_Flags |= W32PF_APPSTARTING;
    ppi->ppiNext = gppiStarting;
    gppiStarting = ppi;
    /*
     * Some system processes are initialized before the RIT has setup the master
     * timer, so check for it.
     */
#line 1086

}

#define PROCESSINFO wow_task_order_process
#line 1098
VOID ClearAppStarting(
    PPROCESSINFO ppi)
{
    REMOVE_FROM_LIST(PROCESSINFO, gppiStarting, ppi, ppiNext);
    ppi->W32PF_Flags &= ~W32PF_APPSTARTING;
}

#undef PROCESSINFO

#line 1114
NTSTATUS zzzInitTask(
    UINT dwExpWinVer,
    DWORD dwAppCompatFlags,
    DWORD dwUserWOWCompatFlags,
    PUNICODE_STRING pstrModName,
    PUNICODE_STRING pstrBaseFileName,
    DWORD hTaskWow,
    DWORD dwHotkey,
    DWORD idTask,
    DWORD dwX,
    DWORD dwY,
    DWORD dwXSize,
    DWORD dwYSize)
{
    PTHREADINFO ptiCurrent;
    PTDB ptdb;
    PPROCESSINFO ppi;
    PWOWTHREADINFO pwti;

    ptiCurrent = PtiCurrent();
    ppi = ptiCurrent->ppi;

    /*
     * Set the real name of the module. (Instead of 'NTVDM')
     * We've already probed pstrModName->Buffer for Length+sizeof(WCHAR) so
     * we can copy the UNICODE_NULL terminator as well.
     */
    if (ptiCurrent->pstrAppName != NULL) {
        UserFreePool(ptiCurrent->pstrAppName);
    }
    ptiCurrent->pstrAppName = UserAllocPoolWithQuota(sizeof(UNICODE_STRING) +
            pstrModName->Length + sizeof(WCHAR), TAG_TEXT);
    if (ptiCurrent->pstrAppName != NULL) {
        ptiCurrent->pstrAppName->Buffer = (PWCHAR)(ptiCurrent->pstrAppName + 1);
        try {
            RtlCopyMemory(ptiCurrent->pstrAppName->Buffer, pstrModName->Buffer,
                    pstrModName->Length);
            ptiCurrent->pstrAppName->Buffer[pstrModName->Length / sizeof(WCHAR)] = 0;
        } except (W32ExceptionHandler(FALSE, RIP_WARNING)) {
            UserFreePool(ptiCurrent->pstrAppName);
            ptiCurrent->pstrAppName = NULL;
            return STATUS_OBJECT_NAME_INVALID;
        }
        ptiCurrent->pstrAppName->MaximumLength = pstrModName->Length + sizeof(WCHAR);
        ptiCurrent->pstrAppName->Length = pstrModName->Length;
    } else
        return STATUS_OBJECT_NAME_INVALID;

    /*
     * An app is starting!
     */
    if (!(ppi->W32PF_Flags & W32PF_APPSTARTING)) {
        SetAppStarting(ppi);
    }

    /*
     * We never want to use the ShowWindow defaulting mechanism for WOW
     * apps. If STARTF_USESHOWWINDOW was set in the client-side
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

    if ((ptdb = (PTDB)UserAllocPoolWithQuota(sizeof(TDB), TAG_WOWTDB)) == NULL)
        return STATUS_NO_MEMORY;

    ptiCurrent->ptdb = ptdb;

    /*
     * Save away the 16 bit task handle: we use this later when calling
     * wow back to close a WOW task.
     */
    ptdb->hTaskWow = LOWORD(hTaskWow);

    ptiCurrent->dwCompatFlags = dwAppCompatFlags;
    ptiCurrent->pClientInfo->dwCompatFlags = dwAppCompatFlags;

    UserAssert(ptiCurrent->ppi->ptiList);

    dwUserWOWCompatFlags &= COMPATFLAGS2_FORWOW;
    ptiCurrent->dwCompatFlags2 = dwUserWOWCompatFlags;
    ptiCurrent->pClientInfo->dwCompatFlags2 = dwUserWOWCompatFlags;

    /*
     * HIWORD: != 0 if wants proportional font
     * LOWORD: Expected windows version (3.00 [300], 3.10 [30A], etc)
     */
    ptiCurrent->dwExpWinVer = dwExpWinVer;
    ptiCurrent->pClientInfo->dwExpWinVer = dwExpWinVer;

    /*
     * We haven't captured pstrBaseFileName's buffer, we
     * may fault touching it in SetAppImeCompatFlags. If
     * so the IME flags have been set already and we
     * can safely assume it's not a setup app.
     */

    try {
        if (SetAppImeCompatFlags(ptiCurrent, ptiCurrent->pstrAppName,
                             pstrBaseFileName)) {
            /*
             * Flag task as a setup app.
             */
            ptdb->TDB_Flags = TDBF_SETUP;
            ppi->W32PF_Flags |= W32PF_SETUPAPP;
        }
    } except (W32ExceptionHandler(FALSE, RIP_WARNING)) {
    }


    /*
     * Set the flags to say this is a TIF_MEOW
     * queues!
     */

    if(hTaskWow & HTW_ISMEOW) {
       ptiCurrent->TIF_flags |= TIF_MEOW;
    }

    if(hTaskWow & HTW_IS16BIT) {
       ptiCurrent->TIF_flags |= TIF_16BIT | TIF_FIRSTIDLE;
    }
    else {
       ptiCurrent->pClientInfo->dwTIFlags |= ptiCurrent->TIF_flags;
    }

    if(hTaskWow & HTW_IS16BIT) {

       /* jarbats
        * Set the flags to say this is a 16-bit thread - before attaching
        * queues!
        */

       /*
        * If this task is running in the shared WOW VDM, we handle
        * WaitForInputIdle a little differently than separate WOW
        * VDMs. This is because CreateProcess returns a real process
        * handle when you start a separate WOW VDM, so the "normal"
        * WaitForInputIdle works. For the shared WOW VDM, CreateProcess
        * returns an event handle.
        */
        ptdb->pwti = NULL;
        if (idTask) {
            ptiCurrent->TIF_flags |= TIF_SHAREDWOW;

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
#if DBG
#error
#line 1308
#endif
#line 1309
            }
        }
        ptiCurrent->pClientInfo->dwTIFlags |= ptiCurrent->TIF_flags;

        /*
         * We need this thread to share the queue of other win16 apps.
         * If we're journalling, all apps are sharing a queue, so we wouldn't
         * want to interrupt that - so only cause queue recalculation
         * if we aren't journalling.
         * ptdb may be freed by DestroyTask during a callback, so defer WinEvent
         * notifications until we don't need ptdb any more.
         */

        if (!FJOURNALRECORD() && !FJOURNALPLAYBACK())
            zzzReattachThreads(FALSE);

        /*
         * Setup the app start cursor for 5 second timeout.
         */
        zzzCalcStartCursorHide((PW32PROCESS)ppi, 5000);

        /*
         * Mark this guy and add him to the global task list so he can run.
         */
        #define NORMAL_PRIORITY_TASK 10

        /*
         * To be Compatible it super important that the new task run immediately
         * Set its priority accordingly. No other task should ever be set to
         * CREATION priority
         */
        ptdb->nPriority = NORMAL_PRIORITY_TASK;
        ptdb->nEvents = 0;
        ptdb->pti = ptiCurrent;
        ptdb->ptdbNext = NULL;
        ptdb->TDB_Flags = 0;

        InsertTask(ppi, ptdb);



        /*
         * Force this new task to be the active task (WOW will ensure the
         * currently running task does a Yield which will put it into the
         * non preemptive scheduler.
         */
        ppi->pwpi->ptiScheduled = ptiCurrent;
        ppi->pwpi->CSLockCount = -1;

        EnterWowCritSect(ptiCurrent, ppi->pwpi);

        /*
         * ensure app gets focus
         */
        zzzShowStartGlass(10000);
    }

    return STATUS_SUCCESS;
}

#line 1377
VOID zzzShowStartGlass(
    DWORD dwTimeout)
{
    PPROCESSINFO ppi;

    /*
     * If this is the first call to zzzShowStartGlass(), then the
     * W32PF_ALLOWFOREGROUNDACTIVATE bit has already been set in the process
     * info - we don't want to set it again because it may have been
     * purposefully cleared when the user hit a key or mouse clicked.
     */
    ppi = PpiCurrent();
    if (ppi->W32PF_Flags & W32PF_SHOWSTARTGLASSCALLED) {
        /*
         * Allow this wow app to come to the foreground. This'll be cancelled
         * if the user mouse clicks or hits any keys.
         */
        SET_PUDF(PUDF_ALLOWFOREGROUNDACTIVATE);
        TAGMSG0(DBGTAG_FOREGROUND, "zzzShowStartGlass set PUDF");
        ppi->W32PF_Flags |= W32PF_ALLOWFOREGROUNDACTIVATE;
        TAGMSG1(DBGTAG_FOREGROUND, "zzzShowStartGlass set W32PF %#p", ppi);
}
    ppi->W32PF_Flags |= W32PF_SHOWSTARTGLASSCALLED;

    /*
     * Show the start glass cursor for this much longer.
     */
    zzzCalcStartCursorHide((PW32PROCESS)ppi, dwTimeout);
}

#line 1453
VOID ClearQueueServerEvent(
    WORD wWakeMask)
{
    PTHREADINFO ptiCurrent = PtiCurrent();
    UserAssert(wWakeMask != 0);
    ptiCurrent->pcti->fsWakeMask = wWakeMask;
    KeClearEvent(ptiCurrent->pEventQueueServer);
}

#line 1462
ULONG ParseReserved(
    WCHAR *cczpReserved,
    WCHAR *pchFind)
{
    ULONG dw;
    WCHAR *cczpch, *cczpchT, ch;
    UNICODE_STRING cczuString;

    dw = 0;
    try {
        if (cczpReserved != NULL && (cczpch = wcsstr(cczpReserved, pchFind)) != NULL) {
            cczpch += wcslen(pchFind);

            cczpchT = cczpch;
            while (*cczpchT >= '0' && *cczpchT <= '9')
                cczpchT++;

            ch = *cczpchT;
            *cczpchT = 0;
            RtlInitUnicodeString(&cczuString, cczpch);
            *cczpchT = ch;

            RtlUnicodeStringToInteger(&cczuString, 0, &dw);
        }
    } except (W32ExceptionHandler(FALSE, RIP_WARNING)) {
        return 0;
    }

    return dw;
}

