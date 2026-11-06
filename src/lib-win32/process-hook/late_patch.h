#ifndef PROCESS_HOOK_LATE_PATCH_H
#define PROCESS_HOOK_LATE_PATCH_H
#include <windows.h>
#include "late_guard.h"
#define PROCESS_HOOK_LATE_PATCH_LIMIT 88u
/* Experimental, single-owner transaction. The MinHook buffer allocator must
 * already be initialized and must not be shared with another hook backend.
 * Installed trampolines are retained; removal is not implemented. */
typedef struct process_hook_late_patch {
    void *target,*replacement,*trampoline,*slot;
    BYTE backup[7]; BOOL above,installed;
    process_hook_late_guard guard;
} process_hook_late_patch;
DWORD process_hook_late_patch_prepare(process_hook_late_patch *,void *,void *);
DWORD process_hook_late_patch_install(process_hook_late_patch *);
/* A late error may occur after all patches were written: inspect each
 * installed flag and retain their trampolines. If guard retains handles,
 * retry guard_end before touching or discarding any batch member. */
DWORD process_hook_late_patch_install_batch(process_hook_late_guard *,process_hook_late_patch **,DWORD);
DWORD process_hook_late_patch_discard(process_hook_late_patch *);
#ifdef PROCESS_HOOK_LATE_GUARD_TEST
void process_hook_late_patch_test_fail_protect_after(DWORD);
#endif
#endif
