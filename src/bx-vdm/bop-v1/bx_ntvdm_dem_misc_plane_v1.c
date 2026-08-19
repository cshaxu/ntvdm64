#include "bx_ntvdm_dem_misc_plane_v1.h"

#include "bx_ntvdm_dem_debug_service.h"
#include "bx_ntvdm_dem_plane_v1.h"
#include "bx_ntvdm_dem_system_symbol_service.h"

/* These are distinct original ordinary-return paths, grouped only because the
 * contained non-debug/non-WOW/no-VDD profile selects the same typed resume.
 * Keep demDiskReset separate: unlike the diagnostic paths it has a physical
 * media owner and a COMMAND-owned FDAccess guest locator. */
static int source_ordinary_return_service(uint32_t service)
{
    switch (service) {
    case 0x1du: /* demSetV86KernelAddr: original body is intentionally empty. */
    case 0x34u: /* demLoadDosAppSym: SignalSegmentNotice is non-debug no-op. */
    case 0x35u: /* demFreeDosAppSym: SignalSegmentNotice is non-debug no-op. */
    case 0x36u: /* demEntryDosApp: no VDD user hook in this CLI profile. */
    case 0x37u: /* demDOSDispCall: compiled diagnostic-only body. */
    case 0x38u: /* demDOSDispRet: compiled diagnostic-only body. */
    case 0x39u: /* demOutputString: debugger-only host output. */
    case 0x3au: /* demInputString: debugger-only host prompt. */
    case 0x3eu: /* demWOWFiles: non-WOW branch preserves caller AL. */
        return 1;
    default:
        return 0;
    }
}

/* demDiskReset calls HostFloppyReset, HostFdiskReset and clears the WORD that
 * COMMAND's cmdSetInfo registered from DS:CX.  This standalone DEM session
 * intentionally has neither a native raw-media owner nor that cross-package
 * locator.  With no admitted media resources its reset loops have no state to
 * release and the original void-return ABI remains observable; it must never
 * be used once COMMAND/DEM registration composition is installed. */
static int disk_reset_without_media_owner(uint32_t service)
{
    return service == 0x23u;
}

int bx_ntvdm_dem_misc_plane_v1_dispatch(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_exception_result_v1 *result)
{
    bx_ntvdm_dem_plane_record_v1 record;
    if (!ingress || !selection || !event || !cpu_before || !window || !result ||
        !bx_ntvdm_dem_plane_v1_classify(ingress, selection, &record) ||
        record.component != BX_NTVDM_DEM_COMPONENT_MISC ||
        record.disposition != BX_NTVDM_DEM_PLANE_DEFERRED)
        return 0;

    /* Keep each source-derived endpoint inside its original DEM component.
     * More miscellaneous endpoints can be admitted here only with their own
     * source contract; composition never dispatches a raw service number. */
    switch (record.service) {
    case 0x3bu: return bx_ntvdm_dem_debug_service_v1_dispatch(event,
        cpu_before, window, result);
    case 0x45u: return bx_ntvdm_dem_system_symbol_service_v1_dispatch(event,
        cpu_before, window, result);
    default:
        if ((!source_ordinary_return_service(record.service) &&
             !disk_reset_without_media_owner(record.service)) || event->vector != 6u ||
            cpu_before->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
            event->fault_rip > UINT64_MAX - 4u)
            return 0;
        return bx_ntvdm_exception_result_v1_resume(result,
            event->fault_rip + 4u);
    }
}
