/*++
 *
 *  WOW v1.0
 *
 *  Copyright (c) 1991, Microsoft Corporation
 *
 *  WKGLOBAL.C
 *  WOW32 16-bit Kernel API support
 *
 *  History:
 *  Created 07-Mar-1991 by Jeff Parsons (jeffpar)
--*/


#include "precomp.h"
#pragma hdrstop
#include "wkglobal.h"

MODNAME(wkglobal.c);


ULONG FASTCALL WK32GlobalAlloc(PVDMFRAME pFrame)
{
    ULONG ul;
    register PGLOBALALLOC16 parg16;

    GETARGPTR(pFrame, sizeof(GLOBALALLOC16), parg16);

    ul = GETHGLOBAL16(GlobalAlloc(
	WORD32(parg16->f1),
	DWORD32(parg16->f2)
    ));

    FREEARGPTR(parg16);
    RETURN(ul);
}


ULONG FASTCALL WK32GlobalCompact(PVDMFRAME pFrame)
{
    ULONG ul;
    register PGLOBALCOMPACT16 parg16;

    GETARGPTR(pFrame, sizeof(GLOBALCOMPACT16), parg16);

#ifdef API16
#error DIVERGENCE MVDM-HOST-DIV-603: unsupported API16 profile
#else
#line 51
    ul = 0;
#endif
#line 53

    FREEARGPTR(parg16);
    RETURN(ul);
}


ULONG FASTCALL WK32GlobalFix(PVDMFRAME pFrame)
{
    register PGLOBALFIX16 parg16;

    GETARGPTR(pFrame, sizeof(GLOBALFIX16), parg16);

#ifdef API16
#error DIVERGENCE MVDM-HOST-DIV-603: unsupported API16 profile
#endif
#line 70

    FREEARGPTR(parg16);
    RETURN(0);
}


ULONG FASTCALL WK32GlobalFlags(PVDMFRAME pFrame)
{
    ULONG ul;
    register PGLOBALFLAGS16 parg16;

    GETARGPTR(pFrame, sizeof(GLOBALFLAGS16), parg16);

    ul = GETWORD16(GlobalFlags(
	HGLOBAL32(parg16->f1)
    ));

    FREEARGPTR(parg16);
    RETURN(ul);
}


ULONG FASTCALL WK32GlobalFree(PVDMFRAME pFrame)
{
    ULONG ul;
    register PGLOBALFREE16 parg16;

    GETARGPTR(pFrame, sizeof(GLOBALFREE16), parg16);

    ul = GETHGLOBAL16(GlobalFree(
	HGLOBAL32(parg16->f1)
    ));

    FREEARGPTR(parg16);
    RETURN(ul);
}


ULONG FASTCALL WK32GlobalHandle(PVDMFRAME pFrame)
{
    ULONG ul;
    register PGLOBALHANDLE16 parg16;

    GETARGPTR(pFrame, sizeof(GLOBALHANDLE16), parg16);

#ifdef API16
#error DIVERGENCE MVDM-HOST-DIV-603: unsupported API16 profile
#else
#line 120
    ul = 0;
#endif
#line 122

    FREEARGPTR(parg16);
    RETURN(ul);
}


ULONG FASTCALL WK32GlobalLRUNewest(PVDMFRAME pFrame)
{
    ULONG ul;
    register PGLOBALLRUNEWEST16 parg16;

    GETARGPTR(pFrame, sizeof(GLOBALLRUNEWEST16), parg16);

    ul = GETHGLOBAL16(GlobalLRUNewest(
	HGLOBAL32(parg16->f1)
    ));

    FREEARGPTR(parg16);
    RETURN(ul);
}


ULONG FASTCALL WK32GlobalLRUOldest(PVDMFRAME pFrame)
{
    ULONG ul;
    register PGLOBALLRUOLDEST16 parg16;

    GETARGPTR(pFrame, sizeof(GLOBALLRUOLDEST16), parg16);

    ul = GETHGLOBAL16(GlobalLRUOldest(
	HGLOBAL32(parg16->f1)
    ));

    FREEARGPTR(parg16);
    RETURN(ul);
}


ULONG FASTCALL WK32GlobalLock(PVDMFRAME pFrame)
{
    ULONG ul;
    register PGLOBALLOCK16 parg16;

    GETARGPTR(pFrame, sizeof(GLOBALLOCK16), parg16);

    ul = GETLPSTRBOGUS(GlobalLock(
	HGLOBAL32(parg16->f1)
    ));

    FREEARGPTR(parg16);
    RETURN(ul);
}


ULONG FASTCALL WK32GlobalNotify(PVDMFRAME pFrame)
{
    register PGLOBALNOTIFY16 parg16;

    GETARGPTR(pFrame, sizeof(GLOBALNOTIFY16), parg16);

// This is a HACK and MUST be fixed, ChandanC, 11/7/91. This function
// has been removed from the system.

//  GlobalNotify(
//  PROC32(parg16->f1)
//  );

    FREEARGPTR(parg16);
    RETURN(0);
}


ULONG FASTCALL WK32GlobalPageLock(PVDMFRAME pFrame)
{
    ULONG ul;
    register PGLOBALPAGELOCK16 parg16;

    GETARGPTR(pFrame, sizeof(GLOBALPAGELOCK16), parg16);

#ifdef API16
#error DIVERGENCE MVDM-HOST-DIV-603: unsupported API16 profile
#else
#line 206
    ul = 0;
#endif
#line 208

    FREEARGPTR(parg16);
    RETURN(ul);
}


ULONG FASTCALL WK32GlobalPageUnlock(PVDMFRAME pFrame)
{
    ULONG ul;
    register PGLOBALPAGEUNLOCK16 parg16;

    GETARGPTR(pFrame, sizeof(GLOBALPAGEUNLOCK16), parg16);

#ifdef API16
#error DIVERGENCE MVDM-HOST-DIV-603: unsupported API16 profile
#else
#line 226
    ul = 0;
#endif
#line 228

    FREEARGPTR(parg16);
    RETURN(ul);
}


ULONG FASTCALL WK32GlobalReAlloc(PVDMFRAME pFrame)
{
    ULONG ul;
    register PGLOBALREALLOC16 parg16;

    GETARGPTR(pFrame, sizeof(GLOBALREALLOC16), parg16);

    ul = GETHGLOBAL16(GlobalReAlloc(
	HGLOBAL32(parg16->f1),
	DWORD32(parg16->f2),
	WORD32(parg16->f3)
    ));

    FREEARGPTR(parg16);
    RETURN(ul);
}


ULONG FASTCALL WK32GlobalSize(PVDMFRAME pFrame)
{
    ULONG ul;
    register PGLOBALSIZE16 parg16;

    GETARGPTR(pFrame, sizeof(GLOBALSIZE16), parg16);

    ul = GETDWORD16(GlobalSize(
	HGLOBAL32(parg16->f1)
    ));

    FREEARGPTR(parg16);
    RETURN(ul);
}


ULONG FASTCALL WK32GlobalUnWire(PVDMFRAME pFrame)
{
    ULONG ul;
    register PGLOBALUNWIRE16 parg16;

    GETARGPTR(pFrame, sizeof(GLOBALUNWIRE16), parg16);

#ifdef API16
#error DIVERGENCE MVDM-HOST-DIV-603: unsupported API16 profile
#else
#line 280
    ul = 0;
#endif
#line 282

    FREEARGPTR(parg16);
    RETURN(ul);
}


ULONG FASTCALL WK32GlobalUnfix(PVDMFRAME pFrame)
{
    ULONG ul;
    register PGLOBALUNFIX16 parg16;

    GETARGPTR(pFrame, sizeof(GLOBALUNFIX16), parg16);

#ifdef API16
#error DIVERGENCE MVDM-HOST-DIV-603: unsupported API16 profile
#else
#line 300
    ul = 0;
#endif
#line 302

    FREEARGPTR(parg16);
    RETURN(ul);
}


ULONG FASTCALL WK32GlobalUnlock(PVDMFRAME pFrame)
{
    ULONG ul;
    register PGLOBALUNLOCK16 parg16;

    GETARGPTR(pFrame, sizeof(GLOBALUNLOCK16), parg16);

    ul = GETBOOL16(GlobalUnlock(
	HGLOBAL32(parg16->f1)
    ));

    FREEARGPTR(parg16);
    RETURN(ul);
}


ULONG FASTCALL WK32GlobalWire(PVDMFRAME pFrame)
{
    ULONG ul;
    register PGLOBALWIRE16 parg16;

    GETARGPTR(pFrame, sizeof(GLOBALWIRE16), parg16);

#ifdef API16
#error DIVERGENCE MVDM-HOST-DIV-603: unsupported API16 profile
#else
#line 336
    ul = 0;
#endif
#line 338

    FREEARGPTR(parg16);
    RETURN(ul);
}
