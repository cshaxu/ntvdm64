/*
 * Selected true subset of OpenNT public/sdk/inc/ntpsapi.h.
 *
 * DIVERGENCE(OPENNT-HOST-002): The original `nt_timer.c` receives the
 * historical `NtAlertThread(HANDLE)` declaration through the broad NT4
 * platform include set.  Modern headers do not expose that private control
 * surface as a suitable user-mode lifecycle contract.  Preserve only the
 * original declaration shape here; adapter-mvdm-host-out owns the eventual
 * session-cancellation binding and no native thread-alert behavior is
 * supplied by this mirror.
 */
#ifndef _NTPSAPI_
#define _NTPSAPI_

NTSYSAPI
NTSTATUS
NTAPI
NtAlertThread(
    IN HANDLE ThreadHandle
    );

#endif
