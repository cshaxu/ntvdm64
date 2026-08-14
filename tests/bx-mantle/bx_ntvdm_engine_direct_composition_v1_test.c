#include "bx_ntvdm_engine_contract_v1.h"
#include "bx_ntvdm_machine_stage_v1.h"
#include "bx_ntvdm_terminal_observation_v1.h"

#include <stdio.h>
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

static int terminal_valid(const struct bx_ntvdm_engine_result_v1 *result)
{
    if (result->terminal_kind == BX_NTVDM_ENGINE_TERMINAL_V1_EXECUTION_BUDGET)
        return result->detail_code == BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_BUDGET;
    if (result->terminal_kind == BX_NTVDM_ENGINE_TERMINAL_V1_CONTROLLED_GUEST_TERMINAL)
        return result->detail_code ==
            BX_NTVDM_MACHINE_STAGE_V1_EXECUTION_CONTROLLED_STOP;
    return 0;
}

int wmain(int argc, wchar_t **argv)
{
    struct bx_ntvdm_engine_request_v1 request;
    struct bx_ntvdm_engine_result_v1 result;
    struct bx_ntvdm_terminal_observation_v1 observation;
    if (argc != 3 || !request_set(&request, argv[1], argv[2])) return 1;
    bx_ntvdm_terminal_observation_v1_enable(1u);
    if (!bx_ntvdm_engine_run_v1(&request, &result) ||
        !bx_ntvdm_engine_result_v1_valid(&result) ||
        !terminal_valid(&result))
        return 2;
    printf("t200-s24 first terminal=%u detail=%u\n", result.terminal_kind,
        result.detail_code);
    if (!bx_ntvdm_terminal_observation_v1_copy(&observation) ||
        observation.captured != 1u || observation.event.vector != 6u ||
        observation.outcome.disposition != BX_NTVDM_GENERIC_UD_STOP)
        return 3;
    printf("t200-s25 stop cs=%04x eip=%08x bytes=%u %02x %02x %02x %02x\n",
        observation.event.cs, observation.event.eip,
        observation.event.window_bytes, observation.event.window[0],
        observation.event.window[1], observation.event.window[2],
        observation.event.window[3]);
    bx_ntvdm_terminal_observation_v1_enable(0u);
    /* A second direct installation is the release witness: the first call's
     * unconditional reset must leave no active process-local composition. */
    if (!bx_ntvdm_engine_run_v1(&request, &result) ||
        !bx_ntvdm_engine_result_v1_valid(&result) ||
        !terminal_valid(&result))
        return 4;
    printf("t200-s24 second terminal=%u detail=%u\n", result.terminal_kind,
        result.detail_code);
    return 0;
}
