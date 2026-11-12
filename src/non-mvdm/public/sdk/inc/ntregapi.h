/*++ BUILD Version: 0009    // Increment this if a change has global effects

Copyright (c) 1991-1999  Microsoft Corporation

Module Name:

    ntregapi.h

Abstract:

    This module contains the registration apis and related structures,
    in the forms for use with the Nt api set (as opposed to the win api set).

Author:

    Bryan M. Willman (bryanwi) 26-Aug-1991

Revision History:

--*/

#line 22
#ifndef _NTREGAPI_
#define _NTREGAPI_

#line 25
#if _MSC_VER > 1000
#pragma once
#endif

#line 29
#ifdef __cplusplus
#line 30
#error Excluded unsupported C++ NT registry declaration context
#endif
#line 233
typedef struct _KEY_VALUE_PARTIAL_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   DataLength;
    UCHAR   Data[1];            // Variable size
} KEY_VALUE_PARTIAL_INFORMATION, *PKEY_VALUE_PARTIAL_INFORMATION;
#line 253
typedef enum _KEY_VALUE_INFORMATION_CLASS {
    KeyValueBasicInformation,
    KeyValueFullInformation,
    KeyValuePartialInformation,
    KeyValueFullInformationAlign64,
    KeyValuePartialInformationAlign64
} KEY_VALUE_INFORMATION_CLASS;
#line 424
NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryValueKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING ValueName,
    IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    OUT PVOID KeyValueInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    );
#line 520
#ifdef __cplusplus
#line 521
#error Excluded unsupported C++ NT registry declaration context
#endif
#line 524
#endif // _NTREGAPI_
