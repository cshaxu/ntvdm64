#include "adapter-mvdm-host-out/debugger/include/dbg_state.h"

/* `dbg.c` keeps these as process globals.  The selected product currently
 * owns one session; retain that original scope without publishing the values
 * outside this debugger boundary. */
static BOOL mvdm_debugged = FALSE;
static DWORD mvdm_toolhelp_hook = 0u;
static BOOL mvdm_toolhelp_is_386 = FALSE;

void mvdm_debugger_set_debugged(BOOL attached)
{
    mvdm_debugged = attached != FALSE;
}

BOOL mvdm_debugger_is_debugged(void)
{
    return mvdm_debugged;
}

void mvdm_debugger_set_toolhelp(DWORD hook, BOOL is_386)
{
    mvdm_toolhelp_hook = hook;
    mvdm_toolhelp_is_386 = is_386 != FALSE;
}

/* Original WOW imports this exact state setter.  The non-debug source path
 * has no further side effect; a later WOW package can consume the same state
 * without changing the debugger dispatch contract. */
VOID DBGNotifyDebugged(BOOL attached)
{
    mvdm_debugger_set_debugged(attached);
}
