/****************************** Module Header ******************************\
* Module Name: globals.h
*
* Copyright (c) 1985-93, Microsoft Corporation
*
* This module contains all of USER.DLL's global variables.  These are all
* instance-specific, i.e. each client has his own copy of these.  In general,
* there shouldn't be much reason to create instance globals.
*
* History:
* 10-18-90 DarrinM      Created.
\***************************************************************************/

#ifndef _GLOBALS_
#define _GLOBALS_

#line 43
extern CONST BOOLEAN gabThunkMessage[];
#line 47
extern WCHAR szOneChar[];

/*
 * WOW HACK - apps can pass a global handle as the hInstance on a call
 * to CreateWindow for an edit control and expect allocations for the
 * control to come out of that global block. (MSJ 1/91 p.122)
 * WOW needs this hInstance during the LocalAlloc callback to set up
 * the DS for the LocalAlloc, so we pass hInstance as an 'extra' parameter.
 * !!! this is dependent on calling convention !!!
 * (SAS 6-18-92) added hack for all macros
 */

#line 104
#define LOCALALLOC(dwFlags, dwBytes, hInstance)         \
                            (*pfnLocalAlloc)(dwFlags, dwBytes, hInstance)
#define LOCALREALLOC(hMem, dwBytes, dwFlags, hInstance, ppv) \
                            (*pfnLocalReAlloc)(hMem, dwBytes, dwFlags, hInstance, ppv)
#define LOCALLOCK(hMem, hInstance)                      \
                            (*pfnLocalLock)(hMem, hInstance)
#define LOCALUNLOCK(hMem, hInstance)                    \
                            (*pfnLocalUnlock)(hMem, hInstance)
#define LOCALSIZE(hMem, hInstance)                      \
                            (*pfnLocalSize)(hMem, hInstance)
#define LOCALFREE(hMem, hInstance)                      \
                            (*pfnLocalFree)(hMem, hInstance)

#line 18
#if DBG
#line 19
#error Excluded unsupported original DBG globals
#endif
#line 21

/*
 * LATER: client-side user needs to use moveable memory objects for
 * WOW compatibility (at least until/if/when we copy all the edit control
 * code into 16-bit space);  that's also why we can't just party with
 * handles like LMHtoP does... -JeffPar
 */
#line 85
#ifndef RC_INVOKED       // RC can't handle #pragmas
#line 86
#undef  LHND
#define LHND                (LMEM_MOVEABLE | LMEM_ZEROINIT)

#undef  LMHtoP
#define LMHtoP(handle)      // Don't use this macro
#endif
#line 92

#line 135
#ifdef WX86
#line 136
#error Excluded unsupported original WX86 globals
#endif
#line 142

#endif // ndef _GLOBALS_
#line 145
