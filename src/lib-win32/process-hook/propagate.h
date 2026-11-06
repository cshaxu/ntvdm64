#ifndef PROCESS_HOOK_PROPAGATE_H
#define PROCESS_HOOK_PROPAGATE_H
#include <windows.h>
#include "helper.h"
/* One process-lifetime registration. Configuration and callback context must
 * remain valid until process exit. The caller's attach transaction owns rollback
 * on partial installation. `active` can disable propagation without removing
 * installed trampolines. Callbacks return errors normally; they must not let
 * exceptions escape. Unwind restores reentry state, not child ownership.
 * `accept_*` is policy; `install` receives only the
 * newly created suspended child and captured, unpatched CreateProcessW. */
typedef struct process_hook_propagation_ops {
    BOOL (*active)(void *);
    BOOL (*accept_w)(LPCWSTR,LPWSTR,void *);
    BOOL (*accept_a)(LPCSTR,LPSTR,void *);
    DWORD (*install)(HANDLE,process_hook_create_w,void *);
    void *context;
} process_hook_propagation_ops;
DWORD process_hook_propagation_attach(DWORD (*attach)(void *,void *),const process_hook_propagation_ops *);
#endif
