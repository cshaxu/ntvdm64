/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    xmem.c

Abstract:

    This module contains routines for allocating and freeing "extended" memory.
    The memory is allocated directly from NT.

Author:

    Dave Hastings (daveh) 12-Dec-1992

Notes:

    Moved from dpmi32\i386
    
Revision History:

    09-Feb-1994 (daveh) 
        Modified to be the common front end for the memory allocation.  Calls
        processor specific code to do actual allocation
        
--*/
#include "precomp.h"
#pragma hdrstop
#include "softpc.h"
#include <malloc.h>
#include "mvdm_host_identity.h"

//
// Xmem structure
//
typedef struct _Xmem {
    PVOID Address;
    ULONG Length;
    struct _Xmem * Prev;
    struct _Xmem * Next;
    WORD Owner;

} XMEM_BLOCK, *PXMEM_BLOCK;

XMEM_BLOCK  XmemHead = { NULL, 0, &XmemHead, &XmemHead, 0};

#define DELETE_BLOCK(BLK)   (BLK->Prev)->Next = BLK->Next;\
                (BLK->Next)->Prev = BLK->Prev

#define INSERT_BLOCK(BLK)   BLK->Next = XmemHead.Next; BLK->Prev= XmemHead.Next->Prev;\
                (XmemHead.Next)->Prev = BLK; XmemHead.Next = BLK

VOID
DpmiAllocateXmem(
    VOID
    )
/*++

Routine Description:

    This routine allocates a block of "extended" memory from NT.  The
    blocks allocated this way will be 64K aligned (for now).  The address
    of the block is returned to the segmented app in bx:cx

Arguments:

    None.

Return Value:

    None.

--*/
{
    ULONG BlockAddress, BlockSize;
    uint32_t BlockIdentity;
    NTSTATUS Status;
    PXMEM_BLOCK XmemBlock;

    //
    // Get a block of memory from NT (any base address)
    //
    BlockSize = ((ULONG)getBX() << 16) | getCX();
    BlockAddress = 0;
    Status = DpmiAllocateVirtualMemory(
        (PVOID)&BlockAddress,
        &BlockSize
        );

    if (!NT_SUCCESS(Status)) {
        setCF(1);
#if DBG
        OutputDebugString("DPMI: DpmiAllocateXmem failed to get memory block\n");
#endif
        return;
    }
    XmemBlock = malloc(sizeof(XMEM_BLOCK));
    if (!XmemBlock) {
        setCF(1);
        DpmiFreeVirtualMemory(
            (PVOID)&BlockAddress,
            &BlockSize
            );
        return;
    }
    XmemBlock->Address = (PVOID)(uintptr_t)BlockAddress;
    XmemBlock->Length = BlockSize;
    XmemBlock->Owner = getDX();
    INSERT_BLOCK(XmemBlock);

    /* DIVERGENCE(MVDM-HOST-DIV-012): the original x86 source split the
     * native XMEM_BLOCK pointer into SI:DI.  Preserve the 32-bit ABI and
     * source lifecycle, but publish an opaque session host-resource identity
     * instead; no native pointer crosses the MVDM boundary on x64. */
    if (!mvdm_host_identity_publish((uintptr_t)XmemBlock, &BlockIdentity)) {
        DELETE_BLOCK(XmemBlock);
        free(XmemBlock);
        DpmiFreeVirtualMemory((PVOID)&BlockAddress, &BlockSize);
        setCF(1);
        return;
    }

    //
    // Return the information about the block
    //
    setBX((USHORT)(BlockAddress >> 16));
    setCX((USHORT)(BlockAddress & 0x0000FFFF));
    //
    // Use xmem block addresss as handle
    //
    setSI((USHORT)(BlockIdentity >> 16));
    setDI((USHORT)(BlockIdentity & 0x0000FFFF));
    setCF(0);
}

VOID
DpmiFreeXmem(
    VOID
    )
/*++

Routine Description:

    This routine frees a block of "extended" memory from NT.

Arguments:

    None.

Return Value:

    None.

--*/
{
    PXMEM_BLOCK XmemBlock;
    uint32_t BlockIdentity;
    uintptr_t BlockValue;
    NTSTATUS Status;
    PVOID BlockAddress;
    ULONG BlockSize;

    BlockIdentity = ((ULONG)getSI() << 16) | getDI();
    if (!mvdm_host_identity_resolve(BlockIdentity, &BlockValue)) {
        setCF(1);
        return;
    }
    XmemBlock = (PXMEM_BLOCK)BlockValue;

    BlockAddress = XmemBlock->Address;
    BlockSize = XmemBlock->Length;

    Status = DpmiFreeVirtualMemory(
        &BlockAddress,
        &BlockSize
        );

    if (!NT_SUCCESS(Status)) {
        setCF(1);
#if DBG
        OutputDebugString("DPMI: DpmiFreeXmem failed to free block\n");
#endif
        return;
    }

    DELETE_BLOCK(XmemBlock);

    (void)mvdm_host_identity_release(BlockIdentity);
    free(XmemBlock);
    setCF(0);
    return;
}

VOID
DpmiReallocateXmem(
    VOID
    )
/*++

Routine Description:

    This routine resizes a block of "extended memory".  If the change in size
    is less than 4K, no change is made.

Arguments:

    None.

Return Value:

    None.

--*/
{
    PXMEM_BLOCK OldBlock;
    uint32_t BlockIdentity;
    uintptr_t BlockValue;
    ULONG BlockAddress, NewSize;
    NTSTATUS Status;

    BlockIdentity = ((ULONG)getSI() << 16) | getDI();
    if (!mvdm_host_identity_resolve(BlockIdentity, &BlockValue)) {
        setCF(1);
        return;
    }
    OldBlock = (PXMEM_BLOCK)BlockValue;
    NewSize = (((ULONG)getBX() << 16) | getCX());

    BlockAddress = 0;
    Status = DpmiReallocateVirtualMemory(
        OldBlock->Address,
        OldBlock->Length,
        (PVOID)&BlockAddress,
        &NewSize
        );

    if (!NT_SUCCESS(Status)) {
        setCF(1);
#if DBG
        OutputDebugString("DPMI: DpmiAllocateXmem failed to get memory block\n");
#endif
        return;
    }

    OldBlock->Address = (PVOID)(uintptr_t)BlockAddress;
    OldBlock->Length = NewSize;
    
    //
    // Return the information about the block
    //
    setBX((USHORT)(BlockAddress >> 16));
    setCX((USHORT)(BlockAddress & 0x0000FFFF));
  
    setCF(0);
}

VOID
DpmiFreeAppXmem(
    VOID
    )
/*++

Routine Description:

    This routine frees Xmem allocated for the application

Arguments:

    Client DX = client PSP selector

Return Value:

    TRUE  if everything goes fine.
    FALSE if unable to release the memory
--*/
{
    PXMEM_BLOCK p1, p2;
    uint32_t BlockIdentity;
    NTSTATUS Status;
    PVOID BlockAddress;
    ULONG BlockSize;
    WORD  selClientPSP;


    p1 = XmemHead.Next;
    selClientPSP = getDX();

    while(p1 != &XmemHead) {
        if (p1->Owner == selClientPSP) {
            BlockAddress = p1->Address;
            BlockSize = p1->Length;

            Status = DpmiFreeVirtualMemory(
                &BlockAddress,
                &BlockSize
                );

            if (!NT_SUCCESS(Status)) {
#if DBG
                OutputDebugString("DPMI: DpmiFreeXmem failed to free block\n");
#endif
                return;
            }
            p2 = p1->Next;
            DELETE_BLOCK(p1);
            if (mvdm_host_identity_lookup((uintptr_t)p1, &BlockIdentity))
                (void)mvdm_host_identity_release(BlockIdentity);
            free(p1);
            p1 = p2;
            continue;
        }
        p1 = p1->Next;
    }
    return;
}

VOID
DpmiFreeAllXmem(
    VOID
    )
/*++

Routine Description:

    This function frees all allocated xmem.

Arguments:

    none
    
Return Value:

    None.

--*/
{
    PXMEM_BLOCK p1, p2;
    uint32_t BlockIdentity;
    NTSTATUS Status;
    PVOID BlockAddress;
    ULONG BlockSize;
    
    p1 = XmemHead.Next;
    while(p1 != &XmemHead) {
        BlockAddress = p1->Address;
        BlockSize = p1->Length;

        Status = DpmiFreeVirtualMemory(
            &BlockAddress,
            &BlockSize
            );

        if (!NT_SUCCESS(Status)) {
#if DBG
            OutputDebugString("DPMI: DpmiFreeXmem failed to free block\n");
#endif
            return;
        }
        p2 = p1->Next;
        DELETE_BLOCK(p1);
        if (mvdm_host_identity_lookup((uintptr_t)p1, &BlockIdentity))
            (void)mvdm_host_identity_release(BlockIdentity);
        free(p1);
        p1 = p2;
    }
}
