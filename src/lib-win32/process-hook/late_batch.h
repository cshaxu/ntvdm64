#ifndef PROCESS_HOOK_LATE_BATCH_H
#define PROCESS_HOOK_LATE_BATCH_H
#include "late_patch.h"
/* Caller zeroes state and patch array, and owns arrays and synchronization
 * for process life. The MinHook buffer allocator must not be shared with a
 * concurrent hook backend. Capacity must not exceed
 * PROCESS_HOOK_LATE_PATCH_LIMIT.
 * Successful patches and sticky failed transactions retain their storage;
 * committed trampolines cannot be physically removed by this API. */
typedef struct process_hook_late_batch {
    process_hook_late_patch *patches;process_hook_late_patch **items;
    process_hook_late_guard guard;DWORD capacity,count,sticky_error;BOOL installed;
} process_hook_late_batch;
DWORD process_hook_late_batch_begin(process_hook_late_batch *,process_hook_late_patch *,process_hook_late_patch **,DWORD);
DWORD process_hook_late_batch_prepare(process_hook_late_batch *,void *,void *);
/* input_error aborts an incomplete preparation; failed transactions that may
 * still own suspended threads or installed patches are retained as sticky. */
DWORD process_hook_late_batch_commit(process_hook_late_batch *,DWORD input_error,DWORD attempts,DWORD delay_ms);
#endif
