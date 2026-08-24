#include "bx_ntvdm_startup_machine_interrupt_v1.h"

#include <string.h>

#include "adapter-softpc/bx_ntvdm_mechanical_action_v1.h"
#include "adapter-softpc/bx_ntvdm_port_action_v1.h"
#include "opennt-bop/softpc/opennt_int06_provider_v1.h"

static int is_real_machine_bop(const struct bx_ntvdm_generic_ud_event_v1 *event,
    uint8_t selector)
{
    return event != 0 && event->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        event->window_bytes >= 3u && event->window[0] == 0xc4u &&
        event->window[1] == 0xc4u && event->window[2] == selector &&
        event->fault_rip <= UINT64_MAX - 3u;
}

static int preflight_writes(const struct bx_ntvdm_generic_ud_event_v1 *event,
    uint8_t selector)
{
    struct bx_ntvdm_mechanical_action_v1 action;
    bx_ntvdm_mechanical_action_v1_clear(&action);
    action.action_id = selector == 0x02u ? 0x2001u : 0x6001u;
    action.kind = BX_NTVDM_MECHANICAL_ACTION_V1_PREFLIGHT_WRITE;
    action.range_count = selector == 0x02u ? 1u : 2u;
    action.payload_bytes = selector == 0x02u ? 1u : 5u;
    if (selector == 0x06u) {
        action.ranges[0].physical_address = ((uint64_t)event->ss << 4) + event->esp;
        action.ranges[0].byte_count = 4u;
        action.ranges[1].physical_address = OPENNT_INT06_BDA_INTERRUPT_CAUSE;
        action.ranges[1].payload_offset = 4u;
        action.ranges[1].byte_count = 1u;
    } else {
        action.ranges[0].physical_address = OPENNT_INT06_BDA_INTERRUPT_CAUSE;
        action.ranges[0].byte_count = 1u;
    }
    return bx_ntvdm_mechanical_action_v1_valid(&action) &&
        bx_ntvdm_mantle_execute_mechanical_action_v1(&action);
}

static int execute_operation(const struct opennt_int06_provider_v1_operation *operation,
    uint32_t action_id, uint8_t *read_value)
{
    struct bx_ntvdm_mechanical_action_v1 memory;
    struct bx_ntvdm_port_action_v1 port;
    if (operation == 0 || read_value == 0) return 0;
    if (operation->kind == OPENNT_INT06_PROVIDER_V1_OPERATION_RAM_READ8 ||
        operation->kind == OPENNT_INT06_PROVIDER_V1_OPERATION_RAM_WRITE8) {
        bx_ntvdm_mechanical_action_v1_clear(&memory);
        memory.action_id = action_id;
        memory.kind = operation->kind == OPENNT_INT06_PROVIDER_V1_OPERATION_RAM_READ8 ?
            BX_NTVDM_MECHANICAL_ACTION_V1_READ : BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
        memory.range_count = 1u;
        memory.payload_bytes = 1u;
        memory.ranges[0].physical_address = operation->address;
        memory.ranges[0].byte_count = 1u;
        memory.payload[0] = operation->value;
        if (!bx_ntvdm_mechanical_action_v1_valid(&memory) ||
            !bx_ntvdm_mantle_execute_mechanical_action_v1(&memory)) return 0;
        *read_value = memory.payload[0];
        return 1;
    }
    if (operation->kind != OPENNT_INT06_PROVIDER_V1_OPERATION_PORT_READ8 &&
        operation->kind != OPENNT_INT06_PROVIDER_V1_OPERATION_PORT_WRITE8) return 0;
    bx_ntvdm_port_action_v1_initialize(&port);
    port.kind = operation->kind == OPENNT_INT06_PROVIDER_V1_OPERATION_PORT_READ8 ?
        BX_NTVDM_PORT_ACTION_V1_READ8 : BX_NTVDM_PORT_ACTION_V1_WRITE8;
    port.port = (uint16_t)operation->address;
    port.value = operation->value;
    if (!bx_ntvdm_mantle_execute_port_action_v1(&port)) return 0;
    *read_value = port.value;
    return 1;
}

int bx_ntvdm_startup_machine_interrupt_v1_dispatch(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    bx_ntvdm_cpu_result_v2 *result)
{
    struct opennt_int06_provider_v1_state state;
    struct opennt_int06_provider_v1_operation operation;
    uint8_t selector;
    uint8_t read_value = 0u;
    uint32_t count = 0u;
    if (event == 0 || result == 0 || event->window_bytes < 3u ||
        (event->window[2] != 0x02u && event->window[2] != 0x06u)) return 0;
    selector = event->window[2];
    if (!is_real_machine_bop(event, selector)) return 0;
    opennt_int06_provider_v1_initialize(&state);
    if ((selector == 0x02u && !opennt_int06_provider_v1_begin_unexpected(&state)) ||
        (selector == 0x06u && !opennt_int06_provider_v1_begin(&state,
            event->ss, event->esp)) || !preflight_writes(event, selector))
        return bx_ntvdm_cpu_result_v2_stop(result);
    while (opennt_int06_provider_v1_next(&state, &operation)) {
        if (++count > 24u || !execute_operation(&operation, 0x6200u + count,
                &read_value) || !opennt_int06_provider_v1_accept(&state,
                &operation, read_value)) return bx_ntvdm_cpu_result_v2_stop(result);
    }
    return opennt_int06_provider_v1_complete(&state) ?
        bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 3u) :
        bx_ntvdm_cpu_result_v2_stop(result);
}
