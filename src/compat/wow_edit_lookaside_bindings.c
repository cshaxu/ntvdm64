#include "non-mvdm/public/sdk/inc/nt.h"
#include "wow_edit_lookaside_bindings.h"

/* OPENNT-HOST-074 finite environment binding. Original rtlinit.c owns the
 * pool, allocation, zeroing and free-list behavior. Keep its process-wide
 * pool and lock alive until process exit, as in original USER. */
static INIT_ONCE lookaside_once = INIT_ONCE_STATIC_INIT;

static BOOL CALLBACK initialize_lookaside_lock(PINIT_ONCE once,
    PVOID parameter, PVOID *context)
{
    (void)once; (void)parameter; (void)context;
    return InitializeCriticalSectionEx(&gcsLookaside, 0, 0);
}

BOOL WINAPI wow_edit_lookaside_ready(void)
{
    DWORD error = GetLastError();
    if (!InitOnceExecuteOnce(&lookaside_once, initialize_lookaside_lock,
            NULL, NULL)) return FALSE;
    SetLastError(error);
    return TRUE;
}
