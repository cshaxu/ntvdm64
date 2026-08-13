#include "bx_ntvdm_boot_namespace_composition_v1.h"
#include "bx_ntvdm_spckbd_init_service.h"
#include "bx_ntvdm_bios_memory_service.h"
#include "bx_ntvdm_dem_boot_drive_service.h"
#include "bx_ntvdm_dem_dpb_service.h"
#include "bx_ntvdm_dem_ioctl_metadata_provider_v1.h"
#include "bx_ntvdm_dem_misc_plane_v1.h"
#include <string.h>

static bx_ntvdm_boot_namespace_composition_v1 *active;

static int valid(const bx_ntvdm_boot_namespace_composition_v1 *value)
{
    return value && value->magic == BX_NTVDM_BOOT_NAMESPACE_COMPOSITION_V1_MAGIC &&
        value->abi_version == BX_NTVDM_BOOT_NAMESPACE_COMPOSITION_V1_VERSION &&
        value->struct_bytes == sizeof(*value) && value->bound <= 1u;
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

static int execute_dpb_multi_write(
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
    value->struct_bytes = sizeof(*value); value->bound = 0; bx_ntvdm_command_launch_plane_v1_clear(&value->launch);
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

int bx_ntvdm_boot_namespace_composition_v1_handle(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *value)
{
    bx_ntvdm_exception_event_v1 boundary; bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window; bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection; bx_ntvdm_cpu_result_v2 result;
    bx_ntvdm_dem_plane_record_v1 dem_plane;
    bx_ntvdm_exception_result_v1 memory_result;
    struct bx_ntvdm_mechanical_action_v1 action, next;
    if (!valid(active) || !active->bound || !value || !unpack(event, &boundary,
            &cpu, &window)) return 0;
    /* The NTIO x86 branch explicitly requests CF after its BEEF 5F handoff.
     * This provider retains only that continuation; keyboard mechanics remain
     * outside this composition. */
    if (bx_ntvdm_spckbd_init_service_v1_dispatch(&boundary, &cpu, &window,
            &result)) return outcome(&result, value);
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
    if (bx_ntvdm_dem_misc_plane_v1_dispatch(&ingress, &selection, &boundary,
            &cpu, &window, &memory_result)) {
        if (memory_result.disposition != BX_NTVDM_EXCEPTION_RESULT_RESUME ||
            !bx_ntvdm_cpu_result_v2_resume(&result, memory_result.resume_rip)) return 0;
        result.cpu_delta = memory_result.cpu_delta;
        return outcome(&result, value);
    }
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
            if (!execute_dpb_multi_write(active, &transaction, payload)) return 0;
            return outcome(&transaction.result, value);
        }
    }
    if (bx_ntvdm_command_launch_plane_v1_dispatch(&active->launch, &ingress,
            &selection, &boundary, &cpu, &window, &result)) return outcome(&result, value);
    if (
        !bx_ntvdm_boot_namespace_plane_v1_dispatch(&active->plane, &ingress,
            &selection, &boundary, &cpu, &window, &action, &result)) return 0;
    if (action.kind != BX_NTVDM_MECHANICAL_ACTION_V1_NONE &&
        !bx_ntvdm_mantle_execute_mechanical_action_v1(&action)) return 0;
    if (action.kind == BX_NTVDM_MECHANICAL_ACTION_V1_READ) {
        if (!bx_ntvdm_boot_namespace_plane_v1_complete_read(&active->plane,
                &action, &next, &result)) return 0;
        if (next.kind != BX_NTVDM_MECHANICAL_ACTION_V1_NONE &&
            !bx_ntvdm_mantle_execute_mechanical_action_v1(&next)) return 0;
    }
    return outcome(&result, value);
}
