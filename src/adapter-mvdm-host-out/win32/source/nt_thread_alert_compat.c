#include "nt_thread_alert_compat.h"

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif
#ifndef STATUS_INVALID_PARAMETER
#define STATUS_INVALID_PARAMETER ((NTSTATUS)0xC000000DL)
#endif
#ifndef STATUS_UNSUCCESSFUL
#define STATUS_UNSUCCESSFUL ((NTSTATUS)0xC0000001L)
#endif

static VOID CALLBACK opennt_thread_alert_apc(ULONG_PTR unused)
{
    (void)unused;
}

NTSTATUS NTAPI NtAlertThread(HANDLE thread_handle)
{
    /* DIVERGENCE: ADAPTER-WIN32-020. NT4's private thread-alert operation
     * is not a supported modern application API. Queue a no-op APC instead:
     * it preserves the original timer's observable cancellation wake-up at
     * its alertable wait boundary without touching process-global state. */
    if (thread_handle == NULL || thread_handle == INVALID_HANDLE_VALUE) {
        return STATUS_INVALID_PARAMETER;
    }
    if (QueueUserAPC(opennt_thread_alert_apc, thread_handle, 0u) == 0u) {
        return STATUS_UNSUCCESSFUL;
    }
    return STATUS_SUCCESS;
}
