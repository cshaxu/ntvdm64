/*++ BUILD Version: 0001    // Increment this if a change has global effects

Copyright (c) 1989-1993  Microsoft Corporation

Module Name:

    ntrtl.h

Abstract:

    Include file for NT runtime routines that are callable by only
    user mode code in various.

Author:

    Steve Wood (stevewo) 10-Aug-1989

Environment:

    These routines are statically linked in the caller's executable and
    are callable in only from user mode.  They make use of Nt system
    services.

Revision History:

--*/

#line 28
#ifndef _NTURTL_
#define _NTURTL_
#include <ntrtl.h>
#line 257
ULONG NTAPI RtlIsDosDeviceName_U(PWSTR DosFileName);
#line 294
BOOLEAN NTAPI RtlDosPathNameToNtPathName_U(PCWSTR DosFileName, PUNICODE_STRING NtFileName, PWSTR *FilePart, PVOID RelativeName);
#line 1103
#endif  // _NTURTL_
