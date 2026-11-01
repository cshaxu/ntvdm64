/*++ BUILD Version: 0003
 *
 *  WOW v1.0
 *
 *  Copyright (c) 1991, 1992, 1993 Microsoft Corporation
 *
 *  FASTWOW.H
 *  WOW32 x86 fast callback/API support
 *
 *  History:
 *  Created 4-Dec-1992  by barry bradie (barryb)
--*/

#if defined(i386) && !defined(DEBUG_OR_WOWPROFILE)

#define FASTBOPPING     1

#else

#define FASTBOPPING     0

#endif

#if FASTBOPPING
extern BYTE fKernelCSIPFixed;
// Used by the monitor to dispatch interupts. Updated for callbacks and bops
extern DECLSPEC_IMPORT PVOID CurrentMonitorTeb;
#endif

#if FASTBOPPING
VOID    WOWBopEntry(VOID);
VPVOID  FastBopVDMStack(void);
VOID    FastBopSetVDMStack(VPVOID vp);
VOID    FastWOWCallbackCall(VOID);
VOID    FastWOWCallbackRet(VOID);
#define FASTVDMSTACK()      FastBopVDMStack()
#define SETFASTVDMSTACK(vp) FastBopSetVDMStack(vp)
#endif

//#if FASTBOPPING
//
// Used to put lock prefixes in appropriate places for MP machines
//
extern VOID FastWowFirstCode(VOID);
extern VOID FixLocks(VOID);
//#endif

#if 0
#error DIVERGENCE MVDM-HOST-DIV-497: original literal #if 0 excludes this legacy branch; see mirror register.
#endif
