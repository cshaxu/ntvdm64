#include "bx_ntvdm_boot_namespace_composition_v1.h"
#include "bx_ntvdm_emm_unavailable_service.h"
#include "bx_ntvdm_mouse_install1_mapping_service.h"
#include "bx_ntvdm_printer_unavailable_service.h"
#include "bx_ntvdm_vdd_create_user_notify_service.h"
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
#include <string.h>

static bx_ntvdm_boot_namespace_composition_v1 *active;

static int valid(const bx_ntvdm_boot_namespace_composition_v1 *value)
{
    return value && value->magic == BX_NTVDM_BOOT_NAMESPACE_COMPOSITION_V1_MAGIC &&
        value->abi_version == BX_NTVDM_BOOT_NAMESPACE_COMPOSITION_V1_VERSION &&
        value->struct_bytes == sizeof(*value) && value->bound <= 1u &&
        (value->guest_display_state == BYOB_GUEST_DISPLAY_STATE_NONE ||
         value->guest_display_state == BYOB_GUEST_DISPLAY_STATE_STREAM_IO_V1);
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

/* This is one source-derived COMMAND bootstrap component, not a generic
 * selector recognizer.  The global ingress and the original-OpenNT provider
 * selection must already have classified either SVC_CMDCOMSPEC or
 * SVC_GETINITENVIRONMENT before the component may make an action. */
static int execute_command_bootstrap(
    bx_ntvdm_boot_namespace_composition_v1 *composition,
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    bx_ntvdm_command_plane_record_v1 command;
    bx_ntvdm_guest_gather_read_action_v1 read;
    bx_ntvdm_multi_write_transaction_v1 write;
    struct bx_ntvdm_mechanical_action_v1 action;
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD];
    uint32_t action_id;

    if (composition == 0 || ingress == 0 || selection == 0 || event == 0 ||
        cpu == 0 || window == 0 || result == 0 ||
        !bx_ntvdm_command_plane_v1_classify(ingress, selection, &command) ||
        command.disposition != BX_NTVDM_COMMAND_PLANE_DEFERRED ||
        (command.service != 2u && command.service != 15u)) return 0;
    if (command.service == 2u) {
        if (bx_ntvdm_cmd_comspec_bootstrap_v1_prepare_comspec(event, cpu, window,
                &composition->command_bootstrap, &read)) {
            if (read.disposition != BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_NEED_READ ||
                read.range_count != 1u ||
                read.total_bytes != BX_NTVDM_CMD_COMSPEC_TEXT_MAX_BYTES ||
                read.ranges[0].length != BX_NTVDM_CMD_COMSPEC_TEXT_MAX_BYTES ||
                read.ranges[0].address > UINT64_C(0x100000) - read.ranges[0].length ||
                composition->plane.next_action_id == 0u) return 0;
            action_id = composition->plane.next_action_id++;
            if (composition->plane.next_action_id == 0u)
                composition->plane.next_action_id = 1u;
            bx_ntvdm_mechanical_action_v1_clear(&action);
            action.action_id = action_id;
            action.kind = BX_NTVDM_MECHANICAL_ACTION_V1_READ;
            action.range_count = 1u;
            action.payload_bytes = (uint32_t)read.total_bytes;
            action.ranges[0].physical_address = read.ranges[0].address;
            action.ranges[0].byte_count = (uint32_t)read.ranges[0].length;
            action.ranges[0].payload_offset = 0u;
            if (!bx_ntvdm_mechanical_action_v1_valid(&action) ||
                !bx_ntvdm_mantle_execute_mechanical_action_v1(&action) ||
                !bx_ntvdm_cmd_comspec_bootstrap_v1_complete_comspec(event, cpu,
                    &read, action.payload, action.payload_bytes,
                    &composition->command_bootstrap, result)) return 0;
            return bx_ntvdm_cpu_result_v2_valid(result);
        }
        return bx_ntvdm_cmd_comspec_bootstrap_v1_repeat_comspec(event, cpu, window,
            &composition->command_bootstrap, result) &&
            bx_ntvdm_cpu_result_v2_valid(result);
    }
    if (!bx_ntvdm_cmd_comspec_bootstrap_v1_prepare_environment(event, cpu, window,
            &composition->command_bootstrap, &write, payload) ||
        !bx_ntvdm_cpu_result_v2_valid(&write.result)) return 0;
    if (write.writes.write_count != 0u &&
        (!execute_multi_write(composition, &write, payload) ||
         !bx_ntvdm_cmd_comspec_bootstrap_v1_complete_environment(
             &composition->command_bootstrap, &write))) return 0;
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
    bx_ntvdm_cmd_comspec_bootstrap_v1_initialize(&value->command_bootstrap);
    bx_ntvdm_command_launch_plane_v1_clear(&value->launch);
    bx_ntvdm_cmd_get_next_state_v1_initialize(&value->cmd_get_next);
    value->has_launch_plan = 0u;
    bx_ntvdm_dem_error_lock_plane_v1_clear(&value->error_lock);
    bx_ntvdm_dem_gset_plane_v1_clear(&value->gset);
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
        bx_ntvdm_dem_gset_plane_v1_set_drive_snapshot(&value->gset, snapshot);
}

int bx_ntvdm_boot_namespace_composition_v1_set_launch_plan(
    bx_ntvdm_boot_namespace_composition_v1 *value, const byob_launch_plan_v2 *plan)
{
    wchar_t encoded[BYOB_LAUNCH_PLAN_V2_ENV_CHARS]; byob_launch_plan_v2 checked;
    if (!valid(value) || value->bound || !plan ||
        !byob_launch_plan_v2_to_environment(plan, encoded) ||
        !byob_launch_plan_v2_from_environment(&checked, encoded)) return 0;
    value->launch_plan = checked; value->has_launch_plan = 1u; return 1;
}

static int execute_command_get_next(bx_ntvdm_boot_namespace_composition_v1 *x,
    const bx_ntvdm_exception_event_v1 *e, const bx_ntvdm_cpu_state_v1 *c,
    const bx_ntvdm_instruction_window_v1 *w, bx_ntvdm_cpu_result_v2 *r)
{
    bx_ntvdm_guest_gather_read_action_v1 read; bx_ntvdm_multi_write_transaction_v1 write;
    struct bx_ntvdm_mechanical_action_v1 action; uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD];
    uint32_t id;
    if (!x || !e || !c || !w || !r || !x->has_launch_plan || !x->launch.valid ||
        !x->gset.has_drive_snapshot || !bx_ntvdm_cmd_get_next_v1_prepare(&x->cmd_get_next,
            &x->launch_plan,e,c,w,&read) || read.range_count != 1u ||
        read.total_bytes != BX_NTVDM_CMDINFO_V1_BYTES || !x->plane.next_action_id) return 0;
    id=x->plane.next_action_id++; if(!x->plane.next_action_id)x->plane.next_action_id=1u;
    bx_ntvdm_mechanical_action_v1_clear(&action); action.action_id=id;
    action.kind=BX_NTVDM_MECHANICAL_ACTION_V1_READ; action.range_count=1u;
    action.payload_bytes=(uint32_t)read.total_bytes; action.ranges[0].physical_address=read.ranges[0].address;
    action.ranges[0].byte_count=(uint32_t)read.ranges[0].length;
    if(!bx_ntvdm_mechanical_action_v1_valid(&action)||!bx_ntvdm_mantle_execute_mechanical_action_v1(&action)||
       !bx_ntvdm_cmd_get_next_v1_complete(&x->plane.provider.readonly_namespace,&x->launch_plan,
          &x->gset.drive_snapshot,&x->launch.registration,&x->cmd_get_next,e,c,&read,action.payload,
          action.payload_bytes,&write,payload)||!execute_multi_write(x,&write,payload))return 0;
    bx_ntvdm_cmd_get_next_state_v1_commit(&x->cmd_get_next); *r=write.result; return bx_ntvdm_cpu_result_v2_valid(r);
}

/* The finite no-install response is a COMMAND console-plane capability
 * substitution.  Identity first comes from common ingress/provider selection;
 * this helper does not independently select a BOP or borrow the legacy runtime
 * gate. */
static int execute_command_keyboard_layout(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    bx_ntvdm_command_plane_record_v1 command;
    return ingress != 0 && selection != 0 && event != 0 && cpu != 0 &&
        window != 0 && result != 0 &&
        bx_ntvdm_command_plane_v1_classify(ingress, selection, &command) &&
        command.component == BX_NTVDM_COMMAND_COMPONENT_CONSOLE &&
        command.disposition == BX_NTVDM_COMMAND_PLANE_DEFERRED &&
        command.service == 14u && bx_ntvdm_cmd_keyboard_layout_v1_dispatch(
            event, cpu, window, result);
}

/* As with the keyboard-layout capability, the original selector identity must
 * first cross the common planes.  The finite current-directory provider uses
 * only the immutable admission mask and generic RAM-write mechanics. */
static int execute_command_current_directory(
    bx_ntvdm_boot_namespace_composition_v1 *composition,
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    bx_ntvdm_command_plane_record_v1 command;
    bx_ntvdm_multi_write_transaction_v1 transaction;
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD];
    if (composition == 0 || ingress == 0 || selection == 0 || event == 0 ||
        cpu == 0 || window == 0 || result == 0 ||
        !composition->gset.has_drive_snapshot ||
        !bx_ntvdm_command_plane_v1_classify(ingress, selection, &command) ||
        command.component != BX_NTVDM_COMMAND_COMPONENT_LAUNCH ||
        command.disposition != BX_NTVDM_COMMAND_PLANE_DEFERRED ||
        command.service != 4u || !bx_ntvdm_cmd_current_dir_service_v1_prepare(
            composition->gset.drive_snapshot.admitted_mask, event, cpu, window,
            &transaction, payload) ||
        !bx_ntvdm_cpu_result_v2_valid(&transaction.result) ||
        (transaction.writes.write_count != 0u &&
         !execute_multi_write(composition, &transaction, payload))) return 0;
    *result = transaction.result;
    return bx_ntvdm_cpu_result_v2_valid(result);
}

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

int bx_ntvdm_boot_namespace_composition_v1_handle(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *value)
{
    bx_ntvdm_exception_event_v1 boundary; bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window; bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection; bx_ntvdm_cpu_result_v2 result;
    bx_ntvdm_dem_plane_record_v1 dem_plane;
    bx_ntvdm_dem_package_route_v1 dem_route;
    bx_ntvdm_command_package_route_v1 command_route;
    bx_ntvdm_exception_result_v1 memory_result;
    struct bx_ntvdm_mechanical_action_v1 action, next;
    if (!valid(active) || !active->bound || !value || !unpack(event, &boundary,
            &cpu, &window)) return 0;
    /* The NTIO x86 branch explicitly requests CF after its BEEF 5F handoff.
     * This provider retains only that continuation; keyboard mechanics remain
     * outside this composition. */
    {
        int stream_state = execute_spckbd_stream_state(active, &boundary, &cpu,
            &window, &result);
        if (stream_state > 0) return outcome(&result, value);
        if (stream_state < 0) return 0;
    }
    if (bx_ntvdm_spckbd_init_service_v1_dispatch(&boundary, &cpu, &window,
            &result)) return outcome(&result, value);
    if (bx_ntvdm_emm_unavailable_service_v1_dispatch(&boundary, &cpu, &window,
            &result)) return outcome(&result, value);
    if (execute_mouse_install1_mapping(active, &boundary, &cpu, &window,
            &result)) return outcome(&result, value);
    if (bx_ntvdm_printer_unavailable_service_v1_dispatch(&boundary, &cpu,
            &window, &result)) return outcome(&result, value);
    if (bx_ntvdm_config_done_service_v1_dispatch(&boundary, &cpu, &window,
            &result)) return outcome(&result, value);
    if (bx_ntvdm_vdd_create_user_notify_service_v1_dispatch(&boundary, &cpu,
            &window, &result)) return outcome(&result, value);
    /* This composition admits only the source-observed top-level memory
     * queries: BIOS 12h and BIOS 15h/AH=88h. */
    if (window.valid_bytes >= 3u && window.bytes[0] == 0xc4u &&
        window.bytes[1] == 0xc4u &&
        (window.bytes[2] == 0x12u || window.bytes[2] == 0x15u) &&
        bx_ntvdm_bios_memory_service_v1_dispatch(&boundary, &cpu, &window,
            &memory_result)) {
        if (memory_result.disposition != BX_NTVDM_EXCEPTION_RESULT_RESUME ||
            !bx_ntvdm_cpu_result_v2_resume(&result, memory_result.resume_rip)) return 0;
        result.cpu_delta = memory_result.cpu_delta;
        return outcome(&result, value);
    }
    if (!bx_ntvdm_bop_ingress_v1_dispatch(&boundary, &cpu,
            &window, &ingress, &result) || !bx_ntvdm_cpu_result_v2_valid(&result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH ||
        !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection)) return 0;
    /* Every DEM member crosses one package gate before an existing provider.
     * Deferred records produce no result, while the original no-op is complete
     * here rather than being a detached service recognizer. */
    if (bx_ntvdm_dem_package_facade_v1_classify(&ingress, &selection,
            &dem_route)) {
        if (bx_ntvdm_dem_package_facade_v1_dispatch(&ingress, &selection,
                &dem_route, &boundary, &cpu, &result)) return outcome(&result, value);
        dem_plane = dem_route.plane;
    }
    if (bx_ntvdm_command_package_facade_v1_classify(&ingress, &selection,
            &command_route) && bx_ntvdm_command_package_facade_v1_dispatch(
            &ingress, &selection, &command_route, &boundary, &cpu, &result))
        return outcome(&result, value);
    if (bx_ntvdm_redir_package_facade_v1_dispatch(&ingress, &selection,
            &boundary, &cpu, &result)) return outcome(&result, value);
    if (bx_ntvdm_dem_misc_plane_v1_dispatch(&ingress, &selection, &boundary,
            &cpu, &window, &memory_result)) {
        if (memory_result.disposition != BX_NTVDM_EXCEPTION_RESULT_RESUME ||
            !bx_ntvdm_cpu_result_v2_resume(&result, memory_result.resume_rip)) return 0;
        result.cpu_delta = memory_result.cpu_delta;
        return outcome(&result, value);
    }
    if (bx_ntvdm_dem_plane_v1_classify(&ingress, &selection, &dem_plane) &&
        bx_ntvdm_dem_session_lifecycle_provider_v1_dispatch(
            &active->plane.provider, &ingress, &selection, &dem_plane,
            &boundary, &cpu, &result)) return outcome(&result, value);
    if (bx_ntvdm_dem_error_lock_plane_v1_dispatch(&active->error_lock,
            &ingress, &selection, &boundary, &cpu, &window, &result))
        return outcome(&result, value);
    if (bx_ntvdm_dem_gset_plane_v1_dispatch(&active->gset, &ingress,
            &selection, &boundary, &cpu, &window, &result))
        return outcome(&result, value);
    if (active->gset.has_drive_snapshot &&
        bx_ntvdm_dem_plane_v1_classify(&ingress, &selection, &dem_plane) &&
        bx_ntvdm_dem_ioctl_metadata_provider_v1_dispatch(&ingress, &selection,
            &dem_plane, &active->gset.drive_snapshot, &boundary, &cpu,
            &result)) return outcome(&result, value);
    if (bx_ntvdm_dem_boot_drive_service_v1_dispatch(&boundary, &cpu,
            &window, &memory_result)) {
        if (memory_result.disposition != BX_NTVDM_EXCEPTION_RESULT_RESUME ||
            !bx_ntvdm_cpu_result_v2_resume(&result, memory_result.resume_rip)) return 0;
        result.cpu_delta = memory_result.cpu_delta;
        return outcome(&result, value);
    }
    if (active->gset.has_drive_snapshot) {
        bx_ntvdm_multi_write_transaction_v1 transaction;
        uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD];
        if (bx_ntvdm_dem_dpb_service_v1_prepare(
                active->gset.drive_snapshot.types, &boundary, &cpu, &window,
                &transaction, payload)) {
            if (!execute_multi_write(active, &transaction, payload)) return 0;
            return outcome(&transaction.result, value);
        }
    }
    if (execute_command_keyboard_layout(&ingress, &selection, &boundary, &cpu,
            &window, &result)) return outcome(&result, value);
    if (execute_command_current_directory(active, &ingress, &selection,
            &boundary, &cpu, &window, &result)) return outcome(&result, value);
    if (execute_command_bootstrap(active, &ingress, &selection, &boundary, &cpu,
            &window, &result)) return outcome(&result, value);
    if (bx_ntvdm_command_launch_plane_v1_dispatch(&active->launch, &ingress,
            &selection, &boundary, &cpu, &window, &result)) return outcome(&result, value);
    if (execute_command_get_next(active, &boundary, &cpu, &window, &result))
        return outcome(&result, value);
    if (
        !bx_ntvdm_boot_namespace_plane_v1_dispatch(&active->plane, &ingress,
            &selection, &boundary, &cpu, &window, &action, &result)) return 0;
    /* A zero-byte read is a normal typed CPU result: it has no guest RAM
     * transfer.  Do not manufacture an empty mantle WRITE, whose mechanical
     * action contract intentionally requires a real range. */
    if (action.kind != BX_NTVDM_MECHANICAL_ACTION_V1_NONE &&
        !(action.kind == BX_NTVDM_MECHANICAL_ACTION_V1_WRITE &&
          action.range_count == 0u && action.payload_bytes == 0u) &&
        !bx_ntvdm_mantle_execute_mechanical_action_v1(&action)) return 0;
    if (action.kind == BX_NTVDM_MECHANICAL_ACTION_V1_READ) {
        if (!bx_ntvdm_boot_namespace_plane_v1_complete_read(&active->plane,
                &action, &next, &result)) return 0;
        if (next.kind != BX_NTVDM_MECHANICAL_ACTION_V1_NONE &&
            !bx_ntvdm_mantle_execute_mechanical_action_v1(&next)) return 0;
    }
    return outcome(&result, value);
}
