#include "bx_ntvdm_command_child_redirection_v1.h"

#include <string.h>

static int selected(const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, uint8_t service)
{
    return event != 0 && cpu != 0 && window != 0 &&
        bx_ntvdm_exception_event_v1_valid(event) &&
        bx_ntvdm_cpu_state_v1_valid(cpu) &&
        bx_ntvdm_instruction_window_v1_valid(window) && event->vector == 6u &&
        cpu->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        event->fault_rip <= UINT64_MAX - 4u && window->valid_bytes >= 4u &&
        window->bytes[0] == 0xc4u && window->bytes[1] == 0xc4u &&
        window->bytes[2] == 0x54u && window->bytes[3] == service;
}

void bx_ntvdm_command_child_redirection_v1_initialize(
    bx_ntvdm_command_child_redirection_v1 *value)
{
    if (!value) return;
    memset(value, 0, sizeof(*value));
    value->magic = BX_NTVDM_COMMAND_CHILD_REDIRECTION_V1_MAGIC;
    value->abi_version = BX_NTVDM_COMMAND_CHILD_REDIRECTION_V1_VERSION;
    value->struct_bytes = (uint32_t)sizeof(*value);
    value->execute = bx_ntvdm_command_child_backend_v1_execute_direct;
}

int bx_ntvdm_command_child_redirection_v1_valid(
    const bx_ntvdm_command_child_redirection_v1 *value)
{
    return value != 0 && value->magic == BX_NTVDM_COMMAND_CHILD_REDIRECTION_V1_MAGIC &&
        value->abi_version == BX_NTVDM_COMMAND_CHILD_REDIRECTION_V1_VERSION &&
        value->struct_bytes == sizeof(*value) && value->reserved0 == 0u &&
        value->completion_present <= 1u && value->completion_consumed <= 1u &&
        value->completion_consumed <= value->completion_present &&
        value->reserved1 == 0u && value->reserved2 == 0u && value->execute != 0;
}


int bx_ntvdm_command_child_redirection_v1_launch(
    bx_ntvdm_command_child_redirection_v1 *value,
    const bx_ntvdm_command_stream_session_v1 *stream,
    const uint8_t *command, uint32_t command_bytes,
    const uint8_t *environment, uint32_t environment_bytes,
    const bx_ntvdm_command_host_context_v1 *host_context,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    bx_ntvdm_command_child_backend_v1_request request;
    bx_ntvdm_command_child_backend_v1_result backend_result;
    uint8_t service; uint32_t error;
    if (!bx_ntvdm_command_child_redirection_v1_valid(value) || !result ||
        window == 0 || (window->bytes[3] != 8u && window->bytes[3] != 10u)) return 0;
    service = window->bytes[3];
    if (!selected(event, cpu, window, service)) return 0;
    memset(&request, 0, sizeof(request));
    request.magic = BX_NTVDM_COMMAND_CHILD_BACKEND_V1_MAGIC;
    request.abi_version = BX_NTVDM_COMMAND_CHILD_BACKEND_V1_VERSION;
    request.struct_bytes = (uint32_t)sizeof(request);
    request.launch_service = service; request.command = command;
    request.command_bytes = command_bytes; request.environment = environment;
    request.environment_bytes = environment_bytes; request.host_context = host_context;
    request.stream = service == 8u ? stream : 0;
    if (!bx_ntvdm_command_child_backend_v1_request_valid(&request) ||
        !value->execute(&request, &backend_result) ||
        !bx_ntvdm_command_child_backend_v1_result_valid(&backend_result)) return 0;
    error = backend_result.host_error;
    if (error == 0u && backend_result.completed != 0u) {
        if (value->launch_count == UINT32_MAX || value->completion_count == UINT32_MAX)
            return 0;
        ++value->launch_count; ++value->completion_count;
        value->last_result = backend_result.exit_code;
        value->completion_present = 1u; value->completion_consumed = 0u;
    }
    value->last_error = error;
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, 0) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u,
            (uint16_t)((cpu->eax & 0xff00u) |
            ((error == 0u && backend_result.completed != 0u ?
              backend_result.exit_code : error) & 0xffu)));
}

int bx_ntvdm_command_child_redirection_v1_complete(
    bx_ntvdm_command_child_redirection_v1 *value,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    uint16_t ax;
    if (!bx_ntvdm_command_child_redirection_v1_valid(value) || !result ||
        value->completion_present == 0u || value->completion_consumed != 0u ||
        !selected(event, cpu, window, 11u)) return 0;
    value->completion_consumed = 1u;
    ax = (uint16_t)((cpu->eax & 0xff00u) | (value->last_result & 0xffu));
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, 0) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, ax);
}