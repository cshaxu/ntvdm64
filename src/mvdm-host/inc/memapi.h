/* DIVERGENCE(MVDM-HOST-DIV-294): explicit cdecl for the existing CPU40
 * worker exports consumed by the original /Gz WOW32 DLL. Parameter types,
 * implementation and ownership are unchanged; no stack-incompatible aliases. */
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
    PULONG Address,
    ULONG Size,
    BOOLEAN Commit
    );

NTSTATUS 
__cdecl VdmFreeVirtualMemory(
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
    ULONG HostAddress,
    ULONG Size,
    PULONG IntelAddress
    );

NTSTATUS
__cdecl VdmRemoveVirtualMemory(
    ULONG IntelAddress
    );


BOOL
__cdecl VdmAddDescriptorMapping(
    USHORT SelectorStart,
    USHORT SelectorCount,
    ULONG LdtBase,
    ULONG Flat
    );
