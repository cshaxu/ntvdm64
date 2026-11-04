/* DIVERGENCE(MVDM-HOST-DIV-294): explicit cdecl for the existing CPU40
 * worker exports consumed by the original /Gz WOW32 DLL. Parameter types,
 * implementation and ownership are unchanged; no stack-incompatible aliases. */
#line 1
/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    memapi.h

Abstract:

    This header defines the function prototypes for the memory
    manipulation routines in the 486 emulator.

Author:

    Neil Sandlin (neilsa)

Notes:

    
Revision History:


--*/

NTSTATUS
__cdecl VdmAllocateVirtualMemory(
#line 28
    PULONG Address,
    ULONG Size,
    BOOLEAN Commit
    );

NTSTATUS 
__cdecl VdmFreeVirtualMemory(
#line 35
    ULONG Address
    );

NTSTATUS
VdmCommitVirtualMemory(
    ULONG Address,
    ULONG Size
    );

NTSTATUS
VdmDeCommitVirtualMemory(
    ULONG Address,
    ULONG Size
    );

NTSTATUS
VdmQueryFreeVirtualMemory(
    PULONG FreeBytes,
    PULONG LargestFreeBlock
    );

NTSTATUS
VdmReallocateVirtualMemory(
    ULONG OldAddress,
    PULONG NewAddress,
    ULONG NewSize
    );

NTSTATUS
__cdecl VdmAddVirtualMemory(
#line 65
    ULONG HostAddress,
    ULONG Size,
    PULONG IntelAddress
    );

NTSTATUS
__cdecl VdmRemoveVirtualMemory(
#line 72
    ULONG IntelAddress
    );


BOOL
__cdecl VdmAddDescriptorMapping(
#line 78
    USHORT SelectorStart,
    USHORT SelectorCount,
    ULONG LdtBase,
    ULONG Flat
    );
