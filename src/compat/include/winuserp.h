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
#include "wow_gdi_alias.h"
#include "non-mvdm/windows/core/w32inc/w32wow64.h"
/* Opaque NT activation-context pointer used by original USER declarations. */
typedef struct _ACTIVATION_CONTEXT *PACTIVATION_CONTEXT;

/* XP windows/published/winuser.w:466-478. The public SDK omits these
 * original private version predicates used by USER creation and dialogs. */
#define VERMAX          0x9900
#define VER51           0x0501
#define VER50           0x0500
#define VER40           0x0400
#define VER31           0x030A
#define VER30           0x0300
#define Is510Compat(dwExpWinVer)  (LOWORD(dwExpWinVer) >= VER51)
#define Is500Compat(dwExpWinVer)  (LOWORD(dwExpWinVer) >= VER50)
#define Is400Compat(dwExpWinVer)  (LOWORD(dwExpWinVer) >= VER40)
#define Is310Compat(dwExpWinVer)  (LOWORD(dwExpWinVer) >= VER31)
#define Is300Compat(dwExpWinVer)  (LOWORD(dwExpWinVer) >= VER30)

/* XP SP1 private USER flags used by WOW32 drawing and menu thunks. */
#define WF_DIALOG_WINDOW 0x00010000
#define WS_EX_VALID40 (0x0000003fL | WS_EX_MDICHILD | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | \
    WS_EX_CONTEXTHELP | WS_EX_TOOLWINDOW | WS_EX_RIGHT | WS_EX_LEFT | WS_EX_RTLREADING | \
    WS_EX_LEFTSCROLLBAR | WS_EX_CONTROLPARENT | WS_EX_STATICEDGE | WS_EX_APPWINDOW)
#define DST_TYPEMASK    0x0007
#define MFT_NONSTRING       0x00000904L
/* XP public/internal/windows/inc/winuserp.h:985; user.h verifies the
 * private menu flags do not overlap the public MNS range. */
#define MNS_LAST            0x04000000
/* XP winuserp.h:1255-1261 and 2289, shared declaration prerequisites. */
#define COLOR_3DALTFACE         25
#define COLOR_ENDCOLORS         WOW_GDI_SYSTEM_COLOR_END
#define COLOR_MAX               (COLOR_ENDCOLORS+1)
typedef DWORD JIF, *PJIF;

/* Reuse the original cursor declaration; no duplicate CURSINFO layout. */
#include "non-mvdm/windows/core/w32inc/w32gdip.h"

#endif /* ADAPTER_MVDM_HOST_OUT_WIN32_WINUSERP_H */

/* Only the selected USER kernel slices need these declaration dependencies.
 * Ordinary WOW/FAX clients retain their original private header owners. */
#if (defined(WOW_ORIGINAL_TASK_ORDER) || defined(WOW_USER_OBJECT_FLAGS_ONLY) || defined(WOW_ORIGINAL_CLEANUP)) && !defined(WOW_USER_KERNEL_DECLARATIONS)
#define WOW_USER_KERNEL_DECLARATIONS
/* Original winuserp.h:1521. Declaration-only dependency of userk.h
 * cursor prototypes; no private USER implementation is imported. */

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
/* OpenNT user.h:765,1543-1555,2268-2270; opaque legacy kernel
 * declarations retained until that header's own XP migration. */
DECLARE_HANDLE(HQ);
typedef struct tagPHYSICAL_DEV_INFO *PPHYSICAL_DEV_INFO;
#define CVKKEYSTATE                 256
#define CBKEYSTATE                  (CVKKEYSTATE >> 2)
#define CBKEYSTATERECENTDOWN        (CVKKEYSTATE >> 3)
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
typedef LRESULT (APIENTRY *WNDPROC_PWND)(PWND, UINT, WPARAM, LPARAM);
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
#ifndef WOW_ORIGINAL_CLEANUP
typedef struct tagQ *PQ;
#endif

/* Original w32p.h:47-55, pointer-only thread declaration. */
typedef struct _W32THREAD *PW32THREAD;
/* XP windows/core/w32inc/w32p.h:72-83. Retained kernel declaration,
 * not client/usercli.h's dummy TL and not an instantiated kernel lock. */
typedef struct _TL {
    struct _TL    *next;
    PVOID          pobj;
    PFREE_FUNCTION pfnFree;
#if DBG
    ULONG          uTLCount;
    PW32THREAD     pW32Thread;
    PVOID          pfnCaller;
    struct _TL    *ptl;
#endif
} TL, * KPTR_MODIFIER PTL;
/* XP public/ddk/inc/ntddk.h:13627. */
#define EVENT_INCREMENT 1

/* XP ntosdef.h MODE is local to this USER view: SoftPC VGA has its own MODE. */
typedef enum _MODE {
    KernelMode,
    UserMode,
    MaximumMode
} MODE;

/* XP retained USER declarations: original tags, pointer-only kernel owners.
 * usergdi.h:555, immstruc.h:99, gre.h:646 and ntosp.h object callouts. */
typedef struct tagWin32PoolHead *PWin32PoolHead;
typedef struct tagIMEINFOEX *PIMEINFOEX;
typedef struct _MDEV *PMDEV;
typedef struct _EJOB *PEJOB;
typedef struct _PAGED_LOOKASIDE_LIST *PPAGED_LOOKASIDE_LIST;
typedef struct _WIN32_JOBCALLOUT_PARAMETERS *PKWIN32_JOBCALLOUT_PARAMETERS;
typedef struct _WIN32_CLOSEMETHOD_PARAMETERS *PKWIN32_CLOSEMETHOD_PARAMETERS;
typedef struct _WIN32_DELETEMETHOD_PARAMETERS *PKWIN32_DELETEMETHOD_PARAMETERS;
typedef struct _WIN32_PARSEMETHOD_PARAMETERS *PKWIN32_PARSEMETHOD_PARAMETERS;
typedef struct _WIN32_OKAYTOCLOSEMETHOD_PARAMETERS *PKWIN32_OKAYTOCLOSEMETHOD_PARAMETERS;
typedef struct _WIN32_OPENMETHOD_PARAMETERS *PKWIN32_OPENMETHOD_PARAMETERS;
/* XP wmidata.h:3746-3758; complete by-value device identifier. */
typedef struct _KEYBOARD_ID_EX {
    ULONG Type;
    ULONG Subtype;
} KEYBOARD_ID_EX, *PKEYBOARD_ID_EX;
/* XP windows/core/w32inc/winuserk.h:91-94. */
typedef struct _CONSOLE_CARET_INFO {
    HWND hwnd;
    RECT rc;
} CONSOLE_CARET_INFO, *PCONSOLE_CARET_INFO;
/* XP windows/core/w32inc/winuserk.h:138, pointer-only desktop restoration. */
typedef struct tagDESKRESTOREDATA *PDESKRESTOREDATA;
/* XP windows/published/winuser.w:5978,6078,8091. */
#define HBMMENU_MBARLAST ((HBITMAP) 7)
#define TPM_FIRSTANIBITPOS 10
#define MFS_CACHEDBMP 0x20000000L
/* Original XP immstruc.h and published imm.w/winuser.w declarations. */
typedef enum _AIC_STATUS {
    AIC_SUCCESS,
    AIC_FOCUSCONTEXTCHANGED,
    AIC_ERROR,
} AIC_STATUS;
typedef enum _UPDATEINPUTCONTEXTCLASS {
    UpdateClientInputContext,
    UpdateInUseImeWindow,
} UPDATEINPUTCONTEXTCLASS;
typedef enum _IMEINFOEXCLASS {
    ImeInfoExKeyboardLayout,
#if defined(CUAS_ENABLE)
    ImeInfoExKeyboardLayoutWithCUAS,
#endif
    ImeInfoExImeWindow,
    ImeInfoExImeFileName,
} IMEINFOEXCLASS;
typedef struct _tagIMEHOTKEYOBJ *PIMEHOTKEYOBJ;
typedef struct tagSOFTKBDDATA *PSOFTKBDDATA;
typedef struct tagWSINFO WSINFO;
#define SPI_STARTBOOLRANGE                  0x1000
#define SPI_MAXBOOLRANGE                    0x1040
#define SPI_BOOLRANGECOUNT ((SPI_MAXBOOLRANGE - SPI_STARTBOOLRANGE) / 2)
#define SPI_BOOLMASKDWORDSIZE (((SPI_BOOLRANGECOUNT - 1) / 32) + 1)
#define SPI_STARTDWORDRANGE                 0x2000
#define SPI_MAXDWORDRANGE                   0x2014
#define SPI_DWORDRANGECOUNT ((SPI_MAXDWORDRANGE - SPI_STARTDWORDRANGE) / 2)
#define GACF2_GIVEUPFOREGROUND    0x00000040  // In W2k, we have changed foreground semantics to stop
#define DCX_INVALID 0x00000800L
#define DCX_REDIRECTED 0x00004000L
/* XP winuserk.h:177-179 and winuser.w:10537-10542. */
typedef enum _USERPROCESSINFOCLASS {
    UserProcessFlags
} USERPROCESSINFOCLASS;
typedef struct tagPOWERSTATEPARAMS {
    POWER_ACTION        SystemAction;
    SYSTEM_POWER_STATE  MinSystemState;
    ULONG               Flags;
    BOOL                FullScreenMode;
} POWERSTATEPARAMS, *PPOWERSTATEPARAMS;
/* XP w32err.h retail diagnostic contract. */
/* Original w32err.h retail diagnostics, when not supplied by the caller. */
#ifndef UserAssert
#define UserAssert(exp)
#endif
#ifndef RIPMSG0
#define RIPMSG0(flags, szFmt)
#endif
#ifndef TAGMSG0
#define TAGMSG0(flags, szFmt)
#endif
#ifndef TAGMSG1
#define TAGMSG1(flags, szFmt, p1)
#endif

/* Original NT4 hook range WH_MIN=-1,WH_MAX=12;not modern SDK hook count. */
#define CWINHOOKS 14

#if !defined(WOW_ORIGINAL_TASK_ORDER)
/* Original w32p.h:96, pointer-only process declaration. */
typedef struct _W32PROCESS *PW32PROCESS;
/* Original userk.h:2374 pointer-only process scheduler dependency. */
typedef struct tagWOWPROCESSINFO *PWOWPROCESSINFO;
typedef struct tagWOWTHREADINFO *PWOWTHREADINFO;
#endif

#endif /* selected USER kernel declarations */
