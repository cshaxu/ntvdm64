/* Copyright (c) Microsoft Corporation.
 * Original windows/core/ntuser/kernel/ntstubs.c true subset.
 * DIVERGENCE(OPENNT-HOST-076): retain NtUserSetWindowFNID unchanged;
 * scoped original EDITWND storage replaces the unavailable receive shell. */
#define WOW_ORIGINAL_EDIT_STATE
#include "wow_edit_state_bindings.h"

#line 149
#if DBG
#error Excluded unsupported original DBG stub exception handler
#else
#line 152
#define StubExceptionHandler()  EXCEPTION_EXECUTE_HANDLER
#endif
#line 154

#line 1405
#ifdef NEVER
#error Excluded original disabled USER server stub
#endif
#line 1428
#line 1543
#ifdef NEVER
#error Excluded original disabled USER server stub
#endif
#line 1564
#line 1793
#ifdef NEVER
#error Excluded original disabled USER server stub
#endif
#line 1816
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

#line 7261
#if defined(_X86_)
#define DATAALIGN sizeof(BYTE)
#define CHARALIGN sizeof(BYTE)
#else
#line 7265
#error Excluded unsupported original non-x86 class-registration alignment
#endif
#line 7313
#undef DATAALIGN
#undef CHARALIGN
#line 7315
#line 8377
#if 0
#error Excluded original disabled full-screen message stub
#endif
#line 8412

#line 268
#ifdef FE_IME
#error Excluded unsupported original FE_IME branch
#endif
#line 281

#line 6241
#ifdef FE_IME
#error Excluded unsupported original FE_IME branch
#endif
#line 6249

#line 7849
#ifdef FE_IME
#error Excluded unsupported original FE_IME branch
#endif
#line 7858

#line 9993
#ifdef FE_IME
#error Excluded unsupported original FE_IME branch
#endif
#line 10310

#line 282
#if DBG
#error Excluded unsupported original DBG branch
#endif
#line 301

#line 9851
#ifdef DEBUG
#error Excluded unsupported original DEBUG branch
#endif
#line 9856

#line 2747
#ifdef MEMPHIS_MENUS
#error Excluded unsupported original MEMPHIS_MENUS branch
#endif
#line 2750

#line 3431
#ifdef MEMPHIS_MENUS
#error Excluded unsupported original MEMPHIS_MENUS branch
#endif
#line 3434

#line 3461
#ifdef MEMPHIS_MENUS
#error Excluded unsupported original MEMPHIS_MENUS branch
#endif
#line 3464

#line 3486
#ifdef MEMPHIS_MENUS
#error Excluded unsupported original MEMPHIS_MENUS branch
#endif
#line 3489

#line 4410
#ifdef MEMPHIS_MENUS
#error Excluded unsupported original MEMPHIS_MENUS branch
#endif
#line 4413

#line 3494
#ifdef MEMPHIS_MENU_WATERMARKS
#error Excluded unsupported original MEMPHIS_MENU_WATERMARKS branch
#endif
#line 3529

#line 352
#if defined(_X86_)
#line 353
#else
#line 355
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 360

#line 390
#if defined(_X86_)
#line 391
#else
#line 393
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 395

#line 563
#if defined(_X86_)
#line 564
#else
#line 566
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 568

#line 715
#if defined(_X86_)
#line 716
#else
#line 718
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 720

#line 738
#if defined(_X86_)
#line 739
#else
#line 741
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 743

#line 964
#if defined(_X86_)
#line 965
#else
#line 967
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 969

#line 1135
#if defined(_X86_)
#line 1136
#else
#line 1139
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 1142

#line 1229
#if defined(_X86_)
#line 1230
#else
#line 1232
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 1234

#line 1312
#if defined(_X86_)
#line 1313
#else
#line 1315
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 1317

#line 1957
#if defined(_X86_)
#line 1958
#else
#line 1960
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 1962

#line 2027
#if defined(_X86_)
#line 2028
#else
#line 2030
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 2032

#line 2112
#if defined(_X86_)
#line 2113
#else
#line 2115
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 2117

#line 2288
#if defined(_X86_)
#line 2289
#else
#line 2291
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 2293

#line 2369
#if defined(_X86_)
#line 2370
#else
#line 2372
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 2374

#line 2723
#if defined(_X86_)
#line 2724
#else
#line 2726
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 2728

#line 3107
#if defined(_X86_)
#line 3108
#else
#line 3110
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 3112

#line 3158
#if defined(_X86_)
#line 3159
#else
#line 3161
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 3163

#line 3446
#if defined(_X86_)
#line 3447
#else
#line 3449
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 3451

#line 3972
#if defined(_X86_)
#line 3973
#else
#line 3975
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 3977

#line 4318
#if defined(_X86_)
#line 4319
#else
#line 4321
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 4324

#line 4727
#if defined(_X86_)
#line 4728
#else
#line 4731
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 4734

#line 5472
#if defined(_X86_)
#line 5473
#else
#line 5475
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 5477

#line 5506
#if defined(_X86_)
#line 5507
#else
#line 5509
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 5511

#line 5768
#if defined(_X86_)
#line 5769
#else
#line 5771
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 5773

#line 5823
#if defined(_X86_)
#line 5824
#else
#line 5826
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 5828

#line 5986
#if defined(_X86_)
#line 5987
#else
#line 5989
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 5992

#line 6312
#if defined(_X86_)
#line 6313
#else
#line 6315
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 6318

#line 6417
#if defined(_X86_)
#line 6418
#else
#line 6436
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 6454

#line 6689
#if defined(_X86_)
#line 6690
#else
#line 6692
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 6694

#line 6929
#if defined(_X86_)
#line 6930
#else
#line 6933
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 6936

#line 6977
#if defined(_X86_)
#line 6978
#else
#line 6981
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 6984

#line 7026
#if defined(_X86_)
#line 7027
#else
#line 7032
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 7037

#line 7071
#if defined(_X86_)
#line 7072
#else
#line 7075
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 7078

#line 7102
#if defined(_X86_)
#line 7103
#else
#line 7105
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 7107

#line 7131
#if defined(_X86_)
#line 7132
#else
#line 7134
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 7136

#line 7161
#if defined(_X86_)
#line 7162
#else
#line 7164
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 7166

#line 7188
#if defined(_X86_)
#line 7189
#else
#line 7191
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 7193

#line 7333
#if defined(_X86_)
#line 7334
#else
#line 7336
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 7338

#line 7363
#if defined(_X86_)
#line 7364
#else
#line 7366
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 7368

#line 7439
#if defined(_X86_)
#line 7440
#else
#line 7442
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 7444

#line 7917
#if defined(_X86_)
#line 7918
#else
#line 7920
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 7922

#line 8558
#if defined(_X86_)
#line 8559
#else
#line 8562
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 8565

#line 8571
#if defined(_X86_)
#line 8572
#else
#line 8575
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 8578

#line 8687
#if defined(_X86_)
#line 8688
#else
#line 8690
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 8693

#line 8747
#if defined(_X86_)
#line 8748
#else
#line 8750
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 8753

#line 9237
#if defined(_X86_)
#line 9238
#else
#line 9240
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 9243

#line 9303
#if defined(_X86_)
#line 9304
#else
#line 9306
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 9309

#line 9354
#if defined(_X86_)
#line 9355
#else
#line 9357
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 9360

#line 9395
#if defined(_X86_)
#line 9396
#else
#line 9398
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 9400

#line 9435
#if defined(_X86_)
#line 9436
#else
#line 9439
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 9442

#line 9558
#if defined(_X86_)
#line 9559
#else
#line 9562
#error Excluded unsupported original non-x86 receive-stub branch
#endif
#line 9565

#line 2756
#ifdef MEMPHIS_MENUS
#line 2757
#error Excluded unsupported original MEMPHIS_MENUS branch
#else
#line 2764
#endif // MEMPHIS_MENUS
#line 2769

#line 3465
#ifdef MEMPHIS_MENUS
#line 3466
#error Excluded unsupported original MEMPHIS_MENUS branch
#else
#line 3468
#endif // MEMPHIS_MENUS
#line 3470

#line 3476
#ifdef MEMPHIS_MENUS
#line 3477
#error Excluded unsupported original MEMPHIS_MENUS branch
#else
#line 3479
#endif // MEMPHIS_MENUS
#line 3481

#line 4420
#ifdef MEMPHIS_MENUS
#line 4421
#error Excluded unsupported original MEMPHIS_MENUS branch
#else
#line 4428
#endif // MEMPHIS_MENUS
#line 4433

