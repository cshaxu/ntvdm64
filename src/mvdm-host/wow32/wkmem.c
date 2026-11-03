/*++
 *
 *  WOW v1.0
 *
 *  Copyright (c) 1992, Microsoft Corporation
 *
 *  WKMEM.C
 *  WOW32 KRNL386 Virtual Memory Management Functions
 *
 *  History:
 *  Created 3-Dec-1992 by Matt Felton (mattfe)
 *
--*/

#include "precomp.h"
#pragma hdrstop
#include "memapi.h"

MODNAME(wkman.c);


LPVOID FASTCALL WK32VirtualAlloc(PVDMFRAME pFrame)
{
    PVIRTUALALLOC16 parg16;
    LPVOID lpBaseAddress;
#ifndef i386
    NTSTATUS Status;
#endif

    GETARGPTR(pFrame, sizeof(VIRTUALALLOC16), parg16);


#ifndef i386
    Status = VdmAllocateVirtualMemory((PULONG)&lpBaseAddress,
                                      parg16->cbSize,
                                      TRUE);

    if (!NT_SUCCESS(Status)) {

        if (Status == STATUS_NOT_IMPLEMENTED) {
#endif // i386

            lpBaseAddress = VirtualAlloc((LPVOID)parg16->lpvAddress,
                                            parg16->cbSize,
                                            parg16->fdwAllocationType,
                                            parg16->fdwProtect);


#ifndef i386
        } else {

            lpBaseAddress = NULL;
        }

    }
#endif // i386

#line 58
#ifdef i386
#line 59
#error DIVERGENCE MVDM-HOST-DIV-589: historical i386 WOW branch is not selected
#endif
#line 87

    FREEARGPTR(parg16);
    return (lpBaseAddress);
}

BOOL FASTCALL WK32VirtualFree(PVDMFRAME pFrame)
{
    PVIRTUALFREE16 parg16;
    BOOL fResult;
#ifndef i386
    NTSTATUS Status;
#endif

    GETARGPTR(pFrame, sizeof(VIRTUALFREE16), parg16);

#ifndef i386
    Status = VdmFreeVirtualMemory((ULONG)parg16->lpvAddress);
    fResult = NT_SUCCESS(Status);

    if (Status == STATUS_NOT_IMPLEMENTED) {
#endif // i386

        fResult = VirtualFree((LPVOID)parg16->lpvAddress,
                                 parg16->cbSize,
                                 parg16->fdwFreeType);


#ifndef i386
    }
#endif // i386

    FREEARGPTR(parg16);
    return (fResult);
}


#line 123
#if 0
#line 124
#error DIVERGENCE MVDM-HOST-DIV-496: original literal #if 0 excludes this legacy branch; see mirror register.
#endif
#line 156


VOID FASTCALL WK32GlobalMemoryStatus(PVDMFRAME pFrame)
{
    PGLOBALMEMORYSTATUS16 parg16;
    LPMEMORYSTATUS pMemStat;

    GETARGPTR(pFrame, sizeof(GLOBALMEMORYSTATUS16), parg16);
    GETVDMPTR(parg16->lpmstMemStat, 32, pMemStat);

    GlobalMemoryStatus(pMemStat);

    FREEVDMPTR(pMemStat);
    FREEARGPTR(parg16);
}
