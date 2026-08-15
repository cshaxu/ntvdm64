#include "bx_ntvdm_boot_namespace_provider_v1.h"

#include <string.h>

int bx_ntvdm_boot_namespace_provider_v1_valid(
    const bx_ntvdm_boot_namespace_provider_v1 *value)
{
    return value != 0 && value->magic == BX_NTVDM_BOOT_NAMESPACE_PROVIDER_V1_MAGIC &&
        value->version == BX_NTVDM_BOOT_NAMESPACE_PROVIDER_V1_VERSION &&
        bx_ntvdm_command_boot_input_v1_valid(&value->command_boot_input) &&
        bx_ntvdm_profile_search_snapshot_v1_valid(&value->search_snapshot) &&
        bx_ntvdm_search_transaction_v1_valid(&value->search_transaction) &&
        value->has_startup_configuration <= 1u &&
        (!value->has_startup_configuration ||
         (bx_ntvdm_startup_configuration_provider_v1_valid(&value->startup_configuration) &&
          value->startup_configuration.result == BX_NTVDM_STARTUP_CONFIGURATION_RESULT_V1_READY &&
          value->readonly_namespace.files[1].bytes == value->startup_configuration.config_image &&
          value->readonly_namespace.files[1].byte_count == value->startup_configuration.config_image_bytes &&
          value->readonly_namespace.files[2].bytes == value->startup_configuration.autoexec_image &&
          value->readonly_namespace.files[2].byte_count == value->startup_configuration.autoexec_image_bytes));
}

int bx_ntvdm_boot_namespace_provider_v1_initialize(
    bx_ntvdm_boot_namespace_provider_v1 *value, const byob_image *command,
    const byob_image *target, const byob_image *terminal_quit,
    const byob_profile_selection *selection)
{
    if (value == 0 || command == 0 || target == 0 || selection == 0) return 0;
    memset(value, 0, sizeof(*value));
    if (!bx_ntvdm_readonly_namespace_v1_initialize(&value->readonly_namespace, command, selection) ||
        !bx_ntvdm_readonly_namespace_v1_append_target(&value->readonly_namespace, target, selection) ||
        (selection->declared_target_count == 2u &&
         (terminal_quit == 0 || !bx_ntvdm_readonly_namespace_v1_append_terminal_quit(
             &value->readonly_namespace, terminal_quit, selection))) ||
        !bx_ntvdm_command_boot_input_v1_initialize(&value->command_boot_input,
            &value->readonly_namespace) ||
        !bx_ntvdm_profile_search_snapshot_v1_initialize(&value->search_snapshot,
            &value->readonly_namespace, selection)) return 0;
    bx_ntvdm_search_transaction_v1_initialize(&value->search_transaction);
    value->magic = BX_NTVDM_BOOT_NAMESPACE_PROVIDER_V1_MAGIC;
    value->version = BX_NTVDM_BOOT_NAMESPACE_PROVIDER_V1_VERSION;
    return bx_ntvdm_boot_namespace_provider_v1_valid(value);
}

int bx_ntvdm_boot_namespace_provider_v1_bind_startup_configuration(
    bx_ntvdm_boot_namespace_provider_v1 *value,
    const bx_ntvdm_startup_configuration_input_v1 *input)
{
    bx_ntvdm_startup_configuration_provider_v1 generated;
    bx_ntvdm_command_boot_input_v1 boot_input;
    if (!bx_ntvdm_boot_namespace_provider_v1_valid(value) ||
        value->has_startup_configuration ||
        !bx_ntvdm_startup_configuration_provider_v1_build(&generated, input) ||
        generated.result != BX_NTVDM_STARTUP_CONFIGURATION_RESULT_V1_READY ||
        !bx_ntvdm_command_boot_input_v1_initialize_paths(&boot_input,
            value->readonly_namespace.drive_index, value->readonly_namespace.files[1].path,
            value->readonly_namespace.files[2].path)) return 0;
    value->startup_configuration = generated;
    value->readonly_namespace.files[1].bytes = value->startup_configuration.config_image;
    value->readonly_namespace.files[1].byte_count = value->startup_configuration.config_image_bytes;
    value->readonly_namespace.files[2].bytes = value->startup_configuration.autoexec_image;
    value->readonly_namespace.files[2].byte_count = value->startup_configuration.autoexec_image_bytes;
    value->command_boot_input = boot_input;
    value->has_startup_configuration = 1u;
    return bx_ntvdm_boot_namespace_provider_v1_valid(value);
}

int bx_ntvdm_boot_namespace_provider_v1_prepare_boot_file_diagnostic(
    const bx_ntvdm_boot_namespace_provider_v1 *value,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window, bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD],
    bx_ntvdm_cmd_boot_file_prepare_diagnostic_v1 *diagnostic)
{
    if (diagnostic == 0 || !bx_ntvdm_boot_namespace_provider_v1_valid(value)) return 0;
    return bx_ntvdm_cmd_boot_file_service_v1_prepare_diagnostic(&value->command_boot_input,
        event, cpu_before, window, transaction, payload, diagnostic);
}

int bx_ntvdm_boot_namespace_provider_v1_prepare_boot_file(
    const bx_ntvdm_boot_namespace_provider_v1 *value,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window, bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD])
{
    bx_ntvdm_cmd_boot_file_prepare_diagnostic_v1 diagnostic;
    return bx_ntvdm_boot_namespace_provider_v1_prepare_boot_file_diagnostic(value, event,
        cpu_before, window, transaction, payload, &diagnostic);
}

int bx_ntvdm_boot_namespace_provider_v1_prepare_open(
    const bx_ntvdm_boot_namespace_provider_v1 *value,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, bx_ntvdm_guest_read_action_v1 *action)
{
    return bx_ntvdm_boot_namespace_provider_v1_valid(value) &&
        bx_ntvdm_dem_readonly_file_v1_prepare_open(event, cpu, window, action);
}

int bx_ntvdm_boot_namespace_provider_v1_complete_open(
    bx_ntvdm_boot_namespace_provider_v1 *value,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_guest_read_action_v1 *action, const uint8_t *path_bytes,
    uint64_t path_byte_count, bx_ntvdm_cpu_result_v2 *result)
{
    return bx_ntvdm_boot_namespace_provider_v1_valid(value) &&
        bx_ntvdm_dem_readonly_file_v1_complete_open(&value->readonly_namespace, event, cpu,
            action, path_bytes, path_byte_count, result);
}

int bx_ntvdm_boot_namespace_provider_v1_seek(
    bx_ntvdm_boot_namespace_provider_v1 *value,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, bx_ntvdm_cpu_result_v2 *result)
{
    return bx_ntvdm_boot_namespace_provider_v1_valid(value) &&
        bx_ntvdm_dem_readonly_file_v1_seek(&value->readonly_namespace, event, cpu, window, result);
}

int bx_ntvdm_boot_namespace_provider_v1_read(
    bx_ntvdm_boot_namespace_provider_v1 *value,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, uint8_t *payload,
    uint32_t payload_capacity, bx_ntvdm_bulk_result_transaction_v1 *transaction,
    bx_ntvdm_cpu_result_v2 *result)
{
    return bx_ntvdm_boot_namespace_provider_v1_valid(value) &&
        bx_ntvdm_dem_readonly_file_v1_read(&value->readonly_namespace, event, cpu, window,
            payload, payload_capacity, transaction, result);
}

int bx_ntvdm_boot_namespace_provider_v1_fast_read(
    bx_ntvdm_boot_namespace_provider_v1 *value,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, uint8_t *payload,
    uint32_t payload_capacity, bx_ntvdm_bulk_result_transaction_v1 *transaction,
    bx_ntvdm_cpu_result_v2 *result)
{
    return bx_ntvdm_boot_namespace_provider_v1_valid(value) &&
        bx_ntvdm_dem_readonly_file_v1_fast_read(&value->readonly_namespace, event, cpu,
            window, payload, payload_capacity, transaction, result);
}

int bx_ntvdm_boot_namespace_provider_v1_close(
    bx_ntvdm_boot_namespace_provider_v1 *value,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, bx_ntvdm_cpu_result_v2 *result)
{
    return bx_ntvdm_boot_namespace_provider_v1_valid(value) &&
        bx_ntvdm_dem_readonly_file_v1_close(&value->readonly_namespace, event, cpu, window, result);
}

int bx_ntvdm_boot_namespace_provider_v1_terminate_pdb(
    bx_ntvdm_boot_namespace_provider_v1 *value, uint16_t pdb,
    uint32_t *released_slots)
{
    if (released_slots != 0) *released_slots = 0u;
    return bx_ntvdm_boot_namespace_provider_v1_valid(value) &&
        bx_ntvdm_search_sessions_v1_release_pdb(
            &value->search_transaction.plan.sessions, pdb, released_slots);
}
