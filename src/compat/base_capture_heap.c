#include "include/base_capture_heap.h"

/* One local heap per process, shared with the original CSR allocator. */
PVOID CsrPortHeap = NULL;

BOOL opennt_base_capture_heap_begin(void)
{
    CsrPortHeap = HeapCreate(0, 0, 0);
    return CsrPortHeap != NULL;
}

BOOL opennt_base_capture_heap_end(void)
{
    BOOL result = !CsrPortHeap || HeapDestroy(CsrPortHeap);
    CsrPortHeap = NULL;
    return result;
}
