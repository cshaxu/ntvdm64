#include "bx_ntvdm_dem_load_dos_service.h"

#include <string.h>

#define BX_NTVDM_DEM_LOAD_DOS_APERTURE UINT64_C(0x100000)

int bx_ntvdm_dem_load_dos_service_v1_prepare(
    const byob_image *ntdos, const byob_component_descriptor *identity,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window, uint32_t action_id,
    struct bx_ntvdm_mechanical_action_v1 *action,
    bx_ntvdm_cpu_result_v2 *result)
{
    uint64_t destination;

    if (!ntdos || !identity || !event || !cpu_before || !window || !action ||
        !result || !action_id || !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_instruction_window_v1_valid(window) || event->vector != 6u ||
        cpu_before->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        window->valid_bytes < 4u || window->bytes[0] != 0xc4u ||
        window->bytes[1] != 0xc4u || window->bytes[2] != 0x50u ||
        window->bytes[3] != 0x11u || event->fault_rip > UINT64_MAX - 4u ||
        !ntdos->bytes || !ntdos->byte_count ||
        ntdos->byte_count != identity->bytes ||
        ntdos->byte_count > BX_NTVDM_MECHANICAL_ACTION_V1_MAX_BYTES)
        return 0;
    destination = ((uint64_t)(uint16_t)cpu_before->edi) << 4;
    if (destination > BX_NTVDM_DEM_LOAD_DOS_APERTURE - ntdos->byte_count)
        return 0;
    bx_ntvdm_mechanical_action_v1_clear(action);
    action->action_id = action_id;
    action->kind = BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
    action->range_count = 1u;
    action->payload_bytes = (uint32_t)ntdos->byte_count;
    action->ranges[0].physical_address = destination;
    action->ranges[0].byte_count = (uint32_t)ntdos->byte_count;
    action->ranges[0].payload_offset = 0u;
    memcpy(action->payload, ntdos->bytes, ntdos->byte_count);
    return bx_ntvdm_mechanical_action_v1_valid(action) &&
        bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u);
}
