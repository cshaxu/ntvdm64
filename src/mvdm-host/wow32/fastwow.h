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
#error Unsupported excluded original configuration
#else
#line 19

#define FASTBOPPING     0

#endif
#line 23

#if FASTBOPPING
#error Unsupported excluded original configuration
#endif
#line 29

#if FASTBOPPING
#error Unsupported excluded original configuration
#endif
#line 39

//#if FASTBOPPING
//
// Used to put lock prefixes in appropriate places for MP machines
//
extern VOID FastWowFirstCode(VOID);
extern VOID FixLocks(VOID);
//#endif

#line 48
#if 0
#line 49
#error DIVERGENCE MVDM-HOST-DIV-497: original literal #if 0 excludes this legacy branch; see mirror register.
#endif
#line 82
