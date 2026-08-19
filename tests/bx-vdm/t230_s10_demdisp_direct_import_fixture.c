/* Whole-DEM catalogue witness for the OpenNT demdisp.c mirror.  Package
 * fixtures cover the owner files; this one covers the original table shape
 * and its nine demdisp-owned ordinary-return entries. */

#include <stdio.h>
#include <string.h>

#include "bop/opennt/dem/demdisp.h"
#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_exception_abi.h"

static int resumed_cf_clear(const bx_ntvdm_exception_event_v1 *event,
    bx_ntvdm_cpu_result_v2 *result)
{
    return bx_ntvdm_cpu_result_v2_valid(result) &&
        result->disposition == BX_NTVDM_CPU_RESULT_V2_RESUME &&
        result->resume_rip == event->fault_rip + 4u &&
        (result->eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) == 0u;
}

int main(void)
{
    static const uint8_t noops[] = { 0x1fu, 0x24u, 0x26u, 0x28u,
        0x2bu, 0x40u, 0x42u, 0x43u };
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_result_v2 result;
    size_t index;

    memset(&event, 0, sizeof(event));
    event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event.struct_bytes = sizeof(event);
    event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event.fault_rip = 0x2000u;
    if (bx_ntvdm_opennt_dem_service_count() != 73u) return 1;
    for (index = 0u; index != 73u; ++index) {
        const bx_ntvdm_opennt_dem_service_entry *entry =
            bx_ntvdm_opennt_dem_service_at((uint8_t)index);
        if (entry == NULL || entry->service != index || entry->handler == NULL ||
            entry->owner_file == NULL) return 2;
    }
    for (index = 0u; index != sizeof(noops); ++index) {
        if (!bx_ntvdm_opennt_dem_not_yet_implemented(&event, &result) ||
            !resumed_cf_clear(&event, &result)) return 3;
    }
    if (!bx_ntvdm_opennt_dem_set_v86_kernel_addr(&event, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != event.fault_rip + 4u) return 4;
    puts("T230 S10 OpenNT demdisp mirror: 73-entry catalogue and nine original ordinary-return entries verified");
    return 0;
}
