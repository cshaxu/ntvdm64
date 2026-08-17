#include "bx_ntvdm_command_bootstrap_provider_v1.h"
#include "bx_ntvdm_cmd_current_dir_service.h"
#include "bx_ntvdm_cmd_boot_file_service.h"
#include "bx_ntvdm_command_plane_v1.h"
#include <string.h>

static void command_event(bx_ntvdm_exception_event_v1 *event, uint64_t rip)
{
    *event = (bx_ntvdm_exception_event_v1){
        BX_NTVDM_EXCEPTION_ABI_MAGIC, BX_NTVDM_EXCEPTION_ABI_VERSION,
        sizeof(*event), BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u,
        0u, 0u, rip
    };
}

static void command_window(bx_ntvdm_instruction_window_v1 *window, uint8_t service)
{
    const uint8_t instruction[4] = { 0xc4u, 0xc4u, 0x54u, service };
    bx_ntvdm_instruction_window_v1_capture(window, instruction, sizeof(instruction));
}

int main(void)
{
    bx_ntvdm_readonly_namespace_v1 ns;
    bx_ntvdm_command_bootstrap_provider_v1 provider;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    bx_ntvdm_command_plane_record_v1 route;
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 command_instruction;
    bx_ntvdm_guest_gather_read_action_v1 gather;
    bx_ntvdm_multi_write_transaction_v1 transaction;
    bx_ntvdm_cmd_set_info_registration_v1 registration;
    bx_ntvdm_cpu_result_v2 result;
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD];
    uint32_t service;
    const uint8_t expected[17] = {
        0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0
    };

    memset(&ns, 0, sizeof(ns));
    ns.drive_index = 2u;
    ns.file_count = 3u;
    wcscpy_s(ns.files[1].path, BYOB_PROFILE_GUEST_PATH_MAX_CHARS, L"\\CONFIG.SYS");
    wcscpy_s(ns.files[2].path, BYOB_PROFILE_GUEST_PATH_MAX_CHARS, L"\\AUTOEXEC.BAT");
    if (!bx_ntvdm_command_bootstrap_provider_v1_initialize(&provider, &ns) ||
        !bx_ntvdm_command_bootstrap_provider_v1_valid(&provider)) return 1;

    for (service = 0u; service != 17u; ++service) {
        uint8_t instruction[4] = { 0xc4u, 0xc4u, 0x54u, (uint8_t)service };
        if (bx_ntvdm_command_bootstrap_provider_v1_owns_service((uint8_t)service) != expected[service])
            return 2;
        bx_ntvdm_instruction_window_v1_capture(&window, instruction, sizeof(instruction));
        if (!bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) ||
            !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) ||
            !bx_ntvdm_command_plane_v1_classify(&ingress, &selection, &route)) return 3;
        if (expected[service] && route.component != BX_NTVDM_COMMAND_COMPONENT_BOOTSTRAP)
            return 4;
    }
    if (bx_ntvdm_command_bootstrap_provider_v1_owns_service(17u)) return 5;
    command_window(&command_instruction, 16u);
    if (!bx_ntvdm_bop_ingress_v1_classify(&command_instruction, &ingress) ||
        !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) ||
        !bx_ntvdm_command_plane_v1_classify(&ingress, &selection, &route) ||
        route.component != BX_NTVDM_COMMAND_COMPONENT_SESSION ||
        route.disposition != BX_NTVDM_COMMAND_PLANE_DEFERRED) return 10;

    /* The provider's members decline malformed guest addresses before a
       machine action or partial result can escape their common boundary. */
    command_event(&event, 0x200u);
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.ds = 0xffffu; cpu.edx = 0xffffu;
    command_window(&command_instruction, 2u);
    if (bx_ntvdm_cmd_comspec_bootstrap_v1_prepare_comspec(&event, &cpu,
            &command_instruction, &provider.comspec, &gather)) return 6;
    command_window(&command_instruction, 5u);
    if (bx_ntvdm_cmd_set_info_service_v1_dispatch(&event, &cpu,
            &command_instruction, &registration, &result)) return 7;
    command_window(&command_instruction, 12u);
    if (bx_ntvdm_cmd_boot_file_service_v1_prepare(&provider.boot_input, &event,
            &cpu, &command_instruction, &transaction, payload)) return 8;

    /* 54:04 has a source-shaped failure when its selected drive has no
       session context.  It is not allowed to synthesize a root directory. */
    cpu.ds = 0x0100u; cpu.esi = 0x0020u; cpu.eax = 2u;
    command_window(&command_instruction, 4u);
    if (!bx_ntvdm_cmd_current_dir_service_v1_prepare(UINT32_C(1) << 2u, 0,
            &event, &cpu, &command_instruction, &transaction, payload) ||
        transaction.writes.write_count != 0u ||
        transaction.result.cpu_delta.gpr16_write_mask != 1u ||
        transaction.result.cpu_delta.gpr16_values[0] != 1u ||
        transaction.result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        transaction.result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 9;
    return 0;
}
