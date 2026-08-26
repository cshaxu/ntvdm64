#include "startup_machine_interrupt.h"

#include <string.h>

#include "adapter-softpc/mechanical_action.h"
#include "adapter-softpc/port_action.h"
#include "adapter-softpc/interrupt06_provider.h"

static int is_real_machine_bop(const struct runtime_generic_ud_event *event,
    uint8_t selector)
{
    return event != 0 && event->execution_mode == RUNTIME_CPU_EXECUTION_REAL &&
        event->window_bytes >= 3u && event->window[0] == 0xc4u &&
        event->window[1] == 0xc4u && event->window[2] == selector &&
        event->fault_rip <= UINT64_MAX - 3u;
}

static int preflight_writes(const struct runtime_generic_ud_event *event,
    uint8_t selector)
{
    struct runtime_mechanical_action action;
    runtime_mechanical_action_clear(&action);
    action.action_id = selector == 0x02u ? 0x2001u : 0x6001u;
    action.kind = RUNTIME_MECHANICAL_ACTION_PREFLIGHT_WRITE;
    action.range_count = selector == 0x02u ? 1u : 2u;
    action.payload_bytes = selector == 0x02u ? 1u : 5u;
    if (selector == 0x06u) {
        action.ranges[0].physical_address = ((uint64_t)event->ss << 4) + event->esp;
        action.ranges[0].byte_count = 4u;
        action.ranges[1].physical_address = INT06_BDA_INTERRUPT_CAUSE;
        action.ranges[1].payload_offset = 4u;
        action.ranges[1].byte_count = 1u;
    } else {
        action.ranges[0].physical_address = INT06_BDA_INTERRUPT_CAUSE;
        action.ranges[0].byte_count = 1u;
    }
    return runtime_mechanical_action_valid(&action) &&
        runtime_machine_execute_mechanical_action(&action);
}

static int execute_operation(const struct int06_provider_operation *operation,
    uint32_t action_id, uint8_t *read_value)
{
    struct runtime_mechanical_action memory;
    struct runtime_port_action port;
    if (operation == 0 || read_value == 0) return 0;
    if (operation->kind == INT06_PROVIDER_OPERATION_RAM_READ8 ||
        operation->kind == INT06_PROVIDER_OPERATION_RAM_WRITE8) {
        runtime_mechanical_action_clear(&memory);
        memory.action_id = action_id;
        memory.kind = operation->kind == INT06_PROVIDER_OPERATION_RAM_READ8 ?
            RUNTIME_MECHANICAL_ACTION_READ : RUNTIME_MECHANICAL_ACTION_WRITE;
        memory.range_count = 1u;
        memory.payload_bytes = 1u;
        memory.ranges[0].physical_address = operation->address;
        memory.ranges[0].byte_count = 1u;
        memory.payload[0] = operation->value;
        if (!runtime_mechanical_action_valid(&memory) ||
            !runtime_machine_execute_mechanical_action(&memory)) return 0;
        *read_value = memory.payload[0];
        return 1;
    }
    if (operation->kind != INT06_PROVIDER_OPERATION_PORT_READ8 &&
        operation->kind != INT06_PROVIDER_OPERATION_PORT_WRITE8) return 0;
    runtime_port_action_initialize(&port);
    port.kind = operation->kind == INT06_PROVIDER_OPERATION_PORT_READ8 ?
        RUNTIME_PORT_ACTION_READ8 : RUNTIME_PORT_ACTION_WRITE8;
    port.port = (uint16_t)operation->address;
    port.value = operation->value;
    if (!runtime_machine_execute_port_action(&port)) return 0;
    *read_value = port.value;
    return 1;
}

int runtime_startup_machine_interrupt_dispatch(
    const struct runtime_generic_ud_event *event,
    runtime_cpu_result *result)
{
    struct int06_provider_state state;
    struct int06_provider_operation operation;
    uint8_t selector;
    uint8_t read_value = 0u;
    uint32_t count = 0u;
    if (event == 0 || result == 0 || event->window_bytes < 3u ||
        (event->window[2] != 0x02u && event->window[2] != 0x06u)) return 0;
    selector = event->window[2];
    if (!is_real_machine_bop(event, selector)) return 0;
    int06_provider_initialize(&state);
    if ((selector == 0x02u && !int06_provider_begin_unexpected(&state)) ||
        (selector == 0x06u && !int06_provider_begin(&state,
            event->ss, event->esp)) || !preflight_writes(event, selector))
        return runtime_cpu_result_stop(result);
    while (int06_provider_next(&state, &operation)) {
        if (++count > 24u || !execute_operation(&operation, 0x6200u + count,
                &read_value) || !int06_provider_accept(&state,
                &operation, read_value)) return runtime_cpu_result_stop(result);
    }
    return int06_provider_complete(&state) ?
        runtime_cpu_result_resume(result, event->fault_rip + 3u) :
        runtime_cpu_result_stop(result);
}
