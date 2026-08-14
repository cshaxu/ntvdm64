#include "bx_ntvdm_engine_contract_v1.h"
#include "bx_ntvdm_host_drive_policy.h"
#include "byob_launch_plan_v2.h"
#include "byob_profile.h"
#include "byob_target_selection.h"

#include <stdio.h>
#include <wchar.h>
#include <windows.h>

static int copied_text(uint16_t *out, uint32_t maximum, const wchar_t *text,
    uint32_t *out_chars)
{
    uint32_t index = 0u;
    if (!out || !text || !out_chars) return 0;
    while (text[index] != L'\0') {
        if (index >= maximum || text[index] > UINT16_MAX) return 0;
        out[index] = (uint16_t)text[index];
        ++index;
    }
    *out_chars = index;
    return index != 0u;
}

static int result_exit(const struct bx_ntvdm_engine_result_v1 *result)
{
    if (!result || !bx_ntvdm_engine_result_v1_valid(result)) return 1;
    if (result->terminal_kind == BX_NTVDM_ENGINE_TERMINAL_V1_CONTROLLED_GUEST_TERMINAL ||
        result->terminal_kind == BX_NTVDM_ENGINE_TERMINAL_V1_ORDINARY_GUEST_COMPLETION)
        return 0;
    if (result->terminal_kind == BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_REQUEST ||
        result->terminal_kind == BX_NTVDM_ENGINE_TERMINAL_V1_REJECTED_COMPOSITION)
        return 3;
    if (result->terminal_kind == BX_NTVDM_ENGINE_TERMINAL_V1_EXECUTION_BUDGET) return 4;
    return 1;
}

int wmain(int argc, wchar_t **argv)
{
    const wchar_t *profile = 0, *root = 0, *target;
    wchar_t target_full[MAX_PATH], launch_text[BYOB_LAUNCH_PLAN_V2_ENV_CHARS];
    uint32_t include_mask = 0u, exclude_mask = 0u;
    int has_include = 0, has_exclude = 0;
    int index = 1;
    byob_profile_selection selection;
    byob_launch_plan_v2 launch;
    struct bx_ntvdm_engine_request_v1 request;
    struct bx_ntvdm_engine_result_v1 result;

    if (argc < 6) goto usage;
    while (index < argc && wcsncmp(argv[index], L"--", 2u) == 0) {
        if (wcscmp(argv[index], L"--byob-profile") == 0 && index + 1 < argc && !profile)
            profile = argv[index + 1], index += 2;
        else if (wcscmp(argv[index], L"--byob-root") == 0 && index + 1 < argc && !root)
            root = argv[index + 1], index += 2;
        else if (wcscmp(argv[index], L"--include-drives") == 0 && index + 1 < argc &&
            !has_include && bx_ntvdm_host_drive_policy_v1_parse(argv[index + 1], &include_mask))
            has_include = 1, index += 2;
        else if (wcscmp(argv[index], L"--exclude-drives") == 0 && index + 1 < argc &&
            !has_exclude && bx_ntvdm_host_drive_policy_v1_parse(argv[index + 1], &exclude_mask))
            has_exclude = 1, index += 2;
        else goto usage;
    }
    if (!profile || !root || index >= argc) goto usage;
    target = argv[index];
    if (!GetFullPathNameW(target, MAX_PATH, target_full, 0) ||
        byob_profile_validate_file_select(profile, root, &selection) != BYOB_PROFILE_ACCEPTED ||
        !byob_target_selection_matches(root, &selection, target_full) ||
        !byob_launch_plan_v2_from_arguments(&launch, &selection, argc - index - 1, argv + index + 1) ||
        !byob_launch_plan_v2_to_environment(&launch, launch_text)) {
        fwprintf(stderr, L"ntdos64-native: BYOB admission failed\n"); return 3;
    }
    bx_ntvdm_engine_request_v1_clear(&request);
    if (!copied_text(request.profile_descriptor, BX_NTVDM_ENGINE_V1_MAX_DESCRIPTOR_CHARS,
            profile, &request.profile_descriptor_chars) ||
        !copied_text(request.root_descriptor, BX_NTVDM_ENGINE_V1_MAX_DESCRIPTOR_CHARS,
            root, &request.root_descriptor_chars) ||
        !copied_text(request.launch_descriptor, BX_NTVDM_ENGINE_V1_MAX_LAUNCH_CHARS,
            launch_text, &request.launch_descriptor_chars)) return 3;
    request.admitted_drive_mask = include_mask;
    request.excluded_drive_mask = exclude_mask;
    request.instruction_tick_budget = UINT64_C(1000000);
    if (!bx_ntvdm_engine_request_v1_valid(&request) || !bx_ntvdm_engine_run_v1(&request, &result) ||
        !bx_ntvdm_engine_result_v1_valid(&result)) return 1;
    wprintf(L"ntdos64-native: terminal=%u detail=%u\n", result.terminal_kind, result.detail_code);
    return result_exit(&result);
usage:
    fwprintf(stderr, L"usage: ntdos64-native --byob-profile profile.json --byob-root directory [--include-drives c,d] [--exclude-drives e] target [args...]\n");
    return 2;
}
