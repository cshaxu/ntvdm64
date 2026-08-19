#include "bx_ntvdm_engine_contract_v1.h"
#include "bop-v1/bx_ntvdm_composition_runtime_v1.h"

static int install_calls;
static int reset_calls;
static int install_response;

int bx_ntvdm_composition_runtime_v1_install_from_copied_input_with_mode(
    const uint16_t *profile, uint32_t profile_chars, const uint16_t *root,
    uint32_t root_chars, const uint16_t *launch, uint32_t launch_chars,
    uint32_t include_mask, uint32_t exclude_mask, uint32_t mutation_mode)
{
    if (profile == 0 || root == 0 || launch == 0 || profile_chars == 0u ||
        root_chars == 0u || launch_chars == 0u || include_mask != 0u ||
        exclude_mask != 0u || (mutation_mode != BX_NTVDM_ENGINE_MUTATION_MODE_V1_DIRECT &&
        mutation_mode != BX_NTVDM_ENGINE_MUTATION_MODE_V1_READONLY)) return -1;
    ++install_calls;
    return install_response;
}

void bx_ntvdm_composition_runtime_v1_reset(void)
{
    ++reset_calls;
}

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

static void request_set(struct bx_ntvdm_engine_request_v1 *request)
{
    bx_ntvdm_engine_request_v1_clear(request);
    descriptor_set(request->profile_descriptor, "profile.json",
        &request->profile_descriptor_chars);
    descriptor_set(request->root_descriptor, "bundle",
        &request->root_descriptor_chars);
    descriptor_set(request->launch_descriptor, "2,1,e,00",
        &request->launch_descriptor_chars);
    request->instruction_tick_budget = UINT64_C(10);
}

int main(void)
{
    struct bx_ntvdm_engine_request_v1 request;
    struct bx_ntvdm_engine_result_v1 result;

    request_set(&request);
    request.instruction_tick_budget = 0u;
    if (!bx_ntvdm_engine_run_v1(&request, &result) ||
        result.terminal_kind != BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_REQUEST ||
        install_calls != 0 || reset_calls != 0) return 1;
    request_set(&request);
    install_response = -1;
    if (!bx_ntvdm_engine_run_v1(&request, &result) ||
        result.terminal_kind != BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_COMPOSITION ||
        result.detail_code != 1u || install_calls != 1 || reset_calls != 1) return 2;
    install_response = 0;
    if (!bx_ntvdm_engine_run_v1(&request, &result) ||
        result.terminal_kind != BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_COMPOSITION ||
        result.detail_code != 2u || install_calls != 2 || reset_calls != 2) return 3;
    install_response = 1;
    if (!bx_ntvdm_engine_run_v1(&request, &result) ||
        result.terminal_kind != BX_NTVDM_ENGINE_TERMINAL_V1_NONE ||
        result.detail_code != 0u || install_calls != 3 || reset_calls != 3) return 4;
    return 0;
}
