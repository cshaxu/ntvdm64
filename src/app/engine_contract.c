#include "engine_contract.h"

#include <string.h>

static int descriptor_valid(const uint16_t *text, uint32_t chars, uint32_t maximum)
{
    uint32_t index;
    if (text == 0 || chars == 0u || chars > maximum || text[chars] != 0u) return 0;
    for (index = 0u; index < chars; ++index) {
        if (text[index] == 0u) return 0;
    }
    return 1;
}

static int mutation_mode_valid(uint32_t mutation_mode)
{
    return mutation_mode == RUNTIME_ENGINE_MUTATION_MODE_V1_DIRECT ||
        mutation_mode == RUNTIME_ENGINE_MUTATION_MODE_V1_READONLY;
}

static int terminal_kind_valid(uint32_t terminal_kind)
{
    return terminal_kind <= RUNTIME_ENGINE_TERMINAL_V1_HOST_CANCELLATION;
}

static int guest_memory_bytes_valid(uint64_t bytes)
{
    return bytes >= UINT64_C(0x100000) && bytes <= UINT64_C(0x1000000) &&
        bytes % UINT64_C(0x10000) == 0u;
}

static int reserved_memory_valid(uint64_t capacity, uint64_t base,
    uint64_t bytes)
{
    return (base == 0u && bytes == 0u) ||
        (base >= UINT64_C(0x100000) && bytes != 0u &&
         base % UINT64_C(0x10000) == 0u && bytes % UINT64_C(0x10000) == 0u &&
         base + bytes > base && base + bytes <= capacity);
}

void runtime_engine_request_v1_clear(struct runtime_engine_request_v1 *request)
{
    if (request == 0) return;
    memset(request, 0, sizeof(*request));
    request->magic = RUNTIME_ENGINE_REQUEST_V1_MAGIC;
    request->abi_version = RUNTIME_ENGINE_CONTRACT_V1_VERSION;
    request->struct_bytes = (uint32_t) sizeof(*request);
    request->mutation_mode = RUNTIME_ENGINE_MUTATION_MODE_V1_DIRECT;
    request->guest_memory_bytes = UINT64_C(0x100000);
}

int runtime_engine_request_v1_valid(const struct runtime_engine_request_v1 *request)
{
    if (request == 0 || request->magic != RUNTIME_ENGINE_REQUEST_V1_MAGIC ||
        request->abi_version != RUNTIME_ENGINE_CONTRACT_V1_VERSION ||
        request->struct_bytes != sizeof(*request) || request->reserved0 != 0u ||
        !mutation_mode_valid(request->mutation_mode) ||
        request->instruction_tick_budget == 0u ||
        !guest_memory_bytes_valid(request->guest_memory_bytes) ||
        !reserved_memory_valid(request->guest_memory_bytes,
            request->reserved_memory_base, request->reserved_memory_bytes) ||
        !descriptor_valid(request->profile_descriptor,
            request->profile_descriptor_chars,
            RUNTIME_ENGINE_V1_MAX_DESCRIPTOR_CHARS) ||
        !descriptor_valid(request->root_descriptor, request->root_descriptor_chars,
            RUNTIME_ENGINE_V1_MAX_DESCRIPTOR_CHARS) ||
        !descriptor_valid(request->launch_descriptor,
            request->launch_descriptor_chars,
            RUNTIME_ENGINE_V1_MAX_LAUNCH_CHARS)) return 0;
    return 1;
}

void runtime_engine_result_v1_clear(struct runtime_engine_result_v1 *result)
{
    if (result == 0) return;
    memset(result, 0, sizeof(*result));
    result->magic = RUNTIME_ENGINE_RESULT_V1_MAGIC;
    result->abi_version = RUNTIME_ENGINE_CONTRACT_V1_VERSION;
    result->struct_bytes = (uint32_t) sizeof(*result);
}

int runtime_engine_result_v1_set(struct runtime_engine_result_v1 *result,
    uint32_t terminal_kind, uint32_t detail_code)
{
    if (result == 0 || !terminal_kind_valid(terminal_kind)) return 0;
    runtime_engine_result_v1_clear(result);
    result->terminal_kind = terminal_kind;
    result->detail_code = detail_code;
    return 1;
}

int runtime_engine_result_v1_valid(const struct runtime_engine_result_v1 *result)
{
    return result != 0 && result->magic == RUNTIME_ENGINE_RESULT_V1_MAGIC &&
        result->abi_version == RUNTIME_ENGINE_CONTRACT_V1_VERSION &&
        result->struct_bytes == sizeof(*result) && result->reserved0 == 0u &&
        terminal_kind_valid(result->terminal_kind);
}
