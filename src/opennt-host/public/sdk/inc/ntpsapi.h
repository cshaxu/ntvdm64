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

#if defined(OPENNT_ADAPTER_NT_ALERT_THREAD)
/*
 * DIVERGENCE: OPENNT-HOST-008
 *
 * The selected SoftPC timer is linked against the source-shaped public
 * adapter rather than NT4's private NTDLL alert primitive.  Keep the exact
 * name, parameter and NTSTATUS contract while omitting DLL import decoration
 * for that one adapter-backed build route.
 */
#define OPENNT_NT_ALERT_THREAD_API
#else
#define OPENNT_NT_ALERT_THREAD_API NTSYSAPI
#endif

OPENNT_NT_ALERT_THREAD_API
NTSTATUS
NTAPI
NtAlertThread(
    IN HANDLE ThreadHandle
    );

#undef OPENNT_NT_ALERT_THREAD_API

#endif
