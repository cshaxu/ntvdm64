#include "nt_thread_alert_compat.h"

NTSTATUS NTAPI NtAlertThread(HANDLE thread_handle)
{
    typedef NTSTATUS (NTAPI *alert_fn)(HANDLE);
    HMODULE module = GetModuleHandleW(L"ntdll.dll");
    alert_fn alert = module == NULL ? NULL :
        (alert_fn)GetProcAddress(module, "NtAlertThread");
    return alert != NULL ? alert(thread_handle) : (NTSTATUS)0xC000007AL;
}
