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



                                           // some apps free global memory
LPVOID  glpvDelayFree[4];           // which is in turn freed by kernel as the asks it
DWORD   gdwDelayFree;               // but then app comes back and tries to access it
                                           // this is our hack variables  to accomodate them


ULONG FASTCALL WK32VirtualAlloc(PVDMFRAME pFrame)
{
    PVIRTUALALLOC16 parg16;
    ULONG lpBaseAddress;
#ifndef i386
    NTSTATUS Status;
#endif

    GETARGPTR(pFrame, sizeof(VIRTUALALLOC16), parg16);


#ifndef i386
    Status = VdmAllocateVirtualMemory(&lpBaseAddress,
                                      parg16->cbSize,
                                      TRUE);

    if (!NT_SUCCESS(Status)) {

        if (Status == STATUS_NOT_IMPLEMENTED) {
#endif // i386

            lpBaseAddress = (ULONG) VirtualAlloc((LPVOID)parg16->lpvAddress,
                                                  parg16->cbSize,
                                                  parg16->fdwAllocationType,
                                                  parg16->fdwProtect);


#ifndef i386
        } else {

            lpBaseAddress = 0;
        }

    }
#endif // i386

#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-589: historical i386 WOW branch is not selected
#endif
#line 94

    FREEARGPTR(parg16);
    return (lpBaseAddress);
}

ULONG FASTCALL WK32VirtualFree(PVDMFRAME pFrame)
{
    PVIRTUALFREE16 parg16;

    ULONG fResult;
#ifndef i386
    NTSTATUS Status;
#endif


// Delay  free
// some apps, ntbug 90849 CreateScreenSavers Quick and Easy
// free 16 bit global heap then try to access it again
// but kernel has already freed/compacted global heap
// this will delay that process for a while (something similar to DisableHeapLookAside in nt
// Millenium implemented something similar
// -jarbats

    if( NULL != glpvDelayFree[gdwDelayFree])
    {

#ifndef i386
    Status = VdmFreeVirtualMemory( glpvDelayFree[gdwDelayFree]);
    fResult = NT_SUCCESS(Status);

    if (Status == STATUS_NOT_IMPLEMENTED) {
#endif // i386

        fResult = VirtualFree(glpvDelayFree[gdwDelayFree],
                              0,
                              MEM_RELEASE);


#ifndef i386
    }
#endif // i386

    }

    GETARGPTR(pFrame, sizeof(VIRTUALFREE16), parg16);

    glpvDelayFree[gdwDelayFree] = (LPVOID) parg16->lpvAddress;
    gdwDelayFree++;
    gdwDelayFree &= 3;


    FREEARGPTR(parg16);
    return (TRUE);
}


#if 0
#error DIVERGENCE MVDM-HOST-DIV-496: original literal #if 0 excludes this legacy branch; see mirror register.
#endif
#line 183


ULONG FASTCALL WK32GlobalMemoryStatus(PVDMFRAME pFrame)
{
    PGLOBALMEMORYSTATUS16 parg16;
    LPMEMORYSTATUS pMemStat;

    GETARGPTR(pFrame, sizeof(GLOBALMEMORYSTATUS16), parg16);
    GETVDMPTR(parg16->lpmstMemStat, 32, pMemStat);

    GlobalMemoryStatus(pMemStat);

    //
    // if /3GB switch is enabled in boot.ini, GlobalmemoryStatus may return
    // 0x7fffffff dwTotalVirtual and dwAvailVirtal.  This will confuse some apps
    // in thinking something is wrong.
    //

    if (pMemStat->dwAvailVirtual == 0x7fffffff &&
        pMemStat->dwTotalVirtual == 0x7fffffff ) {        // yes we need to check dwTotalVirtual too
        pMemStat->dwAvailVirtual -= 0x500000;
    }
    FREEVDMPTR(pMemStat);
    FREEARGPTR(parg16);
    return 0;  // unused
}
