#ifndef WOW_TASK_TERMINATION_H
#define WOW_TASK_TERMINATION_H

#include <windows.h>
#include <stdint.h>

/* The broker owns ticket identity.  This DLL owns only the current original
 * task mapping and invokes the original in-process termination entry. */
DWORD WINAPI wow_task_termination_bind(ULONG task, USHORT htask);
void WINAPI wow_task_termination_retire(USHORT htask);
void WINAPI wow_task_termination_fail(USHORT htask);
DWORD WINAPI W32TerminateTaskTicket(uint64_t ticket);

#endif
