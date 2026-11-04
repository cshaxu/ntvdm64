#ifndef PROCESS_HOOK_STARTUP_HOOKS_H
#define PROCESS_HOOK_STARTUP_HOOKS_H
#include <windows.h>
/* Startup-only transaction before application entry. Slots retain trampolines
 * for the process lifetime after commit. Abort only rolls back a transaction. */
DWORD process_hook_startup_begin(void);
DWORD process_hook_startup_attach(void *original_slot,void *replacement);
DWORD process_hook_startup_commit(void);
void process_hook_startup_abort(void);
#endif
