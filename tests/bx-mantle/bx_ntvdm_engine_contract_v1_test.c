#include "engine_contract_v1.h"

#include <string.h>

static void descriptor_set(uint16_t *out_value, const char *text,
    uint32_t *out_chars)
{
    uint32_t index = 0u;
    while (text[index] != '\0') {
        out_value[index] = (uint16_t) (unsigned char) text[index];
        ++index;
    }
    *out_chars = index;
}

int main(void)
{
    struct bx_ntvdm_engine_request_v1 request;
    struct bx_ntvdm_engine_result_v1 result;
    uint32_t kind;

    bx_ntvdm_engine_request_v1_clear(&request);
    descriptor_set(request.profile_descriptor, "profile.json",
        &request.profile_descriptor_chars);
    descriptor_set(request.root_descriptor, "bundle",
        &request.root_descriptor_chars);
    descriptor_set(request.launch_descriptor, "2,1,e,00",
        &request.launch_descriptor_chars);
    request.instruction_tick_budget = UINT64_C(1000);
    if (request.mutation_mode != BX_NTVDM_ENGINE_MUTATION_MODE_V1_DIRECT ||
        !bx_ntvdm_engine_request_v1_valid(&request)) return 1;
    request.mutation_mode = BX_NTVDM_ENGINE_MUTATION_MODE_V1_READONLY;
    if (!bx_ntvdm_engine_request_v1_valid(&request)) return 2;
    request.mutation_mode = 3u;
    if (bx_ntvdm_engine_request_v1_valid(&request)) return 3;
    request.mutation_mode = BX_NTVDM_ENGINE_MUTATION_MODE_V1_DIRECT;
    if (request.guest_memory_bytes != UINT64_C(0x100000)) return 7;
    request.guest_memory_bytes = UINT64_C(0x1ff000);
    if (bx_ntvdm_engine_request_v1_valid(&request)) return 8;
    request.guest_memory_bytes = UINT64_C(0x200000);
    if (!bx_ntvdm_engine_request_v1_valid(&request)) return 9;
    request.guest_memory_bytes = UINT64_C(0x1010000);
    if (bx_ntvdm_engine_request_v1_valid(&request)) return 10;
    request.guest_memory_bytes = UINT64_C(0x100000);
    request.reserved_memory_base = UINT64_C(0x100000);
    request.reserved_memory_bytes = UINT64_C(0x10000);
    if (bx_ntvdm_engine_request_v1_valid(&request)) return 11;
    request.guest_memory_bytes = UINT64_C(0x200000);
    if (!bx_ntvdm_engine_request_v1_valid(&request)) return 12;
    request.reserved_memory_base = UINT64_C(0x101000);
    if (bx_ntvdm_engine_request_v1_valid(&request)) return 13;
    request.reserved_memory_base = UINT64_C(0x100000);
    request.reserved_memory_bytes = UINT64_C(0x110000);
    if (bx_ntvdm_engine_request_v1_valid(&request)) return 14;
    request.reserved_memory_bytes = 0u;
    request.reserved_memory_base = 0u;
    request.launch_descriptor[request.launch_descriptor_chars] = 1u;
    if (bx_ntvdm_engine_request_v1_valid(&request)) return 2;
    request.launch_descriptor[request.launch_descriptor_chars] = 0u;
    request.root_descriptor[1] = 0u;
    if (bx_ntvdm_engine_request_v1_valid(&request)) return 3;
    descriptor_set(request.root_descriptor, "bundle", &request.root_descriptor_chars);
    request.instruction_tick_budget = 0u;
    if (bx_ntvdm_engine_request_v1_valid(&request)) return 4;
    request.instruction_tick_budget = UINT64_C(1);
    request.struct_bytes--;
    if (bx_ntvdm_engine_request_v1_valid(&request)) return 5;
    request.struct_bytes = (uint32_t) sizeof(request);
    if (!bx_ntvdm_engine_request_v1_valid(&request)) return 6;

    bx_ntvdm_engine_result_v1_clear(&result);
    if (!bx_ntvdm_engine_result_v1_valid(&result)) return 7;
    for (kind = BX_NTVDM_ENGINE_TERMINAL_V1_NONE;
         kind <= BX_NTVDM_ENGINE_TERMINAL_V1_HOST_CANCELLATION; ++kind) {
        if (!bx_ntvdm_engine_result_v1_set(&result, kind, kind + 11u) ||
            !bx_ntvdm_engine_result_v1_valid(&result) ||
            result.terminal_kind != kind || result.detail_code != kind + 11u)
            return (int) (20u + kind);
    }
    if (bx_ntvdm_engine_result_v1_set(&result,
        BX_NTVDM_ENGINE_TERMINAL_V1_HOST_CANCELLATION + 1u, 0u)) return 40;
    result.reserved0 = 1u;
    if (bx_ntvdm_engine_result_v1_valid(&result)) return 41;
    return 0;
}
