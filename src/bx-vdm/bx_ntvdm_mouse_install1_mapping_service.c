#include "bx_ntvdm_mouse_install1_mapping_service.h"

#include <string.h>

#define BX_NTVDM_MOUSE_INSTALL1_BOP 0xc8u
#define BX_NTVDM_MOUSE_INSTALL1_TABLE_BYTES 4u
#define BX_NTVDM_MOUSE_INSTALL1_INT33_IVT_ADDRESS 0xccu

static int bx_ntvdm_mouse_install1_mapping_matches(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window, uint64_t *table_address)
{
    uint64_t address;
    if (event == 0 || cpu_before == 0 || window == 0 || table_address == 0 ||
        !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_instruction_window_v1_valid(window) || event->vector != 6u ||
        cpu_before->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        window->valid_bytes < 3u || window->bytes[0] != 0xc4u ||
        window->bytes[1] != 0xc4u || window->bytes[2] != BX_NTVDM_MOUSE_INSTALL1_BOP ||
        event->fault_rip > UINT64_MAX - 3u) return 0;
    address = ((uint64_t)cpu_before->cs << 4) +
        (uint64_t)(cpu_before->ebx & 0xffffu);
    if (address > UINT64_MAX - BX_NTVDM_MOUSE_INSTALL1_TABLE_BYTES) return 0;
    *table_address = address;
    return 1;
}

int bx_ntvdm_mouse_install1_mapping_service_v1_prepare(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_guest_gather_read_action_v1 *action)
{
    bx_ntvdm_guest_range range;
    uint64_t table_address;
    if (action == 0 || !bx_ntvdm_mouse_install1_mapping_matches(event,
            cpu_before, window, &table_address)) return 0;
    range.address = table_address;
    range.length = BX_NTVDM_MOUSE_INSTALL1_TABLE_BYTES;
    return bx_ntvdm_guest_gather_read_action_v1_need_read_resume(action, &range,
        1u, event->fault_rip + 3u);
}

int bx_ntvdm_mouse_install1_mapping_service_v1_complete(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_gather_read_action_v1 *action,
    const uint8_t *table_bytes, uint64_t byte_count,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[4])
{
    bx_ntvdm_instruction_window_v1 window;
    uint64_t table_address;
    if (event == 0 || cpu_before == 0 || action == 0 || table_bytes == 0 ||
        transaction == 0 || payload == 0 || byte_count != 4u ||
        !bx_ntvdm_guest_gather_read_action_v1_valid(action) ||
        action->disposition != BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_NEED_READ ||
        action->range_count != 1u || action->total_bytes != 4u ||
        action->cpu_result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        action->cpu_result.resume_rip != event->fault_rip + 3u) return 0;
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){ 0xc4u, 0xc4u, BX_NTVDM_MOUSE_INSTALL1_BOP }, 3u);
    if (!bx_ntvdm_mouse_install1_mapping_matches(event, cpu_before, &window,
            &table_address) || action->ranges[0].address != table_address ||
        action->ranges[0].length != 4u) return 0;
    bx_ntvdm_multi_write_transaction_v1_initialize(transaction, event, cpu_before);
    if (!bx_ntvdm_multi_write_v1_add(&transaction->writes,
            BX_NTVDM_MOUSE_INSTALL1_INT33_IVT_ADDRESS, 4u, 0u) ||
        !bx_ntvdm_cpu_result_v2_resume(&transaction->result,
            action->cpu_result.resume_rip)) return 0;
    memcpy(payload, table_bytes, 4u);
    return 1;
}
