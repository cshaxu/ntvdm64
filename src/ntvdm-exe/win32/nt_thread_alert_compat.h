#ifndef ADAPTER_MVDM_HOST_OUT_WIN32_NT_THREAD_ALERT_COMPAT_H
#define ADAPTER_MVDM_HOST_OUT_WIN32_NT_THREAD_ALERT_COMPAT_H

#include <windows.h>

/* Same-shaped forwarding to the native NT alert operation. Missing native
 * support is an error, never an APC substitute. */
NTSTATUS NTAPI NtAlertThread(HANDLE thread_handle);

#endif
