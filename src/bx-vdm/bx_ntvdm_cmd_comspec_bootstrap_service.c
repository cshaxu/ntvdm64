#include "bx_ntvdm_cmd_comspec_bootstrap_service.h"

#include <string.h>

#define BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_APERTURE UINT64_C(0x100000)

static int bx_ntvdm_cmd_comspec_bootstrap_v1_match(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    uint8_t service)
{
    return event && cpu && window && bx_ntvdm_exception_event_v1_valid(event) &&
        bx_ntvdm_cpu_state_v1_valid(cpu) &&
        bx_ntvdm_instruction_window_v1_valid(window) && event->vector == 6u &&
        cpu->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL && window->valid_bytes >= 4u &&
        window->bytes[0] == 0xc4u && window->bytes[1] == 0xc4u &&
        window->bytes[2] == 0x54u && window->bytes[3] == service &&
        event->fault_rip <= UINT64_MAX - 4u;
}

void bx_ntvdm_cmd_comspec_bootstrap_v1_initialize(
    bx_ntvdm_cmd_comspec_bootstrap_v1 *value)
{
    if (value) memset(value, 0, sizeof(*value));
}

int bx_ntvdm_cmd_comspec_bootstrap_v1_prepare_comspec(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    const bx_ntvdm_cmd_comspec_bootstrap_v1 *state,
    bx_ntvdm_guest_gather_read_action_v1 *action)
{
    bx_ntvdm_guest_range range;
    uint64_t address;

    if (!state || !action ||
        state->stage != BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_WAIT_COMSPEC ||
        !bx_ntvdm_cmd_comspec_bootstrap_v1_match(event, cpu, window, 2u)) return 0;
    address = ((uint64_t)cpu->ds << 4) + (uint16_t)cpu->edx;
    if (address > BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_APERTURE -
            BX_NTVDM_CMD_COMSPEC_TEXT_MAX_BYTES) return 0;
    range.address = address;
    range.length = BX_NTVDM_CMD_COMSPEC_TEXT_MAX_BYTES;
    return bx_ntvdm_guest_gather_read_action_v1_need_read_resume(action, &range, 1u,
        event->fault_rip + 4u);
}

int bx_ntvdm_cmd_comspec_bootstrap_v1_complete_comspec(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_guest_gather_read_action_v1 *action,
    const uint8_t *bytes, uint64_t byte_count,
    const bx_ntvdm_command_host_context_v1 *host_context,
    bx_ntvdm_cmd_comspec_bootstrap_v1 *state,
    bx_ntvdm_cpu_result_v2 *result)
{
    uint32_t byte_index;
    uint32_t ax;

    uint32_t environment_bytes;
    if (!state || !result || !action || !bytes ||
        state->stage != BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_WAIT_COMSPEC ||
        byte_count != BX_NTVDM_CMD_COMSPEC_TEXT_MAX_BYTES ||
        !bx_ntvdm_guest_gather_read_action_v1_valid(action) ||
        action->disposition != BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_NEED_READ ||
        action->range_count != 1u ||
        action->total_bytes != BX_NTVDM_CMD_COMSPEC_TEXT_MAX_BYTES ||
        action->cpu_result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        action->cpu_result.resume_rip != event->fault_rip + 4u) return 0;
    for (byte_index = 0u; byte_index < BX_NTVDM_CMD_COMSPEC_TEXT_MAX_BYTES;
            ++byte_index) {
        if (bytes[byte_index] == 0u) break;
    }
    if (byte_index == BX_NTVDM_CMD_COMSPEC_TEXT_MAX_BYTES) return 0;
    if (host_context != 0 && !bx_ntvdm_command_host_context_v1_valid(host_context))
        return 0;
    environment_bytes = host_context == 0 ? 0u : host_context->environment_bytes;
    if (8u + byte_index + 1u > sizeof(state->environment) -
        (environment_bytes == 0u ? 1u : environment_bytes)) return 0;
    memcpy(state->environment, "COMSPEC=", 8u);
    memcpy(state->environment + 8u, bytes, byte_index + 1u);
    state->environment_bytes = 8u + byte_index + 1u;
    if (environment_bytes != 0u) {
        memcpy(state->environment + state->environment_bytes,
            host_context->environment, environment_bytes);
        state->environment_bytes += environment_bytes;
    } else {
        state->environment[state->environment_bytes++] = 0u;
    }
    state->stage = BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_ENVIRONMENT_READY;
    ax = (uint16_t)((cpu->eax & 0xff00u) | 1u);
    return bx_ntvdm_cpu_result_v2_resume(result, action->cpu_result.resume_rip) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, (uint16_t)ax);
}

int bx_ntvdm_cmd_comspec_bootstrap_v1_repeat_comspec(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    const bx_ntvdm_cmd_comspec_bootstrap_v1 *state,
    bx_ntvdm_cpu_result_v2 *result)
{
    if (!state || !result ||
        state->stage == BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_WAIT_COMSPEC ||
        !bx_ntvdm_cmd_comspec_bootstrap_v1_match(event, cpu, window, 2u)) return 0;
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u);
}

int bx_ntvdm_cmd_comspec_bootstrap_v1_prepare_environment(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    const bx_ntvdm_cmd_comspec_bootstrap_v1 *state,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD])
{
    uint32_t paragraphs;
    uint64_t address;

    if (!transaction || !payload || !state ||
        !bx_ntvdm_cmd_comspec_bootstrap_v1_match(event, cpu, window, 15u)) return 0;
    bx_ntvdm_multi_write_transaction_v1_initialize(transaction, event, cpu);
    if (state->stage == BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_ENVIRONMENT_CONSUMED)
        return bx_ntvdm_cpu_result_v2_resume(&transaction->result, event->fault_rip + 4u) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&transaction->result.cpu_delta, 3u, 0u);
    if (state->stage != BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_ENVIRONMENT_READY ||
        state->environment_bytes < 10u ||
        state->environment_bytes > sizeof(state->environment)) return 0;
    paragraphs = (state->environment_bytes + 15u) >> 4;
    if ((uint16_t)cpu->ebx < paragraphs)
        return bx_ntvdm_cpu_result_v2_resume(&transaction->result, event->fault_rip + 4u) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&transaction->result.cpu_delta, 3u,
                (uint16_t)paragraphs);
    address = (uint64_t)cpu->es << 4;
    if (address > BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_APERTURE - state->environment_bytes)
        return 0;
    memcpy(payload, state->environment, state->environment_bytes);
    if (!bx_ntvdm_multi_write_v1_add(&transaction->writes, address,
            state->environment_bytes, 0u) ||
        !bx_ntvdm_cpu_result_v2_resume(&transaction->result, event->fault_rip + 4u) ||
        !bx_ntvdm_cpu_delta_v1_set_gpr16(&transaction->result.cpu_delta, 3u, 0u))
        return 0;
    return bx_ntvdm_multi_write_transaction_v1_preflight(transaction,
        BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_APERTURE, state->environment_bytes);
}

int bx_ntvdm_cmd_comspec_bootstrap_v1_complete_environment(
    bx_ntvdm_cmd_comspec_bootstrap_v1 *state,
    const bx_ntvdm_multi_write_transaction_v1 *transaction)
{
    if (!state || !transaction ||
        state->stage != BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_ENVIRONMENT_READY ||
        transaction->writes.write_count != 1u ||
        !bx_ntvdm_cpu_result_v2_valid(&transaction->result) ||
        transaction->result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        transaction->result.cpu_delta.gpr16_write_mask != (UINT32_C(1) << 3) ||
        transaction->result.cpu_delta.gpr16_values[3] != 0u) return 0;
    state->stage = BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_ENVIRONMENT_CONSUMED;
    return 1;
}
