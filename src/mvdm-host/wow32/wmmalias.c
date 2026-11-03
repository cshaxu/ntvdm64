/*++ BUILD Version: 0001
 *
 *  WOW v1.0
 *
 *  Copyright (c) 1991, Microsoft Corporation
 *
 *  WMMALIAS.C
 *  WOW32 16-bit handle alias support
 *
 *  History:
 *  Created Sept-1-1992 by Chandan Chauhan (ChandanC)
 *  Modified 12-May-1992 by Mike Tricker (miketri) to add MultiMedia support
--*/


#include "precomp.h"
#pragma hdrstop
#include "wmmalias.h"

MODNAME(wmmalias.c);

HINFO   hiMMedia;       // MultiMedia handle alias info - MikeTri 12-May-1992
HINFO   hiWinsock;      // Winsock handle alias info - DavidTr 4-Oct-1992

#ifdef  DEBUG
#line 26
#error Excluded unsupported DEBUG WOW profile
#endif
#line 28
INT iLargestListSlot;

extern CRITICAL_SECTION    mmHandleCriticalSection;

#ifdef  DEBUG
#line 33
#error Excluded unsupported DEBUG WOW profile
#endif
#line 35


/*
 * Added MultiMedia functions - MikeTri 12-May-1992
 */

HAND16 GetMMedia16(HAND32 h32, INT iClass)
{
    PHMAP phm;

    EnterCriticalSection( &mmHandleCriticalSection );
    if (phm = FindHMap32(h32, &hiMMedia, iClass)) {
        LeaveCriticalSection( &mmHandleCriticalSection );
        return phm->h16;
    }
    LeaveCriticalSection( &mmHandleCriticalSection );

    return (HAND16)h32;
}


VOID FreeMMedia16(HAND16 h16)
{
    EnterCriticalSection( &mmHandleCriticalSection );
    FreeHMap16(h16, &hiMMedia);
    LeaveCriticalSection( &mmHandleCriticalSection );
}


HAND32 GetMMedia32(HAND16 h16)
{
    PHMAP phm;

    EnterCriticalSection( &mmHandleCriticalSection );
    if (phm = FindHMap16(h16, &hiMMedia)) {
        LeaveCriticalSection( &mmHandleCriticalSection );
        return phm->h32;
    }
    LeaveCriticalSection( &mmHandleCriticalSection );

    return NULL;

//  return (HAND32)INT32(h16);
}


PHMAP FindHMap32(HAND32 h32, PHINFO phi, INT iClass)
{
    INT iHash;
#ifndef NEWALIAS
    INT iList, iListEmpty;
#endif
    register PHMAP phm, phmPrev, phmEmpty;

    if (!h32 || (INT)h32 == 0xFFFF || (INT)h32 == -1)
        return NULL;

    // If we don't have a hash table yet, allocate one

    if (!phi->pphmHash) {
        if (!(phi->pphmHash = malloc_w(HASH_SLOTS*sizeof(PHMAP)))) {
            LOGDEBUG(0,("    FindHMap32 ERROR: cannot allocate hash table\n"));
            return NULL;
        }
        RtlZeroMemory(phi->pphmHash, HASH_SLOTS*sizeof(PHMAP));
    }

    // Compute the index into the hash table, and retrieve from it
    // the initial HMAP pointer

    iHash = HASH32(h32);
    phmPrev = (PHMAP)(phi->pphmHash + iHash);

    // Start walking the HMAP list, looking for a match (and keeping
    // track of any free entries we may find in case we decide to reuse it)

#ifndef NEWALIAS
    iList = 1;
#endif
    phmEmpty = NULL;
    while (phm = phmPrev->phmNext) {
        if (MASK32(phm->h32) == MASK32(h32)) {
            break;
        }
        if (phm->h32 == NULL && !phmEmpty) {
            phmEmpty = phm;
#ifndef NEWALIAS
            iListEmpty = iList;
#endif
        }
        phmPrev = phm;
#ifndef NEWALIAS
        iList++;
#endif
    }

    // If we couldn't find a match but we did find an empty HMAP structure
    // on the list, reuse it

    if (!phm && phmEmpty) {
        phm = phmEmpty;
#ifndef NEWALIAS
        iList = iListEmpty;
#endif
    }

    // If we have to allocate a new HMAP, here's where we do it

    if (!phm) {
#ifndef NEWALIAS
        if (iList > LIST_SLOTS) {
            LOGDEBUG(0,("    FindHMap32 ERROR: out of list slots for hash slot %d\n", iHash));
            return NULL;
        }
#else
#error Unsupported excluded original configuration
#endif
#line 200
        phm = malloc_w(sizeof(HMAP));
        if (!phm) {
            LOGDEBUG(0,("    FindHMap32 ERROR: cannot allocate new list entry\n"));
            return NULL;
        }
        phm->h32 = NULL;

#ifdef NEWALIAS
#error Unsupported excluded original configuration
#else
#line 220
#ifdef DEBUG
#line 221
#error DEBUG WOW alias accounting is not supported
#endif
#line 227
        phm->h16 = (HAND16)((iHash | (iList << HASH_BITS)) << RES_BITS);

        // New entries must be appended rather than inserted, because
        // our phoney 16-bit handles are dependent on position in the list

        phm->phmNext = NULL;
        phmPrev->phmNext = phm;
#endif
#line 235
    }

    // If this a new entry, initialize it

    if (!phm->h32) {
#ifdef DEBUG
#line 241
#error Excluded unsupported DEBUG WOW profile
#endif
#line 246

        // Insure that the alias pointer is valid
#ifdef NEWALIAS
#error Unsupported excluded original configuration
#endif
#line 251
        phm->h32 = h32;
        phm->htask16 = FETCHWORD(CURRENTPTD()->htask16);
        phm->iClass = iClass;
        phm->dwStyle = 0;
        phm->vpfnWndProc = 0;
        phm->pwcd = 0;
    }

    return phm;
}


PHMAP FindHMap16(HAND16 h16, PHINFO phi)
{
#ifndef NEWALIAS
    INT i, iHash, iList;
#endif
    register PHMAP phm;
#ifdef HACK32
    static HMAP hmDummy = {NULL, NULL, 0, 0, 0, 0, NULL, 0};
#endif

    if (!h16 || h16 == 0xFFFF)
    return NULL;

#ifdef HACK32
    if (h16 == TRUE)
    return &hmDummy;
#endif

    // Verify all the RES_BITS are clear
    if (h16 & ((1 << RES_BITS)-1)) {
        WOW32ASSERT(FALSE);
        return NULL;
    }

    h16 >>= RES_BITS;

#ifdef NEWALIAS
#error Unsupported excluded original configuration
#else
#line 303
    iHash = h16 & HASH_MASK;
    iList = (h16 & LIST_MASK) >> HASH_BITS;

    phm = (PHMAP)(phi->pphmHash + iHash);

    i = iList;
    while (i-- && phm) {
    phm = phm->phmNext;
        }
#endif
#line 313
    if (!phm) {
        LOGDEBUG(0,("    FindHMap16 ERROR: could not find %04x\n", h16<<RES_BITS));
        return NULL;
    }
    // Verify requested handle is same as stored in alias
    if (h16 != (HAND16)(phm->h16>>RES_BITS)) {
        LOGDEBUG(0, ("FindHMap16: Got bad H16\n"));
        WOW32ASSERT(FALSE);
        return NULL;
    }

#ifdef DEBUG
#line 325
#error Excluded unsupported DEBUG WOW profile
#endif
#line 330

    return phm;
}


VOID FreeHMap16(HAND16 h16, PHINFO phi)
{
    register PHMAP phm;

    if (phm = FindHMap16(h16, phi)) {
        LOGDEBUG(7,("    Freeing %s alias %04x for %08lx\n",
                    GetHMapNameM(phi, phm->iClass), phm->h16, phm->h32));

//        if (phm->iClass == WOWCLASS_WIN16)
//            phm->pwcd->nWindows--;

        // BUGBUG -- We'll eventually want some garbage collection... -JTP


        phm->h32 = NULL;

#ifdef NEWALIAS
#error Unsupported excluded original configuration
#endif
#line 364
        return;
    }
    LOGDEBUG(1,("    FreeHMap16: handle alias %04x not found\n"));
}


PSZ GetHMapNameM(PHINFO phi, INT iClass)
{
    return "MMEDIA";
}
