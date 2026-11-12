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
#line 255
ULONG
NTAPI
RtlIsDosDeviceName_U(
    PWSTR DosFileName
    );
#line 262
ULONG
NTAPI
RtlGetFullPathName_U(
    PCWSTR lpFileName,
    ULONG nBufferLength,
    PWSTR lpBuffer,
    PWSTR *lpFilePart
    );

#line 272
ULONG
NTAPI
RtlGetCurrentDirectory_U(
    ULONG nBufferLength,
    PWSTR lpBuffer
    );

#line 280
NTSTATUS
NTAPI
RtlSetCurrentDirectory_U(
    PUNICODE_STRING PathName
    );

#line 292
BOOLEAN
NTAPI
RtlDosPathNameToNtPathName_U(
    PCWSTR DosFileName,
    PUNICODE_STRING NtFileName,
    PWSTR *FilePart OPTIONAL,
    PVOID RelativeName OPTIONAL
    );
#line 1103
#endif  // _NTURTL_
