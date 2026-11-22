#include "wow_edit_lookaside_provider.h"

/* OPENNT-HOST-074 finite environment binding. Original rtlinit.c owns the
 * pool, allocation, zeroing and free-list behavior. Keep its process-wide
 * pool and lock alive until process exit, as in original USER. */
static INIT_ONCE lookaside_once = INIT_ONCE_STATIC_INIT;
static CRITICAL_SECTION *lookaside_lock;
BOOL WINAPI wow_edit_lookaside_bind(CRITICAL_SECTION *lock)
{
    CRITICAL_SECTION *previous;
    if (!lock) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    previous = InterlockedCompareExchangePointer((PVOID volatile *)&lookaside_lock, lock, NULL);
    if (previous && previous != lock) { SetLastError(ERROR_ALREADY_INITIALIZED); return FALSE; }
    return TRUE;
}

static BOOL CALLBACK initialize_lookaside_lock(PINIT_ONCE once,
    PVOID parameter, PVOID *context)
{
    (void)once; (void)context;
    return InitializeCriticalSectionEx((CRITICAL_SECTION *)parameter, 0, 0);
}

BOOL WINAPI wow_edit_lookaside_ready(void)
{
    DWORD error = GetLastError();
    if (!lookaside_lock) RaiseFailFastException(NULL, NULL, 0);
    if (!InitOnceExecuteOnce(&lookaside_once, initialize_lookaside_lock,
            lookaside_lock, NULL)) return FALSE;
    SetLastError(error);
    return TRUE;
}
