
/*++ BUILD Version: 0008    // Increment this if a change has global effects

Copyright (c) 1989-1999  Microsoft Corporation

Module Name:

    ntexapi.h

Abstract:

    This module is the header file for the all the system services that
    are contained in the "ex" directory.

Author:

    David N. Cutler (davec) 5-May-1989

Revision History:

--*/

#line 23
#ifndef _NTEXAPI_
#define _NTEXAPI_

#line 26
#if _MSC_VER > 1000
#pragma once
#endif
#line 30
#ifdef __cplusplus
#line 31
#error Excluded unsupported C++ NT executive declaration context
#endif
#line 615
//
// Performance Counter Definitions
//

NTSYSAPI
NTSTATUS
NTAPI
NtQueryPerformanceCounter (
    OUT PLARGE_INTEGER PerformanceCounter,
    OUT PLARGE_INTEGER PerformanceFrequency OPTIONAL
    );

#line 1063
#if DEVL
#line 1064
#error Excluded unsupported DEVL NT executive declarations
#endif
#line 1409
#ifndef _X86_

#line 1411
#ifndef IsNEC_98
#endif

#line 1415
#ifndef IsNotNEC_98
#endif

#line 1419
#ifndef SetNEC_98
#endif

#line 1423
#ifndef SetNotNEC_98
#endif
#endif
#line 1439
#ifdef _MAC
#line 1440
#error Excluded unsupported Mac NT executive profile
#endif
#line 1459
#if defined(_WIN64)
#line 1460
#error Excluded unsupported 64-bit NT executive shared data profile
#endif
#line 1467
#if defined(_WIN64)
#line 1468
#error Excluded unsupported 64-bit NT executive shared data profile
#else
#endif
#line 1480
#if defined(_WIN64)
#line 1481
#error Excluded unsupported 64-bit NT executive shared data profile
#endif
#line 1573
#if defined(REMOTE_BOOT)
#line 1574
#error Excluded unsupported NT executive remote boot declarations
#endif
#line 1602
#if defined(USER_SHARED_DATA)
#line 1603
#error Excluded unsupported NT executive shared-data tick implementation
#else
#line 1632
NTSYSAPI
ULONG
NTAPI
NtGetTickCount(
    VOID
    );

#endif
#line 1842
#ifdef __cplusplus
#line 1843
#error Excluded unsupported C++ NT executive declaration context
#endif
#line 1846
#endif // _NTEXAPI_
