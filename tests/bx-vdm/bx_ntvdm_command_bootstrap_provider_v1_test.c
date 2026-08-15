#include "bx_ntvdm_command_bootstrap_provider_v1.h"
#include "bx_ntvdm_command_plane_v1.h"
#include <string.h>

int main(void)
{
    bx_ntvdm_readonly_namespace_v1 ns;
    bx_ntvdm_command_bootstrap_provider_v1 provider;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    bx_ntvdm_command_plane_record_v1 route;
    uint32_t service;
    const uint8_t expected[17] = {
        0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1
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
    return 0;
}
