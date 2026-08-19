/* demhndl.c - SVC handlers for calls where a file handle is provided.
 *
 * OpenNT source mirror: src/opennt/base/mvdm/dos/dem/demhndl.c.
 * This translation unit deliberately retains the source-file order:
 * demClose precedes demChgFilePtr.  It provides only the Direct provider
 * path; selector decode and profile dispatch remain outside this file.
 */

#include "demhndl.h"

#define DEM_ERROR_INVALID_FUNCTION 1u
#define DEM_ERROR_INVALID_HANDLE 6u
#define DEM_ERROR_WRITE_PROTECT 19u
#define DEM_ERROR_GEN_FAILURE 31u
#define DEM_ERROR_WRONG_DISK 34u

static uint32_t GETHANDLE_TOKEN(const bx_ntvdm_cpu_state_v1 *cpu)
{
    /* Divergence from OpenNT's GETHANDLE(AX,BP): its 32-bit VDM passed a
     * native HANDLE in guest registers.  AX:BP instead names a bounded,
     * session-local opaque token, whose slot owns a native-width HANDLE.
     * This is required for both x86 and x64 hosts. */
    return ((cpu->eax & 0xffffu) << 16) | (cpu->ebp & 0xffffu);
}

static int setCF_setAX_setDX(const bx_ntvdm_exception_event_v1 *boundary,
    bx_ntvdm_cpu_result_v2 *result, uint16_t ax, uint16_t dx, int carry)
{
    /* Divergence from CCPU setCF/setAX/setDX: the adapter must return a
     * checked, typed CPU delta and cannot mutate Bochs CPU state directly. */
    return bx_ntvdm_cpu_result_v2_resume(result, boundary->fault_rip + 4u) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, ax) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 2u, dx) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, carry);
}

static int demClientError(const bx_ntvdm_exception_event_v1 *boundary,
    bx_ntvdm_cpu_result_v2 *result, DWORD error)
{
    /* Divergence from demClientError(hFile, -1): the original composes a
     * historical INT 24 hard-error path.  That owner is not yet rehosted;
     * preserve the admitted register failure contract through the typed ABI. */
    if ((error >= DEM_ERROR_WRITE_PROTECT && error <= DEM_ERROR_GEN_FAILURE) ||
        error == DEM_ERROR_WRONG_DISK)
        return setCF_setAX_setDX(boundary, result, 0xffffu, 0u, 1);
    return setCF_setAX_setDX(boundary, result,
        (uint16_t)(error == 0u ? DEM_ERROR_INVALID_FUNCTION : error), 0u, 1);
}

static int demCloseClientError(const bx_ntvdm_exception_event_v1 *boundary,
    bx_ntvdm_cpu_result_v2 *result, DWORD error)
{
    uint16_t ax;

    /* demClose's original failure contract changes AX and CF only; unlike
     * demChgFilePtr it does not define a DX result. */
    ax = (uint16_t)(error == 0u ? DEM_ERROR_INVALID_FUNCTION : error);
    if ((error >= DEM_ERROR_WRITE_PROTECT && error <= DEM_ERROR_GEN_FAILURE) ||
        error == DEM_ERROR_WRONG_DISK)
        ax = 0xffffu;
    return bx_ntvdm_cpu_result_v2_resume(result, boundary->fault_rip + 4u) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, ax) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, 1);
}

static int setCF_clear_for_close(const bx_ntvdm_exception_event_v1 *boundary,
    bx_ntvdm_cpu_result_v2 *result)
{
    /* Original: setCF(0).  See the typed CPU-result divergence above. */
    return bx_ntvdm_cpu_result_v2_resume(result, boundary->fault_rip + 4u) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, 0);
}

/* demClose - Close a file
 *
 * OpenNT source mirror: demhndl.c lines 28--90.
 * Entry: AX:BP file handle; CX:DX file position, or FFFFFFFF for no seek.
 * Exit: CF clear on success; CF set and AX status on failure.
 */
int bx_ntvdm_bop_dem_close_v2(
    bx_ntvdm_dem_direct_context *context,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu,
    bx_ntvdm_cpu_result_v2 *result)
{
    HANDLE hFile;
    LONG lLoc;
    USHORT usDX, usCX;
    uint32_t handle_token;
    DWORD release_error;

    if (context == 0 || boundary == 0 || cpu == 0 || result == 0 ||
        !bx_ntvdm_dem_direct_context_valid(context) ||
        !bx_ntvdm_exception_event_v1_valid(boundary) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu) ||
        cpu->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        boundary->fault_rip > UINT64_MAX - 4u)
        return 0;

    /* Original: hFile = GETHANDLE(getAX(), getBP()). */
    handle_token = GETHANDLE_TOKEN(cpu);
    if (handle_token == 0u) {
        return setCF_clear_for_close(boundary, result);
    }
    if (!context->lookup_handle(context->state, handle_token,
            &hFile)) {
        return demCloseClientError(boundary, result, DEM_ERROR_INVALID_HANDLE);
    }

    usCX = (USHORT)(cpu->ecx & 0xffffu);
    usDX = (USHORT)(cpu->edx & 0xffffu);

    if (!((usCX == (USHORT)-1) && (usDX == (USHORT)-1))) {
        /* Original: lLoc = (LONG)((((int)usCX) << 16) + (int)usDX). */
        lLoc = (LONG)(uint32_t)(((uint32_t)usCX << 16) | usDX);
        if (SetFilePointer(hFile, lLoc, NULL, FILE_BEGIN) ==
            INVALID_SET_FILE_POINTER) {
            return demCloseClientError(boundary, result, GetLastError());
        }
    }

    /* Original: CloseHandle(hFile).  Divergence: session release owns both
     * CloseHandle and opaque-token retirement, preventing stale 64-bit host
     * handle reuse by a guest on either x86 or x64 host builds. */
    if (!context->release_handle(context->state, handle_token, &release_error)) {
        return demCloseClientError(boundary, result, release_error);
    }

    /* The original conditionally calls VrRemoveOpenNamedPipeInfo when the
     * Redirector owns this handle.  Redirector remains a separate owner
     * package, so this Direct DEM provider fabricates no named-pipe state. */
    return setCF_clear_for_close(boundary, result);
}

/* demChgFilePtr - Change File Pointer
 *
 * OpenNT source mirror: demhndl.c lines 342--389.
 */
int bx_ntvdm_bop_dem_chg_file_ptr_v2(
    bx_ntvdm_dem_direct_context *context,
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

    if (context == 0 || boundary == 0 || cpu == 0 || result == 0 ||
        !bx_ntvdm_dem_direct_context_valid(context) ||
        !bx_ntvdm_exception_event_v1_valid(boundary) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu) ||
        cpu->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        boundary->fault_rip > UINT64_MAX - 4u)
        return 0;

    /* Original: hFile = GETHANDLE(getAX(), getBP()). */
    if (!context->lookup_handle(context->state,
            GETHANDLE_TOKEN(cpu), &hFile)) {
        return demClientError(boundary, result, DEM_ERROR_INVALID_HANDLE);
    }

    /* Original: lLoc = (LONG)((((int)getCX()) << 16) + (int)getDX()). */
    lLoc = (LONG)(uint32_t)(((cpu->ecx & 0xffffu) << 16) |
        (cpu->edx & 0xffffu));

    /* Original: dwLoc = SetFilePointer(hFile, lLoc, NULL, getBL()). */
    if ((dwLoc = SetFilePointer(hFile, lLoc, NULL,
            (DWORD)(cpu->ebx & 0xffu))) == INVALID_SET_FILE_POINTER) {
        return demClientError(boundary, result, GetLastError());
    }

    /* Original: setCF(0); setAX((USHORT)dwLoc);
     * setDX((USHORT)(dwLoc >> 16)); */
    return setCF_setAX_setDX(boundary, result, (uint16_t)dwLoc,
        (uint16_t)(dwLoc >> 16), 0);
}