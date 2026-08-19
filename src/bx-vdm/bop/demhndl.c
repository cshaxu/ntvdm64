/*
 * demhndl.c -- OpenNT-shaped BOP provider for file-handle services.
 *
 * Source reference: src/opennt/base/mvdm/dos/dem/demhndl.c,
 * demChgFilePtr, lines 342--389.  The control flow, local variable names,
 * DOS/Win32 positioning-value assertion, signed CX:DX conversion, and
 * DX:AX result layout below deliberately follow that source.
 */

#include "demhndl.h"

#define DEM_ERROR_INVALID_FUNCTION 1u
#define DEM_ERROR_INVALID_HANDLE 6u
#define DEM_ERROR_WRITE_PROTECT 19u
#define DEM_ERROR_GEN_FAILURE 31u
#define DEM_ERROR_WRONG_DISK 34u

static uint32_t GETHANDLE_TOKEN(const bx_ntvdm_cpu_state_v1 *cpu)
{
    /* Divergence from OpenNT's GETHANDLE(AX,BP): the legacy VDM passed a
     * native HANDLE through guest registers.  A 64-bit CLI cannot expose a
     * host HANDLE to the guest, so AX:BP names a bounded, session-local
     * opaque token instead. */
    return ((cpu->eax & 0xffffu) << 16) | (cpu->ebp & 0xffffu);
}

static int setCF_setAX_setDX(const bx_ntvdm_exception_event_v1 *boundary,
    bx_ntvdm_cpu_result_v2 *result, uint16_t ax, uint16_t dx, int carry)
{
    /* Divergence from OpenNT's CCPU setCF/setAX/setDX macros: bx-vdm emits a
     * typed copied CPU delta; it never writes Bochs CPU state directly. */
    return bx_ntvdm_cpu_result_v2_resume(result, boundary->fault_rip + 4u) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, ax) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 2u, dx) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, carry);
}

static int demClientError(const bx_ntvdm_exception_event_v1 *boundary,
    bx_ntvdm_cpu_result_v2 *result, DWORD error)
{
    /* Divergence from the original demClientError(hFile, -1): that helper
     * enters the historical INT 24 hard-error composition.  This CLI-owned
     * provider preserves the currently admitted register contract until the
     * original hard-error owner is itself rehosted. */
    if ((error >= DEM_ERROR_WRITE_PROTECT && error <= DEM_ERROR_GEN_FAILURE) ||
        error == DEM_ERROR_WRONG_DISK)
        return setCF_setAX_setDX(boundary, result, 0xffffu, 0u, 1);
    return setCF_setAX_setDX(boundary, result,
        (uint16_t)(error == 0u ? DEM_ERROR_INVALID_FUNCTION : error), 0u, 1);
}

/* demClose's original failure contract changes AX and CF only; unlike
 * demChgFilePtr it does not specify a DX result. */
static int demCloseClientError(const bx_ntvdm_exception_event_v1 *boundary,
    bx_ntvdm_cpu_result_v2 *result, DWORD error)
{
    uint16_t ax = (uint16_t)(error == 0u ? DEM_ERROR_INVALID_FUNCTION : error);
    if ((error >= DEM_ERROR_WRITE_PROTECT && error <= DEM_ERROR_GEN_FAILURE) ||
        error == DEM_ERROR_WRONG_DISK)
        ax = 0xffffu;
    return bx_ntvdm_cpu_result_v2_resume(result, boundary->fault_rip + 4u) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, ax) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, 1);
}
int bx_ntvdm_bop_dem_chg_file_ptr_v2(
    bx_ntvdm_dem_whole_provider_v1 *provider,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu,
    bx_ntvdm_cpu_result_v2 *result)
{
    HANDLE hFile;
    LONG lLoc;
    DWORD dwLoc;

#if (FILE_BEGIN != 0 || FILE_CURRENT != 1 || FILE_END != 2)
#error "Win32 values not DOS compatible"
#endif

    if (provider == 0 || boundary == 0 || cpu == 0 || result == 0 ||
        !bx_ntvdm_dem_whole_provider_v1_valid(provider) ||
        !bx_ntvdm_exception_event_v1_valid(boundary) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu) ||
        cpu->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        boundary->fault_rip > UINT64_MAX - 4u)
        return 0;

    /* Original: hFile = GETHANDLE(getAX(), getBP()).  See the token
     * divergence documented in GETHANDLE_TOKEN above. */
    if (!bx_ntvdm_dem_file_session_v1_lookup(&provider->files,
            GETHANDLE_TOKEN(cpu), &hFile))
        return demClientError(boundary, result, DEM_ERROR_INVALID_HANDLE);

    /* Original: lLoc = (LONG)((((int)getCX()) << 16) + (int)getDX()). */
    lLoc = (LONG)(uint32_t)(((cpu->ecx & 0xffffu) << 16) |
        (cpu->edx & 0xffffu));

    /* Original: dwLoc = SetFilePointer(hFile, lLoc, NULL, getBL()).
     * SetFilePointer remains intentional here: it exactly retains the
     * original 32-bit DOS result contract, including its -1 failure test. */
    if ((dwLoc = SetFilePointer(hFile, lLoc, NULL,
            (DWORD)(cpu->ebx & 0xffu))) == INVALID_SET_FILE_POINTER) {
        return demClientError(boundary, result, GetLastError());
    }

    /* Original: setCF(0); setAX((USHORT)dwLoc);
     * setDX((USHORT)(dwLoc >> 16)); */
    return setCF_setAX_setDX(boundary, result, (uint16_t)dwLoc,
        (uint16_t)(dwLoc >> 16), 0);
}
/* demClose - Close a file
 *
 * Source reference: src/opennt/base/mvdm/dos/dem/demhndl.c, lines 28--90.
 * Entry: AX:BP file handle; CX:DX file position, or FFFFFFFF for no seek.
 * Exit: CF clear on success; CF set and AX status on failure.
 */
static int setCF_clear_for_close(const bx_ntvdm_exception_event_v1 *boundary,
    bx_ntvdm_cpu_result_v2 *result)
{
    /* Original: setCF(0).  As above, return the update through the copied
     * typed CPU result instead of mutating Bochs state in this provider. */
    return bx_ntvdm_cpu_result_v2_resume(result, boundary->fault_rip + 4u) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, 0);
}

int bx_ntvdm_bop_dem_close_v2(
    bx_ntvdm_dem_whole_provider_v1 *provider,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu,
    bx_ntvdm_cpu_result_v2 *result)
{
    HANDLE hFile;
    LONG lLoc;
    USHORT usDX, usCX;
    uint32_t handle_token;

    if (provider == 0 || boundary == 0 || cpu == 0 || result == 0 ||
        !bx_ntvdm_dem_whole_provider_v1_valid(provider) ||
        !bx_ntvdm_exception_event_v1_valid(boundary) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu) ||
        cpu->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        boundary->fault_rip > UINT64_MAX - 4u)
        return 0;

    /* Original: hFile = GETHANDLE(getAX(), getBP()). */
    handle_token = GETHANDLE_TOKEN(cpu);
    if (handle_token == 0u)
        return setCF_clear_for_close(boundary, result);
    if (!bx_ntvdm_dem_file_session_v1_lookup(&provider->files, handle_token,
            &hFile))
        return demCloseClientError(boundary, result, DEM_ERROR_INVALID_HANDLE);

    usCX = (USHORT)(cpu->ecx & 0xffffu);
    usDX = (USHORT)(cpu->edx & 0xffffu);
    if (!((usCX == (USHORT)-1) && (usDX == (USHORT)-1))) {
        /* Original: lLoc = (LONG)((((int)usCX) << 16) + (int)usDX). */
        lLoc = (LONG)(uint32_t)(((uint32_t)usCX << 16) | usDX);
        if (SetFilePointer(hFile, lLoc, NULL, FILE_BEGIN) ==
            INVALID_SET_FILE_POINTER)
            return demCloseClientError(boundary, result, GetLastError());
    }

    /* Original: CloseHandle(hFile).  Divergence: the local file-session owns
     * CloseHandle and token retirement together, so a guest cannot retain a
     * stale 64-bit host handle after close. */
    if (!bx_ntvdm_dem_file_session_v1_release(&provider->files, handle_token))
        return demCloseClientError(boundary, result, GetLastError());

    /* The original then conditionally invokes VrRemoveOpenNamedPipeInfo when
     * the VDM Redirector owns this handle.  Redirector is a distinct BOP
     * owner and is not composed into this CLI DEM provider; no named-pipe
     * bookkeeping is fabricated here. */
    return setCF_clear_for_close(boundary, result);
}
