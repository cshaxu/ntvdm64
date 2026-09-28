#ifndef ADAPTER_MVDM_HOST_OUT_WIN32_NT_THREAD_ALERT_COMPAT_H
#define ADAPTER_MVDM_HOST_OUT_WIN32_NT_THREAD_ALERT_COMPAT_H

#include <windows.h>

/* Same-shaped replacement for the historical private NTDLL alert operation
 * used by the original SoftPC heartbeat.  The adapter schedules a harmless
 * APC, which wakes the target's alertable public Win32 wait. */
NTSTATUS NTAPI NtAlertThread(HANDLE thread_handle);

#endif
