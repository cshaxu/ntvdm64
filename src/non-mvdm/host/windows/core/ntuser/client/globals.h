/* Copyright (c) 1985-93, Microsoft Corporation.
 * Original windows/core/ntuser/client/globals.h true subset.
 * DIVERGENCE(OPENNT-HOST-075): retain reached EDIT memory macros only;
 * callback storage is supplied by the existing WOW registration binding. */
#ifndef OPENNT_GLOBALS_EDIT_MEMORY_SUBSET_H
#define OPENNT_GLOBALS_EDIT_MEMORY_SUBSET_H

/* OPENNT-HOST-092: original declaration; definition remains in clglobal.c. */
extern CONST BOOLEAN gabThunkMessage[];
/* OPENNT-HOST-117: original clglobal.c owns the sizing exemplar. */
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

#endif
