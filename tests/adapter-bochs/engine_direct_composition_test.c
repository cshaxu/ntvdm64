#include "engine_contract.h"
#include "machine_stage.h"

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

static int request_set(struct runtime_engine_request *request,
    const wchar_t *profile, const wchar_t *root)
{
    runtime_engine_request_clear(request);
    if (!descriptor_set(request->profile_descriptor,
            RUNTIME_ENGINE_MAX_DESCRIPTOR_CHARS, profile,
            &request->profile_descriptor_chars) ||
        !descriptor_set(request->root_descriptor,
            RUNTIME_ENGINE_MAX_DESCRIPTOR_CHARS, root,
            &request->root_descriptor_chars) ||
        !descriptor_set(request->launch_descriptor,
            RUNTIME_ENGINE_MAX_LAUNCH_CHARS, L"2,1,e,00",
            &request->launch_descriptor_chars)) return 0;
    request->instruction_tick_budget = UINT64_C(1000000);
    return runtime_engine_request_valid(request);
}

static int terminal_valid(const struct runtime_engine_result *result)
{
    if (result->terminal_kind == RUNTIME_ENGINE_TERMINAL_EXECUTION_BUDGET)
        return result->detail_code == RUNTIME_MACHINE_STAGE_EXECUTION_BUDGET;
    if (result->terminal_kind == RUNTIME_ENGINE_TERMINAL_CONTROLLED_GUEST_TERMINAL)
        return result->detail_code ==
            RUNTIME_MACHINE_STAGE_EXECUTION_CONTROLLED_STOP;
    return 0;
}

int wmain(int argc, wchar_t **argv)
{
    struct runtime_engine_request request;
    struct runtime_engine_result result;
    if (argc != 3 || !request_set(&request, argv[1], argv[2])) return 1;
    if (!runtime_engine_run(&request, &result) ||
        !runtime_engine_result_valid(&result) ||
        !terminal_valid(&result))
        return 2;
    printf("t200-s24 first terminal=%u detail=%u\n", result.terminal_kind,
        result.detail_code);
    /* A second direct installation is the release witness: the first call's
     * unconditional reset must leave no active process-local composition. */
    if (!runtime_engine_run(&request, &result) ||
        !runtime_engine_result_valid(&result) ||
        !terminal_valid(&result))
        return 5;
    printf("t200-s24 second terminal=%u detail=%u\n", result.terminal_kind,
        result.detail_code);
    return 0;
}
