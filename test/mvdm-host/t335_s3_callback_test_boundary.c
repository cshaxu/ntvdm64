/* Boundaries of the source-shaped CCPU callback fixture.  They are not
 * product providers and do not establish WOW USER or DPMI lifecycle. */
#include <windows.h>
#include <stdlib.h>
#include "ntvdm-exe/session/session.h"

/* Match the product worker-active observation for the fixture's session. */
BOOL WINAPI wow_user_worker_active(void)
{
    session *owner = session_thread_current();
    return owner != NULL && owner->state == SESSION_STATE_ACTIVE;
}

__declspec(noreturn) void WINAPI wow_user_native_call_failed(const char *stage, DWORD error)
{
    (void)stage;
    (void)error;
    abort();
}

void NtvdmActivateLifecycle(void)
{
    abort(); /* host_start_cpu is outside this bounded transaction */
}

int mvdm_softpc_fast_bop_redirect(unsigned short selector, unsigned long offset)
{
    (void)selector;
    (void)offset;
    abort(); /* this fixture executes BOP instructions, not CALLF */
}

/* Original termination diagnostics reference DPMI state even when tracing
 * is disabled; this real-mode fixture does not create a DPMI address space. */
void *Ldt;
unsigned long IntelBase;
unsigned long Cpu40GdtShadowAddress;
unsigned long Cpu40LdtShadowAddress;
