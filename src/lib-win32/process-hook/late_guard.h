#ifndef PROCESS_HOOK_LATE_GUARD_H
#define PROCESS_HOOK_LATE_GUARD_H
#include <windows.h>
#include <stddef.h>
/* Internal experimental primitive. It never alters instruction pointers.
 * Caller must finish or abort its patch before releasing the guard.
 * This does not prevent thread creation or prove callback quiescence. */
typedef struct process_hook_late_region { const void *address; SIZE_T bytes; } process_hook_late_region;
typedef struct process_hook_late_guard { HANDLE *threads; DWORD count; DWORD suspended; void *snapshots; } process_hook_late_guard;
DWORD process_hook_late_guard_begin(process_hook_late_guard *,const process_hook_late_region *,DWORD);
/* On a resume failure the guard retains that handle for a later retry. */
DWORD process_hook_late_guard_end(process_hook_late_guard *);
#ifdef PROCESS_HOOK_LATE_GUARD_TEST
void process_hook_late_guard_test_fail_after(DWORD);
void process_hook_late_guard_test_after_suspend(void (*)(void));
void process_hook_late_guard_test_before_suspend(void (*)(void));
void process_hook_late_guard_test_fail_free(DWORD);
DWORD process_hook_late_guard_test_identity(HANDLE,DWORD,DWORD,FILETIME);
#endif
#endif
