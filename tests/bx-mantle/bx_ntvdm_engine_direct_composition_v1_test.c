#include "bx_ntvdm_engine_contract_v1.h"

#include <windows.h>

static int descriptor_set(uint16_t *out_value, uint32_t maximum,
    const wchar_t *text, uint32_t *out_chars)
{
    uint32_t index = 0u;
    if (out_value == 0 || text == 0 || out_chars == 0) return 0;
    while (text[index] != L'\0') {
        if (index >= maximum || text[index] > UINT16_MAX) return 0;
        out_value[index] = (uint16_t) text[index];
        ++index;
    }
    *out_chars = index;
    return 1;
}

static int request_set(struct bx_ntvdm_engine_request_v1 *request,
    const wchar_t *profile, const wchar_t *root)
{
    bx_ntvdm_engine_request_v1_clear(request);
    if (!descriptor_set(request->profile_descriptor,
            BX_NTVDM_ENGINE_V1_MAX_DESCRIPTOR_CHARS, profile,
            &request->profile_descriptor_chars) ||
        !descriptor_set(request->root_descriptor,
            BX_NTVDM_ENGINE_V1_MAX_DESCRIPTOR_CHARS, root,
            &request->root_descriptor_chars) ||
        !descriptor_set(request->launch_descriptor,
            BX_NTVDM_ENGINE_V1_MAX_LAUNCH_CHARS, L"2,1,e,00",
            &request->launch_descriptor_chars)) return 0;
    request->instruction_tick_budget = UINT64_C(1000000);
    request->admitted_drive_mask = UINT32_C(1) << 2u;
    return bx_ntvdm_engine_request_v1_valid(request);
}

int wmain(int argc, wchar_t **argv)
{
    struct bx_ntvdm_engine_request_v1 request;
    struct bx_ntvdm_engine_result_v1 result;
    if (argc != 3 || !request_set(&request, argv[1], argv[2])) return 1;
    if (!bx_ntvdm_engine_run_v1(&request, &result) ||
        !bx_ntvdm_engine_result_v1_valid(&result) ||
        result.terminal_kind != BX_NTVDM_ENGINE_TERMINAL_V1_NONE ||
        result.detail_code != 0u) return 2;
    /* A second direct installation is the release witness: the first call's
     * unconditional reset must leave no active process-local composition. */
    if (!bx_ntvdm_engine_run_v1(&request, &result) ||
        !bx_ntvdm_engine_result_v1_valid(&result) ||
        result.terminal_kind != BX_NTVDM_ENGINE_TERMINAL_V1_NONE ||
        result.detail_code != 0u) return 3;
    return 0;
}
