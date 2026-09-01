#ifndef MVDM_DEBUGGER_STATE_H
#define MVDM_DEBUGGER_STATE_H

#include <windows.h>

/* Adapter-local carrier for the original dbg.c process state.  It keeps the
 * original fixed-width ToolHelp values as guest data; no host pointer crosses
 * this boundary. */
void mvdm_debugger_set_debugged(BOOL attached);
BOOL mvdm_debugger_is_debugged(void);
void mvdm_debugger_set_toolhelp(DWORD hook, BOOL is_386);

#endif
