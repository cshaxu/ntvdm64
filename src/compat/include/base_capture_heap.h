#ifndef COMPAT_BASE_CAPTURE_HEAP_H
#define COMPAT_BASE_CAPTURE_HEAP_H

#include <windows.h>

/* Owned locally; borrowed by the original capture allocator. */
extern PVOID CsrPortHeap;
BOOL opennt_base_capture_heap_begin(void);
BOOL opennt_base_capture_heap_end(void);

#endif
