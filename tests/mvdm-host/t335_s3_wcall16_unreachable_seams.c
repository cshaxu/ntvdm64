/*
 * Link seams for unrelated exports that share wcall16.c with CallBack16.
 *
 * The fixture calls only CallBack16.  LocalLock16/GetExePtr16 are not
 * reachable from that transaction but the historical translation unit keeps
 * them adjacent.  These seams deliberately fail fast if that assumption is
 * ever violated; they are not product providers.
 */
#include "precomp.h"
#include "wow32.h"

BOOL fWowMode = FALSE;
HMODCACHE ghModCache[CHMODCACHE] = { 0 };

/* The original body reaches the worker's private WOW32Reserved slot.  This
 * focused fixture supplies only that per-thread storage; it is not a product
 * TEB provider and cannot stand in for a guest callback. */
POPENNT_SUPPORT_TEB NTAPI opennt_support_current_teb(VOID)
{
    static OPENNT_SUPPORT_TEB teb;
    return &teb;
}

PVOID FASTCALL
GetPModeVDMPointerAssert(DWORD Address
#ifdef DEBUG
                         , DWORD Count
#endif
                         )
{
    UNREFERENCED_PARAMETER(Address);
#ifdef DEBUG
    UNREFERENCED_PARAMETER(Count);
#endif
    DebugBreak();
    return NULL;
}
