#include "bx_ntvdm_dem_plane_v1.h"

static uint32_t expected_component(uint8_t service)
{
    static const uint8_t counts[] = { 25u, 8u, 13u, 3u, 13u, 3u, 1u, 8u };
    static const uint8_t members[][25] = {
        {0,1,2,3,4,5,6,8,9,10,11,12,18,19,22,23,24,30,34,39,60,66,68,71,72},
        {7,32,44,45,46,47,48,49}, {13,14,15,16,20,21,25,26,27,28,37,65,70},
        {33,41,42}, {17,35,52,53,54,55,56,57,58,59,61,62,69}, {50,51,63},
        {29}, {31,36,38,40,43,64,66,67}
    };
    uint32_t group, index;
    for (group = 0u; group < 8u; ++group)
        for (index = 0u; index < counts[group]; ++index)
            if (members[group][index] == service) return group + 1u;
    return 0u;
}

int main(void)
{
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    bx_ntvdm_dem_plane_record_v1 record;
    uint32_t service;
    int failed = 0;
    for (service = 0u; service < 73u; ++service) {
        uint8_t bytes[4] = { 0xc4u, 0xc4u, 0x50u, (uint8_t)service };
        bx_ntvdm_instruction_window_v1_capture(&window, bytes, 4u);
        failed |= !bx_ntvdm_bop_ingress_v1_classify(&window, &ingress);
        failed |= !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection);
        failed |= !bx_ntvdm_dem_plane_v1_classify(&ingress, &selection, &record);
        failed |= record.service != service || record.component != expected_component((uint8_t)service);
        failed |= record.component == BX_NTVDM_DEM_COMPONENT_NONE;
        failed |= record.disposition != (record.component == BX_NTVDM_DEM_COMPONENT_ORIGINAL_NOOP ?
            BX_NTVDM_DEM_PLANE_ORIGINAL_NOOP : BX_NTVDM_DEM_PLANE_DEFERRED);
    }
    {
        uint8_t bytes[4] = { 0xc4u, 0xc4u, 0x50u, 73u };
        bx_ntvdm_instruction_window_v1_capture(&window, bytes, 4u);
        failed |= !bx_ntvdm_bop_ingress_v1_classify(&window, &ingress);
        failed |= !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection);
        failed |= bx_ntvdm_dem_plane_v1_classify(&ingress, &selection, &record);
    }
    return failed ? 1 : 0;
}
