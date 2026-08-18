#include <stdio.h>
#include <string.h>

#include "bx_ntvdm_bop_ingress_v1.h"
#include "bx_ntvdm_bop_provider_registry_v1.h"
#include "bx_ntvdm_dem_package_facade_v1.h"

static int dispatch(uint8_t service, bx_ntvdm_cpu_result_v2 *result)
{
    const uint8_t bytes[4] = {0xc4u, 0xc4u, 0x50u, service};
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    bx_ntvdm_dem_package_route_v1 route;

    memset(&event, 0, sizeof(event));
    event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event.struct_bytes = sizeof(event);
    event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event.cpu_id = 1u;
    event.vector = 6u;
    event.fault_rip = UINT64_C(0x6b20);
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = 0xa55au;
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
    return bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) &&
        bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) &&
        bx_ntvdm_dem_package_facade_v1_classify(&ingress, &selection, &route) &&
        route.disposition == BX_NTVDM_DEM_PACKAGE_ORIGINAL_NOOP &&
        bx_ntvdm_dem_package_facade_v1_dispatch(&ingress, &selection, &route,
            &event, &cpu, result);
}

int main(void)
{
    static const uint8_t services[] = {0x1fu, 0x24u, 0x26u, 0x28u,
        0x2bu, 0x40u, 0x43u};
    bx_ntvdm_cpu_result_v2 result;
    uint32_t index;

    for (index = 0u; index < sizeof(services); ++index) {
        if (!dispatch(services[index], &result) ||
            !bx_ntvdm_cpu_result_v2_valid(&result) ||
            result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
            result.resume_rip != UINT64_C(0x6b24) ||
            result.cpu_delta.gpr16_write_mask != 0u ||
            result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
            result.eflags_values != 0u) return (int)(index + 1u);
    }
    puts("t226-s2 DEM original-noop full-family conformance verified");
    return 0;
}