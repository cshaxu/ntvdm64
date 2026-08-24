#include "engine_contract.h"
#include "opennt-bop/observation/bx_ntvdm_bop_sequence_observation_v1.h"
#include "opennt-bop/observation/bx_ntvdm_command_bootstrap_observation_v1.h"
#include "opennt-bop/observation/bx_ntvdm_command_current_dir_observation_v1.h"
#include "opennt-bop/observation/bx_ntvdm_dem_open_observation_v1.h"
#include "opennt-bop/observation/bx_ntvdm_demfile_create_observation_v1.h"
#include "opennt-bop/observation/bx_ntvdm_ntdos_exec_entry_observation_v1.h"
#include "opennt-bop/observation/bx_ntvdm_generic_ud_sequence_observation_v1.h"
#include "first_fault_observation.h"
#include "machine_stage.h"
#include "software_interrupt_observation.h"
#include "interrupt_return_observation.h"
#include "physical_write_observation.h"
#include "segment_access_observation.h"
#include "byob_launch_plan.h"
#include "byob_target_selection.h"
#include "config.h"
#include "dos_safe_alias.h"
#include "lifecycle.h"
#include "console_cancellation.h"
#include "engine_worker.h"

#include <stdio.h>
#include <wchar.h>
#include <windows.h>

#ifndef RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY
#define RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY 0
#endif

#ifndef RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
#define RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE 0
#endif

#ifndef RUNTIME_ENABLE_MANTLE_SOFTWARE_INTERRUPT_OBSERVATION
#define RUNTIME_ENABLE_MANTLE_SOFTWARE_INTERRUPT_OBSERVATION 0
#endif

#ifndef RUNTIME_ENABLE_MANTLE_PHYSICAL_WRITE_OBSERVATION
#define RUNTIME_ENABLE_MANTLE_PHYSICAL_WRITE_OBSERVATION 0
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
        *out_mode = RUNTIME_ENGINE_MUTATION_MODE_V1_DIRECT;
        return 1;
    }
    if (_wcsicmp(text, L"readonly") == 0) {
        *out_mode = RUNTIME_ENGINE_MUTATION_MODE_V1_READONLY;
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

static int parse_guest_memory_kib(const wchar_t *text, uint64_t *out_bytes)
{
    uint64_t kib;
    if (!parse_instruction_tick_budget(text, &kib) || kib > UINT64_MAX / 1024u)
        return 0;
    *out_bytes = kib * 1024u;
    return *out_bytes >= UINT64_C(0x100000) &&
        *out_bytes <= UINT64_C(0x1000000) &&
        *out_bytes % UINT64_C(0x10000) == 0u;
}

/* A reservation is a copied machine-admission interval, not an allocation
 * request or a provider policy.  Its final containment check remains in the
 * engine contract because that contract owns the selected RAM capacity. */
static int parse_reserved_memory_kib(const wchar_t *text, uint64_t *out_bytes)
{
    uint64_t kib;
    if (!parse_instruction_tick_budget(text, &kib) || kib > UINT64_MAX / 1024u)
        return 0;
    *out_bytes = kib * 1024u;
    return *out_bytes != 0u && *out_bytes % UINT64_C(0x10000) == 0u;
}
static void print_bop_sequence(const struct runtime_bop_sequence_observation_v1 *sequence)
{
    uint32_t index;
    if (sequence == 0) return;
    wprintf(L"ntdos64-native: bop-sequence count=%u overflow=%u\n",
        sequence->record_count, sequence->overflowed);
    for (index = 0u; index < sequence->record_count; ++index) {
        const struct runtime_bop_sequence_observation_record_v1 *record =
            &sequence->records[index];
        wprintf(L"ntdos64-native: bop[%u] cs=%04x eip=%08x selector=%02x service=%02x has-service=%u disposition=%u eax=%08x ebx=%08x ecx=%08x edx=%08x esi=%08x edi=%08x flags=%08x ds=%04x es=%04x ss=%04x\n",
            index, record->cs, record->eip, record->selector, record->service,
            record->has_service, record->disposition, record->eax, record->ebx,
            record->ecx, record->edx, record->esi, record->edi, record->eflags,
            record->ds, record->es, record->ss);
    }
}
static void print_command_bootstrap_observation(
    const struct runtime_command_bootstrap_observation_v1 *observation)
{
    uint32_t index;
    if (observation == 0) return;
    wprintf(L"ntdos64-native: command-bootstrap observed=%u read-failed=%u cs=%04x eip=%08x physical=%08x bytes=",
        observation->observed, observation->read_failed, observation->cs,
        observation->eip, observation->physical_address);
    for (index = 0u; index < RUNTIME_COMMAND_BOOTSTRAP_OBSERVATION_V1_BYTES;
        ++index) wprintf(L"%02x", observation->command[index]);
    wprintf(L"\n");
}
static void print_command_current_dir_observation(
    const struct runtime_command_current_dir_observation_v1 *observation)
{
    uint32_t index;
    if (observation == 0) return;
    wprintf(L"ntdos64-native: command-current-dir observed=%u read-failed=%u ds=%04x esi=%08x physical=%08x bytes=",
        observation->observed, observation->read_failed, observation->ds,
        observation->esi, observation->physical_address);
    for (index = 0u; index < RUNTIME_COMMAND_CURRENT_DIR_OBSERVATION_V1_BYTES;
        ++index) wprintf(L"%02x", observation->directory[index]);
    wprintf(L"\n");
}
static void print_dem_open_observation(const struct runtime_dem_open_observation_v1 *observation)
{
    uint32_t index;
    if (!observation) return;
    wprintf(L"ntdos64-native: dem-open observed=%u count=%u read-failed=%u ds=%04x esi=%08x mode=%u physical=%08x disposition=%u gpr-mask=%08x ax=%04x bp=%04x flags-mask=%08x flags=%08x path=",
        observation->observed, observation->observed_count, observation->read_failed, observation->ds, observation->esi,
        observation->mode, observation->physical_address, observation->disposition,
        observation->gpr16_write_mask, observation->gpr16_values[0], observation->gpr16_values[6],
        observation->eflags_write_mask, observation->eflags_values);
    for (index = 0u; index < RUNTIME_DEM_OPEN_OBSERVATION_V1_PATH_BYTES; ++index) wprintf(L"%02x", observation->path[index]);
    wprintf(L"\n");
}
static void print_demfile_create_observation(
    const struct runtime_demfile_create_observation_v1 *observation)
{
    uint32_t index;
    if (!observation) return;
    wprintf(L"ntdos64-native: demfile-create calls=%u conversion-error=%u first-error=%u retry-error=%u access=%08x share=%08x creation=%08x flags=%08x path=",
        observation->call_count, observation->conversion_error,
        observation->first_error, observation->retry_error, observation->access,
        observation->share, observation->creation, observation->flags);
    for (index = 0u; index < RUNTIME_DEMFILE_CREATE_OBSERVATION_V1_PATH_CHARS &&
        observation->path[index] != 0u; ++index) wprintf(L"%lc", (wchar_t)observation->path[index]);
    wprintf(L"\n");
}
static void print_ntdos_exec_entry_observation(
    const struct runtime_ntdos_exec_entry_observation_v1 *observation)
{
    if (observation == 0) return;
    wprintf(L"ntdos64-native: ntdos-exec-entry observed=%u read-failed=%u cs=%04x eip=%08x pdb=%04x physical=%08x exit=%08x ctrl-c=%08x fatal=%08x parent=%04x environment=%04x\n",
        observation->observed, observation->read_failed, observation->cs,
        observation->eip, observation->pdb_segment, observation->physical_address,
        observation->pdb.exit_vector, observation->pdb.ctrl_c_vector,
        observation->pdb.fatal_vector, observation->pdb.parent_pdb_segment,
        observation->pdb.environment_segment);
}
static void print_generic_ud_sequence(const struct runtime_generic_ud_sequence_observation_v1 *sequence)
{
    uint32_t index, byte_index;
    if (sequence == 0) return;
    wprintf(L"ntdos64-native: ud-sequence count=%u overflow=%u\n",
        sequence->record_count, sequence->overflowed);
    for (index = 0u; index < sequence->record_count; ++index) {
        const struct runtime_generic_ud_sequence_observation_record_v1 *record =
            &sequence->records[index];
        wprintf(L"ntdos64-native: ud[%u] cs=%04x eip=%08x mode=%u disposition=%u window=",
            index, record->cs, record->eip, record->execution_mode,
            record->disposition);
        for (byte_index = 0u; byte_index < record->window_bytes; ++byte_index)
            wprintf(L"%02x", record->window[byte_index]);
        wprintf(L"\n");
    }
}
#if RUNTIME_ENABLE_MANTLE_SOFTWARE_INTERRUPT_OBSERVATION
static void print_software_interrupt_observation(void)
{
    struct runtime_software_interrupt_observation_v1_record record;
    uint32_t index;
    uint32_t count = runtime_mantle_software_interrupt_observation_v1_count();
    wprintf(L"ntdos64-native: software-interrupts count=%u\n", count);
    for (index = 0u; index < count; ++index) {
        if (runtime_mantle_software_interrupt_observation_v1_get(index, &record))
            wprintf(L"ntdos64-native: software-interrupt[%u] sequence=%llu vector=%02x mode=%u cs=%04x eip=%08llx flags=%08x ax=%04x bx=%04x cx=%04x dx=%04x ds=%04x es=%04x ss=%04x sp=%04x\n",
                index, (unsigned long long)record.sequence, record.vector,
                record.execution_mode, record.cs, (unsigned long long)record.rip,
                record.eflags, record.ax, record.bx, record.cx, record.dx,
                record.ds, record.es, record.ss, record.sp);
    }
}
#endif
#if RUNTIME_ENABLE_MANTLE_INTERRUPT_RETURN_OBSERVATION
static void print_interrupt_return_observation(void)
{
    struct runtime_interrupt_return_observation_v1_record record;
    uint32_t index;
    uint32_t count = runtime_mantle_interrupt_return_observation_v1_count();
    wprintf(L"ntdos64-native: interrupt-returns count=%u\n", count);
    for (index = 0u; index < count; ++index) {
        if (runtime_mantle_interrupt_return_observation_v1_get(index, &record))
            wprintf(L"ntdos64-native: interrupt-return[%u] sequence=%llu mode=%u width=%u cs=%04x eip=%08llx flags=%08x ax=%04x bx=%04x cx=%04x dx=%04x ds=%04x es=%04x ss=%04x sp=%08x\n", index, (unsigned long long)record.sequence, record.execution_mode, record.operand_width, record.cs, (unsigned long long)record.rip, record.eflags, record.ax, record.bx, record.cx, record.dx, record.ds, record.es, record.ss, record.sp);
    }
}
#endif
static int result_exit(const struct app_lifecycle_v1_audit *audit)
{
    if (!audit || !app_lifecycle_v1_audit_valid(audit)) return 1;
    if (audit->presentation == APP_LIFECYCLE_V1_PRESENTATION_CONTROLLED_GUEST_TERMINAL ||
        audit->presentation == APP_LIFECYCLE_V1_PRESENTATION_ORDINARY_GUEST_COMPLETION)
        return 0;
    if (audit->presentation == APP_LIFECYCLE_V1_PRESENTATION_REJECTED_ENGINE_REQUEST ||
        audit->presentation == APP_LIFECYCLE_V1_PRESENTATION_REJECTED_COMPOSITION)
        return 3;
    if (audit->presentation == APP_LIFECYCLE_V1_PRESENTATION_EXECUTION_BUDGET) return 4;
    return 1;
}

int wmain(int argc, wchar_t **argv)
{
    const wchar_t *config = 0, *root = 0, *target;
    wchar_t target_full[MAX_PATH], launch_text[BYOB_LAUNCH_PLAN_V2_ENV_CHARS];
    int has_mutation_mode = 0, has_tick_budget = 0, has_guest_memory = 0,
        has_reserved_memory_base = 0, has_reserved_memory = 0,
        has_bop_observation = 0, has_command_bootstrap_observation = 0, has_command_current_dir_observation = 0, has_dem_open_observation = 0, has_demfile_create_observation = 0, has_ntdos_exec_entry_observation = 0, has_generic_ud_observation = 0, has_first_fault_observation = 0, has_terminal_position_observation = 0
#if RUNTIME_ENABLE_MANTLE_SOFTWARE_INTERRUPT_OBSERVATION
        , has_software_interrupt_observation = 0
#endif
#if RUNTIME_ENABLE_MANTLE_INTERRUPT_RETURN_OBSERVATION
        , has_interrupt_return_observation = 0
#endif
#if RUNTIME_ENABLE_MANTLE_PHYSICAL_WRITE_OBSERVATION
        , has_physical_write_observation = 0
#endif
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY
        , has_terminal_history_observation = 0, has_terminal_cs_transitions_observation = 0
#endif
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
        , has_terminal_provenance_observation = 0
#endif
        ;
    uint32_t mutation_mode = RUNTIME_ENGINE_MUTATION_MODE_V1_DIRECT;
    uint64_t instruction_tick_budget = UINT64_C(1000000);
    uint64_t guest_memory_bytes = UINT64_C(0x100000);
    uint64_t reserved_memory_base = 0u, reserved_memory_bytes = 0u;
    int validate_only = 0, observe_bop_sequence = 0, observe_command_bootstrap = 0, observe_command_current_dir = 0, observe_dem_open = 0, observe_demfile_create = 0, observe_ntdos_exec_entry = 0, observe_generic_ud_sequence = 0, observe_first_fault = 0, observe_terminal_position = 0
#if RUNTIME_ENABLE_MANTLE_SOFTWARE_INTERRUPT_OBSERVATION
        , observe_software_interrupts = 0
#endif
#if RUNTIME_ENABLE_MANTLE_INTERRUPT_RETURN_OBSERVATION
        , observe_interrupt_returns = 0
#endif
#if RUNTIME_ENABLE_MANTLE_PHYSICAL_WRITE_OBSERVATION
        , observe_physical_write = 0
#endif
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY
        , observe_terminal_history = 0, observe_terminal_cs_transitions = 0
#endif
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
        , observe_terminal_provenance = 0
#endif
        ;
#if RUNTIME_ENABLE_MANTLE_PHYSICAL_WRITE_OBSERVATION
    uint64_t physical_write_address = 0u, physical_write_bytes = 0u;
#endif
    int index = 1;
    app_startup_selection selection;
    wchar_t config_full[MAX_PATH], root_full[MAX_PATH];
    struct app_dos_safe_alias_v1 dos_root_alias;
    wchar_t config_source[MAX_PATH], autoexec_source[MAX_PATH];
    byob_launch_plan_v2 launch;
    struct runtime_engine_request_v1 request;
    struct runtime_engine_result_v1 result;
    struct app_lifecycle_v1_policy lifecycle_policy;
    struct app_lifecycle_v1_audit lifecycle_audit;
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
        else if (wcscmp(argv[index], L"--guest-memory-kib") == 0 && index + 1 < argc &&
            !has_guest_memory && parse_guest_memory_kib(argv[index + 1], &guest_memory_bytes))
            has_guest_memory = 1, index += 2;
        else if (wcscmp(argv[index], L"--reserved-memory-base-kib") == 0 && index + 1 < argc &&
            !has_reserved_memory_base && parse_reserved_memory_kib(argv[index + 1], &reserved_memory_base))
            has_reserved_memory_base = 1, index += 2;
        else if (wcscmp(argv[index], L"--reserved-memory-kib") == 0 && index + 1 < argc &&
            !has_reserved_memory && parse_reserved_memory_kib(argv[index + 1], &reserved_memory_bytes))
            has_reserved_memory = 1, index += 2;
        else if (wcscmp(argv[index], L"--observe-ud-sequence") == 0 && !has_generic_ud_observation)
            has_generic_ud_observation = 1, observe_generic_ud_sequence = 1, ++index;
        else if (wcscmp(argv[index], L"--observe-bop-sequence") == 0 && !has_bop_observation)
            has_bop_observation = 1, observe_bop_sequence = 1, ++index;
        else if (wcscmp(argv[index], L"--observe-command-bootstrap") == 0 && !has_command_bootstrap_observation)
            has_command_bootstrap_observation = 1, observe_command_bootstrap = 1, ++index;
        else if (wcscmp(argv[index], L"--observe-command-current-dir") == 0 && !has_command_current_dir_observation)
            has_command_current_dir_observation = 1, observe_command_current_dir = 1, ++index;
        else if (wcscmp(argv[index], L"--observe-dem-open") == 0 && !has_dem_open_observation)
            has_dem_open_observation = 1, observe_dem_open = 1, ++index;
        else if (wcscmp(argv[index], L"--observe-demfile-create") == 0 && !has_demfile_create_observation)
            has_demfile_create_observation = 1, observe_demfile_create = 1, ++index;
        else if (wcscmp(argv[index], L"--observe-ntdos-exec-entry") == 0 && !has_ntdos_exec_entry_observation)
            has_ntdos_exec_entry_observation = 1, observe_ntdos_exec_entry = 1, ++index;
        else if (wcscmp(argv[index], L"--observe-terminal-position") == 0 && !has_terminal_position_observation)
            has_terminal_position_observation = 1, observe_terminal_position = 1, ++index;
#if RUNTIME_ENABLE_MANTLE_SOFTWARE_INTERRUPT_OBSERVATION
        else if (wcscmp(argv[index], L"--observe-software-interrupts") == 0 && !has_software_interrupt_observation)
            has_software_interrupt_observation = 1, observe_software_interrupts = 1, ++index;
#endif
#if RUNTIME_ENABLE_MANTLE_INTERRUPT_RETURN_OBSERVATION
        else if (wcscmp(argv[index], L"--observe-interrupt-returns") == 0 && !has_interrupt_return_observation)
            has_interrupt_return_observation = 1, observe_interrupt_returns = 1, ++index;
#endif
#if RUNTIME_ENABLE_MANTLE_PHYSICAL_WRITE_OBSERVATION
        else if (wcscmp(argv[index], L"--observe-physical-write-range") == 0 && index + 2 < argc &&
            !has_physical_write_observation && parse_instruction_tick_budget(argv[index + 1], &physical_write_address) &&
            parse_instruction_tick_budget(argv[index + 2], &physical_write_bytes))
            has_physical_write_observation = 1, observe_physical_write = 1, index += 3;
#endif
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY
        else if (wcscmp(argv[index], L"--observe-budget-terminal-history") == 0 && !has_terminal_history_observation)
            has_terminal_history_observation = 1, observe_terminal_history = 1, ++index;
        else if (wcscmp(argv[index], L"--observe-budget-terminal-cs-transitions") == 0 && !has_terminal_cs_transitions_observation)
            has_terminal_cs_transitions_observation = 1, observe_terminal_cs_transitions = 1, ++index;
#endif
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
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
    app_dos_safe_alias_v1_clear(&dos_root_alias);
    if (!GetFullPathNameW(target, MAX_PATH, target_full, 0) ||
        !GetFullPathNameW(config, MAX_PATH, config_full, 0) ||
        !GetFullPathNameW(root, MAX_PATH, root_full, 0) ||
        /* sysconf.asm writes SHELL= into the fixed 64-byte `commnd` buffer.
         * This admission supplies a real session-local host spelling, not a
         * guest rewrite or a virtual boot volume. */
        !app_dos_safe_alias_v1_admit(config_full,
            63u - (uint32_t)wcslen(L"\\COMMAND.COM"), &dos_root_alias) ||
        !app_bundle_load_roots(dos_root_alias.admitted_root, config_source, autoexec_source) ||
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
        app_dos_safe_alias_v1_release(&dos_root_alias);
        fwprintf(stderr, L"ntdos64-native: sibling dos/wow16 bundle admission failed\n"); return 3;
    }
    if (!SetEnvironmentVariableW(L"NTVDM_CONFIG_SOURCE", config_source) ||
        !SetEnvironmentVariableW(L"NTVDM_AUTOEXEC_SOURCE", autoexec_source) ||
        !SetEnvironmentVariableW(L"NTVDM_CONFIG_ROOT", dos_root_alias.admitted_root) ||
        !SetEnvironmentVariableW(L"NTVDM_TARGET_PATH", target_full) ||
        !SetEnvironmentVariableW(L"NTVDM_WOW16_ROOT", root_full)) {
        app_dos_safe_alias_v1_release(&dos_root_alias);
        return 3;
    }
    app_lifecycle_v1_policy_clear(&lifecycle_policy);
    lifecycle_policy.instruction_tick_budget = instruction_tick_budget;
    if (!app_lifecycle_v1_policy_valid(&lifecycle_policy)) {
        app_dos_safe_alias_v1_release(&dos_root_alias);
        return 3;
    }
    runtime_engine_request_v1_clear(&request);
    if (!copied_text(request.profile_descriptor, RUNTIME_ENGINE_V1_MAX_DESCRIPTOR_CHARS,
            target_full, &request.profile_descriptor_chars) ||
        !copied_text(request.root_descriptor, RUNTIME_ENGINE_V1_MAX_DESCRIPTOR_CHARS,
            dos_root_alias.admitted_root, &request.root_descriptor_chars) ||
        !copied_text(request.launch_descriptor, RUNTIME_ENGINE_V1_MAX_LAUNCH_CHARS,
            launch_text, &request.launch_descriptor_chars)) {
        app_dos_safe_alias_v1_release(&dos_root_alias);
        return 3;
    }
    request.mutation_mode = mutation_mode;
    request.instruction_tick_budget = lifecycle_policy.instruction_tick_budget;
    request.guest_memory_bytes = guest_memory_bytes;
    request.reserved_memory_base = reserved_memory_base;
    request.reserved_memory_bytes = reserved_memory_bytes;
    if (!runtime_engine_request_v1_valid(&request)) {
        app_dos_safe_alias_v1_release(&dos_root_alias);
        return 3;
    }
    if (validate_only) {
        wprintf(L"ntdos64-native: request mode=%u budget=%llu guest-memory=%llu reserved-base=%llu reserved-bytes=%llu observe-bop-sequence=%u observe-command-bootstrap=%u observe-ud-sequence=%u observe-first-fault=%u observe-terminal-position=%u\n",
            request.mutation_mode, (unsigned long long)request.instruction_tick_budget,
            (unsigned long long)request.guest_memory_bytes,
            (unsigned long long)request.reserved_memory_base,
            (unsigned long long)request.reserved_memory_bytes,
            observe_bop_sequence ? 1u : 0u, observe_command_bootstrap ? 1u : 0u, observe_generic_ud_sequence ? 1u : 0u,
            observe_first_fault ? 1u : 0u, observe_terminal_position ? 1u : 0u);
        app_dos_safe_alias_v1_release(&dos_root_alias);
        return 0;
    }
    if (observe_bop_sequence) runtime_bop_sequence_observation_v1_enable(1u);
    if (observe_command_bootstrap) runtime_command_bootstrap_observation_v1_enable(1u);
    if (observe_command_current_dir) runtime_command_current_dir_observation_v1_enable(1u);
    if (observe_dem_open) runtime_dem_open_observation_v1_enable(1u);
    if (observe_demfile_create) runtime_demfile_create_observation_v1_enable(1u);
    if (observe_ntdos_exec_entry) runtime_ntdos_exec_entry_observation_v1_enable(1u);
    if (observe_generic_ud_sequence) runtime_generic_ud_sequence_observation_v1_enable(1u);
    if (observe_terminal_position) runtime_machine_stage_v1_terminal_position_observation_enable(1u);
#if RUNTIME_ENABLE_MANTLE_SOFTWARE_INTERRUPT_OBSERVATION
    if (observe_software_interrupts && !runtime_mantle_software_interrupt_observation_v1_configure(RUNTIME_SOFTWARE_INTERRUPT_OBSERVATION_V1_CAPACITY_MAX)) {
        app_dos_safe_alias_v1_release(&dos_root_alias);
        return 1;
    }
#endif
#if RUNTIME_ENABLE_MANTLE_INTERRUPT_RETURN_OBSERVATION
    if (observe_interrupt_returns && !runtime_mantle_interrupt_return_observation_v1_configure(RUNTIME_INTERRUPT_RETURN_OBSERVATION_V1_CAPACITY_MAX)) {
        app_dos_safe_alias_v1_release(&dos_root_alias);
        return 1;
    }
#endif
#if RUNTIME_ENABLE_MANTLE_PHYSICAL_WRITE_OBSERVATION
    if (observe_physical_write && !runtime_physical_write_observation_v1_configure(
            physical_write_address, physical_write_bytes)) {
        app_dos_safe_alias_v1_release(&dos_root_alias);
        return 1;
    }
#endif
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY
    if (observe_terminal_history) runtime_machine_stage_v1_terminal_history_observation_enable(1u);
    if (observe_terminal_cs_transitions) runtime_machine_stage_v1_terminal_cs_transitions_observation_enable(1u);
#endif
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
    if (observe_terminal_provenance) runtime_machine_stage_v1_terminal_provenance_observation_enable(1u);
    if (observe_terminal_provenance) runtime_machine_stage_v1_terminal_cs_provenance_observation_enable(1u);
#endif
    if (observe_first_fault) {
        runtime_mantle_first_fault_observation_enable(1);
        runtime_mantle_segment_access_observation_enable(1);
    }
    if (!app_console_cancellation_v1_begin(&cancellation_event)) {
        if (observe_bop_sequence) runtime_bop_sequence_observation_v1_enable(0u);
        if (observe_command_bootstrap) runtime_command_bootstrap_observation_v1_enable(0u);
        if (observe_command_current_dir) runtime_command_current_dir_observation_v1_enable(0u);
        if (observe_dem_open) runtime_dem_open_observation_v1_enable(0u);
        if (observe_demfile_create) runtime_demfile_create_observation_v1_enable(0u);
        if (observe_ntdos_exec_entry) runtime_ntdos_exec_entry_observation_v1_enable(0u);
        if (observe_generic_ud_sequence) runtime_generic_ud_sequence_observation_v1_enable(0u);
        if (observe_terminal_position) runtime_machine_stage_v1_terminal_position_observation_enable(0u);
#if RUNTIME_ENABLE_MANTLE_SOFTWARE_INTERRUPT_OBSERVATION
        if (observe_software_interrupts) runtime_mantle_software_interrupt_observation_v1_configure(0u);
#endif
#if RUNTIME_ENABLE_MANTLE_INTERRUPT_RETURN_OBSERVATION
        if (observe_interrupt_returns) runtime_mantle_interrupt_return_observation_v1_configure(0u);
#endif
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY
        if (observe_terminal_history) runtime_machine_stage_v1_terminal_history_observation_enable(0u);
        if (observe_terminal_cs_transitions) runtime_machine_stage_v1_terminal_cs_transitions_observation_enable(0u);
#endif
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
        if (observe_terminal_provenance) runtime_machine_stage_v1_terminal_provenance_observation_enable(0u);
        if (observe_terminal_provenance) runtime_machine_stage_v1_terminal_cs_provenance_observation_enable(0u);
#endif
        if (observe_first_fault) {
            runtime_mantle_segment_access_observation_enable(0);
            runtime_mantle_first_fault_observation_enable(0);
        }
        app_dos_safe_alias_v1_release(&dos_root_alias);
        return 1;
    }
    if (!app_engine_worker_v1_run(&request, cancellation_event, &result,
            &cancellation_accepted)) {
        app_console_cancellation_v1_end();
        if (observe_bop_sequence) runtime_bop_sequence_observation_v1_enable(0u);
        if (observe_command_bootstrap) runtime_command_bootstrap_observation_v1_enable(0u);
        if (observe_command_current_dir) runtime_command_current_dir_observation_v1_enable(0u);
        if (observe_dem_open) runtime_dem_open_observation_v1_enable(0u);
        if (observe_demfile_create) runtime_demfile_create_observation_v1_enable(0u);
        if (observe_ntdos_exec_entry) runtime_ntdos_exec_entry_observation_v1_enable(0u);
        if (observe_generic_ud_sequence) runtime_generic_ud_sequence_observation_v1_enable(0u);
        if (observe_terminal_position) runtime_machine_stage_v1_terminal_position_observation_enable(0u);
#if RUNTIME_ENABLE_MANTLE_SOFTWARE_INTERRUPT_OBSERVATION
        if (observe_software_interrupts) runtime_mantle_software_interrupt_observation_v1_configure(0u);
#endif
#if RUNTIME_ENABLE_MANTLE_INTERRUPT_RETURN_OBSERVATION
        if (observe_interrupt_returns) runtime_mantle_interrupt_return_observation_v1_configure(0u);
#endif
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY
        if (observe_terminal_history) runtime_machine_stage_v1_terminal_history_observation_enable(0u);
        if (observe_terminal_cs_transitions) runtime_machine_stage_v1_terminal_cs_transitions_observation_enable(0u);
#endif
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
        if (observe_terminal_provenance) runtime_machine_stage_v1_terminal_provenance_observation_enable(0u);
        if (observe_terminal_provenance) runtime_machine_stage_v1_terminal_cs_provenance_observation_enable(0u);
#endif
        if (observe_first_fault) {
            runtime_mantle_segment_access_observation_enable(0);
            runtime_mantle_first_fault_observation_enable(0);
        }
        app_dos_safe_alias_v1_release(&dos_root_alias);
        return 1;
    }
    app_console_cancellation_v1_end();
#if RUNTIME_ENABLE_MANTLE_SOFTWARE_INTERRUPT_OBSERVATION
    if (observe_software_interrupts) {
        print_software_interrupt_observation();
        runtime_mantle_software_interrupt_observation_v1_configure(0u);
    }
#endif
#if RUNTIME_ENABLE_MANTLE_INTERRUPT_RETURN_OBSERVATION
    if (observe_interrupt_returns) {
        print_interrupt_return_observation();
        runtime_mantle_interrupt_return_observation_v1_configure(0u);
    }
#endif
#if RUNTIME_ENABLE_MANTLE_PHYSICAL_WRITE_OBSERVATION
    if (observe_physical_write) {
        struct runtime_physical_write_observation_v1 physical_write;
        if (runtime_physical_write_observation_v1_copy(&physical_write)) {
            uint32_t physical_write_index;
            wprintf(L"ntdos64-native: physical-write address=%llx bytes=%u captured=%u sequence=%llu cs=%04x base=%llx eip=%llx ss=%04x sp=%04x data=",
                (unsigned long long)physical_write.physical_address,
                physical_write.byte_count, physical_write.captured_bytes,
                (unsigned long long)physical_write.sequence, physical_write.cs,
                (unsigned long long)physical_write.cs_base,
                (unsigned long long)physical_write.rip, physical_write.ss,
                physical_write.sp);
            for (physical_write_index = 0u; physical_write_index < physical_write.captured_bytes; ++physical_write_index)
                wprintf(L"%02x", physical_write.bytes[physical_write_index]);
            wprintf(L"\n");
        } else wprintf(L"ntdos64-native: physical-write unavailable\n");
        runtime_physical_write_observation_v1_reset();
    }
#endif
    if (observe_bop_sequence) {
        struct runtime_bop_sequence_observation_v1 sequence;
        if (runtime_bop_sequence_observation_v1_copy(&sequence))
            print_bop_sequence(&sequence);
        else
            wprintf(L"ntdos64-native: bop-sequence unavailable\n");
        runtime_bop_sequence_observation_v1_enable(0u);
    }
    if (observe_command_bootstrap) {
        struct runtime_command_bootstrap_observation_v1 observation;
        if (runtime_command_bootstrap_observation_v1_copy(&observation))
            print_command_bootstrap_observation(&observation);
        else
            wprintf(L"ntdos64-native: command-bootstrap unavailable\n");
        runtime_command_bootstrap_observation_v1_enable(0u);
    }
    if (observe_command_current_dir) {
        struct runtime_command_current_dir_observation_v1 observation;
        if (runtime_command_current_dir_observation_v1_copy(&observation))
            print_command_current_dir_observation(&observation);
        else wprintf(L"ntdos64-native: command-current-dir unavailable\n");
        runtime_command_current_dir_observation_v1_enable(0u);
    }
    if (observe_dem_open) {
        struct runtime_dem_open_observation_v1 observation;
        if (runtime_dem_open_observation_v1_copy(&observation)) print_dem_open_observation(&observation);
        else wprintf(L"ntdos64-native: dem-open unavailable\n");
        runtime_dem_open_observation_v1_enable(0u);
    }
    if (observe_demfile_create) {
        struct runtime_demfile_create_observation_v1 observation;
        if (runtime_demfile_create_observation_v1_copy(&observation))
            print_demfile_create_observation(&observation);
        else wprintf(L"ntdos64-native: demfile-create unavailable\n");
        runtime_demfile_create_observation_v1_enable(0u);
    }
    if (observe_ntdos_exec_entry) {
        struct runtime_ntdos_exec_entry_observation_v1 observation;
        if (runtime_ntdos_exec_entry_observation_v1_copy(&observation))
            print_ntdos_exec_entry_observation(&observation);
        else wprintf(L"ntdos64-native: ntdos-exec-entry unavailable\n");
        runtime_ntdos_exec_entry_observation_v1_enable(0u);
    }
    if (observe_first_fault) {
        struct runtime_first_fault_observation_v1 first_fault;
        struct runtime_segment_access_observation_v1 segment_access;
        if (runtime_mantle_first_fault_observation_copy(&first_fault))
            wprintf(L"ntdos64-native: first-fault vector=%u error=%u cs=%04x eip=%08x eax=%08x eflags=%08x mode=%u\n",
                first_fault.vector, first_fault.error_code, first_fault.cs, first_fault.eip,
                first_fault.eax, first_fault.eflags, first_fault.execution_mode);
        else
            wprintf(L"ntdos64-native: first-fault unavailable\n");
        if (runtime_mantle_segment_access_observation_copy(&segment_access))
            wprintf(L"ntdos64-native: segment-access kind=%u branch=%u seg-index=%u selector=%04x offset=%08x width=%u limit=%08x valid=%08x\n", segment_access.access_kind, segment_access.branch_kind, segment_access.segment_index, segment_access.segment_selector, segment_access.offset, segment_access.width, segment_access.limit_scaled, segment_access.cache_valid);
        else
            wprintf(L"ntdos64-native: segment-access unavailable\n");
        runtime_mantle_segment_access_observation_enable(0);
        runtime_mantle_first_fault_observation_enable(0);
    }
    if (observe_terminal_position) {
        struct runtime_machine_stage_v1_terminal_position position;
        if (runtime_machine_stage_v1_terminal_position_observation_copy(&position))
            wprintf(L"ntdos64-native: terminal-position cs=%04x eip=%08x\n", position.cs, position.eip);
        else wprintf(L"ntdos64-native: terminal-position unavailable\n");
        runtime_machine_stage_v1_terminal_position_observation_enable(0u);
    }
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY
    if (observe_terminal_history) {
        struct runtime_machine_stage_v1_terminal_history history;
        uint32_t history_index;
        if (runtime_machine_stage_v1_terminal_history_observation_copy(&history)) {
            wprintf(L"ntdos64-native: budget-terminal-history count=%u\n", history.count);
            for (history_index = 0u; history_index < history.count; ++history_index)
                wprintf(L"ntdos64-native: history[%u] sequence=%llu cs=%04x eip=%016llx ss=%04x sp=%04x\n", history_index, (unsigned long long)history.records[history_index].sequence, history.records[history_index].cs, (unsigned long long)history.records[history_index].rip, history.records[history_index].ss, history.records[history_index].sp);
        } else wprintf(L"ntdos64-native: budget-terminal-history unavailable\n");
        runtime_machine_stage_v1_terminal_history_observation_enable(0u);
    }
    if (observe_terminal_cs_transitions) {
        struct runtime_machine_stage_v1_terminal_cs_transitions transitions;
        uint32_t transition_index;
        if (runtime_machine_stage_v1_terminal_cs_transitions_observation_copy(&transitions)) {
            wprintf(L"ntdos64-native: budget-terminal-cs-transitions count=%u\n", transitions.value.count);
            for (transition_index = 0u; transition_index < transitions.value.count; ++transition_index) {
                const struct runtime_instruction_history_transition_v1 *transition =
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
        runtime_machine_stage_v1_terminal_cs_transitions_observation_enable(0u);
    }
#endif
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
    if (observe_terminal_provenance) {
        struct runtime_machine_stage_v1_terminal_provenance provenance;
        uint32_t provenance_index;
        if (runtime_machine_stage_v1_terminal_provenance_observation_copy(&provenance)) {
            wprintf(L"ntdos64-native: budget-terminal-provenance cs=%04x eip=%08x ss=%04x sp=%04x instruction-valid=%u stack-valid=%u instruction=", provenance.cs, provenance.eip, provenance.ss, provenance.sp, provenance.instruction_valid, provenance.stack_valid);
            for (provenance_index = 0u; provenance_index < RUNTIME_INSTRUCTION_HISTORY_V1_PREDECESSOR_BYTES; ++provenance_index) wprintf(L"%02x", provenance.instruction_bytes[provenance_index]);
            wprintf(L" stack=");
            for (provenance_index = 0u; provenance_index < RUNTIME_INSTRUCTION_HISTORY_V1_STACK_BYTES; ++provenance_index) wprintf(L"%02x", provenance.stack_bytes[provenance_index]);
            wprintf(L"\n");
        } else wprintf(L"ntdos64-native: budget-terminal-provenance unavailable\n");
        {
            struct runtime_machine_stage_v1_terminal_cs_provenance transition;
            if (runtime_machine_stage_v1_terminal_cs_provenance_observation_copy(&transition)) {
                wprintf(L"ntdos64-native: budget-terminal-cs-transition previous=%04x:%08x bp=%04x current=%04x:%08x bp=%04x predecessor-valid=%u successor-valid=%u stack-valid=%u predecessor=", transition.value.transition.previous.cs, (unsigned)transition.value.transition.previous.rip, transition.value.transition.previous.bp, transition.value.transition.current.cs, (unsigned)transition.value.transition.current.rip, transition.value.transition.current.bp, transition.value.predecessor_valid, transition.value.successor_valid, transition.value.stack_valid);
                for (provenance_index = 0u; provenance_index < RUNTIME_INSTRUCTION_HISTORY_V1_PREDECESSOR_BYTES; ++provenance_index) wprintf(L"%02x", transition.value.predecessor_bytes[provenance_index]);
                wprintf(L" successor=");
                for (provenance_index = 0u; provenance_index < RUNTIME_INSTRUCTION_HISTORY_V1_SUCCESSOR_BYTES; ++provenance_index) wprintf(L"%02x", transition.value.successor_bytes[provenance_index]);
                wprintf(L" stack=");
                for (provenance_index = 0u; provenance_index < RUNTIME_INSTRUCTION_HISTORY_V1_STACK_BYTES; ++provenance_index) wprintf(L"%02x", transition.value.stack_bytes[provenance_index]);
                wprintf(L"\n");
            } else wprintf(L"ntdos64-native: budget-terminal-cs-transition unavailable\n");
            runtime_machine_stage_v1_terminal_cs_provenance_observation_enable(0u);
        }
        runtime_machine_stage_v1_terminal_provenance_observation_enable(0u);
    }
#endif
    if (observe_generic_ud_sequence) {
        struct runtime_generic_ud_sequence_observation_v1 sequence;
        if (runtime_generic_ud_sequence_observation_v1_copy(&sequence))
            print_generic_ud_sequence(&sequence);
        else
            wprintf(L"ntdos64-native: ud-sequence unavailable\n");
        runtime_generic_ud_sequence_observation_v1_enable(0u);
    }
    lifecycle_policy.cancellation_request = cancellation_accepted ?
        APP_LIFECYCLE_V1_CANCELLATION_REQUESTED :
        APP_LIFECYCLE_V1_CANCELLATION_NONE;
    if (!app_lifecycle_v1_classify(&lifecycle_policy, &result,
            &lifecycle_audit) || !app_lifecycle_v1_audit_valid(&lifecycle_audit)) {
        app_dos_safe_alias_v1_release(&dos_root_alias);
        return 1;
    }
    wprintf(L"ntdos64-native: terminal=%u detail=%u lifecycle=%u presentation=%u cancellation=%u budget=%llu\n",
        result.terminal_kind, result.detail_code, lifecycle_audit.lifecycle_terminal,
        lifecycle_audit.presentation, lifecycle_audit.cancellation_request,
        (unsigned long long)request.instruction_tick_budget);
    {
        int exit_code = result_exit(&lifecycle_audit);
        app_dos_safe_alias_v1_release(&dos_root_alias);
        return exit_code;
    }
usage:
    fwprintf(stderr, L"usage: ntdos64-native --dos-root directory --wow16-root directory [--mutation-mode direct|readonly] [--instruction-tick-budget positive-decimal] [--observe-bop-sequence] [--observe-command-bootstrap] [--observe-command-current-dir] [--observe-dem-open] [--observe-demfile-create] [--observe-ud-sequence] [--observe-first-fault] [--observe-terminal-position]"
#if RUNTIME_ENABLE_MANTLE_SOFTWARE_INTERRUPT_OBSERVATION
        L" [--observe-software-interrupts]"
#endif
#if RUNTIME_ENABLE_MANTLE_INTERRUPT_RETURN_OBSERVATION
        L" [--observe-interrupt-returns]"
#endif
#if RUNTIME_ENABLE_MANTLE_PHYSICAL_WRITE_OBSERVATION
        L" [--observe-physical-write-range positive-decimal-address positive-decimal-bytes]"
#endif
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY
        L" [--observe-budget-terminal-history] [--observe-budget-terminal-cs-transitions]"
#endif
#if RUNTIME_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE
        L" [--observe-budget-terminal-provenance]"
#endif
        L" [--observe-ntdos-exec-entry] [--guest-memory-kib 1024..16384] [--reserved-memory-base-kib 1024..] [--reserved-memory-kib 64..] [--validate-only] target [args...]\n");
    return 2;
}
