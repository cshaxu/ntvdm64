#ifndef WOW_TASK_TERMINATION_H
#define WOW_TASK_TERMINATION_H

#include <windows.h>
#include <stdint.h>

VOID WINAPI W32ExitWowProcess(DWORD exit_code);

/* The client resolves the ticket to the original in-process termination entry. */
DWORD WINAPI W32TerminateTaskTicket(uint64_t ticket);

#endif
