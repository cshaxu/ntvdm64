#include "demerror_lock_shim.h"

void demSetHardErrorInfo(void);
void demRetry(void);
void demLockOper(void);
extern PVHE pHardErrPacket;
extern PSYSDEV pDeviceChain;

/* src/opennt/base/mvdm/dos/dem/demerror.c retains the VHE/SYSDEV pointers
 * after 50:32.  Historical SAS kept those guest mappings valid forever.
 * The typed boundary cannot export guest RAM directly, so retain exactly the
 * two fixed layouts and their far addresses in thread-local shim storage. */
typedef struct bx_ntvdm_demerror_guest_state {
    VHE hard_error;
    SYSDEV device_chain;
    USHORT hard_error_segment, hard_error_offset;
    USHORT device_chain_segment, device_chain_offset;
    int hard_error_live;
} bx_ntvdm_demerror_guest_state;

static __declspec(thread) bx_ntvdm_demerror_guest_state g_demerror_guest_state;

LPVOID bx_ntvdm_demerror_get_vdm_addr(USHORT segment, USHORT offset)
{
    bx_ntvdm_demerror_guest_state *state = &g_demerror_guest_state;

    if (bx_ntvdm_demhndl_current_service() != 0x32u)
        return bx_ntvdm_demhndl_get_vdm_addr(segment, offset);
    if (offset == bx_ntvdm_demhndl_get_dx()) {
        if (!bx_ntvdm_demhndl_copy_guest(segment, offset, &state->hard_error,
                sizeof(state->hard_error))) {
            SetLastError(ERROR_INVALID_ADDRESS);
            return NULL;
        }
        state->hard_error_segment = segment;
        state->hard_error_offset = offset;
        state->hard_error_live = 1;
        return &state->hard_error;
    }
    if (offset == bx_ntvdm_demhndl_get_bx()) {
        if (!bx_ntvdm_demhndl_copy_guest(segment, offset, &state->device_chain,
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

void bx_ntvdm_demerror_flush_hard_error(void)
{
    bx_ntvdm_demerror_guest_state *state = &g_demerror_guest_state;

    if (state->hard_error_live && pHardErrPacket == &state->hard_error &&
        !bx_ntvdm_demhndl_write_guest(state->hard_error_segment,
            state->hard_error_offset, &state->hard_error,
            sizeof(state->hard_error)))
        SetLastError(ERROR_INVALID_ADDRESS);
    (void)pDeviceChain;
}

NTSTATUS bx_ntvdm_demerror_append_unicode(PUNICODE_STRING s, PCWSTR tail)
{ (void)s; (void)tail; return STATUS_NOT_SUPPORTED; }
NTSTATUS bx_ntvdm_demerror_open_symbolic_link(PHANDLE h, ULONG a, POBJECT_ATTRIBUTES o)
{ (void)h; (void)a; (void)o; return STATUS_NOT_SUPPORTED; }
NTSTATUS bx_ntvdm_demerror_query_symbolic_link(HANDLE h, PUNICODE_STRING s, PULONG n)
{ (void)h; (void)s; (void)n; return STATUS_NOT_SUPPORTED; }
BOOLEAN bx_ntvdm_demerror_equal_unicode(const UNICODE_STRING *a,const UNICODE_STRING *b,BOOLEAN c)
{ (void)a;(void)b;(void)c; return FALSE; }
