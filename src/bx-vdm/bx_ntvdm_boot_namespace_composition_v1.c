#include "bx_ntvdm_boot_namespace_composition_v1.h"
#include "bx_ntvdm_emm_unavailable_service.h"
#include "bx_ntvdm_mouse_install1_mapping_service.h"
#include "bx_ntvdm_printer_unavailable_service.h"
#include "bx_ntvdm_spckbd_init_service.h"
#include "bx_ntvdm_bios_memory_service.h"
#include "bx_ntvdm_command_plane_v1.h"
#include "bx_ntvdm_cmd_keyboard_layout_service.h"
#include "bx_ntvdm_cmd_current_dir_service.h"
#include "bx_ntvdm_config_done_service.h"
#include "bx_ntvdm_dem_boot_drive_service.h"
#include "bx_ntvdm_dem_dpb_service.h"
#include "bx_ntvdm_dem_ioctl_metadata_provider_v1.h"
#include "bx_ntvdm_dem_misc_plane_v1.h"
#include "bx_ntvdm_dem_session_lifecycle_provider_v1.h"
#include "bx_ntvdm_dem_package_facade_v1.h"
#include "bx_ntvdm_command_package_facade_v1.h"
#include "bx_ntvdm_redir_package_facade_v1.h"
#include "bx_ntvdm_debugger_package_facade_v1.h"
#include "bx_ntvdm_top_level_package_facade_v1.h"
#include "bx_ntvdm_machine_bop_facade_v1.h"
#include <string.h>

static bx_ntvdm_boot_namespace_composition_v1 *active;

static int valid(const bx_ntvdm_boot_namespace_composition_v1 *value)
{
    return value && value->magic == BX_NTVDM_BOOT_NAMESPACE_COMPOSITION_V1_MAGIC &&
        value->abi_version == BX_NTVDM_BOOT_NAMESPACE_COMPOSITION_V1_VERSION &&
        value->struct_bytes == sizeof(*value) && value->bound <= 1u &&
        (value->guest_display_state == BYOB_GUEST_DISPLAY_STATE_NONE ||
         value->guest_display_state == BYOB_GUEST_DISPLAY_STATE_STREAM_IO_V1) &&
        bx_ntvdm_dem_package_session_v1_valid(&value->dem) &&
        bx_ntvdm_command_package_session_v1_valid(&value->command);
}

static int unpack(const struct bx_ntvdm_generic_ud_event_v1 *event,
    bx_ntvdm_exception_event_v1 *boundary, bx_ntvdm_cpu_state_v1 *cpu,
    bx_ntvdm_instruction_window_v1 *window)
{
    if (!event || !boundary || !cpu || !window ||
        event->magic != BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC ||
        event->abi_version != BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION ||
        event->struct_bytes != sizeof(*event) || event->vector != 6u ||
        event->window_bytes > BX_NTVDM_GENERIC_UD_WINDOW_BYTES) return 0;
    boundary->magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    boundary->abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    boundary->struct_bytes = sizeof(*boundary);
    boundary->kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    boundary->cpu_id = event->cpu_id; boundary->vector = event->vector;
    boundary->error_code = event->error_code; boundary->reserved0 = 0;
    boundary->fault_rip = event->fault_rip;
    bx_ntvdm_cpu_state_v1_initialize(cpu, event->execution_mode);
    cpu->eax=event->eax; cpu->ebx=event->ebx; cpu->ecx=event->ecx;
    cpu->edx=event->edx; cpu->esi=event->esi; cpu->edi=event->edi;
    cpu->ebp=event->ebp; cpu->esp=event->esp; cpu->eip=event->eip;
    cpu->eflags=event->eflags; cpu->cs=event->cs; cpu->ds=event->ds;
    cpu->es=event->es; cpu->ss=event->ss; cpu->fs=event->fs; cpu->gs=event->gs;
    bx_ntvdm_instruction_window_v1_capture(window, event->window, event->window_bytes);
    return bx_ntvdm_exception_event_v1_valid(boundary) &&
        bx_ntvdm_cpu_state_v1_valid(cpu) && bx_ntvdm_instruction_window_v1_valid(window);
}

static int outcome(const bx_ntvdm_cpu_result_v2 *result,
    struct bx_ntvdm_generic_ud_outcome_v1 *value)
{
    if (!result || !value || !bx_ntvdm_cpu_result_v2_valid(result) ||
        result->disposition == BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH) return 0;
    memset(value, 0, sizeof(*value));
    value->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    value->disposition = result->disposition;
    value->resume_rip = result->resume_rip;
    value->gpr16_write_mask = result->cpu_delta.gpr16_write_mask;
    memcpy(value->gpr16_values, result->cpu_delta.gpr16_values,
        sizeof(value->gpr16_values));
    value->eflags_write_mask = result->eflags_write_mask;
    value->eflags_values = result->eflags_values;
    return 1;
}

static int execute_multi_write(
    bx_ntvdm_boot_namespace_composition_v1 *composition,
    const bx_ntvdm_multi_write_transaction_v1 *transaction,
    const uint8_t *payload)
{
    struct bx_ntvdm_mechanical_action_v1 action;
    uint32_t index;
    uint32_t action_id;

    if (composition == 0 || transaction == 0 || payload == 0 ||
        !bx_ntvdm_multi_write_transaction_v1_preflight(transaction,
            UINT64_C(0x100000), transaction->writes.payload_bytes)) return 0;
    if (transaction->writes.write_count == 0u) return 1;
    if (transaction->writes.write_count > BX_NTVDM_MECHANICAL_ACTION_V1_MAX_RANGES ||
        transaction->writes.payload_bytes > BX_NTVDM_MECHANICAL_ACTION_V1_MAX_BYTES ||
        composition->plane.next_action_id == 0u) return 0;
    action_id = composition->plane.next_action_id++;
    if (composition->plane.next_action_id == 0u) composition->plane.next_action_id = 1u;
    bx_ntvdm_mechanical_action_v1_clear(&action);
    action.action_id = action_id;
    action.kind = BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
    action.range_count = transaction->writes.write_count;
    action.payload_bytes = (uint32_t)transaction->writes.payload_bytes;
    for (index = 0; index < action.range_count; ++index) {
        action.ranges[index].physical_address =
            transaction->writes.writes[index].guest_physical_address;
        action.ranges[index].byte_count =
            (uint32_t)transaction->writes.writes[index].byte_count;
        action.ranges[index].payload_offset =
            (uint32_t)transaction->writes.writes[index].payload_offset;
    }
    memcpy(action.payload, payload, action.payload_bytes);
    return bx_ntvdm_mechanical_action_v1_valid(&action) &&
        bx_ntvdm_mantle_execute_mechanical_action_v1(&action);
}

static int execute_mouse_install1_mapping(
    bx_ntvdm_boot_namespace_composition_v1 *composition,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    bx_ntvdm_guest_gather_read_action_v1 read;
    bx_ntvdm_multi_write_transaction_v1 write;
    struct bx_ntvdm_mechanical_action_v1 action;
    uint8_t payload[4];
    uint32_t index, action_id, offset = 0u;

    if (composition == 0 || event == 0 || cpu == 0 || window == 0 ||
        result == 0 || !bx_ntvdm_mouse_install1_mapping_service_v1_prepare(
            event, cpu, window, &read)) return 0;
    if (read.disposition != BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_NEED_READ ||
        read.range_count == 0u ||
        read.range_count > BX_NTVDM_MECHANICAL_ACTION_V1_MAX_RANGES ||
        read.total_bytes == 0u ||
        read.total_bytes > BX_NTVDM_MECHANICAL_ACTION_V1_MAX_BYTES ||
        composition->plane.next_action_id == 0u) return 0;
    action_id = composition->plane.next_action_id++;
    if (composition->plane.next_action_id == 0u) composition->plane.next_action_id = 1u;
    bx_ntvdm_mechanical_action_v1_clear(&action);
    action.action_id = action_id;
    action.kind = BX_NTVDM_MECHANICAL_ACTION_V1_READ;
    action.range_count = read.range_count;
    action.payload_bytes = read.total_bytes;
    for (index = 0u; index < read.range_count; ++index) {
        if (read.ranges[index].address >= UINT64_C(0x100000) ||
            read.ranges[index].length == 0u ||
            read.ranges[index].length > UINT32_MAX ||
            read.ranges[index].length > UINT64_C(0x100000) -
                read.ranges[index].address ||
            offset > action.payload_bytes - (uint32_t)read.ranges[index].length)
            return 0;
        action.ranges[index].physical_address = read.ranges[index].address;
        action.ranges[index].byte_count = (uint32_t)read.ranges[index].length;
        action.ranges[index].payload_offset = offset;
        offset += action.ranges[index].byte_count;
    }
    if (offset != action.payload_bytes || !bx_ntvdm_mechanical_action_v1_valid(&action) ||
        !bx_ntvdm_mantle_execute_mechanical_action_v1(&action) ||
        !bx_ntvdm_mouse_install1_mapping_service_v1_complete(event, cpu, &read,
            action.payload, action.payload_bytes, &write, payload) ||
        !execute_multi_write(composition, &write, payload)) return 0;
    *result = write.result;
    return bx_ntvdm_cpu_result_v2_valid(result);
}

/* Returns 1 for a completed selected stream transaction, 0 when this is not
 * a selected BOP 5F request, and -1 when a selected request cannot complete.
 * The latter deliberately declines rather than falsely taking the old
 * continuation without publishing the profile-selected state. */
static int execute_spckbd_stream_state(
    bx_ntvdm_boot_namespace_composition_v1 *composition,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    bx_ntvdm_guest_gather_read_action_v1 read;
    bx_ntvdm_multi_write_transaction_v1 write;
    struct bx_ntvdm_mechanical_action_v1 action;
    uint8_t payload[1];
    uint32_t action_id;
    int selected_bop;

    if (composition == 0 || event == 0 || cpu == 0 || window == 0 ||
        result == 0 || composition->guest_display_state !=
            BYOB_GUEST_DISPLAY_STATE_STREAM_IO_V1) return 0;
    selected_bop = event->vector == 6u && window->valid_bytes >= 3u &&
        window->bytes[0] == 0xc4u && window->bytes[1] == 0xc4u &&
        window->bytes[2] == 0x5fu;
    if (!selected_bop) return 0;
    if (!bx_ntvdm_spckbd_stream_state_v1_prepare(
            composition->guest_display_state, event, cpu, window, &read) ||
        read.disposition != BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_NEED_READ ||
        read.range_count != 1u || read.total_bytes != 2u ||
        composition->plane.next_action_id == 0u) return -1;
    action_id = composition->plane.next_action_id++;
    if (composition->plane.next_action_id == 0u) composition->plane.next_action_id = 1u;
    bx_ntvdm_mechanical_action_v1_clear(&action);
    action.action_id = action_id;
    action.kind = BX_NTVDM_MECHANICAL_ACTION_V1_READ;
    action.range_count = 1u; action.payload_bytes = 2u;
    action.ranges[0].physical_address = read.ranges[0].address;
    action.ranges[0].byte_count = (uint32_t)read.ranges[0].length;
    if (!bx_ntvdm_mechanical_action_v1_valid(&action) ||
        !bx_ntvdm_mantle_execute_mechanical_action_v1(&action) ||
        !bx_ntvdm_spckbd_stream_state_v1_complete(
            composition->guest_display_state, event, cpu, &read,
            action.payload, action.payload_bytes, &write, payload) ||
        !execute_multi_write(composition, &write, payload)) return -1;
    *result = write.result;
    return bx_ntvdm_cpu_result_v2_valid(result) ? 1 : -1;
}

int bx_ntvdm_boot_namespace_composition_v1_initialize(
    bx_ntvdm_boot_namespace_composition_v1 *value, const byob_image *ntdos,
    const byob_image *command,
    const byob_image *target, const byob_image *quit,
    const byob_profile_selection *selection)
{
    if (!value || !bx_ntvdm_boot_namespace_plane_v1_initialize(&value->plane,
            ntdos, command, target, quit, selection)) return 0;
    value->magic = BX_NTVDM_BOOT_NAMESPACE_COMPOSITION_V1_MAGIC;
    value->abi_version = BX_NTVDM_BOOT_NAMESPACE_COMPOSITION_V1_VERSION;
    value->struct_bytes = sizeof(*value); value->bound = 0;
    value->guest_display_state = selection->guest_display_state;
    if (!bx_ntvdm_dem_package_session_v1_initialize(&value->dem,
            &value->plane) || !bx_ntvdm_command_package_session_v1_initialize(
            &value->command, &value->plane, &value->dem.drive_view.gset)) return 0;
    return valid(value);
}

int bx_ntvdm_boot_namespace_composition_v1_bind(
    bx_ntvdm_boot_namespace_composition_v1 *value)
{
    if (!valid(value) || active || value->bound) return 0;
    active = value; value->bound = 1; return 1;
}

void bx_ntvdm_boot_namespace_composition_v1_unbind(
    bx_ntvdm_boot_namespace_composition_v1 *value)
{
    if (value && active == value) active = 0;
    if (value && valid(value)) value->bound = 0;
}

int bx_ntvdm_boot_namespace_composition_v1_set_drive_snapshot(
    bx_ntvdm_boot_namespace_composition_v1 *value,
    const bx_ntvdm_host_drive_snapshot_v1 *snapshot)
{
    return valid(value) && !value->bound &&
        bx_ntvdm_dem_package_session_v1_set_drive_snapshot(&value->dem, snapshot);
}

int bx_ntvdm_boot_namespace_composition_v1_set_volume_snapshot(
    bx_ntvdm_boot_namespace_composition_v1 *value,
    const bx_ntvdm_host_volume_snapshot_v1 *snapshot)
{
    return valid(value) && !value->bound &&
        bx_ntvdm_dem_package_session_v1_set_volume_snapshot(&value->dem,
            snapshot);
}

int bx_ntvdm_boot_namespace_composition_v1_set_dem_mutation_profile(
    bx_ntvdm_boot_namespace_composition_v1 *value,
    const bx_ntvdm_mutation_profile_v1 *profile)
{
    return valid(value) && !value->bound &&
        bx_ntvdm_dem_package_session_v1_set_mutation_profile(&value->dem,
            profile) && bx_ntvdm_command_package_session_v1_set_session_host_context(
            &value->command, &value->dem.drive_view.cwd);
}

int bx_ntvdm_boot_namespace_composition_v1_set_command_mutation_profile(
    bx_ntvdm_boot_namespace_composition_v1 *value,
    const bx_ntvdm_mutation_profile_v1 *profile)
{
    return valid(value) && !value->bound &&
        bx_ntvdm_command_package_session_v1_set_mutation_profile(&value->command,
            profile);
}

int bx_ntvdm_boot_namespace_composition_v1_set_command_host_context(
    bx_ntvdm_boot_namespace_composition_v1 *value,
    const bx_ntvdm_command_host_context_v1 *context)
{
    return valid(value) && !value->bound &&
        bx_ntvdm_command_package_session_v1_set_host_context(&value->command,
            context) && (!value->dem.drive_view.has_mutation_profile ||
            bx_ntvdm_session_host_context_v1_seed_command(&value->dem.drive_view.cwd,
                context));
}

int bx_ntvdm_boot_namespace_composition_v1_set_startup_configuration(
    bx_ntvdm_boot_namespace_composition_v1 *value,
    const bx_ntvdm_startup_configuration_input_v1 *input)
{
    if (!valid(value) || value->bound ||
        !bx_ntvdm_boot_namespace_provider_v1_bind_startup_configuration(
            &value->plane.provider, input)) return 0;
    return bx_ntvdm_command_package_session_v1_set_boot_input(&value->command,
        &value->plane.provider.command_boot_input);
}

int bx_ntvdm_boot_namespace_composition_v1_set_dem_host_namespace(
    bx_ntvdm_boot_namespace_composition_v1 *value,
    const bx_ntvdm_host_namespace_v1 *host_namespace)
{
    return valid(value) && !value->bound &&
        bx_ntvdm_dem_package_session_v1_set_host_namespace(&value->dem,
            host_namespace);
}

int bx_ntvdm_boot_namespace_composition_v1_set_dem_boot_drive(
    bx_ntvdm_boot_namespace_composition_v1 *value, uint32_t drive_index)
{
    return valid(value) && !value->bound &&
        bx_ntvdm_dem_package_session_v1_set_boot_drive(&value->dem, drive_index);
}

int bx_ntvdm_boot_namespace_composition_v1_set_launch_plan(
    bx_ntvdm_boot_namespace_composition_v1 *value, const byob_launch_plan_v2 *plan)
{ return valid(value) && !value->bound &&
      bx_ntvdm_command_package_session_v1_set_launch_plan(&value->command, plan); }

int bx_ntvdm_boot_namespace_composition_v1_copy_namespace_diagnostic(
    const bx_ntvdm_boot_namespace_composition_v1 *value,
    bx_ntvdm_boot_namespace_diagnostic_v1 *out)
{
    const bx_ntvdm_readonly_namespace_v1 *space;
    if (!valid(value) || !out) return 0;
    space = &value->plane.provider.readonly_namespace;
    out->version = BX_NTVDM_BOOT_NAMESPACE_DIAGNOSTIC_V1_VERSION;
    out->open = space->open ? 1u : 0u;
    out->open_file_index = space->open ? space->open_file_index : UINT32_MAX;
    out->generation = space->generation;
    out->offset = space->offset;
    return 1;
}

int bx_ntvdm_boot_namespace_composition_v1_copy_ordinary_terminal(
    const bx_ntvdm_boot_namespace_composition_v1 *value)
{
    bx_ntvdm_command_terminal_v1 terminal;
    return value != 0 && value->bound != 0u &&
        bx_ntvdm_command_lifecycle_provider_v1_copy_terminal(
            &value->command.lifecycle_provider, &terminal) &&
        terminal.present == 1u &&
        terminal.terminal_kind == BX_NTVDM_COMMAND_TERMINAL_V1_TOP_LEVEL_EXIT &&
        terminal.has_dos_exit_code == 0u;
}

int bx_ntvdm_boot_namespace_composition_v1_handle(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *value)
{
    bx_ntvdm_exception_event_v1 boundary; bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window; bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection; bx_ntvdm_cpu_result_v2 result;
    uint32_t top_level_route;
    uint32_t machine_route;
    bx_ntvdm_exception_result_v1 memory_result;
    if (!valid(active) || !active->bound || !value || !unpack(event, &boundary,
            &cpu, &window)) return 0;
    if (bx_ntvdm_emm_unavailable_service_v1_dispatch(&boundary, &cpu, &window,
            &result)) return outcome(&result, value);
    if (execute_mouse_install1_mapping(active, &boundary, &cpu, &window,
            &result)) return outcome(&result, value);
    if (bx_ntvdm_printer_unavailable_service_v1_dispatch(&boundary, &cpu,
            &window, &result)) return outcome(&result, value);
    if (!bx_ntvdm_bop_ingress_v1_dispatch(&boundary, &cpu,
            &window, &ingress, &result) || !bx_ntvdm_cpu_result_v2_valid(&result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH ||
        !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection)) return 0;
    if (bx_ntvdm_top_level_package_facade_v1_classify(&ingress, &selection,
            &top_level_route)) {
        if (bx_ntvdm_top_level_package_facade_v1_dispatch(top_level_route,
                &boundary, &cpu, &result)) return outcome(&result, value);
        if (top_level_route == BX_NTVDM_TOP_LEVEL_PACKAGE_CONFIG &&
            bx_ntvdm_config_done_service_v1_dispatch(&boundary, &cpu, &window,
                &result)) return outcome(&result, value);
    }
    if (bx_ntvdm_machine_bop_facade_v1_classify(&ingress, &selection,
            &machine_route)) {
        if (bx_ntvdm_machine_bop_facade_v1_dispatch(machine_route, &boundary,
                &cpu, &result)) return outcome(&result, value);
        if (machine_route == BX_NTVDM_MACHINE_BOP_MEMORY &&
            bx_ntvdm_bios_memory_service_v1_dispatch(&boundary, &cpu, &window,
                &memory_result)) {
            if (memory_result.disposition != BX_NTVDM_EXCEPTION_RESULT_RESUME ||
                !bx_ntvdm_cpu_result_v2_resume(&result, memory_result.resume_rip)) return 0;
            result.cpu_delta = memory_result.cpu_delta;
            return outcome(&result, value);
        }
        if (machine_route == BX_NTVDM_MACHINE_BOP_HANDOFF) {
            int stream_state = execute_spckbd_stream_state(active, &boundary,
                &cpu, &window, &result);
            if (stream_state > 0) return outcome(&result, value);
            if (stream_state < 0) return 0;
            if (bx_ntvdm_spckbd_init_service_v1_dispatch(&boundary, &cpu,
                    &window, &result)) return outcome(&result, value);
        }
    }
    if (bx_ntvdm_dem_package_session_v1_dispatch(&active->dem, &ingress,
            &selection, &boundary, &cpu, &window, &result))
        return outcome(&result, value);
    if (bx_ntvdm_command_package_session_v1_dispatch(&active->command,
            &ingress, &selection, &boundary, &cpu, &window, &result))
        return outcome(&result, value);
    if (bx_ntvdm_redir_package_facade_v1_dispatch(&ingress, &selection,
            &boundary, &cpu, &result)) return outcome(&result, value);
    if (bx_ntvdm_debugger_package_facade_v1_dispatch(&ingress, &selection,
            &boundary, &cpu, &result)) return outcome(&result, value);
    return 0;
}
