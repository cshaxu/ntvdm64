#include "bx_ntvdm_command_launch_execution_provider_v1.h"
#include "bx_ntvdm_command_plane_v1.h"

#include <string.h>

static void event_initialize(bx_ntvdm_exception_event_v1 *event)
{
    *event = (bx_ntvdm_exception_event_v1){ BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(*event),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x200u };
}

static int profile_initialize(bx_ntvdm_mutation_profile_v1 *profile,
    bx_ntvdm_command_profile_consumer_v1 *consumer, uint32_t mode)
{
    bx_ntvdm_mutation_profile_v1_initialize(profile, mode);
    return bx_ntvdm_command_profile_consumer_v1_register_class(profile,
        BX_NTVDM_MUTATION_CLASS_V1_HOST_GLOBAL, 0x03u) &&
        bx_ntvdm_command_profile_consumer_v1_initialize(consumer, profile);
}

static uint32_t dispatch_token(bx_ntvdm_command_launch_execution_provider_v1 *provider,
    bx_ntvdm_exception_event_v1 *event, bx_ntvdm_cpu_state_v1 *cpu,
    bx_ntvdm_instruction_window_v1 *window, uint32_t slot, int direct)
{
    bx_ntvdm_cpu_result_v2 result;
    cpu->ecx = slot;
    if (!bx_ntvdm_command_stream_session_v1_dispatch_stream(
            &provider->stream_session, direct, event, cpu, window, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u)
        return 0u;
    return ((uint32_t)result.cpu_delta.gpr16_values[3] << 16u) |
        result.cpu_delta.gpr16_values[1];
}

int main(void)
{
    bx_ntvdm_command_launch_execution_provider_v1 provider, later, readonly;
    bx_ntvdm_mutation_profile_v1 direct_profile, readonly_profile;
    bx_ntvdm_command_profile_consumer_v1 direct_consumer, readonly_consumer;
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_cpu_result_v2 result;
    uint32_t service, slot = 0u, token, stale, current[3];
    uint8_t packed[12];
    const uint8_t expected[17] = { 0,0,0,0,0,0,1,1,1,0,1,0,0,0,0,0,0 };

    if (!bx_ntvdm_command_launch_execution_provider_v1_initialize(&provider) ||
        !bx_ntvdm_command_launch_execution_provider_v1_valid(&provider) ||
        provider.stream_session.available_mask != 0u) return 1;
    for (service = 0u; service != 17u; ++service)
        if (bx_ntvdm_command_launch_execution_provider_v1_owns_service((uint8_t)service) != expected[service]) return 2;
    if (bx_ntvdm_command_launch_execution_provider_v1_owns_service(17u) ||
        !profile_initialize(&direct_profile, &direct_consumer,
            BX_NTVDM_MUTATION_MODE_V1_DIRECT) ||
        !bx_ntvdm_command_launch_execution_provider_v1_admit_cli_streams(
            &provider, &direct_consumer) || provider.stream_session.available_mask != 0x07u)
        return 3;
    while ((provider.stream_session.available_mask & (1u << slot)) == 0u) ++slot;

    event_initialize(&event);
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    { const uint8_t bytes[4] = {0xc4u,0xc4u,0x54u,6u};
      bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes)); }
    token = dispatch_token(&provider, &event, &cpu, &window, slot, 1);
    if ((token & 0xf0000000u) != 0xb0000000u ||
        token == (uint32_t)provider.stream_session.private_handle_values[slot]) return 4;

    if (!bx_ntvdm_command_launch_execution_provider_v1_initialize(&later) ||
        !bx_ntvdm_command_launch_execution_provider_v1_admit_cli_streams(
            &later, &direct_consumer)) return 5;
    stale = token;
    cpu.ecx = slot;
    if (!bx_ntvdm_command_stream_session_v1_dispatch_stream(&later.stream_session,
            1, &event, &cpu, &window, &result) ||
        ((uint32_t)result.cpu_delta.gpr16_values[3] << 16u | result.cpu_delta.gpr16_values[1]) == stale)
        return 6;

    current[0] = dispatch_token(&later, &event, &cpu, &window, 0u, 1);
    current[1] = dispatch_token(&later, &event, &cpu, &window, 1u, 1);
    current[2] = dispatch_token(&later, &event, &cpu, &window, 2u, 1);
    if (current[0] == 0u || current[1] == 0u || current[2] == 0u) return 7;
    memcpy(packed + 0u, &current[2], sizeof(current[2]));
    memcpy(packed + 4u, &current[1], sizeof(current[1]));
    memcpy(packed + 8u, &current[0], sizeof(current[0]));
    memcpy(packed + 0u, &stale, sizeof(stale));
    if (bx_ntvdm_command_stream_session_v1_validate_std_handles(
            &later.stream_session, packed, sizeof(packed)) ||
        later.stream_session.rejected_record_count == 0u) return 8;
    memcpy(packed + 0u, &current[2], sizeof(current[2]));
    if (!bx_ntvdm_command_stream_session_v1_validate_std_handles(
            &later.stream_session, packed, sizeof(packed)) ||
        later.stream_session.validated_record_count != 1u) return 9;

    if (!bx_ntvdm_command_launch_execution_provider_v1_initialize(&readonly) ||
        !profile_initialize(&readonly_profile, &readonly_consumer,
            BX_NTVDM_MUTATION_MODE_V1_READONLY) ||
        bx_ntvdm_command_launch_execution_provider_v1_admit_cli_streams(
            &readonly, &readonly_consumer) || readonly.stream_session.available_mask != 0u ||
        !bx_ntvdm_command_stream_session_v1_dispatch_stream(&readonly.stream_session,
            0, &event, &cpu, &window, &result) ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) == 0u)
        return 8;

    { const uint8_t bytes[4] = {0xc4u,0xc4u,0x54u,7u};
      bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes)); }
    if (!bx_ntvdm_command_execution_lifecycle_v1_dispatch(&provider.execution,
            0, 0, &event, &cpu, &window, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME || result.eflags_values != 0u) return 9;
    return 0;
}