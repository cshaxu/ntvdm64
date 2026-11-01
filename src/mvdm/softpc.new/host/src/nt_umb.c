/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    UMB.C

Abstract:

    UMB management functions for NT MVDM

Author:

    William Hsieh  (williamh) Created 21-Sept-1992

[Environment:]

    User Mode, running in the context of MVDM

[Notes:]

    optional-notes

Revision History:

--*/
#include "nt.h"
#include "ntrtl.h"              // for romdump
#include "nturtl.h"
#include "windows.h"
#include "host_def.h"
#include "insignia.h"
#include "stdlib.h"
#include "xt.h"
#include CpuH
#include "error.h"
#include "sas.h"
#include "ios.h"
#include "umb.h"

#include <nt_vdd.h>
#include <nt_vddp.h>
#include <emm.h>

PUMBNODE
SpliceUMB(
PUMBNODE    UMB,
DWORD	    dwBase,
DWORD	    Size,
WORD	    Owner
);

BOOL
VDDReserveUMB(
DWORD	    dwBase,
DWORD	    Size,
PUMBNODE    UMB
);
VOID
xmsReleaseUMBNotify(
PVOID	    Address,
DWORD	    Size
);
PUMBNODE    UMBList;

HANDLE	    UMBSectionHandle;

// This function allocate a address space from the UMB area.
// Depends on the requester, this function changes the given address
// space reservation/commitment and ownership states of the block.
// From the very beginning, InitUMBList reserves every possible UMB and
// each unused UMB has owner of UMB_OWNER_NONE or UMB_OWNER_ROM.
// An UMB_OWNER_NONE block is freed and can be claimed by anybody;
// An UMB_OWNER_ROM is mapped to system ROM area and nobody can
// claim it except UMB_OWNER_NONE which includes the ROM UMB as
// usual UMB so that its address space can be used for other purpose.
// This exception was added for VDDs(trusted requesters) only.
// An UMB_OWNER_RAM block is reserved and committed. Nobody can claim
// it except UMB_OWNER_XMS(and UMB_OWNER_XMS can only claim UMB_OWNER_RAM).
// An UMB_OWNER_EMM block is simply reserved.
// An UMB_OWNER_XMS block is reserved, committed and under XMS	control.
// An UMB_OWNER_VDD block is claimed by a VDD via VDDInstallMemoryHook.
// VDD block got special treatment because memory are committed to and
// decommitted from within the block dynamycally, depends on how the
// VDD wants to manipulate it.
//
// WARNING: The given Size and Address must on system page boundary.
//
BOOL
ReserveUMB(
WORD	Owner,
PVOID	*Address,
DWORD	*Size
)
{

#ifdef MONITOR
/* DIVERGENCE MVDM-HOST-DIV-507: MONITOR branch is not selected for this source; see mirror register. */
#endif

    PUMBNODE	UMB;
    LARGE_INTEGER SectionOffset;

    DWORD dwBase;

    dwBase = (DWORD)*Address;

    // get the UMB list header
    UMB = UMBList;
    SectionOffset.HighPart = 0;

    switch (Owner) {
	case UMB_OWNER_RAM:
	// commit memory to every free UMB in the list
	// this is the only case which works on multiple blocks
	    while (UMB != NULL) {
		if (UMB->Owner == UMB_OWNER_NONE) {
#ifndef	MONITOR
		    // Is this necessary?
		    sas_connect_memory(UMB->Base, UMB->Base + UMB->Size - 1, SAS_RAM);

#else
/* DIVERGENCE MVDM-HOST-DIV-508: MONITOR alternative is not selected for this source; see mirror register. */
#endif
		    UMB->Owner = UMB_OWNER_RAM;
		}
		UMB = UMB->Next;
	    }
	    break;

	case UMB_OWNER_EMM:

	    while (UMB != NULL) {
		if (UMB->Owner == UMB_OWNER_NONE &&
		    UMB->Size >= *Size &&
		    (dwBase == 0 || (dwBase >= UMB->Base &&
				     (dwBase + *Size) <= UMB->Base + UMB->Size))
		    )
		    break;
		UMB = UMB->Next;
	    }
	    if (UMB == NULL) {
		SetLastError(ERROR_OUTOFMEMORY);
		return FALSE;
	    }
	    if (dwBase == 0)
		dwBase = UMB->Base;

	    // the found block may be too big for the request
	    // we have to splice the block if that is the case
	    UMB = SpliceUMB(UMB, dwBase, *Size, UMB_OWNER_EMM);
	    // if failed to do something, simple fail
	    if (UMB == NULL) {
		return FALSE;
	    }
#ifdef MONITOR
/* DIVERGENCE MVDM-HOST-DIV-507: MONITOR branch is not selected for this source; see mirror register. */
#endif
	    // return the block address
	    *Address = (PVOID)UMB->Base;
	    break;

	case UMB_OWNER_XMS:
	    // search UMB_OWNER_RAM and claim the ownership
	    while (UMB != NULL && UMB->Owner != UMB_OWNER_RAM) {
		UMB = UMB->Next;
	    }
	    if (UMB == NULL) {
		// don't set last error here
		return FALSE;
	    }
	    else {
		UMB->Owner = UMB_OWNER_XMS;
		*Address = (PVOID)UMB->Base;
		*Size = UMB->Size;
	    }
	    break;

	case UMB_OWNER_VDD:
	    return(VDDReserveUMB(dwBase, *Size, UMB));

// VDDExcludeMem and VDDIncludeMem support cases
	case UMB_OWNER_NONE:
	//call to change a rom block to a free UMB block
	//the given address and size must exactly match

#ifndef	MONITOR
	    // on MIPS, rom blocks are BIOS and VIDEO. No reason to change it
	    return FALSE;
#else
/* DIVERGENCE MVDM-HOST-DIV-508: MONITOR alternative is not selected for this source; see mirror register. */
#endif

	case UMB_OWNER_ROM:
	// case to excluede a UMB
	    while (UMB != NULL && (UMB->Owner != UMB_OWNER_NONE ||
				   UMB->Base > dwBase + *Size ||
				   UMB->Base + UMB->Size < dwBase + *Size)) {
		UMB = UMB->Next;
	    }
	    if (UMB == NULL) {
		SetLastError(ERROR_ACCESS_DENIED);
		return FALSE;
	    }
	    UMB = SpliceUMB(UMB, dwBase, *Size, UMB_OWNER_ROM);
	    if (UMB == NULL) {
		return FALSE;
	    }
#ifdef	MONITOR
/* DIVERGENCE MVDM-HOST-DIV-507: MONITOR branch is not selected for this source; see mirror register. */
#endif
	    UMB->Owner = UMB_OWNER_ROM;
	    break;


	default:
	    SetLastError(ERROR_INVALID_ADDRESS);
	    return FALSE;
    }
    return TRUE;
}

// This function reclaims the given UMB.
// Note that every reclaimed UMB is set to UMB_OWNER_RAM, reserved and
// committed. After the call, the UMB can be claimed by XMS driver.
// A VDD will find that it can not reserve an UMB second time. This is
// because we have to put the address space into committed states so
// that we won't get access violation and crach VDM(remember we are
// simulating  DOS machine, a machine where applications can do whatever
// they want).

BOOL
ReleaseUMB(
WORD	Owner,
PVOID	Address,
DWORD	Size
)
{

#ifdef MONITOR
/* DIVERGENCE MVDM-HOST-DIV-507: MONITOR branch is not selected for this source; see mirror register. */
#endif

    PUMBNODE    UMB;
    LARGE_INTEGER   SectionOffset;
    DWORD       SizeView, dwBase;



    dwBase = (DWORD)Address;

    UMB = UMBList;
    // size, address and owner must match before releasing
    while (UMB != NULL && (UMB->Owner != Owner ||
			   dwBase != UMB->Base ||
			   Size != UMB->Size)) {
	UMB = UMB->Next;
    }
    if (UMB == NULL) {
	SetLastError(ERROR_INVALID_ADDRESS);
	return FALSE;
    }
    SizeView = UMB->Size;
    SectionOffset.HighPart = 0;
    SectionOffset.LowPart = dwBase - UMB_BASE_ADDRESS;

    switch (Owner) {
	case UMB_OWNER_EMM:
#ifndef	MONITOR
	    sas_connect_memory(UMB->Base, UMB->Base + UMB->Size - 1, SAS_RAM);
#else
/* DIVERGENCE MVDM-HOST-DIV-508: MONITOR alternative is not selected for this source; see mirror register. */
#endif
	    UMB->Owner = UMB_OWNER_RAM;
	    xmsReleaseUMBNotify((PVOID)UMB->Base, UMB->Size);
	    break;

	case UMB_OWNER_VDD:
#ifndef	MONITOR
	    sas_connect_memory(dwBase, dwBase + Size - 1, SAS_RAM);
#else
/* DIVERGENCE MVDM-HOST-DIV-508: MONITOR alternative is not selected for this source; see mirror register. */
#endif
	    UMB->Owner = UMB_OWNER_RAM;
	    xmsReleaseUMBNotify((PVOID)UMB->Base, UMB->Size);
	    break;

	default:
	    SetLastError(ERROR_INVALID_ADDRESS);
	    return FALSE;
    }
    return TRUE;

}
// This function commit memory to the specific address space
// for VDD.
BOOL
VDDCommitUMB(
PVOID	Address,
DWORD	Size
)
{

#ifdef MONITOR
/* DIVERGENCE MVDM-HOST-DIV-507: MONITOR branch is not selected for this source; see mirror register. */
#endif

    PUMBNODE    UMB;
    DWORD   dwBase;

    UMB = UMBList;

    dwBase = (DWORD)Address;
    while(UMB != NULL && (UMB->Owner != UMB_OWNER_VDD ||
			  UMB->Base + UMB->Size < dwBase + Size ||
			  UMB->Base > dwBase + Size)) {
	UMB = UMB->Next;
    }

    if (UMB == NULL){
	SetLastError(ERROR_INVALID_ADDRESS);
	return FALSE;
    }
#ifndef	MONITOR
    sas_connect_memory(dwBase, dwBase + Size - 1, SAS_RAM);
#else
/* DIVERGENCE MVDM-HOST-DIV-508: MONITOR alternative is not selected for this source; see mirror register. */
#endif

    return TRUE;
}
// This function decommit memory from the specific address space
// WARNING: We can never really decommit the memory. We just
// change the address space states from committed to reserve so
// that VDD will get page fault.
BOOL
VDDDeCommitUMB(
PVOID	Address,
DWORD	Size
)
{
#ifdef MONITOR
/* DIVERGENCE MVDM-HOST-DIV-507: MONITOR branch is not selected for this source; see mirror register. */
#endif

    PUMBNODE UMB;
    DWORD   dwBase;



    dwBase = (DWORD)Address;
    UMB = UMBList;
    while(UMB != NULL && (UMB->Owner != UMB_OWNER_VDD ||
			  UMB->Base + UMB->Size < dwBase + Size ||
			  UMB->Base > dwBase + Size)) {
	UMB = UMB->Next;
    }

    if (UMB == NULL) {
	SetLastError(ERROR_INVALID_ADDRESS);
	return FALSE;
    }
#ifndef	MONITOR
    sas_connect_memory(dwBase, dwBase + Size - 1, SAS_VDD);
#else
/* DIVERGENCE MVDM-HOST-DIV-508: MONITOR alternative is not selected for this source; see mirror register. */
#endif

     return TRUE;
}

VOID UMBNotify(
unsigned char code
)
{
    return;
}

// This function release the reserved EMM memory space to the caller
// After the call is made, the address space is FREE and the caller
// has to map the space immediately so that it won't be used by
// the system for storage allocation.
BOOL
GetUMBForEMM(VOID)
{
#ifdef MONITOR
/* DIVERGENCE MVDM-HOST-DIV-507: MONITOR branch is not selected for this source; see mirror register. */
#endif
    return TRUE;
}
// This function reserves an address space for VDD
// Here we map a view of section for each page within the requested
// block. This was done because the VDD may want to allocate/deallocate
// physical memory page by page.
BOOL
VDDReserveUMB(
DWORD	dwBase,
DWORD	Size,
PUMBNODE  UMB
)
{

#ifdef MONITOR
/* DIVERGENCE MVDM-HOST-DIV-507: MONITOR branch is not selected for this source; see mirror register. */
#endif

    while (UMB != NULL) {
	if (UMB->Owner == UMB_OWNER_NONE &&
	    UMB->Base <= dwBase &&
	    UMB->Base + UMB->Size >= dwBase + Size)

	    break;
	else
	    UMB = UMB->Next;
    }
    if (UMB == NULL){
	SetLastError(ERROR_INVALID_ADDRESS);
	return FALSE;
    }
    UMB = SpliceUMB(UMB, dwBase, Size, UMB_OWNER_VDD);
    if (UMB == NULL) {
	return FALSE;
    }
#ifndef	MONITOR
    sas_connect_memory(dwBase, dwBase + Size - 1, SAS_VDD);
#else
/* DIVERGENCE MVDM-HOST-DIV-508: MONITOR alternative is not selected for this source; see mirror register. */
#endif
    return TRUE;
}

// This helper function splice the given block into multiple
// sublocks(max, 3) and reserves each newly created subblock

PUMBNODE
SpliceUMB(
PUMBNODE    UMB,
DWORD	    dwBase,
DWORD	    Size,
WORD	    Owner
)
{
#ifdef MONITOR
/* DIVERGENCE MVDM-HOST-DIV-507: MONITOR branch is not selected for this source; see mirror register. */
#endif

    DWORD SizeBefore, SizeAfter;
    PUMBNODE	UMBBefore, UMBAfter;



    SizeBefore = dwBase - UMB->Base;
    SizeAfter = UMB->Size - Size - SizeBefore;
    UMBBefore = UMB;
    if (SizeAfter > 0) {
	// allocate new node(s) before we unmap the block
	UMBAfter =  (PUMBNODE) malloc(sizeof(UMBNODE));
	if (UMBAfter == NULL) {
	    SetLastError(ERROR_OUTOFMEMORY);
	    return NULL;
	}
	UMBAfter->Size = SizeAfter;
	UMBAfter->Base = dwBase + Size;
	UMBAfter->Owner = UMB_OWNER_NONE;
	UMBAfter->Next = UMB->Next;
	UMB->Next = UMBAfter;
	UMB->Size -= SizeAfter;
    }

    if (SizeBefore > 0) {
	UMBBefore = (PUMBNODE) malloc(sizeof(UMBNODE));
	if (UMBBefore == NULL) {
	    SetLastError(ERROR_OUTOFMEMORY);
	    return NULL;
	}

	UMBBefore->Size = Size;
	UMBBefore->Base = dwBase;
	UMBBefore->Owner = Owner;
	UMBBefore->Next = UMB->Next;
	UMB->Next = UMBBefore;
	UMB->Size = SizeBefore;
	UMB->Owner = UMB_OWNER_NONE;
    }
    else {
	UMB->Owner = Owner;
    }

#ifdef MONITOR
/* DIVERGENCE MVDM-HOST-DIV-507: MONITOR branch is not selected for this source; see mirror register. */
#endif
    return UMBBefore;
}

// This function initialize UMB list. Every block in the UMA
// area are chained together in a single list
// Each node in the list is either UMB_OWNER_NONE or UMB_OWNER_ROM.
#ifndef	MONITOR
BOOL
InitUMBList(VOID)
{
    PUMBNODE   UMB, UMBNew;
    static DWORD ROMs[] = { EGA_ROM_START, EGA_ROM_END,
				 BIOS_START, 0x100000
			       };
    DWORD CurAddress;
    USHORT  Index;

    UMBList = NULL;

    CurAddress = UMB_BASE_ADDRESS;
    Index  = 0;
    while (CurAddress < UMB_BASE_ADDRESS + UMB_MAX_OFFSET) {
	UMBNew = (PUMBNODE) malloc(sizeof(UMBNODE));
	if (UMBNew == NULL) {
	    SetLastError(ERROR_OUTOFMEMORY);
	    return FALSE;
	}
	UMBNew->Base = CurAddress;
	if (CurAddress == ROMs[Index]) {
	    UMBNew->Owner = UMB_OWNER_ROM;
	    UMBNew->Size = ROMs[Index + 1] - CurAddress;
	    Index += 2;
	}
	else {
	    UMBNew->Owner = UMB_OWNER_NONE;
	    UMBNew->Size  = ROMs[Index] - CurAddress;
	}
	CurAddress += UMBNew->Size;
	if (UMBList == NULL) {
	    UMBList = UMBNew;
	}
	else {
	    UMB->Next = UMBNew;
	}
	UMBNew->Next = NULL;
	UMB = UMBNew;

    }
}

#else
/* DIVERGENCE MVDM-HOST-DIV-508: MONITOR alternative is not selected for this source; see mirror register. */
#endif
