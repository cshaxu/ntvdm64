/*++ BUILD Version: 0007    // Increment this if a change has global effects

Copyright (c) 1989-1993  Microsoft Corporation

Module Name:

    ntpsapi.h

Abstract:

    This module contains the process structure APIs and any public data
    structures needed to call these APIs.

Author:

    Mark Lucovsky (markl) 24-Feb-1989

Revision History:

--*/

#ifndef _NTPSAPI_
#define _NTPSAPI_

#if defined(OPENNT_ADAPTER_NT_ALERT_THREAD)
/*
 * DIVERGENCE: OPENNT-HOST-008
 *
 * The selected SoftPC timer is linked against the source-shaped public
 * adapter rather than NT4's private NTDLL alert primitive.  Keep the exact
 * name, parameter and NTSTATUS contract while omitting DLL import decoration
 * for that one adapter-backed build route.
 */
#define OPENNT_NT_ALERT_THREAD_API
#else
#define OPENNT_NT_ALERT_THREAD_API NTSYSAPI
#endif

#line 1004
OPENNT_NT_ALERT_THREAD_API
NTSTATUS
NTAPI
NtAlertThread(
    IN HANDLE ThreadHandle
    );

#undef OPENNT_NT_ALERT_THREAD_API

#line 138
#ifndef CLIENT_ID64_DEFINED
#line 139
#endif
#line 150

#line 386
#ifdef _IA64_
#line 387
#error Excluded unsupported original IA64 process API profile
#endif
#line 390

#line 396
#if !defined(_X86_)
#line 397
#else
#line 420
#endif
#line 422

#line 463
#if DBG
#line 464
#error Excluded unsupported original DBG process API profile
#endif
#line 477

#line 799
#ifdef _IA64_
#line 800
#error Excluded unsupported original IA64 process API profile
#endif
#line 802

#line 825
#ifdef _IA64_
#line 826
#error Excluded unsupported original IA64 process API profile
#endif
#line 829

#line 832
#ifdef _IA64_
#line 833
#error Excluded unsupported original IA64 process API profile
#endif
#line 836

#endif
#line 1294
