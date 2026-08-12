#include "bx_ntvdm_dem_drive_service.h"

static int bx_ntvdm_dem_drive_count(const uint8_t inventory_types[26],
    uint16_t *out_count)
{
    uint32_t index;
    uint16_t count = 2u;
    int counting = 1;
    if (inventory_types == 0 || out_count == 0) return 0;
    for (index = 0u; index < 26u; ++index) {
        if (inventory_types[index] > 6u) return 0;
    }
    for (index = 2u; index < 26u; ++index) {
        uint8_t type = inventory_types[index];
        if (counting && (type == 2u || type == 3u || type == 5u || type == 6u))
            ++count;
        else counting = 0;
    }
    *out_count = count;
    return 1;
}

int bx_ntvdm_dem_drive_service_v1_dispatch(
    const uint8_t inventory_types[26],
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    uint16_t ax, count;
    if (inventory_types == 0 || event == 0 || cpu_before == 0 || window == 0 ||
        result == 0 || !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_instruction_window_v1_valid(window) || event->vector != 6u ||
        cpu_before->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        window->valid_bytes < 4u || window->bytes[0] != 0xc4u ||
        window->bytes[1] != 0xc4u || window->bytes[2] != 0x50u ||
        window->bytes[3] != 0x0fu || event->fault_rip > UINT64_MAX - 4u ||
        !bx_ntvdm_dem_drive_count(inventory_types, &count)) return 0;
    /* demGetDrives calls setAX(nDrives), so both bytes are defined. */
    ax = count;
    if (!bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) ||
        !bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, ax) ||
        !bx_ntvdm_cpu_result_v2_set_cf(result, 0)) {
        bx_ntvdm_cpu_result_v2_pass_through(result);
        return 0;
    }
    return 1;
}
