#include <stdint.h>

#include "bop-v1/bx_ntvdm_host_service_catalog.h"

int main(void)
{
    static const uint8_t dem_load[] = { 0xc4u, 0xc4u, 0x50u, 0x11u };
    static const uint8_t command[] = { 0xc4u, 0xc4u, 0x54u, 0x10u };
    static const uint8_t dem_out_of_range[] = { 0xc4u, 0xc4u, 0x50u, 0x49u };
    static const uint8_t command_out_of_range[] = { 0xc4u, 0xc4u, 0x54u, 0x11u };
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_host_service_request_v1 request;

    bx_ntvdm_instruction_window_v1_capture(&window, dem_load, 4u);
    if (!bx_ntvdm_host_service_catalog_v1_lookup(&window, &request) ||
        request.family != BX_NTVDM_HOST_SERVICE_FAMILY_DEM ||
        request.service != 0x11u) return 1;
    bx_ntvdm_instruction_window_v1_capture(&window, command, 4u);
    if (!bx_ntvdm_host_service_catalog_v1_lookup(&window, &request) ||
        request.family != BX_NTVDM_HOST_SERVICE_FAMILY_COMMAND ||
        request.service != 0x10u) return 2;
    bx_ntvdm_instruction_window_v1_capture(&window, dem_out_of_range, 4u);
    if (bx_ntvdm_host_service_catalog_v1_lookup(&window, &request) ||
        request.family != BX_NTVDM_HOST_SERVICE_FAMILY_NONE) return 3;
    bx_ntvdm_instruction_window_v1_capture(&window, command_out_of_range, 4u);
    if (bx_ntvdm_host_service_catalog_v1_lookup(&window, &request)) return 4;
    bx_ntvdm_instruction_window_v1_capture(&window, dem_load, 3u);
    if (bx_ntvdm_host_service_catalog_v1_lookup(&window, &request)) return 5;
    return bx_ntvdm_host_service_catalog_v1_lookup(&window, 0) ? 6 : 0;
}
