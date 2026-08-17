#include "bx_ntvdm_engine_contract_v1.h"
#include "bx_ntvdm_host_drive_policy.h"
#include "bx_ntvdm_bop_sequence_observation_v1.h"
#include "byob_launch_plan_v2.h"
#include "byob_profile.h"
#include "byob_target_selection.h"
#include "ntdos64_lifecycle_v1.h"
#include "ntdos64_console_cancellation_v1.h"
#include "ntdos64_engine_worker_v1.h"

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

static int parse_mutation_mode(const wchar_t *text, uint32_t *out_mode)
{
    if (text == 0 || out_mode == 0) return 0;
    if (_wcsicmp(text, L"direct") == 0) {
        *out_mode = BX_NTVDM_ENGINE_MUTATION_MODE_V1_DIRECT;
        return 1;
    }
    if (_wcsicmp(text, L"readonly") == 0) {
        *out_mode = BX_NTVDM_ENGINE_MUTATION_MODE_V1_READONLY;
        return 1;
    }
    return 0;
}

/* The engine/mantle contract already owns the finite tick watchdog. This
 * parser only selects its existing copied value; it has no guest, BOP or
 * machine-state meaning. */
static int parse_instruction_tick_budget(const wchar_t *text, uint64_t *out_budget)
{
    uint64_t value = 0u;
    uint32_t index;
    if (text == 0 || out_budget == 0 || text[0] == L'\0') return 0;
    for (index = 0u; text[index] != L'\0'; ++index) {
        uint32_t digit;
        if (text[index] < L'0' || text[index] > L'9') return 0;
        digit = (uint32_t)(text[index] - L'0');
        if (value > (UINT64_MAX - digit) / UINT64_C(10)) return 0;
        value = value * UINT64_C(10) + digit;
    }
    if (value == 0u) return 0;
    *out_budget = value;
    return 1;
}
static void print_bop_sequence(const struct bx_ntvdm_bop_sequence_observation_v1 *sequence)
{
    uint32_t index;
    if (sequence == 0) return;
    wprintf(L"ntdos64-native: bop-sequence count=%u overflow=%u\n",
        sequence->record_count, sequence->overflowed);
    for (index = 0u; index < sequence->record_count; ++index) {
        const struct bx_ntvdm_bop_sequence_observation_record_v1 *record =
            &sequence->records[index];
        wprintf(L"ntdos64-native: bop[%u] cs=%04x eip=%08x selector=%02x service=%02x has-service=%u disposition=%u\n",
            index, record->cs, record->eip, record->selector, record->service,
            record->has_service, record->disposition);
    }
}
static int result_exit(const struct ntdos64_lifecycle_v1_audit *audit)
{
    if (!audit || !ntdos64_lifecycle_v1_audit_valid(audit)) return 1;
    if (audit->presentation == NTDOS64_LIFECYCLE_V1_PRESENTATION_CONTROLLED_GUEST_TERMINAL ||
        audit->presentation == NTDOS64_LIFECYCLE_V1_PRESENTATION_ORDINARY_GUEST_COMPLETION)
        return 0;
    if (audit->presentation == NTDOS64_LIFECYCLE_V1_PRESENTATION_REJECTED_ENGINE_REQUEST ||
        audit->presentation == NTDOS64_LIFECYCLE_V1_PRESENTATION_REJECTED_COMPOSITION)
        return 3;
    if (audit->presentation == NTDOS64_LIFECYCLE_V1_PRESENTATION_EXECUTION_BUDGET) return 4;
    return 1;
}

int wmain(int argc, wchar_t **argv)
{
    const wchar_t *profile = 0, *root = 0, *target;
    wchar_t target_full[MAX_PATH], launch_text[BYOB_LAUNCH_PLAN_V2_ENV_CHARS];
    uint32_t include_mask = 0u, exclude_mask = 0u;
    int has_include = 0, has_exclude = 0, has_mutation_mode = 0, has_tick_budget = 0,
        has_bop_observation = 0;
    uint32_t mutation_mode = BX_NTVDM_ENGINE_MUTATION_MODE_V1_DIRECT;
    uint64_t instruction_tick_budget = UINT64_C(1000000);
    int validate_only = 0, observe_bop_sequence = 0;
    int index = 1;
    byob_profile_selection selection;
    byob_launch_plan_v2 launch;
    struct bx_ntvdm_engine_request_v1 request;
    struct bx_ntvdm_engine_result_v1 result;
    struct ntdos64_lifecycle_v1_policy lifecycle_policy;
    struct ntdos64_lifecycle_v1_audit lifecycle_audit;
    HANDLE cancellation_event;
    uint32_t cancellation_accepted = 0u;

    if (argc < 6) goto usage;
    while (index < argc && wcsncmp(argv[index], L"--", 2u) == 0) {
        if (wcscmp(argv[index], L"--byob-profile") == 0 && index + 1 < argc && !profile)
            profile = argv[index + 1], index += 2;
        else if (wcscmp(argv[index], L"--byob-root") == 0 && index + 1 < argc && !root)
            root = argv[index + 1], index += 2;
        else if (wcscmp(argv[index], L"--include-drives") == 0 && index + 1 < argc &&
            !has_include && argv[index + 1][0] != L'\0' &&
            bx_ntvdm_host_drive_policy_v1_parse(argv[index + 1], &include_mask))
            has_include = 1, index += 2;
        else if (wcscmp(argv[index], L"--exclude-drives") == 0 && index + 1 < argc &&
            !has_exclude && argv[index + 1][0] != L'\0' &&
            bx_ntvdm_host_drive_policy_v1_parse(argv[index + 1], &exclude_mask))
            has_exclude = 1, index += 2;
        else if (wcscmp(argv[index], L"--mutation-mode") == 0 && index + 1 < argc &&
            !has_mutation_mode && parse_mutation_mode(argv[index + 1], &mutation_mode))
            has_mutation_mode = 1, index += 2;
        else if (wcscmp(argv[index], L"--instruction-tick-budget") == 0 && index + 1 < argc &&
            !has_tick_budget && parse_instruction_tick_budget(argv[index + 1], &instruction_tick_budget))
            has_tick_budget = 1, index += 2;
        else if (wcscmp(argv[index], L"--observe-bop-sequence") == 0 && !has_bop_observation)
            has_bop_observation = 1, observe_bop_sequence = 1, ++index;
        else if (wcscmp(argv[index], L"--validate-only") == 0 && !validate_only)
            validate_only = 1, ++index;
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
    ntdos64_lifecycle_v1_policy_clear(&lifecycle_policy);
    lifecycle_policy.instruction_tick_budget = instruction_tick_budget;
    if (!ntdos64_lifecycle_v1_policy_valid(&lifecycle_policy)) return 3;
    bx_ntvdm_engine_request_v1_clear(&request);
    if (!copied_text(request.profile_descriptor, BX_NTVDM_ENGINE_V1_MAX_DESCRIPTOR_CHARS,
            profile, &request.profile_descriptor_chars) ||
        !copied_text(request.root_descriptor, BX_NTVDM_ENGINE_V1_MAX_DESCRIPTOR_CHARS,
            root, &request.root_descriptor_chars) ||
        !copied_text(request.launch_descriptor, BX_NTVDM_ENGINE_V1_MAX_LAUNCH_CHARS,
            launch_text, &request.launch_descriptor_chars)) return 3;
    request.admitted_drive_mask = include_mask;
    request.excluded_drive_mask = exclude_mask;
    request.mutation_mode = mutation_mode;
    request.instruction_tick_budget = lifecycle_policy.instruction_tick_budget;
    if (!bx_ntvdm_engine_request_v1_valid(&request)) return 3;
    if (validate_only) {
        wprintf(L"ntdos64-native: request include=%08x exclude=%08x mode=%u budget=%llu observe-bop-sequence=%u\n",
            request.admitted_drive_mask, request.excluded_drive_mask,
            request.mutation_mode, (unsigned long long)request.instruction_tick_budget,
            observe_bop_sequence ? 1u : 0u);
        return 0;
    }
    if (observe_bop_sequence) bx_ntvdm_bop_sequence_observation_v1_enable(1u);
    if (!ntdos64_console_cancellation_v1_begin(&cancellation_event)) {
        if (observe_bop_sequence) bx_ntvdm_bop_sequence_observation_v1_enable(0u);
        return 1;
    }
    if (!ntdos64_engine_worker_v1_run(&request, cancellation_event, &result,
            &cancellation_accepted)) {
        ntdos64_console_cancellation_v1_end();
        if (observe_bop_sequence) bx_ntvdm_bop_sequence_observation_v1_enable(0u);
        return 1;
    }
    ntdos64_console_cancellation_v1_end();
    if (observe_bop_sequence) {
        struct bx_ntvdm_bop_sequence_observation_v1 sequence;
        if (bx_ntvdm_bop_sequence_observation_v1_copy(&sequence))
            print_bop_sequence(&sequence);
        else
            wprintf(L"ntdos64-native: bop-sequence unavailable\n");
        bx_ntvdm_bop_sequence_observation_v1_enable(0u);
    }
    lifecycle_policy.cancellation_request = cancellation_accepted ?
        NTDOS64_LIFECYCLE_V1_CANCELLATION_REQUESTED :
        NTDOS64_LIFECYCLE_V1_CANCELLATION_NONE;
    if (!ntdos64_lifecycle_v1_classify(&lifecycle_policy, &result,
            &lifecycle_audit) || !ntdos64_lifecycle_v1_audit_valid(&lifecycle_audit)) return 1;
    wprintf(L"ntdos64-native: terminal=%u detail=%u lifecycle=%u presentation=%u cancellation=%u budget=%llu\n",
        result.terminal_kind, result.detail_code, lifecycle_audit.lifecycle_terminal,
        lifecycle_audit.presentation, lifecycle_audit.cancellation_request,
        (unsigned long long)request.instruction_tick_budget);
    return result_exit(&lifecycle_audit);
usage:
    fwprintf(stderr, L"usage: ntdos64-native --byob-profile profile.json --byob-root directory [--mutation-mode direct|readonly] [--instruction-tick-budget positive-decimal] [--observe-bop-sequence] [--include-drives c,d] [--exclude-drives e] [--validate-only] target [args...]\n");
    return 2;
}
