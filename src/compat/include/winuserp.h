/*
 * Source-shaped include boundary for the historical private USER header.
 *
 * DIVERGENCE(ADAPTER-WIN32-022): the complete NT4 winuserp.h belongs to the
 * USER product shell and redeclares private, non-public UI contracts.  The
 * selected original SoftPC nt_event.c reaches no declaration from that file:
 * its concrete input, console and window types come from its other original
 * headers and the modern public SDK.  Keep the include spelling buildable
 * without importing the private USER product shell or inventing a provider.
 */
#ifndef ADAPTER_MVDM_HOST_OUT_WIN32_WINUSERP_H
#define ADAPTER_MVDM_HOST_OUT_WIN32_WINUSERP_H

#include <windows.h>

/* Only the selected USER kernel slices need these declaration dependencies.
 * Ordinary WOW/FAX clients retain their original private header owners. */
#if defined(WOW_ORIGINAL_TASK_ORDER) || defined(WOW_USER_OBJECT_FLAGS_ONLY)
/* Original winuserp.h:1521. Declaration-only dependency of userk.h
 * cursor prototypes; no private USER implementation is imported. */
typedef DWORD JIF, *PJIF;

/* Original windows/core/w32inc/winuserk.h declaration enums. */
typedef enum _CONSOLECONTROL {
    ConsoleDesktopConsoleThread,     // 0
    ConsoleClassAtom,                // 1
    ConsolePermanentFont,            // 2
    ConsoleSetVDMCursorBounds,       // 3
    ConsoleNotifyConsoleApplication, // 4
    ConsolePublicPalette,            // 5
    ConsoleWindowStationProcess      // 6
} CONSOLECONTROL;

typedef enum _USERTHREADINFOCLASS {
    UserThreadShutdownInformation,
    UserThreadFlags,
    UserThreadTaskName,
    UserThreadWOWInformation,
    UserThreadHungStatus,
    UserThreadInitiateShutdown,
    UserThreadEndShutdown,
    UserThreadUseDesktop,
    UserThreadPolled,           // obsolete
    UserThreadKeyboardState,    // obsolete
    UserThreadCsrApiPort,
    UserThreadResyncKeyState,   // obsolete
    UserThreadUseActiveDesktop
} USERTHREADINFOCLASS;

/* Pointer-only original userk.h:4663/4679 DDE tags. */
typedef struct tagDDECONV *PDDECONV;
typedef struct tagXSTATE *PXSTATE;
/* Original ntuser/inc/ddetrack.h:28-39, pointer-only dependency. */
typedef struct tagINTDDEINFO *PINTDDEINFO;
/* Original user.h:1165-1174 handle entry pointer. */
typedef struct _HANDLEENTRY *PHE;
/* Original userk.h:1853-1857 accelerator-table pointer. */
typedef struct tagACCELTABLE *LPACCELTABLE;

/* Original public/internal/windows/inc/winuserp.h:953-954. */
#define SPI_UP_ACTIVEWINDOWTRACKING     0
#define SPI_UP_COUNT                    1

/* Original ntuser/inc/kbd.h:464-499; pointer-only keyboard table dependency. */
typedef struct tagKbdLayer *PKBDTABLES;
/* Original kbd.h:28-31 keyboard-event pointer. */
typedef struct tagKE *PKE;
/* Original winddi.h:80 and gre.h:151 declaration dependencies. */
DECLARE_HANDLE(HDEV);
typedef PERESOURCE PDEVICE_LOCK;

/* Original ntuser/inc/user.h:59. Pointer-only USER declaration dependency;
 * no window layout or window-manager implementation is imported. */
typedef struct tagWND               *PWND;
/* Pointer-only forms of original user.h MENU,ITEM,HOOK tags
 * (definitions at1958-2011 and1471-1485);their layouts are not imported. */
typedef struct tagMENU *PMENU;
typedef struct tagITEM *PITEM;
typedef struct tagHOOK *PHOOK;
/* Pointer-only original USER class and scrollbar-info tags. */
typedef struct tagCLS *PCLS;
typedef struct tagSBINFO *PSBINFO;
/* Original user.h:2048/2374 and userk.h:1729 pointer dependencies. */
typedef struct tagSBCALC *PSBCALC;
typedef struct tagSBWND *PSBWND;
typedef struct tagDESKWND *PDESKWND;
/* Original user.h:767 callback declaration. */
typedef LONG (APIENTRY *WNDPROC_PWND)(PWND, UINT, DWORD, LONG);
/* Original user.h:769 enumeration callback. */
typedef BOOL (APIENTRY *WNDENUMPROC_PWND)(PWND, LPARAM);

/* The handle-table-only consumer has no scheduler or cleanup thread view.
 * Supply the original user.h:762 pointer there;the other consumers already
 * bind PTHREADINFO before including userk.h. */
#if defined(WOW_USER_OBJECT_FLAGS_ONLY) && !defined(WOW_ORIGINAL_CLEANUP)
typedef struct tagTHREADINFO *PTHREADINFO;
/* Original user.h:763 process pointer;no process layout is supplied. */
typedef struct tagPROCESSINFO *PPROCESSINFO;
/* Original kernel pointers belong only to this handle-table view.
 * Ordinary WOW consumers retain their separate guest PTDB from tdb16.h. */
typedef struct tagSMS *PSMS;
typedef struct tagTDB *PTDB;
#endif

/* Original user.h:761 queue pointer for consumers without the cleanup
 * input-group binding. No native queue object is created or reinterpreted. */
#if !defined(WOW_ORIGINAL_CLEANUP) && (defined(WOW_ORIGINAL_TASK_ORDER) || defined(WOW_USER_OBJECT_FLAGS_ONLY))
typedef struct tagQ *PQ;
#endif

/* Original w32p.h:47-55, pointer-only thread declaration. */
typedef struct _W32THREAD *PW32THREAD;
/* Original NT4 hook range WH_MIN=-1,WH_MAX=12;not modern SDK hook count. */
#define CWINHOOKS 14

#if !defined(WOW_ORIGINAL_TASK_ORDER)
/* Original w32p.h:96, pointer-only process declaration. */
typedef struct _W32PROCESS *PW32PROCESS;
/* Original userk.h:2374 pointer-only process scheduler dependency. */
typedef struct tagWOWPROCESSINFO *PWOWPROCESSINFO;
#endif

#endif /* selected USER kernel declarations */

#endif /* ADAPTER_MVDM_HOST_OUT_WIN32_WINUSERP_H */
