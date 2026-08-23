#include "bx_ntvdm_engine_contract_v1.h"
#include "bop/observation/bx_ntvdm_bop_sequence_observation_v1.h"
#include "bop/observation/bx_ntvdm_generic_ud_sequence_observation_v1.h"
#include "bx_ntvdm_first_fault_observation_v1.h"
#include "bx_ntvdm_machine_stage_v1.h"
#include "bx_ntvdm_software_interrupt_observation_v1.h"
#include "bx_ntvdm_interrupt_return_observation_v1.h"
#include "bx_ntvdm_segment_access_observation_v1.h"
#include "byob_launch_plan_v2.h"
#include "byob_target_selection.h"
#include "ntdos64_config.h"
#include "ntdos64_lifecycle_v1.h"
#include "ntdos64_console_cancellation_v1.h"
#include "ntdos64_engine_worker_v1.h"

#include <stdio.h>
#include <wchar.h>
#include <windows.h>

#ifndef BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY
#define BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY 0
#endif

#ifndef BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
#define BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE 0
#endif

#ifndef BX_NTVDM_ENABLE_MANTLE_SOFTWARE_INTERRUPT_OBSERVATION
#define BX_NTVDM_ENABLE_MANTLE_SOFTWARE_INTERRUPT_OBSERVATION 0
#endif

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
        wprintf(L"ntdos64-native: bop[%u] cs=%04x eip=%08x selector=%02x service=%02x has-service=%u disposition=%u eax=%08x ebx=%08x ecx=%08x edx=%08x esi=%08x edi=%08x flags=%08x ds=%04x es=%04x ss=%04x\n",
            index, record->cs, record->eip, record->selector, record->service,
            record->has_service, record->disposition, record->eax, record->ebx,
            record->ecx, record->edx, record->esi, record->edi, record->eflags,
            record->ds, record->es, record->ss);
    }
}
static void print_generic_ud_sequence(const struct bx_ntvdm_generic_ud_sequence_observation_v1 *sequence)
{
    uint32_t index, byte_index;
    if (sequence == 0) return;
    wprintf(L"ntdos64-native: ud-sequence count=%u overflow=%u\n",
        sequence->record_count, sequence->overflowed);
    for (index = 0u; index < sequence->record_count; ++index) {
        const struct bx_ntvdm_generic_ud_sequence_observation_record_v1 *record =
            &sequence->records[index];
        wprintf(L"ntdos64-native: ud[%u] cs=%04x eip=%08x mode=%u disposition=%u window=",
            index, record->cs, record->eip, record->execution_mode,
            record->disposition);
        for (byte_index = 0u; byte_index < record->window_bytes; ++byte_index)
            wprintf(L"%02x", record->window[byte_index]);
        wprintf(L"\n");
    }
}
#if BX_NTVDM_ENABLE_MANTLE_SOFTWARE_INTERRUPT_OBSERVATION
static void print_software_interrupt_observation(void)
{
    struct bx_ntvdm_software_interrupt_observation_v1_record record;
    uint32_t index;
    uint32_t count = bx_ntvdm_mantle_software_interrupt_observation_v1_count();
    wprintf(L"ntdos64-native: software-interrupts count=%u\n", count);
    for (index = 0u; index < count; ++index) {
        if (bx_ntvdm_mantle_software_interrupt_observation_v1_get(index, &record))
            wprintf(L"ntdos64-native: software-interrupt[%u] sequence=%llu vector=%02x mode=%u cs=%04x eip=%08llx flags=%08x ax=%04x bx=%04x cx=%04x dx=%04x ds=%04x es=%04x ss=%04x sp=%04x\n",
                index, (unsigned long long)record.sequence, record.vector,
                record.execution_mode, record.cs, (unsigned long long)record.rip,
                record.eflags, record.ax, record.bx, record.cx, record.dx,
                record.ds, record.es, record.ss, record.sp);
    }
}
#endif
#if BX_NTVDM_ENABLE_MANTLE_INTERRUPT_RETURN_OBSERVATION
static void print_interrupt_return_observation(void)
{
    struct bx_ntvdm_interrupt_return_observation_v1_record record;
    uint32_t index;
    uint32_t count = bx_ntvdm_mantle_interrupt_return_observation_v1_count();
    wprintf(L"ntdos64-native: interrupt-returns count=%u\n", count);
    for (index = 0u; index < count; ++index) {
        if (bx_ntvdm_mantle_interrupt_return_observation_v1_get(index, &record))
            wprintf(L"ntdos64-native: interrupt-return[%u] sequence=%llu mode=%u width=%u cs=%04x eip=%08llx flags=%08x ax=%04x bx=%04x cx=%04x dx=%04x ds=%04x es=%04x ss=%04x sp=%08x\n", index, (unsigned long long)record.sequence, record.execution_mode, record.operand_width, record.cs, (unsigned long long)record.rip, record.eflags, record.ax, record.bx, record.cx, record.dx, record.ds, record.es, record.ss, record.sp);
    }
}
#endif
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
    const wchar_t *config = 0, *root = 0, *target;
    wchar_t target_full[MAX_PATH], launch_text[BYOB_LAUNCH_PLAN_V2_ENV_CHARS];
    int has_mutation_mode = 0, has_tick_budget = 0,
        has_bop_observation = 0, has_generic_ud_observation = 0, has_first_fault_observation = 0, has_budget_terminal_position_observation = 0
#if BX_NTVDM_ENABLE_MANTLE_SOFTWARE_INTERRUPT_OBSERVATION
        , has_software_interrupt_observation = 0
#endif
#if BX_NTVDM_ENABLE_MANTLE_INTERRUPT_RETURN_OBSERVATION
        , has_interrupt_return_observation = 0
#endif
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY
        , has_terminal_history_observation = 0, has_terminal_cs_transitions_observation = 0
#endif
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
        , has_terminal_provenance_observation = 0
#endif
        ;
    uint32_t mutation_mode = BX_NTVDM_ENGINE_MUTATION_MODE_V1_DIRECT;
    uint64_t instruction_tick_budget = UINT64_C(1000000);
    int validate_only = 0, observe_bop_sequence = 0, observe_generic_ud_sequence = 0, observe_first_fault = 0, observe_budget_terminal_position = 0
#if BX_NTVDM_ENABLE_MANTLE_SOFTWARE_INTERRUPT_OBSERVATION
        , observe_software_interrupts = 0
#endif
#if BX_NTVDM_ENABLE_MANTLE_INTERRUPT_RETURN_OBSERVATION
        , observe_interrupt_returns = 0
#endif
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY
        , observe_terminal_history = 0, observe_terminal_cs_transitions = 0
#endif
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
        , observe_terminal_provenance = 0
#endif
        ;
    int index = 1;
    ntdos64_startup_selection selection;
    wchar_t config_source[MAX_PATH], autoexec_source[MAX_PATH];
    byob_launch_plan_v2 launch;
    struct bx_ntvdm_engine_request_v1 request;
    struct bx_ntvdm_engine_result_v1 result;
    struct ntdos64_lifecycle_v1_policy lifecycle_policy;
    struct ntdos64_lifecycle_v1_audit lifecycle_audit;
    HANDLE cancellation_event;
    uint32_t cancellation_accepted = 0u;

    if (argc < 6) goto usage;
    while (index < argc && wcsncmp(argv[index], L"--", 2u) == 0) {
        if (wcscmp(argv[index], L"--dos-root") == 0 && index + 1 < argc && !config)
            config = argv[index + 1], index += 2;
        else if (wcscmp(argv[index], L"--wow16-root") == 0 && index + 1 < argc && !root)
            root = argv[index + 1], index += 2;
        else if (wcscmp(argv[index], L"--mutation-mode") == 0 && index + 1 < argc &&
            !has_mutation_mode && parse_mutation_mode(argv[index + 1], &mutation_mode))
            has_mutation_mode = 1, index += 2;
        else if (wcscmp(argv[index], L"--instruction-tick-budget") == 0 && index + 1 < argc &&
            !has_tick_budget && parse_instruction_tick_budget(argv[index + 1], &instruction_tick_budget))
            has_tick_budget = 1, index += 2;
        else if (wcscmp(argv[index], L"--observe-ud-sequence") == 0 && !has_generic_ud_observation)
            has_generic_ud_observation = 1, observe_generic_ud_sequence = 1, ++index;
        else if (wcscmp(argv[index], L"--observe-bop-sequence") == 0 && !has_bop_observation)
            has_bop_observation = 1, observe_bop_sequence = 1, ++index;
        else if (wcscmp(argv[index], L"--observe-budget-terminal-position") == 0 && !has_budget_terminal_position_observation)
            has_budget_terminal_position_observation = 1, observe_budget_terminal_position = 1, ++index;
#if BX_NTVDM_ENABLE_MANTLE_SOFTWARE_INTERRUPT_OBSERVATION
        else if (wcscmp(argv[index], L"--observe-software-interrupts") == 0 && !has_software_interrupt_observation)
            has_software_interrupt_observation = 1, observe_software_interrupts = 1, ++index;
#endif
#if BX_NTVDM_ENABLE_MANTLE_INTERRUPT_RETURN_OBSERVATION
        else if (wcscmp(argv[index], L"--observe-interrupt-returns") == 0 && !has_interrupt_return_observation)
            has_interrupt_return_observation = 1, observe_interrupt_returns = 1, ++index;
#endif
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY
        else if (wcscmp(argv[index], L"--observe-budget-terminal-history") == 0 && !has_terminal_history_observation)
            has_terminal_history_observation = 1, observe_terminal_history = 1, ++index;
        else if (wcscmp(argv[index], L"--observe-budget-terminal-cs-transitions") == 0 && !has_terminal_cs_transitions_observation)
            has_terminal_cs_transitions_observation = 1, observe_terminal_cs_transitions = 1, ++index;
#endif
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
        else if (wcscmp(argv[index], L"--observe-budget-terminal-provenance") == 0 && !has_terminal_provenance_observation)
            has_terminal_provenance_observation = 1, observe_terminal_provenance = 1, ++index;
#endif
        else if (wcscmp(argv[index], L"--observe-first-fault") == 0 && !has_first_fault_observation)
            has_first_fault_observation = 1, observe_first_fault = 1, ++index;
        else if (wcscmp(argv[index], L"--validate-only") == 0 && !validate_only)
            validate_only = 1, ++index;
        else goto usage;
    }
    if (!config || !root || index >= argc) goto usage;
    target = argv[index];
    if (!GetFullPathNameW(target, MAX_PATH, target_full, 0) ||
        !ntdos64_bundle_load_roots(config, config_source, autoexec_source) ||
        (memset(&selection, 0, sizeof(selection)), 0) ||
        wcslen(wcsrchr(target_full, L'\\') != NULL ? wcsrchr(target_full, L'\\') + 1u : target_full) >=
            sizeof(selection.target.file_name) / sizeof(selection.target.file_name[0]) ||
        swprintf(selection.target.file_name,
            sizeof(selection.target.file_name) / sizeof(selection.target.file_name[0]), L"%ls",
            wcsrchr(target_full, L'\\') != NULL ? wcsrchr(target_full, L'\\') + 1u : target_full) < 0 ||
        (selection.declared_target_count = 1u, selection.has_target_placement = 1u,
         selection.target_placement.drive_index = 2u,
         selection.declared_targets[0].component = selection.target,
         selection.declared_targets[0].placement = selection.target_placement, 0) ||
        !byob_launch_plan_v2_from_arguments(&launch, &selection, argc - index - 1, argv + index + 1) ||
        !byob_launch_plan_v2_to_environment(&launch, launch_text)) {
        fwprintf(stderr, L"ntdos64-native: sibling dos/wow16 bundle admission failed\n"); return 3;
    }
    if (!SetEnvironmentVariableW(L"NTVDM_CONFIG_SOURCE", config_source) ||
        !SetEnvironmentVariableW(L"NTVDM_AUTOEXEC_SOURCE", autoexec_source) ||
        !SetEnvironmentVariableW(L"NTVDM_CONFIG_ROOT", config) ||
        !SetEnvironmentVariableW(L"NTVDM_TARGET_PATH", target_full) ||
        !SetEnvironmentVariableW(L"NTVDM_WOW16_ROOT", root)) return 3;
    ntdos64_lifecycle_v1_policy_clear(&lifecycle_policy);
    lifecycle_policy.instruction_tick_budget = instruction_tick_budget;
    if (!ntdos64_lifecycle_v1_policy_valid(&lifecycle_policy)) return 3;
    bx_ntvdm_engine_request_v1_clear(&request);
    if (!copied_text(request.profile_descriptor, BX_NTVDM_ENGINE_V1_MAX_DESCRIPTOR_CHARS,
            target_full, &request.profile_descriptor_chars) ||
        !copied_text(request.root_descriptor, BX_NTVDM_ENGINE_V1_MAX_DESCRIPTOR_CHARS,
            config, &request.root_descriptor_chars) ||
        !copied_text(request.launch_descriptor, BX_NTVDM_ENGINE_V1_MAX_LAUNCH_CHARS,
            launch_text, &request.launch_descriptor_chars)) return 3;
    /* OpenNT parity: host-drive enumeration is not a CLI capability filter.
     * The retained request fields are zero, which means no added exclusion. */
    request.admitted_drive_mask = 0u;
    request.excluded_drive_mask = 0u;
    request.mutation_mode = mutation_mode;
    request.instruction_tick_budget = lifecycle_policy.instruction_tick_budget;
    if (!bx_ntvdm_engine_request_v1_valid(&request)) return 3;
    if (validate_only) {
        wprintf(L"ntdos64-native: request include=%08x exclude=%08x mode=%u budget=%llu observe-bop-sequence=%u observe-ud-sequence=%u observe-first-fault=%u observe-budget-terminal-position=%u\n",
            request.admitted_drive_mask, request.excluded_drive_mask,
            request.mutation_mode, (unsigned long long)request.instruction_tick_budget,
            observe_bop_sequence ? 1u : 0u, observe_generic_ud_sequence ? 1u : 0u,
            observe_first_fault ? 1u : 0u, observe_budget_terminal_position ? 1u : 0u);
        return 0;
    }
    if (observe_bop_sequence) bx_ntvdm_bop_sequence_observation_v1_enable(1u);
    if (observe_generic_ud_sequence) bx_ntvdm_generic_ud_sequence_observation_v1_enable(1u);
    if (observe_budget_terminal_position) bx_ntvdm_machine_stage_v1_terminal_position_observation_enable(1u);
#if BX_NTVDM_ENABLE_MANTLE_SOFTWARE_INTERRUPT_OBSERVATION
    if (observe_software_interrupts && !bx_ntvdm_mantle_software_interrupt_observation_v1_configure(BX_NTVDM_SOFTWARE_INTERRUPT_OBSERVATION_V1_CAPACITY_MAX)) return 1;
#endif
#if BX_NTVDM_ENABLE_MANTLE_INTERRUPT_RETURN_OBSERVATION
    if (observe_interrupt_returns && !bx_ntvdm_mantle_interrupt_return_observation_v1_configure(BX_NTVDM_INTERRUPT_RETURN_OBSERVATION_V1_CAPACITY_MAX)) return 1;
#endif
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY
    if (observe_terminal_history) bx_ntvdm_machine_stage_v1_terminal_history_observation_enable(1u);
    if (observe_terminal_cs_transitions) bx_ntvdm_machine_stage_v1_terminal_cs_transitions_observation_enable(1u);
#endif
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
    if (observe_terminal_provenance) bx_ntvdm_machine_stage_v1_terminal_provenance_observation_enable(1u);
    if (observe_terminal_provenance) bx_ntvdm_machine_stage_v1_terminal_cs_provenance_observation_enable(1u);
#endif
    if (observe_first_fault) {
        bx_ntvdm_mantle_first_fault_observation_enable(1);
        bx_ntvdm_mantle_segment_access_observation_enable(1);
    }
    if (!ntdos64_console_cancellation_v1_begin(&cancellation_event)) {
        if (observe_bop_sequence) bx_ntvdm_bop_sequence_observation_v1_enable(0u);
        if (observe_generic_ud_sequence) bx_ntvdm_generic_ud_sequence_observation_v1_enable(0u);
        if (observe_budget_terminal_position) bx_ntvdm_machine_stage_v1_terminal_position_observation_enable(0u);
#if BX_NTVDM_ENABLE_MANTLE_SOFTWARE_INTERRUPT_OBSERVATION
        if (observe_software_interrupts) bx_ntvdm_mantle_software_interrupt_observation_v1_configure(0u);
#endif
#if BX_NTVDM_ENABLE_MANTLE_INTERRUPT_RETURN_OBSERVATION
        if (observe_interrupt_returns) bx_ntvdm_mantle_interrupt_return_observation_v1_configure(0u);
#endif
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY
        if (observe_terminal_history) bx_ntvdm_machine_stage_v1_terminal_history_observation_enable(0u);
        if (observe_terminal_cs_transitions) bx_ntvdm_machine_stage_v1_terminal_cs_transitions_observation_enable(0u);
#endif
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
        if (observe_terminal_provenance) bx_ntvdm_machine_stage_v1_terminal_provenance_observation_enable(0u);
        if (observe_terminal_provenance) bx_ntvdm_machine_stage_v1_terminal_cs_provenance_observation_enable(0u);
#endif
        if (observe_first_fault) {
            bx_ntvdm_mantle_segment_access_observation_enable(0);
            bx_ntvdm_mantle_first_fault_observation_enable(0);
        }
        return 1;
    }
    if (!ntdos64_engine_worker_v1_run(&request, cancellation_event, &result,
            &cancellation_accepted)) {
        ntdos64_console_cancellation_v1_end();
        if (observe_bop_sequence) bx_ntvdm_bop_sequence_observation_v1_enable(0u);
        if (observe_generic_ud_sequence) bx_ntvdm_generic_ud_sequence_observation_v1_enable(0u);
        if (observe_budget_terminal_position) bx_ntvdm_machine_stage_v1_terminal_position_observation_enable(0u);
#if BX_NTVDM_ENABLE_MANTLE_SOFTWARE_INTERRUPT_OBSERVATION
        if (observe_software_interrupts) bx_ntvdm_mantle_software_interrupt_observation_v1_configure(0u);
#endif
#if BX_NTVDM_ENABLE_MANTLE_INTERRUPT_RETURN_OBSERVATION
        if (observe_interrupt_returns) bx_ntvdm_mantle_interrupt_return_observation_v1_configure(0u);
#endif
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY
        if (observe_terminal_history) bx_ntvdm_machine_stage_v1_terminal_history_observation_enable(0u);
        if (observe_terminal_cs_transitions) bx_ntvdm_machine_stage_v1_terminal_cs_transitions_observation_enable(0u);
#endif
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
        if (observe_terminal_provenance) bx_ntvdm_machine_stage_v1_terminal_provenance_observation_enable(0u);
        if (observe_terminal_provenance) bx_ntvdm_machine_stage_v1_terminal_cs_provenance_observation_enable(0u);
#endif
        if (observe_first_fault) {
            bx_ntvdm_mantle_segment_access_observation_enable(0);
            bx_ntvdm_mantle_first_fault_observation_enable(0);
        }
        return 1;
    }
    ntdos64_console_cancellation_v1_end();
#if BX_NTVDM_ENABLE_MANTLE_SOFTWARE_INTERRUPT_OBSERVATION
    if (observe_software_interrupts) {
        print_software_interrupt_observation();
        bx_ntvdm_mantle_software_interrupt_observation_v1_configure(0u);
    }
#endif
#if BX_NTVDM_ENABLE_MANTLE_INTERRUPT_RETURN_OBSERVATION
    if (observe_interrupt_returns) {
        print_interrupt_return_observation();
        bx_ntvdm_mantle_interrupt_return_observation_v1_configure(0u);
    }
#endif
    if (observe_bop_sequence) {
        struct bx_ntvdm_bop_sequence_observation_v1 sequence;
        if (bx_ntvdm_bop_sequence_observation_v1_copy(&sequence))
            print_bop_sequence(&sequence);
        else
            wprintf(L"ntdos64-native: bop-sequence unavailable\n");
        bx_ntvdm_bop_sequence_observation_v1_enable(0u);
    }
    if (observe_first_fault) {
        struct bx_ntvdm_first_fault_observation_v1 first_fault;
        struct bx_ntvdm_segment_access_observation_v1 segment_access;
        if (bx_ntvdm_mantle_first_fault_observation_copy(&first_fault))
            wprintf(L"ntdos64-native: first-fault vector=%u error=%u cs=%04x eip=%08x eax=%08x eflags=%08x mode=%u\n",
                first_fault.vector, first_fault.error_code, first_fault.cs, first_fault.eip,
                first_fault.eax, first_fault.eflags, first_fault.execution_mode);
        else
            wprintf(L"ntdos64-native: first-fault unavailable\n");
        if (bx_ntvdm_mantle_segment_access_observation_copy(&segment_access))
            wprintf(L"ntdos64-native: segment-access kind=%u branch=%u seg-index=%u selector=%04x offset=%08x width=%u limit=%08x valid=%08x\n", segment_access.access_kind, segment_access.branch_kind, segment_access.segment_index, segment_access.segment_selector, segment_access.offset, segment_access.width, segment_access.limit_scaled, segment_access.cache_valid);
        else
            wprintf(L"ntdos64-native: segment-access unavailable\n");
        bx_ntvdm_mantle_segment_access_observation_enable(0);
        bx_ntvdm_mantle_first_fault_observation_enable(0);
    }
    if (observe_budget_terminal_position) {
        struct bx_ntvdm_machine_stage_v1_terminal_position position;
        if (bx_ntvdm_machine_stage_v1_terminal_position_observation_copy(&position))
            wprintf(L"ntdos64-native: budget-terminal-position cs=%04x eip=%08x\n", position.cs, position.eip);
        else wprintf(L"ntdos64-native: budget-terminal-position unavailable\n");
        bx_ntvdm_machine_stage_v1_terminal_position_observation_enable(0u);
    }
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY
    if (observe_terminal_history) {
        struct bx_ntvdm_machine_stage_v1_terminal_history history;
        uint32_t history_index;
        if (bx_ntvdm_machine_stage_v1_terminal_history_observation_copy(&history)) {
            wprintf(L"ntdos64-native: budget-terminal-history count=%u\n", history.count);
            for (history_index = 0u; history_index < history.count; ++history_index)
                wprintf(L"ntdos64-native: history[%u] sequence=%llu cs=%04x eip=%016llx ss=%04x sp=%04x\n", history_index, (unsigned long long)history.records[history_index].sequence, history.records[history_index].cs, (unsigned long long)history.records[history_index].rip, history.records[history_index].ss, history.records[history_index].sp);
        } else wprintf(L"ntdos64-native: budget-terminal-history unavailable\n");
        bx_ntvdm_machine_stage_v1_terminal_history_observation_enable(0u);
    }
    if (observe_terminal_cs_transitions) {
        struct bx_ntvdm_machine_stage_v1_terminal_cs_transitions transitions;
        uint32_t transition_index;
        if (bx_ntvdm_machine_stage_v1_terminal_cs_transitions_observation_copy(&transitions)) {
            wprintf(L"ntdos64-native: budget-terminal-cs-transitions count=%u\n", transitions.value.count);
            for (transition_index = 0u; transition_index < transitions.value.count; ++transition_index) {
                const struct bx_ntvdm_instruction_history_transition_v1 *transition =
                    &transitions.value.transitions[transition_index];
                wprintf(L"ntdos64-native: cs-transition[%u] previous=%04x:%08x ss=%04x sp=%04x bp=%04x sequence=%llu current=%04x:%08x ss=%04x sp=%04x bp=%04x sequence=%llu\n", transition_index,
                    transition->previous.cs, (unsigned)transition->previous.rip,
                    transition->previous.ss, transition->previous.sp, transition->previous.bp,
                    (unsigned long long)transition->previous.sequence,
                    transition->current.cs, (unsigned)transition->current.rip,
                    transition->current.ss, transition->current.sp, transition->current.bp,
                    (unsigned long long)transition->current.sequence);
            }
        } else wprintf(L"ntdos64-native: budget-terminal-cs-transitions unavailable\n");
        bx_ntvdm_machine_stage_v1_terminal_cs_transitions_observation_enable(0u);
    }
#endif
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
    if (observe_terminal_provenance) {
        struct bx_ntvdm_machine_stage_v1_terminal_provenance provenance;
        uint32_t provenance_index;
        if (bx_ntvdm_machine_stage_v1_terminal_provenance_observation_copy(&provenance)) {
            wprintf(L"ntdos64-native: budget-terminal-provenance cs=%04x eip=%08x ss=%04x sp=%04x instruction-valid=%u stack-valid=%u instruction=", provenance.cs, provenance.eip, provenance.ss, provenance.sp, provenance.instruction_valid, provenance.stack_valid);
            for (provenance_index = 0u; provenance_index < BX_NTVDM_INSTRUCTION_HISTORY_V1_PREDECESSOR_BYTES; ++provenance_index) wprintf(L"%02x", provenance.instruction_bytes[provenance_index]);
            wprintf(L" stack=");
            for (provenance_index = 0u; provenance_index < BX_NTVDM_INSTRUCTION_HISTORY_V1_STACK_BYTES; ++provenance_index) wprintf(L"%02x", provenance.stack_bytes[provenance_index]);
            wprintf(L"\n");
        } else wprintf(L"ntdos64-native: budget-terminal-provenance unavailable\n");
        {
            struct bx_ntvdm_machine_stage_v1_terminal_cs_provenance transition;
            if (bx_ntvdm_machine_stage_v1_terminal_cs_provenance_observation_copy(&transition)) {
                wprintf(L"ntdos64-native: budget-terminal-cs-transition previous=%04x:%08x bp=%04x current=%04x:%08x bp=%04x predecessor-valid=%u successor-valid=%u stack-valid=%u predecessor=", transition.value.transition.previous.cs, (unsigned)transition.value.transition.previous.rip, transition.value.transition.previous.bp, transition.value.transition.current.cs, (unsigned)transition.value.transition.current.rip, transition.value.transition.current.bp, transition.value.predecessor_valid, transition.value.successor_valid, transition.value.stack_valid);
                for (provenance_index = 0u; provenance_index < BX_NTVDM_INSTRUCTION_HISTORY_V1_PREDECESSOR_BYTES; ++provenance_index) wprintf(L"%02x", transition.value.predecessor_bytes[provenance_index]);
                wprintf(L" successor=");
                for (provenance_index = 0u; provenance_index < BX_NTVDM_INSTRUCTION_HISTORY_V1_SUCCESSOR_BYTES; ++provenance_index) wprintf(L"%02x", transition.value.successor_bytes[provenance_index]);
                wprintf(L" stack=");
                for (provenance_index = 0u; provenance_index < BX_NTVDM_INSTRUCTION_HISTORY_V1_STACK_BYTES; ++provenance_index) wprintf(L"%02x", transition.value.stack_bytes[provenance_index]);
                wprintf(L"\n");
            } else wprintf(L"ntdos64-native: budget-terminal-cs-transition unavailable\n");
            bx_ntvdm_machine_stage_v1_terminal_cs_provenance_observation_enable(0u);
        }
        bx_ntvdm_machine_stage_v1_terminal_provenance_observation_enable(0u);
    }
#endif
    if (observe_generic_ud_sequence) {
        struct bx_ntvdm_generic_ud_sequence_observation_v1 sequence;
        if (bx_ntvdm_generic_ud_sequence_observation_v1_copy(&sequence))
            print_generic_ud_sequence(&sequence);
        else
            wprintf(L"ntdos64-native: ud-sequence unavailable\n");
        bx_ntvdm_generic_ud_sequence_observation_v1_enable(0u);
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
    fwprintf(stderr, L"usage: ntdos64-native --dos-root directory --wow16-root directory [--mutation-mode direct|readonly] [--instruction-tick-budget positive-decimal] [--observe-bop-sequence] [--observe-ud-sequence] [--observe-first-fault] [--observe-budget-terminal-position]"
#if BX_NTVDM_ENABLE_MANTLE_SOFTWARE_INTERRUPT_OBSERVATION
        L" [--observe-software-interrupts]"
#endif
#if BX_NTVDM_ENABLE_MANTLE_INTERRUPT_RETURN_OBSERVATION
        L" [--observe-interrupt-returns]"
#endif
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY
        L" [--observe-budget-terminal-history] [--observe-budget-terminal-cs-transitions]"
#endif
#if BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
        L" [--observe-budget-terminal-provenance]"
#endif
        L" [--validate-only] target [args...]\n");
    return 2;
}
