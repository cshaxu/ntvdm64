//*****************************************************************************
//
// Small Heap -
//
//     This heap is used for allocating small size linked list structures.
//     This will reduce WOW's working set as the linked structures will be
//     together (less scattered) than if they were allocated from the
//     general purpose wow heap.
//
// 07-Oct-93  NanduriR   Created.
//
//*****************************************************************************

#include "precomp.h"
#pragma hdrstop

MODNAME(wheap.c);



HANDLE hWOWHeapSmall;


BOOL FASTCALL CreateSmallHeap()
{
  hWOWHeapSmall = HeapCreate (HEAP_NO_SERIALIZE, 4096, GROW_HEAP_AS_NEEDED);
  return (BOOL)hWOWHeapSmall;
}


PVOID FASTCALL malloc_w_small (ULONG size)
{
    PVOID pv = HeapAlloc(hWOWHeapSmall, 0, size);

#line 35
#ifdef DEBUG
#line 36
#error Excluded unsupported DEBUG WOW profile
#endif
#line 40
    return pv;

}


BOOL FASTCALL free_w_small(PVOID p)
{
    return HeapFree(hWOWHeapSmall, 0, (LPSTR)(p));
}
