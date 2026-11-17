/****************************** Module Header ******************************\
* Module Name: ntstubs.c
*
* Copyright (c) 1985-95, Microsoft Corporation
*
* Kernel-mode stubs
*
* History:
* 03-16-95 JimA             Created.
* 08-12-96 jparsons         Added lparam validate for WM_NCCREATE [51986]
\***************************************************************************/

#define WOW_ORIGINAL_EDIT_STATE
#line 13
#include "wow_edit_state_bindings.h"
#pragma hdrstop

/*
 * Setup and control macros
 */
#define BEGINRECV(type, err)    \
    type retval;        \
    type errret = err;  \
    EnterCrit();

#define BEGINRECV_SHARED(type, err) \
    type retval;        \
    type errret = err;  \
    EnterSharedCrit();

#define BEGINRECV_VOID() \
    EnterCrit();

#line 41

#define BEGINRECV_HWND_VOID(hwnd) \
    PWND pwnd;                                      \
    EnterCrit();                                    \
    if ((pwnd = ValidateHwnd((hwnd))) == NULL) {    \
        goto errorexit;                             \
    }

#define BEGINRECV_HWND_SHARED(type, err, hwnd) \
    type retval;                                    \
    type errret = err;                              \
    PWND pwnd;                                      \
    EnterSharedCrit();                              \
    if ((pwnd = ValidateHwnd((hwnd))) == NULL) {    \
        retval = errret;                            \
        goto errorexit;                             \
    }

#define BEGINRECV_HWNDLOCK(type, err, hwnd) \
    type retval;                                    \
    type errret = err;                              \
    PWND pwnd;                                      \
    TL tlpwnd;                                      \
    PTHREADINFO ptiCurrent;                         \
    EnterCrit();                                    \
    if ((pwnd = ValidateHwnd((hwnd))) == NULL) {    \
        LeaveCrit();                                \
        return err;                                 \
    }                                               \
    ptiCurrent = PtiCurrent();                      \
    ThreadLockAlwaysWithPti(ptiCurrent, pwnd, &tlpwnd);

#define BEGINRECV_HWNDLOCK_OPT(type, err, hwnd) \
    type retval;                                    \
    type errret = err;                              \
    PWND pwnd;                                      \
    TL tlpwnd;                                      \
    PTHREADINFO ptiCurrent;                         \
    EnterCrit();                                    \
    if (hwnd) {                                     \
        if ((pwnd = ValidateHwnd(hwnd)) == NULL) {  \
            LeaveCrit();                            \
            return err;                             \
        }                                           \
    } else {                                        \
        pwnd = NULL;                                \
    }                                               \
    ptiCurrent = PtiCurrent();                      \
    ThreadLockWithPti(ptiCurrent, pwnd, &tlpwnd);

#define BEGINRECV_HWNDLOCK_VOID(hwnd) \
    PWND pwnd;                                          \
    TL tlpwnd;                                          \
    PTHREADINFO ptiCurrent;                             \
    EnterCrit();                                        \
    if ((pwnd = ValidateHwnd((hwnd))) == NULL) {        \
        LeaveCrit();                                    \
        return;                                         \
    }                                                   \
    ptiCurrent = PtiCurrent();                          \
    ThreadLockAlwaysWithPti(ptiCurrent, pwnd, &tlpwnd);

#define CLEANUPRECV() \
cleanupexit:

#define ENDRECV() \
    goto errorexit; \
errorexit:          \
    LeaveCrit();    \
    return retval;

#define ENDRECV_VOID() \
    goto errorexit; \
errorexit:          \
    LeaveCrit();    \
    return;

#line 123

#define ENDRECV_HWNDLOCK() \
    goto errorexit;         \
errorexit:                  \
    ThreadUnlock(&tlpwnd);  \
    LeaveCrit();            \
    return retval;

#define ENDRECV_HWNDLOCK_VOID() \
    goto errorexit;         \
errorexit:                  \
    ThreadUnlock(&tlpwnd);  \
    LeaveCrit();            \
    return;

#define MSGERROR() { \
    retval = errret;    \
    goto errorexit; }

#define MSGERROR_VOID() { \
    goto errorexit; }

#define MSGERRORCLEANUP() { \
    retval = errret;    \
    goto cleanupexit; }

#if DBG
#error
#else
#define StubExceptionHandler()  EXCEPTION_EXECUTE_HANDLER
#endif

#define TESTFLAGS(flags, mask) \
    if (((flags) & ~(mask)) != 0) { \
        RIPERR2(ERROR_INVALID_FLAGS, RIP_WARNING, "Invalid flags, %x & ~%x != 0 " #mask, \
            flags, mask); \
        MSGERROR();   \
    }

#define LIMITVALUE(value, limit, szText) \
    if ((value) > (limit)) {     \
        RIPERR3(ERROR_INVALID_PARAMETER, RIP_WARNING, "Invalid parameter, %d > %d in %s", \
             value, limit, szText); \
        MSGERROR();     \
    }

#define MESSAGECALL(api) \
LONG NtUserfn ## api(    \
    HWND hwnd,           \
    UINT msg,            \
    DWORD wParam,        \
    LONG lParam,         \
    DWORD xParam,        \
    DWORD xpfnProc,      \
    BOOL bAnsi)

#line 180

/*
 * Validation macros
 */
#define ValidateHWNDNoRIP(p,h)              \
    if ((p = ValidateHwnd(h)) == NULL)      \
        MSGERROR();

#define ValidateHWND(p,h)                   \
    if ((p = ValidateHwnd(h)) == NULL)      \
        MSGERROR();

#define ValidateHWNDND(p,h)                 \
    if ( ((p = ValidateHwnd(h)) == NULL) || \
         (p == _GetDesktopWindow()) )       \
        MSGERROR();

#define ValidateHWNDOPT(p,h) \
    if (h) {                                \
        if ((p = ValidateHwnd(h)) == NULL)  \
            MSGERROR();                     \
    } else {                                \
        p = NULL;                           \
    }

#define ValidateHWNDIA(p,h)                      \
    if (h == (HWND)0x0000FFFF) {                 \
        h = (HWND)-1;                            \
    }                                            \
    if (h != HWND_TOP &&                         \
        h != HWND_BOTTOM &&                      \
        h != HWND_TOPMOST &&                     \
        h != HWND_NOTOPMOST) {                   \
        if ( ((p = ValidateHwnd(h)) == NULL) ||  \
             (p == _GetDesktopWindow()) )        \
            MSGERROR();                          \
    } else {                                     \
        p = (PWND)h;                             \
    }

#define ValidateHWNDFF(p,h) \
    if ((h != (HWND)-1) && (h !=(HWND)0xffff)) { \
        if ((p = ValidateHwnd(h)) == NULL)       \
            MSGERROR();                          \
    } else {                                     \
        p = (PWND)-1;                            \
    }

#define ValidateHMENUOPT(p,h) \
    if (h) {                                \
        if ((p = ValidateHmenu(h)) == NULL) \
            MSGERROR();                     \
    } else {                                \
        p = NULL;                           \
    }

#define ValidateHMENU(p,h) \
    if ((p = ValidateHmenu(h)) == NULL) \
        MSGERROR();

#define ValidateHACCEL(p,h) \
    if ((p = HMValidateHandle(h, TYPE_ACCELTABLE)) == NULL) \
        MSGERROR();

#define ValidateHCURSOR(p,h) \
    if ((p = HMValidateHandle(h, TYPE_CURSOR)) == NULL) \
        MSGERROR();

#define ValidateHCURSOROPT(p,h) \
    if (h) {                                 \
        if ((p = HMValidateHandle(h, TYPE_CURSOR)) == NULL) \
        MSGERROR();                          \
    } else {                                \
        p = NULL;                           \
    }

#define ValidateHICON(p,h) \
    if ((p = HMValidateHandle(h, TYPE_CURSOR)) == NULL) \
        MSGERROR();

#define ValidateHHOOK(p,h) \
    if ((p = HMValidateHandle(h, TYPE_HOOK)) == NULL) \
        MSGERROR();

#define ValidateHDWP(p,h) \
    if ((p = HMValidateHandle(h, TYPE_SETWINDOWPOS)) == NULL) \
        MSGERROR();

#ifdef FE_IME
#error
#line 280
#endif

#line 282
#if DBG
#error
#line 300
#endif
#line 352
#if defined(_X86_)
#line 354
#else
#error
#line 359
#endif
#line 390
#if defined(_X86_)
#line 392
#else
#error
#endif
#line 563
#if defined(_X86_)
#line 565
#else
#error
#endif
#line 715
#if defined(_X86_)
#line 717
#else
#error
#endif
#line 738
#if defined(_X86_)
#line 740
#else
#error
#endif
#line 964
#if defined(_X86_)
#line 966
#else
#error
#endif
#line 1135
#if defined(_X86_)
#line 1138
#else
#error
#line 1141
#endif
#line 1229
#if defined(_X86_)
#line 1231
#else
#error
#endif
#line 1312
#if defined(_X86_)
#line 1314
#else
#error
#endif
#line 1405
#ifdef NEVER
#error
#line 1427
#endif
#line 1543
#ifdef NEVER
#error
#line 1563
#endif
#line 1793
#ifdef NEVER
#error
#line 1815
#endif
#line 1957
#if defined(_X86_)
#line 1959
#else
#error
#endif
#line 2027
#if defined(_X86_)
#line 2029
#else
#error
#endif
#line 2112
#if defined(_X86_)
#line 2114
#else
#error
#endif
#line 2288
#if defined(_X86_)
#line 2290
#else
#error
#endif
#line 2369
#if defined(_X86_)
#line 2371
#else
#error
#endif
#line 2723
#if defined(_X86_)
#line 2725
#else
#error
#endif
#line 2747
#ifdef MEMPHIS_MENUS
#error
#endif // MEMPHIS_MENUS
#line 2756
#ifdef MEMPHIS_MENUS
#error
#line 2763
#else
#line 2768
#endif // MEMPHIS_MENUS
#line 3107
#if defined(_X86_)
#line 3109
#else
#error
#endif
#line 3158
#if defined(_X86_)
#line 3160
#else
#error
#endif
#line 3431
#ifdef MEMPHIS_MENUS
#error
#endif // MEMPHIS_MENUS
#line 3446
#if defined(_X86_)
#line 3448
#else
#error
#endif
#line 3461
#ifdef MEMPHIS_MENUS
#error
#endif // MEMPHIS_MENUS
#line 3465
#ifdef MEMPHIS_MENUS
#error
#else
#line 3469
#endif // MEMPHIS_MENUS
#line 3476
#ifdef MEMPHIS_MENUS
#error
#else
#line 3480
#endif // MEMPHIS_MENUS
#line 3486
#ifdef MEMPHIS_MENUS
#error
#endif // MEMPHIS_MENUS
#line 3494
#ifdef MEMPHIS_MENU_WATERMARKS
#error
#line 3528
#endif // MEMPHIS_MENU_WATERMARKS
#line 3972
#if defined(_X86_)
#line 3974
#else
#error
#endif
#line 4318
#if defined(_X86_)
#line 4320
#else
#error
#line 4323
#endif
#line 4410
#ifdef MEMPHIS_MENUS
#error
#endif // MEMPHIS_MENUS
#line 4420
#ifdef MEMPHIS_MENUS
#error
#line 4427
#else
#line 4432
#endif // MEMPHIS_MENUS
#line 4727
#if defined(_X86_)
#line 4730
#else
#error
#line 4733
#endif
#line 5472
#if defined(_X86_)
#line 5474
#else
#error
#endif
#line 5506
#if defined(_X86_)
#line 5508
#else
#error
#endif
#line 5768
#if defined(_X86_)
#line 5770
#else
#error
#endif
#line 5823
#if defined(_X86_)
#line 5825
#else
#error
#endif
#line 5986
#if defined(_X86_)
#line 5988
#else
#error
#line 5991
#endif
#line 6170
BOOL NtUserSetWindowFNID(
    IN HWND hwnd,
    IN WORD fnid)
{
    BEGINRECV_HWND(BOOL, FALSE, hwnd);

    /*
     * Remember what window class this window belongs to.  Can't use
     * the real class because any app can call CallWindowProc()
     * directly no matter what the class is!
     */
    if ((GETFNID(pwnd) == 0) || (fnid == FNID_CLEANEDUP_BIT)) {
        pwnd->fnid |= fnid;
        retval = TRUE;
    }

    TRACE("NtUserSetWindowFNID");
    ENDRECV_HWND();
}

#line 6241
#ifdef FE_IME
#error
#line 6248
#endif
#line 6312
#if defined(_X86_)
#line 6314
#else
#error
#line 6317
#endif
#line 6417
#if defined(_X86_)
#line 6435
#else
#error
#line 6453
#endif
#line 6689
#if defined(_X86_)
#line 6691
#else
#error
#endif
#line 6929
#if defined(_X86_)
#line 6932
#else
#error
#line 6935
#endif
#line 6977
#if defined(_X86_)
#line 6980
#else
#error
#line 6983
#endif
#line 7026
#if defined(_X86_)
#line 7031
#else
#error
#line 7036
#endif
#line 7071
#if defined(_X86_)
#line 7074
#else
#error
#line 7077
#endif
#line 7102
#if defined(_X86_)
#line 7104
#else
#error
#endif
#line 7131
#if defined(_X86_)
#line 7133
#else
#error
#endif
#line 7161
#if defined(_X86_)
#line 7163
#else
#error
#endif
#line 7188
#if defined(_X86_)
#line 7190
#else
#error
#endif
#line 7261
#if defined(_X86_)
#define DATAALIGN sizeof(BYTE)
#define CHARALIGN sizeof(BYTE)
#else
#error
#line 7267
#endif
#line 7313
#undef DATAALIGN
#undef CHARALIGN
#line 7333
#if defined(_X86_)
#line 7335
#else
#error
#endif
#line 7363
#if defined(_X86_)
#line 7365
#else
#error
#endif
#line 7439
#if defined(_X86_)
#line 7441
#else
#error
#endif
#line 7849
#ifdef FE_IME
#error
#line 7857
#endif
#line 7917
#if defined(_X86_)
#line 7919
#else
#error
#endif
#line 8377
#if 0
#error
#line 8411
#endif // 0
#line 8558
#if defined(_X86_)
#line 8561
#else
#error
#line 8564
#endif
#line 8571
#if defined(_X86_)
#line 8574
#else
#error
#line 8577
#endif
#line 8687
#if defined(_X86_)
#line 8689
#else
#error
#line 8692
#endif
#line 8747
#if defined(_X86_)
#line 8749
#else
#error
#line 8752
#endif
#line 9237
#if defined(_X86_)
#line 9239
#else
#error
#line 9242
#endif
#line 9303
#if defined(_X86_)
#line 9305
#else
#error
#line 9308
#endif
#line 9354
#if defined(_X86_)
#line 9356
#else
#error
#line 9359
#endif
#line 9395
#if defined(_X86_)
#line 9397
#else
#error
#endif
#line 9435
#if defined(_X86_)
#line 9438
#else
#error
#line 9441
#endif
#line 9558
#if defined(_X86_)
#line 9561
#else
#error
#line 9564
#endif
#line 9851
#ifdef DEBUG
#error
#line 9855
#endif
#line 9993
#ifdef FE_IME
#error
#line 10309
#endif
