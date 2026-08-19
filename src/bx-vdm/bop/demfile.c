/* demfile.c - DEM file-name service providers.
 *
 * OpenNT source mirror: src/opennt/base/mvdm/dos/dem/demfile.c.
 * This translation unit preserves demChMod's local data and source control
 * flow.  The outer namespace partition owns selector decoding, checked guest
 * DS:DX copying and session-profile admission.
 */

#include "demfile.h"

#define DOS_ATTR_MASK 0x37u

static int setCF(const bx_ntvdm_exception_event_v1 *boundary,
    bx_ntvdm_cpu_result_v2 *result, int carry)
{
    /* Divergence from CCPU setCF: adapter providers return a checked, typed
     * CPU delta rather than directly modifying Bochs CPU state. */
    return bx_ntvdm_cpu_result_v2_resume(result, boundary->fault_rip + 4u) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, carry);
}

static int setCX_setCF_clear(const bx_ntvdm_exception_event_v1 *boundary,
    bx_ntvdm_cpu_result_v2 *result, uint16_t cx)
{
    return setCF(boundary, result, 0) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 1u, cx);
}

static int demClientError(const bx_ntvdm_exception_event_v1 *boundary,
    bx_ntvdm_cpu_result_v2 *result, DWORD error)
{
    /* Divergence from demClientError(INVALID_HANDLE_VALUE, *lpFileName):
     * historical INT 24 composition is not independently rehostable yet.
     * Preserve its admitted AX/CF failure ABI through the typed result. */
    return bx_ntvdm_cpu_result_v2_resume(result, boundary->fault_rip + 4u) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u,
            (uint16_t)(error == 0u ? 1u : error)) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, 1);
}

/* demChMod - Get or set file attributes.
 *
 * OpenNT source mirror: demfile.c lines 437--506.
 */
int bx_ntvdm_bop_dem_ch_mod_v2(
    bx_ntvdm_dem_whole_provider_v1 *provider,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu,
    uint8_t drive,
    const wchar_t *relative_path,
    bx_ntvdm_cpu_result_v2 *result)
{
    const wchar_t *lpFileName;
    DWORD dwAttr;
    DWORD error = 0u;

    if (provider == 0 || boundary == 0 || cpu == 0 || relative_path == 0 ||
        result == 0 || !bx_ntvdm_dem_whole_provider_v1_valid(provider) ||
        !bx_ntvdm_exception_event_v1_valid(boundary) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu) ||
        cpu->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        boundary->fault_rip > UINT64_MAX - 4u) {
        return 0;
    }

    /* Original: lpFileName = (LPSTR)GetVDMAddr(getDS(), getDX()).
     * Divergence: the outer namespace partition has already performed the
     * checked guest-memory gather and canonical path conversion.  This
     * provider consumes only that copied path, never a guest pointer. */
    lpFileName = relative_path;

    if ((cpu->eax & 0xffu) == 0u) {
        /* Original: dwAttr = GetFileAttributesOem(lpFileName).
         * Divergence: use the admitted CLI host-namespace capability, whose
         * drive policy and UTF-16 conversion are outside OpenNT's OEM helper. */
        if (!bx_ntvdm_host_namespace_v1_query_file_attributes(
                provider->host_namespace, drive, lpFileName, &dwAttr, &error)) {
            goto dcerr;
        }

        if (dwAttr == FILE_ATTRIBUTE_NORMAL) {
            dwAttr = 0u;
        }
        else {
            dwAttr &= DOS_ATTR_MASK;
        }

        return setCX_setCF_clear(boundary, result, (uint16_t)dwAttr);
    }

    /* Original: if ((dwAttr = getCX()) == 0)
     *               dwAttr = FILE_ATTRIBUTE_NORMAL. */
    if ((dwAttr = (uint16_t)cpu->ecx) == 0u) {
        dwAttr = FILE_ATTRIBUTE_NORMAL;
    }

    dwAttr &= DOS_ATTR_MASK;

    /* Original: if (!SetFileAttributesOem(lpFileName, dwAttr)) goto dcerr.
     * The capability call is the x86/x64-safe host integration substitute. */
    if (!bx_ntvdm_host_namespace_v1_set_file_attributes(provider->host_namespace,
            drive, lpFileName, dwAttr, &error)) {
        goto dcerr;
    }

    return setCF(boundary, result, 0);

dcerr:
    return demClientError(boundary, result, error);
}