/* Neutral execution seam for the direct OpenNT demhndl.c import.
 *
 * Original owner: src/opennt/base/mvdm/dos/dem/demhndl.c at
 * d450bfc1b7711234072eebfdfa23735ea509e7c5.  OpenNT obtained registers,
 * guest pointers and native HANDLEs from CCPU/SAS.  This file provides the
 * smallest replacement composition for that unavailable boundary; it does
 * not dispatch BOPs or implement DEM service semantics.
 */

#include "demhndl_shim.h"

#include <stdlib.h>
#include <string.h>

/* Imported, verbatim-order OpenNT service bodies. */
void demClose(void);
void demRead(void);
void demWrite(void);
void demChgFilePtr(void);
void demFileTimes(void);
void demCommit(void);
void demPipeFileDataEOF(void);
void demPipeFileEOF(void);

typedef struct bx_ntvdm_demhndl_active_call {
    bx_ntvdm_demhndl_call *call;
    uint32_t handle_token;
    uint32_t guest_address;
    uint32_t guest_bytes;
    uint8_t *guest_buffer;
    int transfer_from_guest;
} bx_ntvdm_demhndl_active_call;

static __declspec(thread) bx_ntvdm_demhndl_active_call *g_active_call;
static __declspec(thread) bx_ntvdm_demhndl_extended_error g_extended_error;
__declspec(thread) bx_ntvdm_demhndl_extended_error *pExtendedError;

static bx_ntvdm_demhndl_active_call *active_call(void)
{
    return g_active_call;
}

static uint32_t real_mode_address(USHORT segment, USHORT offset)
{
    return ((uint32_t)segment << 4) + (uint32_t)offset;
}

static uint16_t low16(uint32_t value)
{
    return (uint16_t)(value & 0xffffu);
}

static int set_register(uint32_t register_index, USHORT value)
{
    bx_ntvdm_demhndl_active_call *active = active_call();
    return active != 0 && active->call != 0 &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&active->call->result->cpu_delta,
            register_index, value);
}

int bx_ntvdm_demhndl_call_valid(const bx_ntvdm_demhndl_call *call)
{
    return call != 0 && call->magic == BX_NTVDM_DEMHNDL_CALL_MAGIC &&
        call->abi_version == BX_NTVDM_DEMHNDL_CALL_VERSION &&
        call->struct_bytes == sizeof(*call) && call->direct != 0 &&
        bx_ntvdm_dem_direct_context_valid(call->direct) &&
        call->boundary != 0 && bx_ntvdm_exception_event_v1_valid(call->boundary) &&
        call->cpu != 0 && bx_ntvdm_cpu_state_v1_valid(call->cpu) &&
        call->cpu->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        call->result != 0 && call->guest_read != 0 && call->guest_write != 0;
}

USHORT bx_ntvdm_demhndl_get_ax(void) { return low16(active_call()->call->cpu->eax); }
USHORT bx_ntvdm_demhndl_get_bx(void) { return low16(active_call()->call->cpu->ebx); }
USHORT bx_ntvdm_demhndl_get_cx(void) { return low16(active_call()->call->cpu->ecx); }
USHORT bx_ntvdm_demhndl_get_dx(void) { return low16(active_call()->call->cpu->edx); }
USHORT bx_ntvdm_demhndl_get_si(void) { return low16(active_call()->call->cpu->esi); }
USHORT bx_ntvdm_demhndl_get_bp(void) { return low16(active_call()->call->cpu->ebp); }
USHORT bx_ntvdm_demhndl_get_ds(void) { return active_call()->call->cpu->ds; }
USHORT bx_ntvdm_demhndl_get_bl(void) { return (USHORT)(active_call()->call->cpu->ebx & 0xffu); }
int bx_ntvdm_demhndl_get_zf(void) { return (active_call()->call->cpu->eflags & 0x40u) != 0u; }
void bx_ntvdm_demhndl_set_ax(USHORT value) { (void)set_register(0u, value); }
void bx_ntvdm_demhndl_set_bp(USHORT value) { (void)set_register(5u, value); }
void bx_ntvdm_demhndl_set_cx(USHORT value) { (void)set_register(2u, value); }
void bx_ntvdm_demhndl_set_dx(USHORT value) { (void)set_register(3u, value); }
void bx_ntvdm_demhndl_set_cf(int value) { (void)bx_ntvdm_cpu_result_v2_set_cf(active_call()->call->result, value); }
void bx_ntvdm_demhndl_set_zf(int value) { (void)bx_ntvdm_cpu_result_v2_set_zf(active_call()->call->result, value); }

HANDLE bx_ntvdm_demhndl_get_handle(USHORT high, USHORT low)
{
    bx_ntvdm_demhndl_active_call *active = active_call();
    HANDLE handle = NULL;
    uint32_t token;

    if (active == 0 || active->call == 0) return NULL;
    token = ((uint32_t)high << 16) | low;
    active->handle_token = token;
    if (token == 0u) return NULL;
    if (!active->call->direct->lookup_handle(active->call->direct->state, token,
            &handle)) {
        SetLastError(ERROR_INVALID_HANDLE);
        return INVALID_HANDLE_VALUE;
    }
    return handle;
}

BOOL bx_ntvdm_demhndl_close_handle(HANDLE file)
{
    bx_ntvdm_demhndl_active_call *active = active_call();
    DWORD error = ERROR_INVALID_HANDLE;
    (void)file;
    if (active == 0 || active->call == 0 || active->handle_token == 0u ||
        !active->call->direct->release_handle(active->call->direct->state,
            active->handle_token, &error)) {
        SetLastError(error);
        return FALSE;
    }
    return TRUE;
}

LPVOID bx_ntvdm_demhndl_get_vdm_addr(USHORT segment, USHORT offset)
{
    bx_ntvdm_demhndl_active_call *active = active_call();
    uint32_t bytes;

    if (active == 0 || active->call == 0) return NULL;
    bytes = bx_ntvdm_demhndl_get_cx();
    active->guest_address = real_mode_address(segment, offset);
    active->guest_bytes = bytes;
    /* A zero-length DOS transfer still supplies a valid historical pointer:
     * demWrite may use it before its CX==0 truncate/extend branch. */
    active->guest_buffer = (uint8_t *)malloc(bytes == 0u ? 1u : bytes);
    if (active->guest_buffer == NULL) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }
    /* Sim32GetVDMPointer historically returned the guest's actual backing
     * span.  Seed every bounce span from checked guest RAM, including a read
     * destination: a failed or short host read must not overwrite bytes that
     * the original direct mapping would have left intact. */
    if (bytes != 0u && !active->call->guest_read(active->call->guest_state,
            active->guest_address, active->guest_buffer, bytes)) {
        free(active->guest_buffer);
        active->guest_buffer = NULL;
        SetLastError(ERROR_INVALID_ADDRESS);
        return NULL;
    }
    return active->guest_buffer;
}

void bx_ntvdm_demhndl_flush_vdm_pointer(ULONG far_pointer, USHORT bytes,
    PBYTE pointer, BOOL write_back)
{
    bx_ntvdm_demhndl_active_call *active = active_call();
    (void)far_pointer; (void)write_back;
    if (active != 0 && active->call != 0 && !active->transfer_from_guest &&
        pointer != NULL && bytes != 0u && !active->call->guest_write(
            active->call->guest_state, active->guest_address, pointer, bytes))
        SetLastError(ERROR_INVALID_ADDRESS);
}

void bx_ntvdm_demhndl_free_vdm_pointer(ULONG far_pointer, USHORT bytes,
    PBYTE pointer, BOOL write_back)
{
    bx_ntvdm_demhndl_active_call *active = active_call();
    (void)far_pointer; (void)bytes; (void)write_back;
    if (active != 0 && pointer == active->guest_buffer) {
        free(active->guest_buffer);
        active->guest_buffer = NULL;
    }
}

void bx_ntvdm_demhndl_client_error(HANDLE file, CHAR drive)
{
    bx_ntvdm_demhndl_active_call *active = active_call();
    DWORD error = GetLastError();
    (void)file; (void)drive;
    if (error == ERROR_SUCCESS) error = ERROR_ACCESS_DENIED;
    if ((error >= ERROR_WRITE_PROTECT && error <= ERROR_GEN_FAILURE) ||
        error == ERROR_WRONG_DISK) {
        active->call->hard_error_pending = 1u;
        active->call->hard_error_code = error;
    } else {
        bx_ntvdm_demhndl_set_ax((USHORT)error);
    }
    bx_ntvdm_demhndl_set_cf(1);
}

int IsVdmRedirLoaded(void) { return 0; }
void VrRemoveOpenNamedPipeInfo(HANDLE file) { (void)file; }
int VrIsNamedPipeHandle(HANDLE file) { (void)file; return 0; }
int VrReadNamedPipe(HANDLE file, LPVOID buffer, DWORD count, DWORD *read_out, DWORD *error_out)
{ (void)file; (void)buffer; (void)count; if (read_out) *read_out = 0u; if (error_out) *error_out = ERROR_CALL_NOT_IMPLEMENTED; SetLastError(ERROR_CALL_NOT_IMPLEMENTED); return 0; }
int VrWriteNamedPipe(HANDLE file, LPVOID buffer, DWORD count, DWORD *written_out)
{ (void)file; (void)buffer; (void)count; if (written_out) *written_out = 0u; SetLastError(ERROR_CALL_NOT_IMPLEMENTED); return 0; }
int cmdPipeFileDataEOF(HANDLE file, BOOL *eof_out)
{ bx_ntvdm_demhndl_active_call *active = active_call(); if (eof_out) *eof_out = FALSE; return active != NULL && active->call->pipe_data_eof != NULL ? active->call->pipe_data_eof(active->call->pipe_state, file, eof_out) : 0; }
int cmdPipeFileEOF(HANDLE file)
{ bx_ntvdm_demhndl_active_call *active = active_call(); return active != NULL && active->call->pipe_eof != NULL ? active->call->pipe_eof(active->call->pipe_state, file) : 0; }

int bx_ntvdm_demhndl_invoke(bx_ntvdm_demhndl_call *call)
{
    bx_ntvdm_demhndl_active_call active;
    void (*service)(void) = NULL;

    if (!bx_ntvdm_demhndl_call_valid(call) || g_active_call != NULL ||
        call->boundary->fault_rip > UINT64_MAX - 4u) return 0;
    switch (call->service) {
    case BX_NTVDM_DEMHNDL_CHG_FILE_PTR: service = demChgFilePtr; break;
    case BX_NTVDM_DEMHNDL_CLOSE: service = demClose; break;
    case BX_NTVDM_DEMHNDL_FILE_TIMES: service = demFileTimes; break;
    case BX_NTVDM_DEMHNDL_READ: service = demRead; break;
    case BX_NTVDM_DEMHNDL_WRITE: service = demWrite; break;
    case BX_NTVDM_DEMHNDL_COMMIT: service = demCommit; break;
    case BX_NTVDM_DEMHNDL_PIPE_DATA_EOF: service = demPipeFileDataEOF; break;
    case BX_NTVDM_DEMHNDL_PIPE_EOF: service = demPipeFileEOF; break;
    default: return 0;
    }
    memset(&active, 0, sizeof(active));
    active.call = call;
    active.transfer_from_guest = call->service == BX_NTVDM_DEMHNDL_WRITE;
    call->hard_error_pending = 0u;
    call->hard_error_code = ERROR_SUCCESS;
    memset(&g_extended_error, 0, sizeof(g_extended_error));
    pExtendedError = &g_extended_error;
    bx_ntvdm_cpu_result_v2_pass_through(call->result);
    if (!bx_ntvdm_cpu_result_v2_resume(call->result, call->boundary->fault_rip + 4u))
        return 0;
    g_active_call = &active;
    service();
    if (active.guest_buffer != NULL) free(active.guest_buffer);
    g_active_call = NULL;
    return bx_ntvdm_cpu_result_v2_valid(call->result);
}
