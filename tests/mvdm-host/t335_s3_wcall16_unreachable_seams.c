/*
 * Link seams for the bounded CallBack16 transaction in wcall16.c.
 *
 * The fixture calls only CallBack16.  LocalLock16/GetExePtr16 are not
 * reachable from that transaction but the historical translation unit keeps
 * them adjacent.  The pointer seam below serves only the fixture's explicit
 * real-mode memory domain and is not a product selector provider.
 */
#include "precomp.h"
#include "wow32.h"
#include <stdlib.h>

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
    extern unsigned char *Start_of_M_area;
#ifdef DEBUG
    UNREFERENCED_PARAMETER(Count);
#endif
    /* This bounded fixture deliberately uses selector zero and a real-mode
     * 64 KiB stack.  It does not exercise the product's protected selector
     * translation; reject any address outside that explicit test boundary. */
    if (Address > 0xffffu || Start_of_M_area == NULL) abort();
    return Start_of_M_area + Address;
}
