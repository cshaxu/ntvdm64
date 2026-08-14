#include "bx_ntvdm_spckbd_init_service.h"

#include <stdint.h>

#define BX_NTVDM_SPCKBD_INIT_VERSION 0xbeefu
#define BX_NTVDM_SPCKBD_INIT_BOP 0x5fu
#define BX_NTVDM_SPCKBD_KIO_USE_HOST_INT10_INDEX 17u
#define BX_NTVDM_SPCKBD_KIO_USE_HOST_INT10_BYTES 2u
#define BX_NTVDM_SPCKBD_APERTURE_BYTES UINT64_C(0x100000)

static int bx_ntvdm_spckbd_stream_state_matches(
    uint32_t display_mode, const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    uint64_t *table_address)
{
    uint64_t base, address;
    if (display_mode != BX_NTVDM_SPCKBD_DISPLAY_STREAM_IO || event == 0 ||
        cpu_before == 0 || window == 0 || table_address == 0 ||
        !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_instruction_window_v1_valid(window) || event->vector != 6u ||
        cpu_before->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        (cpu_before->eax & 0xffffu) != BX_NTVDM_SPCKBD_INIT_VERSION ||
        window->valid_bytes < 3u || window->bytes[0] != 0xc4u ||
        window->bytes[1] != 0xc4u || window->bytes[2] != BX_NTVDM_SPCKBD_INIT_BOP ||
        event->fault_rip > UINT64_MAX - 3u) return 0;
    base = (uint64_t)cpu_before->ds << 4;
    address = base + (uint64_t)(cpu_before->esi & 0xffffu) +
        BX_NTVDM_SPCKBD_KIO_USE_HOST_INT10_INDEX * 2u;
    if (base >= BX_NTVDM_SPCKBD_APERTURE_BYTES ||
        address > BX_NTVDM_SPCKBD_APERTURE_BYTES -
            BX_NTVDM_SPCKBD_KIO_USE_HOST_INT10_BYTES) return 0;
    *table_address = address;
    return 1;
}

int bx_ntvdm_spckbd_stream_state_v1_prepare(
    uint32_t display_mode, const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_guest_gather_read_action_v1 *action)
{
    bx_ntvdm_guest_range range;
    uint64_t table_address;
    if (action == 0 || !bx_ntvdm_spckbd_stream_state_matches(display_mode,
            event, cpu_before, window, &table_address)) return 0;
    range.address = table_address;
    range.length = BX_NTVDM_SPCKBD_KIO_USE_HOST_INT10_BYTES;
    return bx_ntvdm_guest_gather_read_action_v1_need_read_resume(action, &range,
        1u, event->fault_rip + 3u);
}

int bx_ntvdm_spckbd_stream_state_v1_complete(
    uint32_t display_mode, const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_gather_read_action_v1 *action,
    const uint8_t table_bytes[2], uint64_t byte_count,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[1])
{
    bx_ntvdm_instruction_window_v1 window;
    uint64_t table_address, target_address;
    uint16_t target_offset;
    if (event == 0 || cpu_before == 0 || action == 0 || table_bytes == 0 ||
        transaction == 0 || payload == 0 || byte_count !=
            BX_NTVDM_SPCKBD_KIO_USE_HOST_INT10_BYTES ||
        !bx_ntvdm_guest_gather_read_action_v1_valid(action) ||
        action->disposition != BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_NEED_READ ||
        action->range_count != 1u || action->total_bytes !=
            BX_NTVDM_SPCKBD_KIO_USE_HOST_INT10_BYTES ||
        action->cpu_result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        action->cpu_result.resume_rip != event->fault_rip + 3u) return 0;
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){ 0xc4u, 0xc4u, BX_NTVDM_SPCKBD_INIT_BOP }, 3u);
    if (!bx_ntvdm_spckbd_stream_state_matches(display_mode, event, cpu_before,
            &window, &table_address) || action->ranges[0].address != table_address ||
        action->ranges[0].length != BX_NTVDM_SPCKBD_KIO_USE_HOST_INT10_BYTES)
        return 0;
    target_offset = (uint16_t)table_bytes[0] | ((uint16_t)table_bytes[1] << 8);
    target_address = ((uint64_t)cpu_before->ds << 4) + target_offset;
    if (target_address >= BX_NTVDM_SPCKBD_APERTURE_BYTES) return 0;
    bx_ntvdm_multi_write_transaction_v1_initialize(transaction, event, cpu_before);
    if (!bx_ntvdm_multi_write_v1_add(&transaction->writes, target_address, 1u, 0u) ||
        !bx_ntvdm_cpu_result_v2_resume(&transaction->result,
            action->cpu_result.resume_rip) ||
        !bx_ntvdm_cpu_result_v2_set_cf(&transaction->result, 1)) return 0;
    payload[0] = BX_NTVDM_SPCKBD_DISPLAY_STREAM_IO;
    return bx_ntvdm_multi_write_transaction_v1_preflight(transaction,
        BX_NTVDM_SPCKBD_APERTURE_BYTES, 1u);
}

int bx_ntvdm_spckbd_init_service_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    if (event == 0 || cpu_before == 0 || window == 0 || result == 0 ||
        !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_instruction_window_v1_valid(window) || event->vector != 6u ||
        cpu_before->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        (cpu_before->eax & 0xffffu) != BX_NTVDM_SPCKBD_INIT_VERSION ||
        window->valid_bytes < 3u || window->bytes[0] != 0xc4u ||
        window->bytes[1] != 0xc4u || window->bytes[2] != BX_NTVDM_SPCKBD_INIT_BOP ||
        event->fault_rip > UINT64_MAX - 3u)
        return 0;
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 3u) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, 1);
}
