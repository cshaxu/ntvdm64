#include "bop-v1/bx_ntvdm_command_bootstrap_provider_v1.h"
#include "bop-v1/bx_ntvdm_command_console_keyboard_provider_v1.h"
#include "bop-v1/bx_ntvdm_command_launch_execution_provider_v1.h"
#include "bop-v1/bx_ntvdm_command_lifecycle_provider_v1.h"
#include "bop-v1/bx_ntvdm_command_package_facade_v1.h"

int main(void)
{
    uint8_t service;
    for (service=0u; service<17u; ++service) {
        const uint8_t bytes[4]={0xc4u,0xc4u,0x54u,service};
        bx_ntvdm_instruction_window_v1 window;
        bx_ntvdm_bop_ingress_v1 ingress;
        bx_ntvdm_bop_provider_selection_v1 selection;
        bx_ntvdm_command_package_route_v1 route;
        unsigned owners=0u;
        bx_ntvdm_instruction_window_v1_capture(&window,bytes,sizeof(bytes));
        if (!bx_ntvdm_bop_ingress_v1_classify(&window,&ingress) ||
            !bx_ntvdm_bop_provider_registry_v1_select(&ingress,&selection) ||
            !bx_ntvdm_command_package_facade_v1_classify(&ingress,&selection,&route) ||
            route.plane.service!=service) return 1;
        owners += bx_ntvdm_command_bootstrap_provider_v1_owns_service(service)!=0;
        owners += bx_ntvdm_command_launch_execution_provider_v1_owns_service(service)!=0;
        owners += bx_ntvdm_command_console_keyboard_provider_v1_owns_service(service)!=0;
        owners += bx_ntvdm_command_lifecycle_provider_v1_owns_service(service)!=0;
        if (service==16u) {
            if (owners!=0u || route.plane.component!=BX_NTVDM_COMMAND_COMPONENT_SESSION ||
                route.disposition!=BX_NTVDM_COMMAND_PACKAGE_DEFERRED) return 3;
        } else if (owners!=1u || route.disposition!=BX_NTVDM_COMMAND_PACKAGE_EXISTING_PROVIDER) return 4;
    }
    return 0;
}
