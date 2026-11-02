/*++
 *
 *  WOW v1.0
 *
 *  Copyright (c) 1991, Microsoft Corporation
 *
 *  WOW32.C
 *  WOW32 16-bit API support
 *
 *  History:
 *  Created 27-Jan-1991 by Jeff Parsons (jeffpar)
 *  Multi-Tasking 23-May-1991 Matt Felton [mattfe]
 *  WOW as DLL 06-Dec-1991 Sudeep Bharati (sudeepb)
 *  Cleanup and rework multi tasking feb 6 (mattfe)
 *  added notification thread for task creation mar-11 (mattfe)
 *  added basic exception handling for retail build apr-3 92 mattfe
 *  use host_ExitThread apr-17 92 daveh
 *  Hung App Support june-22 82 mattfe
--*/

#include "precomp.h"
/* DIVERGENCE(MVDM-HOST-DIV-296): native private USER aliases retain their
 * storage across original thunk calls, including reentry and exception unwind. */
#include "wow_user_thunk_scope.h"
#pragma hdrstop
#include "wktbl.h"
#include "wutbl.h"
#include "wgtbl.h"
#include "wstbl.h"
#include "wkbtbl.h"
#include "wshltbl.h"
#include "wmmtbl.h"
#include "wsocktbl.h"
#include "wthtbl.h"
#include <stdarg.h>
#include <ntlpcapi.h>
#include <ntcsrdll.h>
#define SHAREWOW_MAIN
#include <sharewow.h>
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_fast_bop.h"
/* This selected original read path retains its query/fallback body while
 * using the worker-local NTVDM.REG/system snapshot facade. */
#include <mvdm_shadow_registry.h>
/* This TU needs only the Win32 registry-reader redirection. */
#undef NtOpenKey
#undef NtEnumerateValueKey
#undef NtQueryValueKey
#undef NtClose


/* Function Prototypes */
DWORD   W32SysErrorBoxThread2(PTDB pTDB);
VOID    StartDebuggerForWow(VOID);
BOOLEAN LoadCriticalStringResources(void);


extern DECLSPEC_IMPORT ULONG *ExpLdt;
#define LDT_DESC_PRESENT 0x8000
#define STD_SELECTOR_BITS 0x7

MODNAME(wow32.c);

#define REGISTRY_BUFFER_SIZE 512

// for logging iloglevel to a file
#ifdef DEBUG
#error Excluded unsupported DEBUG WOW profile
#endif
#line 59

/*  iloglevel = 16 MAX the world (all 16 bit kernel internal calls
 *  iloglevel = 14 All internal WOW kernel Calls
 *  ilogeveel = 12 All USER GDI call + return Codes
 *  iloglevel = 5  Returns From Calls
 *  iloglevel = 3  Calling Parameters
 */
INT     flOptions = 0;           // command line optin
#ifdef DEBUG
#error Excluded unsupported DEBUG WOW profile
#endif
#line 71

HANDLE  hmodWOW32;
HANDLE  hHostInstance = 0;
#ifdef DEBUG
#error Excluded unsupported DEBUG WOW profile
#endif
#line 78
#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-589: historical i386 WOW branch is not selected
#endif
#line 81

#ifdef DEBUG
#error Excluded unsupported DEBUG WOW profile
#endif
#line 92

UINT    iW32ExecTaskId = (UINT)-1;    // Base Task ID of Task Being Exec'd
WORD    iWOWTaskCur = 0;        // pTDB of currently running task ID
UINT    nWOWTasks = 0 ;         // # of WOW tasks running
BOOL    fBoot = TRUE;           // TRUE During the Boot Process
HANDLE  ghevWaitCreatorThread = (HANDLE)-1; // Used to Syncronize creation of a new thread


BOOL    fWowMode = FALSE;   // Flag used to determine wow mode.
                // currently defaults to FALSE (real mode wow)
                // This is used by the memory access macros
                // to properly form linear addresses.
                // When running on an x86 box, it will be
                // initialized to the mode the first wow
                // bop call is made in.  This flag can go
                // away when we no longer want to run real
                // mode wow.  (Daveh 7/25/91)

HANDLE hSharedTaskMemory;
DWORD  dwSharedProcessOffset;
HANDLE hWOWHeap;
HANDLE ghProcess;       // WOW Process Handle
PFNWOWHANDLERSOUT pfnOut;
PTD *  pptdWOA = NULL;
PTD    gptdShell = NULL;
DWORD  fThunkStrRtns;           // used as a BOOL
BOOL   gfDebugExceptions = FALSE;  // set to 1 in debugger to
                                   // enable debugging of W32Exception
BOOL   gfIgnoreInputAssertGiven = FALSE;

#ifndef _X86_
PUCHAR IntelMemoryBase;  // Start of emulated CPU's memory
#endif


DWORD   gpsi = 0;

#define TOOLONGLIMIT     _MAX_PATH
#define WARNINGMSGLENGTH 255

PSZ aszCriticalStrings[CRITICAL_STRING_COUNT];

char szEmbedding[] =        "embedding";
char szDevices[] =          "devices";
char szBoot[] =             "boot";
char szShell[] =            "shell";
char szServerKey[] =        "protocol\\StdFileEditing\\server";
char szPicture[] =          "picture";
char szPostscript[] =       "postscript";
char szZapfDingbats[] =     "ZAPFDINGBATS";
char szZapf_Dingbats[] =    "ZAPF DINGBATS";
char szSymbol[] =           "SYMBOL";
char szTmsRmn[] =           "TMS RMN";
char szHelv[] =             "HELV";
char szMavisCourier[]=      "MAVIS BEACON COURIER FP";
char szWinDotIni[] =        "win.ini";
char szSystemDotIni[] =     "system.ini";
char szExplorerDotExe[] =   "Explorer.exe";
char szDrWtsn32[] =         "drwtsn32";
PSTR pszWinIniFullPath;
PSTR pszWindowsDirectory;
PSTR pszSystemDirectory;

extern BOOL GdiReserveHandles(VOID);
extern CRITICAL_SECTION VdmLoadCritSec;
extern LIST_ENTRY TimerList;

#if defined (_X86_)
#error Unsupported excluded original configuration
#endif
#line 192

BOOLEAN
W32DllInitialize(
    IN PVOID DllHandle,
    IN ULONG Reason,
    IN PCONTEXT Context OPTIONAL
    )

/*++

Routine Description:  DllMain function called during ntvdm's
                      LoadLibrary("wow32")

Arguments:

    DllHandle - set global hmodWOW32

    Reason - Attach or Detach

    Context - Not Used

Return Value:

    STATUS_SUCCESS

--*/

{
    UNREFERENCED_PARAMETER(Context);

    hmodWOW32 = DllHandle;

    switch ( Reason ) {

    case DLL_PROCESS_ATTACH:

        if (!CreateSmallHeap()) {
            return FALSE;
        }

        if ((hWOWHeap = HeapCreate (0,
                    INITIAL_WOW_HEAP_SIZE,
                    GROW_HEAP_AS_NEEDED)) == NULL)
            return FALSE;

        // initialize hook stubs data.

        W32InitHookState(hmodWOW32);

        // initialize the thunk table offsets.  do it here so the debug process
        // gets them.

        InitThunkTableOffsets();

        //
        // initialization for named pipe handling in file thunks
        //

        InitializeCriticalSection(&VdmLoadCritSec);

        //
        // Load Critical Error Strings
        //

        if (!LoadCriticalStringResources()) {
            MessageBox(NULL, "The Win16 subsystem could not load critical string resources from wow32.dll, terminating.",
                       "Win16 subsystem load failure", MB_ICONEXCLAMATION | MB_OK);
        }

        W32EWExecer();

        InitializeListHead(&TimerList);
        break;

    case DLL_THREAD_ATTACH:
        IsDebuggerAttached();   // Yes, this routine has side-effects.
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        /*
         * Tell base he can nolonger callback to us.
         */
        RegisterWowBaseHandlers(NULL);

        HeapDestroy (hWOWHeap);
        break;

    default:
        break;
    }

    return TRUE;
}


BOOLEAN
LoadCriticalStringResources(
    void
    )

/*++

Routine Description:  Loads strings we want around even if we can't allocate
                      memory.  Called during wow32 DLL load.

Arguments:

    none

Return Value:

    TRUE if all strings loaded and aszCriticalStrings initialized.

--*/

{
    int n;
    PSZ psz, pszStringBuffer;
    DWORD cbTotal;
    DWORD cbUsed;
    DWORD cbStrLen;

    //
    // Allocate too much memory for strings (maximum possible) at first,
    // reallocate to the real size when we're done loading strings.
    //

    cbTotal = CRITICAL_STRING_COUNT * CCH_MAX_STRING_RESOURCE;

    psz = pszStringBuffer = malloc_w(cbTotal);

    if ( ! psz ) {
        return FALSE;
    }

    cbUsed = 0;

    for ( n = 0; n < CRITICAL_STRING_COUNT; n++ ) {

        //
        // LoadString return value doesn't count null terminator.
        //

        cbStrLen = LoadString(hmodWOW32, n, psz, CCH_MAX_STRING_RESOURCE);

        if ( ! cbStrLen ) {
            return FALSE;
        }

        aszCriticalStrings[n] = psz;

        psz += cbStrLen + 1;
        cbUsed += cbStrLen + 1;

    }

    realloc_w(pszStringBuffer, cbUsed, HEAP_REALLOC_IN_PLACE_ONLY);

    return TRUE;
}


//***************************************************************************
// Continues ExitWindowsExec api call after logoff and subsequent logon
// Uses Events to synchronize across all wow vdms
//
//***************************************************************************

BOOL W32EWExecer(VOID)
{
    STARTUPINFO StartupInfo;
    PROCESS_INFORMATION ProcessInformation;
    BOOL CreateProcessStatus;
    BYTE abT[REGISTRY_BUFFER_SIZE];

    if (W32EWExecData(EWEXEC_QUERY, (LPSTR)abT, sizeof(abT))) {
        HANDLE hevT;
        if (hevT = CreateEvent(NULL, TRUE, FALSE, WOWSZ_EWEXECEVENT)) {
            if (GetLastError() == 0) {
                W32EWExecData(EWEXEC_DEL, (LPSTR)NULL, 0);

                LOGDEBUG(0, ("WOW:Execing dos app -  %s\r\n", abT));
                RtlZeroMemory((PVOID)&StartupInfo, (DWORD)sizeof(StartupInfo));
                StartupInfo.cb = sizeof(StartupInfo);
                StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
                StartupInfo.wShowWindow = SW_NORMAL;

                CreateProcessStatus = CreateProcess(
                                        NULL,
                                        abT,
                                        NULL,               // security
                                        NULL,               // security
                                        FALSE,              // inherit handles
                                        CREATE_NEW_CONSOLE | CREATE_DEFAULT_ERROR_MODE,
                                        NULL,               // environment strings
                                        NULL,               // current directory
                                        &StartupInfo,
                                        &ProcessInformation
                                        );

                if (CreateProcessStatus) {
                    WaitForSingleObject(ProcessInformation.hProcess, INFINITE);
                    CloseHandle( ProcessInformation.hProcess );
                    CloseHandle( ProcessInformation.hThread );
                }

                SetEvent(hevT);
            }
            else {
                WaitForSingleObject(hevT, INFINITE);
            }

            CloseHandle(hevT);
        }
    }
    return 0;
}

//***************************************************************************
// W32EWExecData -
//   sets/resets the 'commandline', ie input to ExitWindowssExec api in the
//   registry - 'WOW' key 'EWExec' value
//
//***************************************************************************

BOOL W32EWExecData(DWORD fnid, LPSTR lpData, DWORD cb)
{
    BOOL bRet = FALSE;
    BYTE abT[REGISTRY_BUFFER_SIZE];


    switch (fnid) {
        case EWEXEC_SET:
            bRet = WriteProfileString(WOWSZ_EWEXECVALUE,
                                         WOWSZ_EWEXECVALUE,
                                           lpData);
            break;

        case EWEXEC_DEL:
            bRet = WriteProfileString(WOWSZ_EWEXECVALUE,
                                          NULL, NULL);
            break;

        case EWEXEC_QUERY:
            if (bRet = GetProfileString(WOWSZ_EWEXECVALUE,
                                           WOWSZ_EWEXECVALUE,
                                             "", abT, sizeof(abT))) {
                strcpy(lpData, abT);
            }

            break;

        default:
            WOW32ASSERT(FALSE);
            break;
    }
    return !!bRet;
}



/* W32Init - Initialize WOW support
 *
 * ENTRY
 *
 * EXIT
 *  TRUE if successful, FALSE if not
 */


BOOL W32Init(VOID)
{
    HKEY  WowKey = NULL;
#ifdef DEBUG
#error DIVERGENCE MVDM-HOST-DIV-595: DEBUG initialization profile is not selected
#endif
#line 475
    PTD ptd;
    PFNWOWHANDLERSIN pfnIn;
    LPVOID lpSharedTaskMemory;
#ifdef _X86_
#error Unsupported excluded original configuration
#endif                                             // UP.
#line 481

#ifndef _X86_
    //
    // This is the one and only call to Sim32GetVDMPointer in WOW32.
    // All other cases should use WOWGetVDMPointer.  This one is necessary
    // to set up the base memory address used by GetRModeVDMPointerMacro.
    // (There's also a call in GetPModeVDMPointerAssert, but that's in
    // the checked build only and only as a fallback mechanism.)
    //

    IntelMemoryBase = Sim32GetVDMPointer(0,0,0);
#endif

    fWowMode = ((getMSW() & MSW_PE) ? TRUE : FALSE);

    // Boost the HourGlass

    ShowStartGlass(10000);

    //
    // Set up a global WindowsDirectory to be used by other WOW functions.
    //

    {
        char szBuf[ MAX_PATH ];
        int cb;

        /* DIVERGENCE(MVDM-HOST-DIV-259): preserve the original directory
         * publication and short-name normalization, but source only its
         * directory spelling from the current worker's selected MVDM media.
         * Modern GetSystemDirectory/GetWindowsDirectory identify the host OS,
         * not this standalone product package. */
        if (!NtvdmGetSystemDirectoryA(szBuf, sizeof szBuf)) {
            return FALSE;
        }
        GetShortPathName(szBuf, szBuf, sizeof szBuf);
        cb = strlen(szBuf) + 1;
        pszSystemDirectory = malloc_w_or_die(cb);
        RtlCopyMemory(pszSystemDirectory, szBuf, cb);

        if (!NtvdmGetWindowsDirectoryA(szBuf, sizeof szBuf)) {
            return FALSE;
        }
        GetShortPathName(szBuf, szBuf, sizeof szBuf);
        cb = strlen(szBuf) + 1;
        pszWindowsDirectory = malloc_w_or_die(cb);
        RtlCopyMemory(pszWindowsDirectory, szBuf, cb);

        pszWinIniFullPath = malloc_w_or_die(cb + 8);   // "\win.ini"
        RtlCopyMemory(pszWinIniFullPath, szBuf, cb);
        pszWinIniFullPath[ cb - 1 ] = '\\';
        RtlCopyMemory(pszWinIniFullPath + cb, szWinDotIni, 8);
    }

    // Give USER32 our entry points

    pfnIn.pfnLocalAlloc = W32LocalAlloc;
    pfnIn.pfnLocalReAlloc = W32LocalReAlloc;
    pfnIn.pfnLocalLock = W32LocalLock;
    pfnIn.pfnLocalUnlock = W32LocalUnlock;
    pfnIn.pfnLocalSize = W32LocalSize;
    pfnIn.pfnLocalFree = W32LocalFree;
    pfnIn.pfnGetExpWinVer = W32GetExpWinVer;
    pfnIn.pfnInitDlgCb = W32InitDlg;
    pfnIn.pfn16GlobalAlloc = W32GlobalAlloc16;
    pfnIn.pfn16GlobalFree = W32GlobalFree16;
    pfnIn.pfnEmptyCB = W32EmptyClipboard;
    pfnIn.pfnFindResourceEx = W32FindResource;
    pfnIn.pfnLoadResource = W32LoadResource;
    pfnIn.pfnFreeResource = W32FreeResource;
    pfnIn.pfnLockResource = W32LockResource;
    pfnIn.pfnUnlockResource = W32UnlockResource;
    pfnIn.pfnSizeofResource = W32SizeofResource;
    pfnIn.pfnWowWndProcEx = (PFNWOWWNDPROCEX)W32Win16WndProcEx;
    pfnIn.pfnWowEditNextWord = W32EditNextWord;
    pfnIn.pfnWowSetFakeDialogClass = SetFakeDialogClass;
    pfnIn.pfnWowCBStoreHandle = WU32ICBStoreHandle;

    gpsi = UserRegisterWowHandlers(&pfnIn, &pfnOut);

    RegisterWowBaseHandlers(W32DDEFreeGlobalMem32);

    // Prepare us to be in the shared memory process list

    lpSharedTaskMemory = LOCKSHAREWOW();

    WOW32ASSERTMSG(lpSharedTaskMemory, "WOW32: Could not access shared memory object\n");

    if ( lpSharedTaskMemory ) {
        UNLOCKSHAREWOW();
    }

    CleanseSharedList();
    AddProcessSharedList();

    // Allocate a Temporary TD for the first thread

    ptd = CURRENTPTD() = malloc_w_or_die(sizeof(TD));

    RtlZeroMemory(ptd, sizeof(*ptd));

    // Create Global Wait Event - Used During Task Creation To Syncronize with New Thread

    if (!(ghevWaitCreatorThread = CreateEvent(NULL, FALSE, FALSE, NULL))) {
        LOGDEBUG(0,("    W32INIT ERROR: event creation failure\n"));
        return FALSE;
    }


    if (RegOpenKeyEx ( HKEY_LOCAL_MACHINE,
               "SYSTEM\\CurrentControlSet\\Control\\WOW",
               0,
               KEY_QUERY_VALUE,
               &WowKey
             ) != 0) {
#ifdef DEBUG
#error DIVERGENCE MVDM-HOST-DIV-595: DEBUG initialization profile is not selected
#else
#line 600
        /* An absent section denotes the original empty-KnownDLL configuration.
         * The worker-local facade never creates or mutates host state. */
        WowKey = NULL;
#endif
#line 604
    }

    //
    // If present (it usually isn't) read ThunkNLS value entry.
    //

#ifdef DEBUG
#error DIVERGENCE MVDM-HOST-DIV-595: DEBUG initialization profile is not selected
#else
#line 619
    {
#endif
#line 621

        //
        // Didn't find the registry value or it's the wrong type,
        // so we use the default behavior which is to thunk outside the
        // US.
        //

        fThunkStrRtns = GetSystemDefaultLCID() !=
                            MAKELCID(
                                MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                                SORT_DEFAULT
                                );
    }

#ifdef DEBUG
#error Excluded unsupported DEBUG WOW profile
#endif
#line 697

    //
    // Initialize list of known DLLs used by WK32WowIsKnownDLL
    // from the registry.
    //

    WK32InitWowIsKnownDLL(WowKey);

    if (WowKey != NULL) {
        RegCloseKey (WowKey);
    }
    // 
    // Initialize list of app names known to be setup applications
    //
    //

    W32InitWOWSetupNames();

    //
    // Initialize param mapping cache
    //
    //

    InitParamMap();

    //
    // Set our GDI batching limit from win.ini.  This is useful for SGA and
    // other performance measurements which require each API to do its own
    // work.  To set the batching size to 1, which is most common, put the
    // following in win.ini:
    //
    // [WOW]
    // BatchLimit=1
    //
    // or using ini:
    //
    // ini WOW.BatchLimit = 1
    //
    // Note that this code only changes the batch limit if the above
    // line is in win.ini, otherwise we use default batching.  It's
    // important that this code be in the free build to be useful.
    //

    {
        extern DWORD dwWOWBatchLimit;                    // declared in wkman.c

        dwWOWBatchLimit = GetProfileInt("WOW",           // section
                                        "BatchLimit",    // key
                                        0                // default if not found
                                        );
    }

    ghProcess = NtCurrentProcess();

#ifdef DEBUG
#error Excluded unsupported DEBUG WOW profile
#endif
#line 745

    // Initialize ClipBoard formats structure.

    InitCBFormats ();

    // This is to initialize the InquireVisRgn for FileMaker Pro 2.0
    // InquireVisRgn is an undocumented API Win 3.1 API.

    InitVisRgn();


    // HUNG APP SUPPORT

    if (!WK32InitializeHungAppSupport()) {
        LOGDEBUG(LOG_ALWAYS, ("W32INIT Error: InitializeHungAppSupport Failed"));
        return FALSE;
    }

    SetPriorityClass(ghProcess, NORMAL_PRIORITY_CLASS);

    return TRUE;
}

/*  Thunk Dispatch Table
 *
 *  see fastwow.h for instructions on how to create a new thunk table
 *
 */
#ifdef DEBUG_OR_WOWPROFILE
#error Excluded unsupported WOW profiling configuration
#endif
#line 803

#ifdef DEBUG_OR_WOWPROFILE // define symbols for API profiling only (debugger extension)
#error Excluded unsupported WOW profiling configuration
#endif // WOWPROFILE
#line 809


/* WOW32UnimplementedAPI - Error Thunk is Not Implemented
 *
 * Stub thunk table entry for all unimplemented APIs on
 * the checked build, and on the free build NOPAPI and
 * LOCALAPI entries point here as well.
 *
 * ENTRY
 *
 * EXIT
 *
 */

ULONG FASTCALL WOW32UnimplementedAPI(PVDMFRAME pFrame)
{
#ifdef DEBUG
#error Excluded unsupported DEBUG WOW profile
#else
#line 837
    UNREFERENCED_PARAMETER(pFrame);
#endif
#line 839
    return FALSE;
}


#ifdef DEBUG
#error Excluded unsupported DEBUG WOW profile
#endif // DEBUG
#line 896


LPFNW32 FASTCALL W32PatchCodeWithLpfnw32(PVDMFRAME pFrame , INT iFun )
{
    LPFNW32 lpfnW32;
    VPVOID vpCode;
    LPBYTE lpCode;

#ifdef DEBUG_OR_WOWPROFILE
#error Excluded unsupported WOW profiling configuration
#endif
#line 920

    //
    // just return the thunk function if called in real mode
    //
    if (!fWowMode) {
        return aw32WOW[iFun].lpfnW32;
    }

    // the thunk looks like so.
    //
    //    push HI_WCALLID (3bytes) - 0th byte is opcode.
    //    push 0xfnid     (3bytes)
    //    call wow16call  (5bytes)
    // ThunksCSIP:
    //

    // point to the 1st word (the hiword)
    vpCode = (DWORD)pFrame->wThunkCSIP - (0x5 + 0x3 + 0x2);

    WOW32ASSERT(HI_WCALLID == 0);  // we need to revisit wow32.c if this
                                   // value is changed to a non-zero value

    WOW32ASSERT(HIWORD(iFun) == HI_WCALLID);
    GETVDMPTR(vpCode, 0x2 + 0x3, lpCode);
    WOW32ASSERT(lpCode != NULL);

    WOW32ASSERT(*(PWORD16)(lpCode) == HIWORD(iFun));
    WOW32ASSERT(*(PWORD16)(lpCode+0x3) == LOWORD(iFun));

    lpfnW32 = aw32WOW[iFun].lpfnW32;
    iFun = (DWORD)lpfnW32;

    *((PWORD16)lpCode) = HIWORD(iFun);
    lpCode += 0x3;                                // seek to the 2nd word (the loword)
    *((PWORD16)lpCode) = LOWORD(iFun);

    FLUSHVDMCODEPTR(vpCode, 0x2 + 0x3, lpCode);
    FREEVDMPTR(lpCode);

    return lpfnW32;

}


/* W32Dispatch - Recipient of all WOW16 API calls
 *
 * This routine dispatches to the relavant WOW thunk routine via
 * jump tables wktbl.c wutbl.c wgtbl.c based on a function id on the 16 bit
 * stack.
 *
 * In debug versions it also calls routines to log parameters.
 *
 * ENTRY
 *  None (x86 registers contain parameters)
 *
 * EXIT
 *  None (x86 registers/memory updated appropriately)
 */
VOID W32Dispatch()
{
    INT iFun;
    ULONG ulReturn;
    /* DIVERGENCE(MVDM-HOST-DIV-337): a handled exception must complete
     * CallBack16's deferred CPU-stack pop, but not before a stack exists. */
    BOOL fStackEstablished = FALSE;
    VPVOID vpCurrentStack = 0; // DIV-348: filter arguments precede setup too
    register PTD ptd = NULL;
    register PVDMFRAME pFrame;
#ifdef DEBUG_OR_WOWPROFILE
#error Excluded unsupported WOW profiling configuration
#endif
#line 991

#ifdef  WOWPROFILE
#error Excluded unsupported WOW profiling configuration
#endif
#line 995

    //
    // WARNING: DO NOT ADD ANYTHING TO THIS FUNCTION UNLESS YOU ADD THE SAME
    // STUFF TO i386/FastWOW.asm.   i386/FastWOW.ASM is used for speedy
    // thunk dispatching on retail builds.
    //

    try {

        //
        // if we get here then even if we're going to be fastbopping
        // then the faststack stuff must not be enabled yet.  that's why
        // there's no #if FASTBOPPING for this fetching of the vdmstack
        //

        vpCurrentStack = VDMSTACK();                // Get 16 bit ss:sp

        // Use WOWGetVDMPointer here since we can get called in RealMode on
        // Errors

        pFrame = WOWGetVDMPointer(vpCurrentStack, sizeof(VDMFRAME), fWowMode);

        ptd = CURRENTPTD();                         // Setup Task Pointer
        ptd->vpStack = vpCurrentStack;              // Save 16 bit ss:sp
        fStackEstablished = TRUE;                 // DIV-337

        WOW32ASSERT( FIELD_OFFSET(TD,vpStack) == 0 );
        WOW32ASSERT( FIELD_OFFSET(TEB,WOW32Reserved) == 0xC0 );

        iFun = pFrame->wCallID;

#ifdef DEBUG_OR_WOWPROFILE
#error Excluded unsupported WOW profiling configuration
#endif
#line 1030
        iWOWTaskCur = pFrame->wTDB;                     // Record the task's current ID

        if (ISFUNCID(iFun)) {
#ifdef DEBUG
#error Excluded unsupported DEBUG WOW profile
#endif
#line 1040
            iFun = (INT)W32PatchCodeWithLpfnw32(pFrame, iFun);
        }


        LOGARGS(3,pFrame);                              // Perform Function Logging

#ifdef WOWPROFILE // For API profiling only (debugger extension)
#error Excluded unsupported WOW profiling configuration
#endif // WOWPROFILE
#line 1049

        //
        // WARNING: DO NOT ADD ANYTHING TO THIS FUNCTION UNLESS YOU ADD THE SAME
        // STUFF TO i386/FastWOW.asm.   i386/FastWOW.ASM is used for speedy
        // thunk dispatching on retail builds.
        //
        ulReturn = wow_user_invoke_thunk((LPFNW32)iFun, pFrame); // DIV-296

#ifdef DEBUG_OR_WOWPROFILE
#error Excluded unsupported WOW profiling configuration
#endif
#line 1060

#ifdef WOWPROFILE // For API profiling only (debugger extension)
#error Excluded unsupported WOW profiling configuration
#endif // WOWPROFILE
#line 1039

        FREEVDMPTR(pFrame);                                                     // Set the 16-bit return code
        GETFRAMEPTR(ptd->vpStack, pFrame);

        LOGRETURN(5,pFrame,ulReturn);                                           // Log return Values
        pFrame->wAX = LOW(ulReturn);                                            // Pass Back Return Value form thunk
        pFrame->wDX = HIW(ulReturn);

#ifdef DEBUG
#error Excluded unsupported DEBUG WOW profile
#endif // DEBUG
#line 1090

        FREEVDMPTR(pFrame);

        SETVDMSTACK(ptd->vpStack);

    } except (wow_user_dispatch_exception(GetExceptionCode(),
            GetExceptionInformation(), ptd, vpCurrentStack, fStackEstablished)) { // DIV-348
        /* DIV-337: neither continue-search nor continue-execution comes here. */
        if (fStackEstablished) { // DIVERGENCE(MVDM-HOST-DIV-348)
            ptd->vpStack = vpCurrentStack;
            SETVDMSTACK(ptd->vpStack);
        }
    }
    //
    // WARNING: DO NOT ADD ANYTHING TO THIS FUNCTION UNLESS YOU ADD THE SAME
    // STUFF TO i386/FastWOW.asm.   i386/FastWOW.ASM is used for speedy
    // thunk dispatching on retail builds.
    //
}


#if NO_W32TRYCALL
#error Excluded unsupported NO_W32TRYCALL WOW profile
#else
#line 1125

/* W32TryCall - Called from FASTWOW to do a TryExcept Around API Calls
 *
 *
 *
 *
 *
 *
 */
DWORD FASTCALL W32TryCall(PVDMFRAME pFrame , LPFNW32 lpfnW32 )
{
    DWORD ulReturn = 0;

    try {

        if (ISFUNCID(lpfnW32)) {
            lpfnW32 = W32PatchCodeWithLpfnw32(pFrame, (INT)lpfnW32);
        }

        ulReturn = wow_user_invoke_thunk(lpfnW32, pFrame); // DIV-296

    } except (W32Exception(GetExceptionCode(), GetExceptionInformation())) {
    }
    return (ulReturn);
}

#endif
#line 1152

/* W32Exception - Handle WOW32 thread exceptions
 *
 * ENTRY
 *  None (x86 registers contain parameters)
 *
 * EXIT
 *  None (x86 registers/memory updated appropriately)
 *
 */

INT W32Exception(DWORD dwException, PEXCEPTION_POINTERS pexi)
{
    PTD     ptd;
    PVDMFRAME pFrame;

    DWORD   dwButtonPushed;
    char    szTask[9];
    HMODULE hModule;
    char    szModule[_MAX_PATH + 1];
    PSZ     pszModuleFilePart;
    PSZ     pszErrorFormatString;
    char    szErrorMessage[TOOLONGLIMIT + 4*WARNINGMSGLENGTH];
    char    szDialogText[TOOLONGLIMIT + 4*WARNINGMSGLENGTH];
    PTDB    pTDB;
    NTSTATUS Status;
    HANDLE DebugPort;
    PRTL_CRITICAL_SECTION PebLockPointer;
    CHAR AeDebuggerCmdLine[256];
    CHAR AeAutoDebugString[8];
    BOOL AeAutoDebug;
    WORD wDebugButton;

    if (!gfDebugExceptions) {

        //
        // If the process is being debugged, just let the exception happen
        // so that the debugger can see it. This way the debugger can ignore
        // all first chance exceptions.
        //

        DebugPort = (HANDLE)NULL;
        Status = NtQueryInformationProcess(
                    GetCurrentProcess(),
                    ProcessDebugPort,
                    (PVOID)&DebugPort,
                    sizeof(DebugPort),
                    NULL
                    );

        if ( NT_SUCCESS(Status) && DebugPort) {

            //
            // Process is being debugged.
            // Return a code that specifies that the exception
            // processing is to continue
            //
            return EXCEPTION_CONTINUE_SEARCH;
        }
    }


    //
    // NtClose can raise exceptions if NtGlobalFlag is set for it.
    // We want to ignore these exceptions if we're not being debugged,
    // since the errors will be returned from the APIs and we generally
    // don't have control over what handles the app closes.  (Well, that's
    // not true for file I/O, but it is true for RegCloseKey.)
    //

    if (STATUS_INVALID_HANDLE == dwException ||
        STATUS_HANDLE_NOT_CLOSABLE == dwException) {

        return EXCEPTION_CONTINUE_EXECUTION;
    }

    //
    // See if a debugger has been programmed in. If so, use the
    // debugger specified. If not then there is no AE Cancel support
    // DEVL systems will default the debugger command line. Retail
    // systems will not.
    //
    // The above paragraph was copied from the system exception
    // popup in base.  It is no longer true.  On retail systems,
    // AeDebug.Auto is set to 1 and AeDebug.Debugger is
    // "drwtsn32 -p %ld -e %ld -g".
    //
    // This means if we support AeDebug for stress, customers don't see
    // our exception popup and misalignment handling -- instead they get
    // a nearly-useless drwtsn32.log and popup.
    //
    // SO, we check for this situation and act as if no debugger was
    // enabled.
    //

    wDebugButton = 0;
    AeAutoDebug = FALSE;

    //
    // If we are holding the PebLock, then the createprocess will fail
    // because a new thread will also need this lock. Avoid this by peeking
    // inside the PebLock and looking to see if we own it. If we do, then just allow
    // a regular popup.
    //

    PebLockPointer = NtCurrentPeb()->FastPebLock;

    if ( PebLockPointer->OwningThread != NtCurrentTeb()->ClientId.UniqueThread ) {

        try {
            if ( GetProfileString(
                    "AeDebug",
                    "Debugger",
                    NULL,
                    AeDebuggerCmdLine,
                    sizeof(AeDebuggerCmdLine)-1
                    ) ) {
                wDebugButton = SEB_CANCEL;

                if ( GetProfileString(
                        "AeDebug",
                        "Auto",
                        "0",
                        AeAutoDebugString,
                        sizeof(AeAutoDebugString)-1
                        ) ) {

                    if ( !strcmp(AeAutoDebugString,"1") ) {
                        AeAutoDebug = TRUE;
                    }
                }
            }

        } except (EXCEPTION_EXECUTE_HANDLER) {
            wDebugButton = 0;
            AeAutoDebug = FALSE;
        }
    }

    //
    // See comment above about drwtsn32
    //

    if (AeAutoDebug &&
        !_strnicmp(AeDebuggerCmdLine, szDrWtsn32, (sizeof szDrWtsn32) - 1)) {

        wDebugButton = 0;
        AeAutoDebug = FALSE;
    }

    ptd = CURRENTPTD();
    GETFRAMEPTR(ptd->vpStack, pFrame);

    pTDB = (PVOID)SEGPTR(ptd->htask16,0);

    //
    // Get a zero-terminated copy of the Win16 task name.
    //

    RtlZeroMemory(szTask, sizeof(szTask));
    RtlCopyMemory(szTask, pTDB->TDB_ModName, sizeof(szTask)-1);

    //
    // Translate exception address to module name in szModule.
    //

    strcpy(szModule, CRITSTR(TheWin16Subsystem));
    RtlPcToFileHeader(pexi->ExceptionRecord->ExceptionAddress, (PVOID *)&hModule);
    GetModuleFileName(hModule, szModule, sizeof(szModule));
    pszModuleFilePart = strrchr(szModule, '\\');
    if (pszModuleFilePart) {
        pszModuleFilePart++;
    } else {
        pszModuleFilePart = szModule;
    }


    //
    // Format error message into szErrorMessage
    //

    switch (dwException) {

        case EXCEPTION_ACCESS_VIOLATION:
            pszErrorFormatString = CRITSTR(CausedAV);
            break;

        case EXCEPTION_STACK_OVERFLOW:
            pszErrorFormatString = CRITSTR(CausedStackOverflow);
            break;

        case EXCEPTION_DATATYPE_MISALIGNMENT:
            pszErrorFormatString = CRITSTR(CausedAlignmentFault);
            break;

        case EXCEPTION_ILLEGAL_INSTRUCTION:
        case EXCEPTION_PRIV_INSTRUCTION:
            pszErrorFormatString = CRITSTR(CausedIllegalInstr);
            break;

        case EXCEPTION_IN_PAGE_ERROR:
            pszErrorFormatString = CRITSTR(CausedInPageError);
            break;

        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            pszErrorFormatString = CRITSTR(CausedIntDivideZero);
            break;

        case EXCEPTION_FLT_DENORMAL_OPERAND:
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        case EXCEPTION_FLT_INEXACT_RESULT:
        case EXCEPTION_FLT_INVALID_OPERATION:
        case EXCEPTION_FLT_OVERFLOW:
        case EXCEPTION_FLT_STACK_CHECK:
        case EXCEPTION_FLT_UNDERFLOW:
            pszErrorFormatString = CRITSTR(CausedFloatException);

        default:
            pszErrorFormatString = CRITSTR(CausedException);
    }

    wsprintf(szErrorMessage,
             pszErrorFormatString,
             szTask,
             pszModuleFilePart,
             pexi->ExceptionRecord->ExceptionAddress,
             dwException
             );

    LOGDEBUG(LOG_ALWAYS, ("W32Exception:\n%s\n",szErrorMessage));

    //
    // Format dialog text into szDialogText and display.
    //

    if (AeAutoDebug) {

        dwButtonPushed = 2;

    } else {

        if (wDebugButton == SEB_CANCEL) {

            wsprintf(szDialogText,
                     "%s\n%s\n%s\n%s\n",
                     szErrorMessage,
                     CRITSTR(ChooseClose),
                     CRITSTR(ChooseCancel),
                     (dwException == EXCEPTION_DATATYPE_MISALIGNMENT)
                         ? CRITSTR(ChooseIgnoreAlignment)
                         : CRITSTR(ChooseIgnore)
                     );
        } else {

            wsprintf(szDialogText,
                     "%s\n%s\n%s\n",
                     szErrorMessage,
                     CRITSTR(ChooseClose),
                     (dwException == EXCEPTION_DATATYPE_MISALIGNMENT)
                         ? CRITSTR(ChooseIgnoreAlignment)
                         : CRITSTR(ChooseIgnore)
                     );

        }

        dwButtonPushed = WOWSysErrorBox(
                CRITSTR(ApplicationError),
                szDialogText,
                SEB_CLOSE,
                wDebugButton,
                SEB_IGNORE | SEB_DEFBUTTON
                );

    }

    //
    // If CANCEL is chosen Launch Debugger.
    //

    if (dwButtonPushed == 2) {

        BOOL b;
        STARTUPINFO StartupInfo;
        PROCESS_INFORMATION ProcessInformation;
        CHAR CmdLine[256];
        NTSTATUS Status;
        HANDLE EventHandle;
        SECURITY_ATTRIBUTES sa;

        sa.nLength = sizeof(sa);
        sa.lpSecurityDescriptor = NULL;
        sa.bInheritHandle = TRUE;
        EventHandle = CreateEvent(&sa,TRUE,FALSE,NULL);
        RtlZeroMemory(&StartupInfo,sizeof(StartupInfo));
        sprintf(CmdLine,AeDebuggerCmdLine,GetCurrentProcessId(),EventHandle);
        StartupInfo.cb = sizeof(StartupInfo);
        StartupInfo.lpDesktop = "Winsta0\\Default";
        CsrIdentifyAlertableThread();
        b =  CreateProcess(
                NULL,
                CmdLine,
                NULL,
                NULL,
                TRUE,
                0,
                NULL,
                NULL,
                &StartupInfo,
                &ProcessInformation
                );

        if ( b && EventHandle) {

            //
            // Do an alertable wait on the event
            //

            Status = NtWaitForSingleObject(
                        EventHandle,
                        TRUE,
                        NULL
                        );
            return EXCEPTION_CONTINUE_SEARCH;

        } else {

            LOGDEBUG(0, ("W32Exception unable to start debugger.\n"));
            goto KillTask;
        }
    }

    //
    // If IGNORE is chosen and it's an EXCEPTION_DATATYPE_MISALIGNMENT,
    // turn on software emulation of misaligned access and restart the
    // faulting instruction.  Otherwise,  just fail the API and continue.
    //

    if (dwButtonPushed == 3) {

        if (dwException == EXCEPTION_DATATYPE_MISALIGNMENT) {
            SetErrorMode(SEM_NOALIGNMENTFAULTEXCEPT);
            LOGDEBUG(0, ("W32Exception disabling alignment fault exceptions at user's request.\n"));
            return EXCEPTION_CONTINUE_EXECUTION;
        }

        LOGDEBUG(0, ("W32Exception ignoring at user's request via EXCEPTION_EXECUTE_HANDLER\n"));
        return EXCEPTION_EXECUTE_HANDLER;
    }

    //
    // If user typed CLOSE or Any of the above fail,
    // force just the task to die.
    //

KillTask:
    LOGDEBUG(0, ("W32Exception killing task via RET_FORCETASKEXIT\n"));
    GETFRAMEPTR(ptd->vpStack, pFrame);
    pFrame->wRetID = RET_FORCETASKEXIT;
    return EXCEPTION_EXECUTE_HANDLER;

}


#ifdef DEBUG
#error Excluded unsupported DEBUG WOW profile
#endif // DEBUG
#line 1640


BOOL IsDebuggerAttached(VOID)
/*++

Routine Description:

    Checks to see if there's a debugger attached to WOW.  If there is,
    this routine also turns on a bit in the 16-bit kernel's DS so it
    can do its part to report debug events.

Arguments:

    None.

Return Value:

    FALSE - no debugger attached or NtQueryInformationProcess fails.
    TRUE  - debugger is definitely attached.

--*/

{
    NTSTATUS     Status;
    HANDLE       MyDebugPort;
    LPBYTE       lpDebugWOW;
    static BOOL  fDebuggerAttached = FALSE;
    static BOOL  fKernel16Notified = FALSE;

    //
    // Don't bother checking if we already have been told that
    // there is a debugger attached, since debuggers cannot detach.
    //

    if (!fDebuggerAttached) {

        //
        // Query our ProcessDebugPort, if it is nonzero we have
        // a debugger attached.
        //

        Status = NtQueryInformationProcess(
                     NtCurrentProcess(),
                     ProcessDebugPort,
                     (PVOID)&MyDebugPort,
                     sizeof(MyDebugPort),
                     NULL
                     );

        fDebuggerAttached = NT_SUCCESS(Status) && MyDebugPort;

    }

    //
    // If we have a debugger attached share that information
    // with the 16-bit kernel.
    //

    if (!fKernel16Notified && fDebuggerAttached && vpDebugWOW != 0) {

        GETVDMPTR(vpDebugWOW, 1, lpDebugWOW);
        *lpDebugWOW |= 1;
        FREEVDMPTR(lpDebugWOW);

        DBGNotifyDebugged( TRUE );

        fKernel16Notified = TRUE;
    }

    return fDebuggerAttached;
}


void *
WOWGetVDMPointer(
    VPVOID Address,
    DWORD  Count,
    BOOL   ProtectedMode
    )
/*++

Routine Description:

    This routine converts a 16/16 address to a linear address.

    WARNING NOTE - This routine has been optimized so protect mode LDT lookup
    falls stright through.

Arguments:

    Address -- specifies the address in seg:offset format
    Size -- specifies the size of the region to be accessed.
    ProtectedMode -- true if the address is a protected mode address

Return Value:

    The pointer.

--*/

{
    if (ProtectedMode) {
        return GetPModeVDMPointer(Address, Count);
    } else {
        return GetRModeVDMPointer(Address);
    }
}


PVOID FASTCALL
GetPModeVDMPointerAssert(
    DWORD Address
#ifdef DEBUG
#error Excluded unsupported DEBUG WOW profile
#endif
#line 1788
    )
/*++

Routine Description:

    Convert a 16:16 protected mode address to the equivalent flat pointer.

Arguments:

    Address -- specifies the address in selector:offset format

Return Value:

    The pointer.

--*/

{
#ifdef DEBUG
#error Excluded unsupported DEBUG WOW profile
#endif
#line 1809

    //
    // Check to see if the descriptor is marked present
    // We assume here that ExpLdt is DWORD ALIGNED to avoid a slower
    // unaligned access on risc.
    //

    if (!((ExpLdt)[(Address >> 18) | 1] & LDT_DESC_PRESENT)) {
        PARM16 Parm16;
        ULONG ul;

        if ((HIWORD(Address) & STD_SELECTOR_BITS) == STD_SELECTOR_BITS) {
            // We've determined that the selector is valid and not
            // present. So we call over to kernel16 to have it load
            // the selector into a segment register. This forces a
            // segment fault, and the segment should be brought in.
            // Note that CallBack16 also calls this routine, so we could
            // theoretically get into an infinite recursion loop here.
            // This could only happen if selectors like the 16-bit stack
            // were not present, which would mean we are hosed anyway.
            // Such a loop should terminate with a stack fault eventually.

            Parm16.WndProc.lParam = (LONG) Address;
            CallBack16(RET_FORCESEGMENTFAULT, &Parm16, 0, &ul);
        } else {

            // We come here if the address can't be resolved. A null
            // selector is special-cased to allow for a null 16:16
            // pointer to be passed.
            if (HIWORD(Address)) {

                LOGDEBUG(LOG_ALWAYS,("WOW::GetVDMPointer: *** Invalid 16:16 address %04x:%04x\n",
                    HIWORD(Address), LOWORD(Address)));
                // If we get here, then we are about to return a bogus
                // flat pointer.
                // I would prefer to eventually assert this, but it
                // appears to be overactive for winfax lite.
                //WOW32ASSERT(FALSE);

            }

        }
    }


#ifdef DEBUG
#error Excluded unsupported DEBUG WOW profile
#else
#line 1889
    return GetPModeVDMPointerMacro(Address, 0);  // No limit check on free build.
#endif // DEBUG
#line 1891
}




ULONG FASTCALL W32GetFastAddress( PVDMFRAME pFrame )
{
#if defined(CPU_40_STYLE)
    UNREFERENCED_PARAMETER(pFrame);
    return mvdm_softpc_fast_bop_dispatch_offset();
#elif FASTBOPPING
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#else
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#endif
}

ULONG FASTCALL W32GetFastCbRetAddress( PVDMFRAME pFrame )
{
#if defined(CPU_40_STYLE)
    UNREFERENCED_PARAMETER(pFrame);
    return mvdm_softpc_fast_bop_callback_offset();
#elif FASTBOPPING
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#else
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#endif
}

ULONG FASTCALL W32GetTableOffsets( PVDMFRAME pFrame )
{
    PWOWGETTABLEOFFSETS16 parg16;
    PTABLEOFFSETS   pto16;

    GETARGPTR(pFrame, sizeof(PDWORD16), parg16);
    GETVDMPTR(parg16->vpThunkTableOffsets, sizeof(TABLEOFFSETS), pto16);

    RtlCopyMemory(pto16, &tableoffsets, sizeof(TABLEOFFSETS));

    FLUSHVDMPTR(parg16->vpThunkTableOffsets, sizeof(TABLEOFFSETS), pto16);
    FREEVDMPTR(pto16);

    FREEARGPTR(parg16);

#if FASTBOPPING
#error Unsupported excluded original configuration
#endif
#line 1938

    return 1;
}

ULONG FASTCALL W32GetFlatAddressArray( PVDMFRAME pFrame )
{
#if defined(CPU_40_STYLE)
    /* Original PMODE32 KRNL386 asks for this table before taking its
     * WOW-specific selector publication path.  Return the same table shape,
     * but in worker guest memory rather than leaking a host array address. */
    return mvdm_softpc_wow_page_domain_flat_address_array(FlatAddress);
#elif FASTBOPPING
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#else
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#endif
}


#ifdef DEBUG
#error Excluded unsupported DEBUG WOW profile
#endif // DEBUG
#line 2290

PVOID FASTCALL malloc_w (ULONG size)
{
    PVOID pv;

    pv = HeapAlloc(hWOWHeap, 0, size);
    WOW32ASSERTMSG(pv, "WOW32: malloc_w failing, returning NULL\n");
    return pv;
}

PVOID FASTCALL malloc_w_zero (ULONG size)
{
    PVOID pv;

    pv = HeapAlloc(hWOWHeap, HEAP_ZERO_MEMORY, size);
    WOW32ASSERTMSG(pv, "WOW32: malloc_w_zero failing, returning NULL\n");
    return pv;
}

PVOID FASTCALL realloc_w (PVOID p, ULONG size, DWORD dwFlags)
{
    PVOID pv;

    pv = HeapReAlloc(hWOWHeap, dwFlags, p, size);
    WOW32ASSERTMSG(pv, "WOW32: realloc_w failing, returning NULL\n");
    return pv;
}

VOID FASTCALL free_w (PVOID p)
{
    HeapFree(hWOWHeap, 0, (LPSTR)(p));
}

//
// malloc_w_or_die is for use by *initialization* code only, when we
// can't get WOW going because, for example, we can't allocate a buffer
// to hold the known DLL list.
//
// malloc_w_or_die should not be used by API or message thunks or worker
// routines called by API or message thunks.
//

PVOID FASTCALL malloc_w_or_die(ULONG size)
{
    PVOID pv;
    if (!(pv = malloc_w(size))) {
        WOW32ASSERTMSG(pv, "WOW32: malloc_w_or_die failing, terminating.\n");
        WOWStartupFailed();  // never returns.
    }
    return pv;
}

//
// WOWStartupFailed puts up a fatal error box and terminates WOW.
//

PVOID WOWStartupFailed(VOID)
{
    char szCaption[256];
    char szMsgBoxText[1024];
    DWORD exit_code;
    /* DIVERGENCE(MVDM-HOST-DIV-356): parent owns whole-worker exit permission. */
    extern DWORD WINAPI NtvdmAuthorizeExit(DWORD exit_code);

    LoadString(hmodWOW32, iszStartupFailed, szMsgBoxText, sizeof szMsgBoxText);
    LoadString(hmodWOW32, iszSystemError, szCaption, sizeof szCaption);

    MessageBox(GetDesktopWindow(),
        szMsgBoxText,
        szCaption,
        MB_SETFOREGROUND | MB_TASKMODAL | MB_ICONSTOP | MB_OK | MB_DEFBUTTON1);

    /* DIVERGENCE(MVDM-HOST-DIV-356): before original task-record retirement. */
    exit_code = NtvdmAuthorizeExit(EXIT_FAILURE);
    ExitVDM(WOWVDM, ALL_TASKS);         // Tell Win32 All Tasks are gone.
    ExitProcess(exit_code);
    return (PVOID)NULL;
}


//****************************************************************************
#ifdef DEBUG_OR_WOWPROFILE
#error Excluded unsupported WOW profiling configuration
#endif  // DEBUG_OR_WOWPROFILE
#line 2421
