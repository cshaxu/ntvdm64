/* True subset of OpenNT base/mvdm/softpc.new/host/inc/nt_event.h.
 * DIVERGENCE(HOST-DIV-029,HOST-DIV-030): Unreached event-loop, console,
 * VDD, graphics and keyboard declarations are omitted because their product
 * owners are not admitted by the standalone host closure. */
#ifndef OPENNT_HOST_NT_EVENT_H
#define OPENNT_HOST_NT_EVENT_H

#include <windows.h>

extern BOOL stdoutRedirected;
extern ULONG CntrlHandlerState;
#define CNTRL_SHELLCOUNT         0xFFFF
#define CNTRL_PIFALLOWCLOSE      0x10000
#define CNTRL_VDMBLOCKED         0x20000
#define CNTRL_SYSTEMROOTCONSOLE  0x40000
#define CNTRL_PUSHEXIT           0x80000

void nt_block_event_thread(ULONG BlockFlags);
void nt_resume_event_thread(void);
void cmdPushExitInConsoleBuffer(void);
void nt_std_handle_notification(BOOL fIsRedirection);

#endif
