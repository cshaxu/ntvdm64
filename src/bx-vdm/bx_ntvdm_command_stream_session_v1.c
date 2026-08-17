#include "bx_ntvdm_command_stream_session_v1.h"

#include <windows.h>
#include <string.h>

#define BX_NTVDM_COMMAND_STREAM_TOKEN_PREFIX 0xb0000000u
#define BX_NTVDM_COMMAND_STREAM_TOKEN_GENERATION_MASK 0x03ffffffu
#define BX_NTVDM_COMMAND_CHILD_COMMAND_BYTES 260u

static uint32_t next_generation = 1u;

static int selected(const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window)
{
    return event != 0 && cpu != 0 && window != 0 &&
        bx_ntvdm_exception_event_v1_valid(event) &&
        bx_ntvdm_cpu_state_v1_valid(cpu) &&
        bx_ntvdm_instruction_window_v1_valid(window) && event->vector == 6u &&
        cpu->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        event->fault_rip <= UINT64_MAX - 4u && window->valid_bytes >= 4u &&
        window->bytes[0] == 0xc4u && window->bytes[1] == 0xc4u &&
        window->bytes[2] == 0x54u && window->bytes[3] == 6u;
}

static uint32_t token(const bx_ntvdm_command_stream_session_v1 *value,
    uint32_t slot)
{
    return BX_NTVDM_COMMAND_STREAM_TOKEN_PREFIX |
        ((value->generation & BX_NTVDM_COMMAND_STREAM_TOKEN_GENERATION_MASK) << 2u) |
        slot;
}

static int selected_execution(const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window)
{
    return event != 0 && cpu != 0 && window != 0 &&
        bx_ntvdm_exception_event_v1_valid(event) &&
        bx_ntvdm_cpu_state_v1_valid(cpu) &&
        bx_ntvdm_instruction_window_v1_valid(window) && event->vector == 6u &&
        cpu->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        event->fault_rip <= UINT64_MAX - 4u && window->valid_bytes >= 4u &&
        window->bytes[0] == 0xc4u && window->bytes[1] == 0xc4u &&
        window->bytes[2] == 0x54u && window->bytes[3] == 8u;
}

static int terminated(const uint8_t *value, uint32_t bytes)
{
    return value != 0 && bytes >= 2u && value[bytes - 1u] == 0u;
}

static int environment_terminated(const uint8_t *value, uint32_t bytes)
{
    return value != 0 && bytes >= 2u && value[bytes - 1u] == 0u &&
        value[bytes - 2u] == 0u;
}

static uint32_t read_u32le(const uint8_t *value)
{
    return (uint32_t)value[0] | ((uint32_t)value[1] << 8u) |
        ((uint32_t)value[2] << 16u) | ((uint32_t)value[3] << 24u);
}

void bx_ntvdm_command_stream_session_v1_initialize(
    bx_ntvdm_command_stream_session_v1 *value)
{
    if (value == 0) return;
    memset(value, 0, sizeof(*value));
    value->magic = BX_NTVDM_COMMAND_STREAM_SESSION_V1_MAGIC;
    value->abi_version = BX_NTVDM_COMMAND_STREAM_SESSION_V1_VERSION;
    value->struct_bytes = sizeof(*value);
    value->generation = next_generation++ & BX_NTVDM_COMMAND_STREAM_TOKEN_GENERATION_MASK;
    if (value->generation == 0u) value->generation = 1u;
}

int bx_ntvdm_command_stream_session_v1_admit_cli_standard_handles(
    bx_ntvdm_command_stream_session_v1 *value)
{
    static const DWORD identifiers[BX_NTVDM_COMMAND_STREAM_SESSION_V1_SLOT_COUNT] = {
        STD_INPUT_HANDLE, STD_OUTPUT_HANDLE, STD_ERROR_HANDLE
    };
    uint32_t slot;
    if (!bx_ntvdm_command_stream_session_v1_valid(value) ||
        value->available_mask != 0u) return 0;
    for (slot = 0u; slot < BX_NTVDM_COMMAND_STREAM_SESSION_V1_SLOT_COUNT; ++slot) {
        HANDLE handle = GetStdHandle(identifiers[slot]);
        if (handle != NULL && handle != INVALID_HANDLE_VALUE) {
            value->private_handle_values[slot] = (uintptr_t)handle;
            value->available_mask |= 1u << slot;
        }
    }
    return 1;
}

int bx_ntvdm_command_stream_session_v1_valid(
    const bx_ntvdm_command_stream_session_v1 *value)
{
    uint32_t slot;
    if (value == 0 || value->magic != BX_NTVDM_COMMAND_STREAM_SESSION_V1_MAGIC ||
        value->abi_version != BX_NTVDM_COMMAND_STREAM_SESSION_V1_VERSION ||
        value->struct_bytes != sizeof(*value) || value->generation == 0u ||
        (value->generation & ~BX_NTVDM_COMMAND_STREAM_TOKEN_GENERATION_MASK) != 0u ||
        (value->available_mask & ~0x07u) != 0u || value->reserved0 != 0u)
        return 0;
    for (slot = 0u; slot < BX_NTVDM_COMMAND_STREAM_SESSION_V1_SLOT_COUNT; ++slot) {
        if ((value->available_mask & (1u << slot)) == 0u &&
            value->private_handle_values[slot] != 0u) return 0;
    }
    return 1;
}

int bx_ntvdm_command_stream_session_v1_dispatch_stream(
    const bx_ntvdm_command_stream_session_v1 *value, int direct_granted,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    uint32_t slot, opaque;
    if (!bx_ntvdm_command_stream_session_v1_valid(value) || result == 0 ||
        !selected(event, cpu, window)) return 0;
    slot = cpu->ecx & 0xffffu;
    if (!direct_granted || slot >= BX_NTVDM_COMMAND_STREAM_SESSION_V1_SLOT_COUNT ||
        (value->available_mask & (1u << slot)) == 0u) {
        return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
            bx_ntvdm_cpu_result_v2_set_cf(result, 1) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, 0u) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 2u, 0u);
    }
    opaque = token(value, slot);
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, 0) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, 0u) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 1u,
            (uint16_t)opaque) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 2u, 0u) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 3u,
            (uint16_t)(opaque >> 16u));
}

int bx_ntvdm_command_stream_session_v1_validate_std_handles(
    bx_ntvdm_command_stream_session_v1 *value, const uint8_t *payload,
    uint32_t payload_bytes)
{
    /* OpenNT's packed STD_HANDLES order is stderr, stdout, stdin. */
    static const uint32_t slots[BX_NTVDM_COMMAND_STREAM_SESSION_V1_SLOT_COUNT] = {
        2u, 1u, 0u
    };
    uint32_t index;
    if (!bx_ntvdm_command_stream_session_v1_valid(value)) return 0;
    if (payload == 0 || payload_bytes != 12u) {
        if (value->rejected_record_count != UINT32_MAX)
            ++value->rejected_record_count;
        return 0;
    }
    for (index = 0u; index < BX_NTVDM_COMMAND_STREAM_SESSION_V1_SLOT_COUNT;
         ++index) {
        uint32_t slot = slots[index];
        if ((value->available_mask & (1u << slot)) == 0u ||
            read_u32le(payload + index * 4u) != token(value, slot)) {
            if (value->rejected_record_count != UINT32_MAX)
                ++value->rejected_record_count;
            return 0;
        }
    }
    if (value->validated_record_count == UINT32_MAX) return 0;
    ++value->validated_record_count;
    return 1;
}

int bx_ntvdm_command_stream_session_v1_launch(
    bx_ntvdm_command_stream_session_v1 *value, const uint8_t *command,
    uint32_t command_bytes, const uint8_t *environment,
    uint32_t environment_bytes,
    const bx_ntvdm_command_host_context_v1 *host_context,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    wchar_t command_wide[BX_NTVDM_COMMAND_CHILD_COMMAND_BYTES];
    wchar_t environment_wide[BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES];
    wchar_t directory_wide[BX_NTVDM_COMMAND_HOST_CONTEXT_V1_DIRECTORY_BYTES];
    HANDLE duplicated[3] = { INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE };
    STARTUPINFOW startup = { 0 };
    PROCESS_INFORMATION process = { 0 };
    DWORD exit_code = 0u, error = ERROR_NOT_SUPPORTED;
    uint32_t slot;
    if (!bx_ntvdm_command_stream_session_v1_valid(value) || !terminated(command,
            command_bytes) || command_bytes > BX_NTVDM_COMMAND_CHILD_COMMAND_BYTES ||
        !environment_terminated(environment, environment_bytes) ||
        environment_bytes > BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES ||
        !bx_ntvdm_command_host_context_v1_valid(host_context) || result == 0 ||
        !selected_execution(event, cpu, window)) return 0;
    if (!MultiByteToWideChar(CP_OEMCP, 0, (const char *)command,
            (int)command_bytes, command_wide, BX_NTVDM_COMMAND_CHILD_COMMAND_BYTES) ||
        !MultiByteToWideChar(CP_OEMCP, 0, (const char *)environment,
            (int)environment_bytes, environment_wide,
            BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES)) {
        error = GetLastError(); goto complete;
    }
    for (slot = 0u; slot < 3u; ++slot) {
        if (!DuplicateHandle(GetCurrentProcess(), (HANDLE)value->private_handle_values[slot],
                GetCurrentProcess(), &duplicated[slot], 0u, TRUE,
                DUPLICATE_SAME_ACCESS)) { error = GetLastError(); goto complete; }
    }
    startup.cb = sizeof(startup); startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = duplicated[0]; startup.hStdOutput = duplicated[1];
    startup.hStdError = duplicated[2];
    if (!MultiByteToWideChar(CP_OEMCP, 0,
            (const char *)host_context->selected_directory,
            (int)host_context->directory_bytes, directory_wide,
            BX_NTVDM_COMMAND_HOST_CONTEXT_V1_DIRECTORY_BYTES)) {
        error = GetLastError(); goto complete;
    }
    if (!CreateProcessW(0, command_wide, 0, 0, TRUE, CREATE_UNICODE_ENVIRONMENT,
            environment_wide, GetFileAttributesW(directory_wide) == INVALID_FILE_ATTRIBUTES ?
            0 : directory_wide, &startup, &process)) { error = GetLastError(); goto complete; }
    ++value->launch_count;
    if (WaitForSingleObject(process.hProcess, INFINITE) != WAIT_OBJECT_0 ||
        !GetExitCodeProcess(process.hProcess, &exit_code)) error = GetLastError();
    else { value->last_result = exit_code; ++value->completion_count; error = ERROR_SUCCESS; }
complete:
    if (process.hThread) CloseHandle(process.hThread);
    if (process.hProcess) CloseHandle(process.hProcess);
    for (slot = 0u; slot < 3u; ++slot)
        if (duplicated[slot] != INVALID_HANDLE_VALUE) CloseHandle(duplicated[slot]);
    value->last_error = error;
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, 0) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u,
            (uint16_t)((cpu->eax & 0xff00u) | ((error == ERROR_SUCCESS ?
                exit_code : error) & 0xffu)));
}
