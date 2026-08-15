#include "bx_ntvdm_dem_session_lifecycle_provider_v1.h"
#include "bx-mantle/bx_ntvdm_mechanical_action_v1.h"

#include <string.h>

/* The lifecycle provider itself makes no mechanical action.  The shared
 * composition closure references the mantle ABI, so provide a local fixture
 * stub to keep this owner/precedence test independently linkable. */
int bx_ntvdm_mantle_execute_mechanical_action_v1(
    struct bx_ntvdm_mechanical_action_v1 *action)
{
    return action != 0 && bx_ntvdm_mechanical_action_v1_valid(action);
}

static int provider_initialize(bx_ntvdm_boot_namespace_provider_v1 *provider)
{
    uint8_t command[] = { 0x90u }, target[] = { 0xf4u };
    byob_image command_image = { command, sizeof(command) };
    byob_image target_image = { target, sizeof(target) };
    byob_profile_selection selection = { 0 };
    wcscpy(selection.command_placement.path, L"\\COMMAND.COM");
    selection.command_placement.drive_index = 2u; selection.has_command_placement = 1u;
    wcscpy(selection.target_placement.path, L"\\TARGET.COM");
    selection.target_placement.drive_index = 2u; selection.has_target_placement = 1u;
    wcscpy(selection.target.file_name, L"TARGET.COM");
    wcscpy(selection.config_file.path, L"\\CONFIG.SYS");
    selection.config_file.materialization = BYOB_GUEST_BOOT_FILE_MINIMAL_COMMENT_V1;
    wcscpy(selection.autoexec_file.path, L"\\AUTOEXEC.BAT");
    selection.autoexec_file.materialization = BYOB_GUEST_BOOT_FILE_EMPTY_V1;
    selection.has_guest_boot_files = 1u; selection.has_guest_search_metadata = 1u;
    selection.command_metadata.attributes = selection.target_metadata.attributes =
        selection.config_metadata.attributes = selection.autoexec_metadata.attributes = 0x20u;
    selection.command_metadata.dos_date = selection.target_metadata.dos_date =
        selection.config_metadata.dos_date = selection.autoexec_metadata.dos_date = 1u;
    return bx_ntvdm_boot_namespace_provider_v1_initialize(provider, &command_image,
        &target_image, NULL, &selection);
}

int main(void)
{
    bx_ntvdm_boot_namespace_provider_v1 provider;
    bx_ntvdm_host_namespace_entry_v1 entries[2] = { 0 }, out;
    bx_ntvdm_search_token_v1 token_a, token_b, token_foreign;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    bx_ntvdm_dem_plane_record_v1 plane;
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event), BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION,
        0u, 6u, 0u, 0u, 0x400u };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_cpu_result_v2 result;
    const uint8_t bytes[] = { 0xc4u, 0xc4u, 0x50u, 0x3cu };

    wcscpy(entries[0].dos_name, L"ONE.TXT"); wcscpy(entries[1].dos_name, L"TWO.TXT");
    if (!provider_initialize(&provider)) return 11;
    if (!bx_ntvdm_search_sessions_v1_begin(&provider.search_transaction.plan.sessions,
            0x1234u, entries, 2u, &out, &token_a)) return 12;
    if (!bx_ntvdm_search_sessions_v1_begin(&provider.search_transaction.plan.sessions,
            0x1234u, entries, 2u, &out, &token_b)) return 13;
    if (!bx_ntvdm_search_sessions_v1_begin(&provider.search_transaction.plan.sessions,
            0x5678u, entries, 2u, &out, &token_foreign)) return 14;
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.ebx = 0xabcd1234u;
    if (!bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) ||
        !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) ||
        selection.precedence != BX_NTVDM_BOP_PROVIDER_PRECEDENCE_ORIGINAL_OPENNT ||
        !bx_ntvdm_dem_plane_v1_classify(&ingress, &selection, &plane) ||
        plane.component != BX_NTVDM_DEM_COMPONENT_MISC ||
        !bx_ntvdm_dem_session_lifecycle_provider_v1_dispatch(&provider, &ingress,
            &selection, &plane, &event, &cpu, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME || result.resume_rip != 0x404u ||
        result.cpu_delta.gpr16_write_mask != 0u || result.eflags_values != 0u ||
        bx_ntvdm_search_sessions_v1_next(&provider.search_transaction.plan.sessions,
            0x1234u, &token_a, &out, &token_a) ||
        bx_ntvdm_search_sessions_v1_next(&provider.search_transaction.plan.sessions,
            0x1234u, &token_b, &out, &token_b) ||
        !bx_ntvdm_search_sessions_v1_next(&provider.search_transaction.plan.sessions,
            0x5678u, &token_foreign, &out, &token_a)) return 2;
    if (!bx_ntvdm_dem_session_lifecycle_provider_v1_dispatch(&provider, &ingress,
            &selection, &plane, &event, &cpu, &result)) return 3; /* absent list is success */
    event.vector = 13u;
    if (bx_ntvdm_dem_session_lifecycle_provider_v1_dispatch(&provider, &ingress,
            &selection, &plane, &event, &cpu, &result)) return 4;
    bx_ntvdm_search_transaction_v1_release(&provider.search_transaction);
    return 0;
}
