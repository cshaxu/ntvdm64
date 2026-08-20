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
#include <setjmp.h>

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
    uint8_t *path_buffers[4];
    uint32_t path_buffer_count;
    int transfer_from_guest;
    int flush_guest_buffer_on_return;
    int loader_mode;
    jmp_buf terminate_jump;
} bx_ntvdm_demhndl_active_call;

static __declspec(thread) bx_ntvdm_demhndl_active_call *g_active_call;
static __declspec(thread) bx_ntvdm_demhndl_extended_error g_extended_error;
__declspec(thread) bx_ntvdm_demhndl_extended_error *pExtendedError;

static bx_ntvdm_demhndl_active_call *active_call(void)
{
    return g_active_call;
}

static int is_demsrch_dta_service(uint32_t service)
{
    return service == 0x09u || service == 0x0bu;
}

static int is_demsrch_fcb_service(uint32_t service)
{
    return service == 0x0au || service == 0x0cu;
}

static int is_demfcb_path_service(uint32_t service)
{
    return service == 0x07u || service == 0x20u || service == 0x2cu ||
        service == 0x2du || service == 0x31u;
}

static LPVOID acquire_fixed_guest_span(bx_ntvdm_demhndl_active_call *active,
    uint32_t bytes)
{
    if (active == NULL || active->call == NULL || active->guest_buffer != NULL)
        return NULL;
    active->guest_bytes = bytes;
    active->guest_buffer = (uint8_t *)malloc(bytes);
    if (active->guest_buffer == NULL) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
    if (!active->call->guest_read(active->call->guest_state, active->guest_address,
            active->guest_buffer, bytes)) {
        free(active->guest_buffer); active->guest_buffer = NULL;
        SetLastError(ERROR_INVALID_ADDRESS); return NULL;
    }
    active->flush_guest_buffer_on_return = 1;
    return active->guest_buffer;
}

static uint32_t real_mode_address(USHORT segment, USHORT offset)
{
    return ((uint32_t)segment << 4) + (uint32_t)offset;
}

static uint16_t low16(uint32_t value)
{
    return (uint16_t)(value & 0xffffu);
}

static int is_demfile_path_service(uint32_t service)
{
    switch (service) {
    case 0x01u: case 0x03u: case 0x04u: case 0x05u: case 0x06u: case 0x12u:
    case 0x17u: case 0x22u: case 0x44u:
    case 0x18u: case 0x34u: case 0x35u:
        return 1;
    default:
        return 0;
    }
}

static int is_demdir_cds_service(uint32_t service)
{
    return service == 0x13u;
}

static int is_demgset_path_service(uint32_t service)
{
    return service == 0x1au;
}

static uint32_t demgset_fixed_guest_bytes(uint32_t service)
{
    /* These are original packed 16-bit guest data structures.  Their bytes
     * enter/leave the imported body only through the checked bounce span. */
    switch (service) {
    case 0x10u: return 25u;       /* VOLINFO */
    case 0x25u: return 33u;       /* DPB: 32-bit Next on either host width */
    case 0x41u: return 16u;       /* DOS computer-name buffer */
    case 0x46u: return 33u * 26u; /* one DPB per DOS drive */
    default: return 0u;
    }
}

static LPVOID acquire_guest_oem_path(bx_ntvdm_demhndl_active_call *active,
    uint32_t address)
{
    uint8_t *buffer;
    uint32_t index;

    if (active->path_buffer_count >= 4u) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }
    /* OpenNT received an unbounded flat SAS pointer.  The replacement reads
     * one NUL-terminated OEM pathname through checked RAM and caps it at the
     * Win32 path contract; this is the ABI boundary, not a DEM pathname
     * algorithm. */
    buffer = (uint8_t *)malloc(MAX_PATH + 1u);
    if (buffer == NULL) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
    for (index = 0u; index < MAX_PATH; ++index) {
        if (!active->call->guest_read(active->call->guest_state, address + index,
                buffer + index, 1u)) {
            free(buffer); SetLastError(ERROR_INVALID_ADDRESS); return NULL;
        }
        if (buffer[index] == 0u) {
            active->path_buffers[active->path_buffer_count++] = buffer;
            return buffer;
        }
    }
    free(buffer);
    SetLastError(ERROR_BUFFER_OVERFLOW);
    return NULL;
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
USHORT bx_ntvdm_demhndl_get_di(void) { return low16(active_call()->call->cpu->edi); }
USHORT bx_ntvdm_demhndl_get_bp(void) { return low16(active_call()->call->cpu->ebp); }
USHORT bx_ntvdm_demhndl_get_ds(void) { return active_call()->call->cpu->ds; }
USHORT bx_ntvdm_demhndl_get_es(void) { return active_call()->call->cpu->es; }
USHORT bx_ntvdm_demhndl_get_al(void) { return (USHORT)(active_call()->call->cpu->eax & 0xffu); }
USHORT bx_ntvdm_demhndl_get_cl(void) { return (USHORT)(active_call()->call->cpu->ecx & 0xffu); }
USHORT bx_ntvdm_demhndl_get_ch(void) { return (USHORT)((active_call()->call->cpu->ecx >> 8) & 0xffu); }
USHORT bx_ntvdm_demhndl_get_bl(void) { return (USHORT)(active_call()->call->cpu->ebx & 0xffu); }
USHORT bx_ntvdm_demhndl_get_dh(void) { return (USHORT)((active_call()->call->cpu->edx >> 8) & 0xffu); }
USHORT bx_ntvdm_demhndl_get_dl(void) { return (USHORT)(active_call()->call->cpu->edx & 0xffu); }
USHORT bx_ntvdm_demhndl_get_ah(void) { return (USHORT)((active_call()->call->cpu->eax >> 8) & 0xffu); }
int bx_ntvdm_demhndl_get_zf(void) { return (active_call()->call->cpu->eflags & 0x40u) != 0u; }
void bx_ntvdm_demhndl_set_ax(USHORT value) { (void)set_register(0u, value); }
void bx_ntvdm_demhndl_set_al(USHORT value)
{ bx_ntvdm_demhndl_set_ax((USHORT)((bx_ntvdm_demhndl_get_ax() & 0xff00u) | (value & 0xffu))); }
void bx_ntvdm_demhndl_set_ah(USHORT value)
{ bx_ntvdm_demhndl_set_ax((USHORT)((bx_ntvdm_demhndl_get_ax() & 0x00ffu) | ((value & 0xffu) << 8))); }
void bx_ntvdm_demhndl_set_bx(USHORT value) { (void)set_register(3u, value); }
void bx_ntvdm_demhndl_set_bl(USHORT value)
{ bx_ntvdm_demhndl_set_bx((USHORT)((bx_ntvdm_demhndl_get_bx() & 0xff00u) | (value & 0xffu))); }
void bx_ntvdm_demhndl_set_bp(USHORT value) { (void)set_register(5u, value); }
/* The copied-result GPR numbering is Bochs AX,CX,DX,BX,SP,BP,SI,DI; keep
 * the historical helper spellings at this neutral boundary rather than make
 * imported DEM code depend on Bochs headers. */
void bx_ntvdm_demhndl_set_cx(USHORT value) { (void)set_register(1u, value); }
void bx_ntvdm_demhndl_set_cl(USHORT value)
{ bx_ntvdm_demhndl_set_cx((USHORT)((bx_ntvdm_demhndl_get_cx() & 0xff00u) | (value & 0xffu))); }
void bx_ntvdm_demhndl_set_ch(USHORT value)
{ bx_ntvdm_demhndl_set_cx((USHORT)((bx_ntvdm_demhndl_get_cx() & 0x00ffu) | ((value & 0xffu) << 8))); }
void bx_ntvdm_demhndl_set_dx(USHORT value) { (void)set_register(2u, value); }
void bx_ntvdm_demhndl_set_dl(USHORT value)
{ bx_ntvdm_demhndl_set_dx((USHORT)((bx_ntvdm_demhndl_get_dx() & 0xff00u) | (value & 0xffu))); }
void bx_ntvdm_demhndl_set_dh(USHORT value)
{ bx_ntvdm_demhndl_set_dx((USHORT)((bx_ntvdm_demhndl_get_dx() & 0x00ffu) | ((value & 0xffu) << 8))); }
void bx_ntvdm_demhndl_set_si(USHORT value) { (void)set_register(6u, value); }
void bx_ntvdm_demhndl_set_di(USHORT value) { (void)set_register(7u, value); }
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

BOOL bx_ntvdm_demhndl_publish_handle(HANDLE file)
{
    bx_ntvdm_demhndl_active_call *active = active_call();
    DWORD error = ERROR_INVALID_HANDLE;
    uint32_t token = 0u;
    if (active == NULL || active->call == NULL || file == INVALID_HANDLE_VALUE ||
        !active->call->direct->publish_handle(active->call->direct->state, file,
            &token, &error) || token == 0u) {
        SetLastError(error);
        return FALSE;
    }
    bx_ntvdm_demhndl_set_ax((USHORT)(token >> 16));
    bx_ntvdm_demhndl_set_bp((USHORT)token);
    return TRUE;
}

LPVOID bx_ntvdm_demhndl_get_vdm_addr(USHORT segment, USHORT offset)
{
    bx_ntvdm_demhndl_active_call *active = active_call();
    uint32_t bytes;

    if (active == 0 || active->call == 0) return NULL;
    active->guest_address = real_mode_address(segment, offset);
    if (active->call->service == 0x11u) {
        /* Original demLoadDos increments its initial SAS pointer as it reads
         * NTDOS.SYS in 16 KiB chunks.  Reserve the remaining real-mode
         * aperture as a private bounce span; demmisc_shim writes each actual
         * host-read chunk back through checked guest RAM. */
        if (active->guest_address >= 0x100000u) {
            SetLastError(ERROR_INVALID_ADDRESS);
            return NULL;
        }
        bytes = 0x100000u - active->guest_address;
        /* The source body asks ReadFile for 16 KiB even for its final, short
         * chunk.  Keep an adjacent guard allocation so that the host API has
         * a valid destination; loader_write still rejects any bytes beyond
         * the actual real-mode aperture. */
        active->guest_buffer = (uint8_t *)malloc(bytes + 0x4000u);
        if (active->guest_buffer == NULL) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
        active->guest_bytes = bytes;
        active->loader_mode = 1;
        return active->guest_buffer;
    }
    /* OpenNT demsrch.c maps fixed DOS DTA/SRCHBUF layouts directly through
     * SAS.  The Direct CLI maps the same exact 43/52-byte guest layouts via a
     * checked bounce span and writes them back after the imported body.  The
     * first FindFirst/FindFirstFCB pointer is still its original OEM path. */
    if (is_demsrch_dta_service(active->call->service)) {
        if (active->call->service == 0x09u && active->path_buffer_count == 0u)
            return acquire_guest_oem_path(active, active->guest_address);
        return acquire_fixed_guest_span(active, 43u);
    }
    if (is_demsrch_fcb_service(active->call->service)) {
        if (active->call->service == 0x0au && active->path_buffer_count == 0u)
            return acquire_guest_oem_path(active, active->guest_address);
        return acquire_fixed_guest_span(active, 52u);
    }
    if (is_demfcb_path_service(active->call->service))
        return acquire_guest_oem_path(active, active->guest_address);
    if (is_demfile_path_service(active->call->service))
        return acquire_guest_oem_path(active, active->guest_address);
    if (is_demgset_path_service(active->call->service))
        return acquire_guest_oem_path(active, active->guest_address);
    if (is_demdir_cds_service(active->call->service)) {
        /* OpenNT demdir.c maps the packed 71-byte CDS in place.  Copy its
         * fixed historical layout through checked RAM and write it back after
         * the imported body, rather than exporting a guest pointer. */
        bytes = 71u;
        active->guest_bytes = bytes;
        active->guest_buffer = (uint8_t *)malloc(bytes);
        if (active->guest_buffer == NULL) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
        if (!active->call->guest_read(active->call->guest_state, active->guest_address,
                active->guest_buffer, bytes)) {
            free(active->guest_buffer); active->guest_buffer = NULL;
            SetLastError(ERROR_INVALID_ADDRESS); return NULL;
        }
        return active->guest_buffer;
    }
    bytes = demgset_fixed_guest_bytes(active->call->service);
    if (bytes != 0u)
        return acquire_fixed_guest_span(active, bytes);
    bytes = bx_ntvdm_demhndl_get_cx();
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

int bx_ntvdm_demhndl_copy_guest(USHORT segment, USHORT offset, void *buffer,
    uint32_t bytes)
{
    bx_ntvdm_demhndl_active_call *active = active_call();
    if (active == NULL || active->call == NULL || buffer == NULL ||
        active->call->guest_read == NULL) return 0;
    return active->call->guest_read(active->call->guest_state,
        real_mode_address(segment, offset), (uint8_t *)buffer, bytes);
}

uint32_t bx_ntvdm_demhndl_current_service(void)
{
    bx_ntvdm_demhndl_active_call *active = active_call();
    return active != NULL && active->call != NULL ? active->call->service : UINT32_MAX;
}

int bx_ntvdm_demhndl_write_guest(USHORT segment, USHORT offset,
    const void *buffer, uint32_t bytes)
{
    bx_ntvdm_demhndl_active_call *active = active_call();
    if (active == NULL || active->call == NULL || buffer == NULL ||
        active->call->guest_write == NULL) return 0;
    return active->call->guest_write(active->call->guest_state,
        real_mode_address(segment, offset), (const uint8_t *)buffer, bytes);
}

int bx_ntvdm_demhndl_loader_write(const void *buffer, uint32_t bytes)
{
    bx_ntvdm_demhndl_active_call *active = active_call();
    uintptr_t base, cursor;
    uint32_t offset;

    if (active == NULL || active->call == NULL || !active->loader_mode ||
        active->guest_buffer == NULL || buffer == NULL) return 0;
    base = (uintptr_t)active->guest_buffer;
    cursor = (uintptr_t)buffer;
    if (cursor < base || cursor - base > active->guest_bytes ||
        bytes > active->guest_bytes - (uint32_t)(cursor - base)) return 0;
    offset = (uint32_t)(cursor - base);
    return active->call->guest_write(active->call->guest_state,
        active->guest_address + offset, (const uint8_t *)buffer, bytes);
}

void bx_ntvdm_demhndl_terminate(void)
{
    bx_ntvdm_demhndl_active_call *active = active_call();
    if (active == NULL || active->call == NULL) return;
    (void)bx_ntvdm_cpu_result_v2_stop(active->call->result);
    longjmp(active->terminate_jump, 1);
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
int bx_ntvdm_demhndl_invoke_body(bx_ntvdm_demhndl_call *call,
    void (*body)(void))
{
    bx_ntvdm_demhndl_active_call active;

    if (!bx_ntvdm_demhndl_call_valid(call) || body == NULL || g_active_call != NULL ||
        call->boundary->fault_rip > UINT64_MAX - 4u) return 0;
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
    if (setjmp(active.terminate_jump) == 0)
        body();
    /* demerror.c is the only imported owner that intentionally retains the
     * VHE address across calls.  Its shim flushes that fixed guest layout
     * while this checked-call context is still live. */
    bx_ntvdm_demerror_flush_hard_error();
    if ((is_demdir_cds_service(call->service) || active.flush_guest_buffer_on_return) &&
        active.guest_buffer != NULL &&
        !call->guest_write(call->guest_state, active.guest_address,
            active.guest_buffer, active.guest_bytes))
        SetLastError(ERROR_INVALID_ADDRESS);
    if (active.guest_buffer != NULL) free(active.guest_buffer);
    while (active.path_buffer_count != 0u)
        free(active.path_buffers[--active.path_buffer_count]);
    g_active_call = NULL;
    return bx_ntvdm_cpu_result_v2_valid(call->result);
}

int bx_ntvdm_demhndl_invoke(bx_ntvdm_demhndl_call *call)
{
    void (*service)(void) = NULL;

    if (!bx_ntvdm_demhndl_call_valid(call)) return 0;
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
    return bx_ntvdm_demhndl_invoke_body(call, service);
}
