/****************************** Module Header ******************************\
* Module Name: handtabl.c
*
* Copyright (c) 1985-95, Microsoft Corporation
*
* Implements the USER handle table.
*
* 01-13-92 ScottLu      Created.
\***************************************************************************/

/* DIVERGENCE(OPENNT-HOST-063): unchanged object ownership table; native
 * USER remains the allocator. Do not allocate a second handle namespace. */
#include <windows.h>
#include "compat/include/opennt_user_multipass.h"
#define WOW_USER_OBJECT_FLAGS_ONLY
#include "userk.h"

CONST BYTE gabObjectCreateFlags[TYPE_CTYPES] = {
    0,                                                  /* free */
    OCF_THREADOWNED | OCF_MARKTHREAD | OCF_USEQUOTA,    /* window */
    OCF_PROCESSOWNED,                                   /* menu */
    OCF_PROCESSOWNED | OCF_USEQUOTA,                    /* cursor/icon */
    OCF_THREADOWNED | OCF_USEQUOTA,                     /* hswpi (SetWindowPos Information) */
    OCF_THREADOWNED | OCF_MARKTHREAD,                   /* hook */
    OCF_THREADOWNED | OCF_USEQUOTA,                     /* thread info object (internal) */
    0,                                                  /* clipboard data (internal) */
    OCF_THREADOWNED,                                    /* CALLPROCDATA */
    OCF_PROCESSOWNED | OCF_USEQUOTA,                    /* accel table */
    OCF_THREADOWNED | OCF_USEQUOTA,                     /* dde access */
    OCF_THREADOWNED | OCF_MARKTHREAD | OCF_USEQUOTA,    /* dde conversation */
    OCF_THREADOWNED | OCF_MARKTHREAD | OCF_USEQUOTA,    /* ddex */
    OCF_PROCESSOWNED,                                   /* zombie */
    OCF_PROCESSOWNED,                                   /* keyboard layout */
    OCF_PROCESSOWNED,                                   /* keyboard file */
#line 56
#ifdef FE_IME
#line 57
#error DIVERGENCE OPENNT-HOST-132: FE_IME branch is not selected; see mirror register.
#endif
#line 59
};

#line 544
#if 0
#line 545
#error Excluded original disabled USER handle-table check
#endif
#line 564


#line 18
#ifdef DEBUG_LOCKS
#line 19
#error Excluded unsupported original DEBUG_LOCKS branch
#else
#line 21
#endif
#line 23

#line 86
#ifdef DEBUG
#line 87
#error Excluded unsupported original DEBUG branch
#endif
#line 99

#line 852
#ifdef DEBUG
#line 853
#error Excluded unsupported original DEBUG branch
#endif
#line 953

#line 963
#ifdef DEBUG
#line 964
#error Excluded unsupported original DEBUG branch
#endif
#line 995

#line 1169
#ifdef DEBUG
#line 1170
#error Excluded unsupported original DEBUG branch
#endif
#line 1189

#line 1198
#ifdef DEBUG
#line 1199
#error Excluded unsupported original DEBUG branch
#endif
#line 1240

#line 1250
#ifdef DEBUG
#line 1251
#error Excluded unsupported original DEBUG branch
#endif
#line 1302

#line 1321
#ifdef DEBUG
#line 1322
#error Excluded unsupported original DEBUG branch
#else
#line 1326
#endif
#line 1330

#line 1394
#ifdef DEBUG
#line 1395
#error Excluded unsupported original DEBUG branch
#endif
#line 1424

#line 1691
#ifdef DEBUG
#line 1692
#error Excluded unsupported original DEBUG branch
#endif
#line 1713

#line 1714
#ifdef DEBUG
#line 1715
#error Excluded unsupported original DEBUG branch
#endif
#line 1795

#line 1933
#ifdef DEBUG
#line 1934
#error Excluded unsupported original DEBUG branch
#endif
#line 2014

#line 286
#if (DBG || TRACE_OBJECT_ALLOCS)
#line 287
#error Excluded unsupported original TRACE_OBJECT_ALLOCS branch
#endif
#line 303

#line 81
#ifdef FE_IME
#line 82
#error Excluded unsupported original FE_IME branch
#endif
#line 84

#line 134
#ifdef DEBUG
#line 135
#error Excluded unsupported original DEBUG branch
#endif
#line 138

#line 456
#ifdef FE_IME
#line 457
#error Excluded unsupported original FE_IME branch
#endif
#line 459

#line 585
#ifdef DEBUG
#line 586
#error Excluded unsupported original DEBUG branch
#endif
#line 588

#line 764
#ifdef DEBUG
#line 765
#error Excluded unsupported original DEBUG branch
#endif
#line 777

#line 797
#ifdef DEBUG
#line 798
#error Excluded unsupported original DEBUG branch
#endif
#line 804

#line 1056
#ifdef DEBUG
#line 1057
#error Excluded unsupported original DEBUG branch
#endif
#line 1070

#line 1084
#ifdef DEBUG
#line 1085
#error Excluded unsupported original DEBUG branch
#endif
#line 1099

#line 1139
#ifdef DEBUG
#line 1140
#error Excluded unsupported original DEBUG branch
#endif
#line 1153

#line 1353
#ifdef DEBUG
#line 1354
#error Excluded unsupported original DEBUG branch
#endif
#line 1362

#line 1519
#ifdef FE_IME
#line 1520
#error Excluded unsupported original FE_IME branch
#endif
#line 1524

#line 1886
#ifdef DEBUG
#line 1887
#error Excluded unsupported original DEBUG branch
#endif // DEBUG
#line 1912


#line 594
#ifndef DEBUG
#line 595
#line 600
#ifdef FE_IME
#line 601
#error Unsupported FE_IME original branch
#endif
#line 603
#else // DEBUG
#line 623
#error Unsupported DEBUG original branch
#endif
#line 713
