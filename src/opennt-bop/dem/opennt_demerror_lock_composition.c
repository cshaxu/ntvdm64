#include "opennt_demerror_lock_composition.h"

void demSetHardErrorInfo(void);
void demRetry(void);
void demLockOper(void);
extern PVHE pHardErrPacket;
extern PSYSDEV pDeviceChain;

/* src/opennt/base/mvdm/dos/dem/demerror.c retains the VHE/SYSDEV pointers
 * after 50:32.  Historical SAS kept those guest mappings valid forever.
 * The typed boundary cannot export guest RAM directly, so retain exactly the
 * two fixed layouts and their far addresses in thread-local shim storage. */
typedef struct runtime_demerror_guest_state {
    VHE hard_error;
    SYSDEV device_chain;
    USHORT hard_error_segment, hard_error_offset;
    USHORT device_chain_segment, device_chain_offset;
    int hard_error_live;
} runtime_demerror_guest_state;

static __declspec(thread) runtime_demerror_guest_state g_demerror_guest_state;

LPVOID runtime_demerror_get_vdm_addr(USHORT segment, USHORT offset)
{
    runtime_demerror_guest_state *state = &g_demerror_guest_state;

    if (runtime_demhndl_current_service() != 0x32u)
        return runtime_demhndl_get_vdm_addr(segment, offset);
    if (offset == runtime_demhndl_get_dx()) {
        if (!runtime_demhndl_copy_guest(segment, offset, &state->hard_error,
                sizeof(state->hard_error))) {
            SetLastError(ERROR_INVALID_ADDRESS);
            return NULL;
        }
        state->hard_error_segment = segment;
        state->hard_error_offset = offset;
        state->hard_error_live = 1;
        return &state->hard_error;
    }
    if (offset == runtime_demhndl_get_bx()) {
        if (!runtime_demhndl_copy_guest(segment, offset, &state->device_chain,
                sizeof(state->device_chain))) {
            SetLastError(ERROR_INVALID_ADDRESS);
            return NULL;
        }
        state->device_chain_segment = segment;
        state->device_chain_offset = offset;
        return &state->device_chain;
    }
    SetLastError(ERROR_INVALID_ADDRESS);
    return NULL;
}

void runtime_demerror_flush_hard_error(void)
{
    runtime_demerror_guest_state *state = &g_demerror_guest_state;

    if (state->hard_error_live && pHardErrPacket == &state->hard_error &&
        !runtime_demhndl_write_guest(state->hard_error_segment,
            state->hard_error_offset, &state->hard_error,
            sizeof(state->hard_error)))
        SetLastError(ERROR_INVALID_ADDRESS);
    (void)pDeviceChain;
}

/* The original demerror.c body reaches these same-named NTDLL contracts while
 * resolving a DOS drive letter for a hard-error report.  They are host-only
 * counted strings/object handles; no member is guest-visible.  Resolve the
 * existing host spelling dynamically so this x86/x64 facade does not depend
 * on private SDK declarations or replace the original source algorithm. */
typedef NTSTATUS (NTAPI *runtime_rtl_append_unicode_fn)(PUNICODE_STRING, PCWSTR);
typedef NTSTATUS (NTAPI *runtime_nt_open_symbolic_link_fn)(PHANDLE, ULONG,
    POBJECT_ATTRIBUTES);
typedef NTSTATUS (NTAPI *runtime_nt_query_symbolic_link_fn)(HANDLE,
    PUNICODE_STRING, PULONG);
typedef BOOLEAN (NTAPI *runtime_rtl_equal_unicode_fn)(const UNICODE_STRING *,
    const UNICODE_STRING *, BOOLEAN);

static FARPROC runtime_demerror_ntdll_proc(const char *name)
{
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    return ntdll == NULL ? NULL : GetProcAddress(ntdll, name);
}

NTSTATUS runtime_demerror_append_unicode(PUNICODE_STRING string, PCWSTR tail)
{
    runtime_rtl_append_unicode_fn routine =
        (runtime_rtl_append_unicode_fn)runtime_demerror_ntdll_proc(
            "RtlAppendUnicodeToString");
    return routine == NULL ? STATUS_NOT_IMPLEMENTED : routine(string, tail);
}

NTSTATUS runtime_demerror_open_symbolic_link(PHANDLE handle, ULONG access,
    POBJECT_ATTRIBUTES attributes)
{
    runtime_nt_open_symbolic_link_fn routine =
        (runtime_nt_open_symbolic_link_fn)runtime_demerror_ntdll_proc(
            "NtOpenSymbolicLinkObject");
    return routine == NULL ? STATUS_NOT_IMPLEMENTED :
        routine(handle, access, attributes);
}

NTSTATUS runtime_demerror_query_symbolic_link(HANDLE handle,
    PUNICODE_STRING string, PULONG returned_length)
{
    runtime_nt_query_symbolic_link_fn routine =
        (runtime_nt_query_symbolic_link_fn)runtime_demerror_ntdll_proc(
            "NtQuerySymbolicLinkObject");
    return routine == NULL ? STATUS_NOT_IMPLEMENTED :
        routine(handle, string, returned_length);
}

BOOLEAN runtime_demerror_equal_unicode(const UNICODE_STRING *left,
    const UNICODE_STRING *right, BOOLEAN case_insensitive)
{
    runtime_rtl_equal_unicode_fn routine =
        (runtime_rtl_equal_unicode_fn)runtime_demerror_ntdll_proc(
            "RtlEqualUnicodeString");
    return routine == NULL ? FALSE : routine(left, right, case_insensitive);
}
