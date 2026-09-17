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
    typedef NTSTATUS (NTAPI *opennt_nt_alert_thread_fn)(HANDLE);
    static opennt_nt_alert_thread_fn native_alert;
    static int native_alert_checked;

    /* DIVERGENCE: ADAPTER-WIN32-020. Prefer the original same-shaped NTDLL
     * entry when the running NT exposes it.  QueueUserAPC is retained only as
     * a public-API fallback: an APC wake-up is observably similar at one
     * alertable wait, but is not a general replacement for a pending NT alert
     * on an arbitrary original SoftPC thread. */
    if (thread_handle == NULL || thread_handle == INVALID_HANDLE_VALUE) {
        return STATUS_INVALID_PARAMETER;
    }
    if (!native_alert_checked) {
        HMODULE ntdll = GetModuleHandleA("ntdll.dll");
        if (ntdll != NULL) {
            native_alert = (opennt_nt_alert_thread_fn)GetProcAddress(ntdll,
                "NtAlertThread");
        }
        native_alert_checked = 1;
    }
    if (native_alert != NULL) return native_alert(thread_handle);
    if (QueueUserAPC(opennt_thread_alert_apc, thread_handle, 0u) == 0u) {
        return STATUS_UNSUCCESSFUL;
    }
    return STATUS_SUCCESS;
}
