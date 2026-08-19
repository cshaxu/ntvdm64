#include "bop-v1/bx_ntvdm_dem_disk_reset_service_v1.h"

#include <string.h>

int main(void)
{
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu;
    struct bx_ntvdm_mechanical_action_v1 action;
    bx_ntvdm_cpu_result_v2 result;

    memset(&ingress, 0, sizeof(ingress));
    ingress.family = BX_NTVDM_BOP_FAMILY_DEM;
    ingress.service = 0x23u;
    memset(&event, 0, sizeof(event));
    event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event.struct_bytes = sizeof(event);
    event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event.vector = 6u;
    event.fault_rip = 0x100u;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);

    if (!bx_ntvdm_dem_disk_reset_service_v1_prepare(
            0x0d95u, &ingress, &event, &cpu, 7u, &action, &result) ||
        action.kind != BX_NTVDM_MECHANICAL_ACTION_V1_WRITE ||
        action.range_count != 1u || action.payload_bytes != 2u ||
        action.ranges[0].physical_address != 0x0d95u ||
        result.resume_rip != 0x104u || result.cpu_delta.gpr16_write_mask ||
        result.eflags_write_mask) {
        return 1;
    }
    if (bx_ntvdm_dem_disk_reset_service_v1_prepare(
            0xfffffu, &ingress, &event, &cpu, 7u, &action, &result)) {
        return 2;
    }
    ingress.service = 0x24u;
    if (bx_ntvdm_dem_disk_reset_service_v1_prepare(
            0x0d95u, &ingress, &event, &cpu, 7u, &action, &result)) {
        return 3;
    }
    return 0;
}