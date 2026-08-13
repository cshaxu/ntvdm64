#include "bx_ntvdm_legacy_plane_gate_v1.h"
#include "bx_ntvdm_bop_provider_registry_v1.h"
#include "bx_ntvdm_dem_plane_v1.h"
#include "bx_ntvdm_command_plane_v1.h"

int bx_ntvdm_legacy_plane_gate_v1_dem(const bx_ntvdm_instruction_window_v1 *w,
    uint8_t expected) {
    bx_ntvdm_bop_ingress_v1 i; bx_ntvdm_bop_provider_selection_v1 s;
    bx_ntvdm_dem_plane_record_v1 p;
    return bx_ntvdm_bop_ingress_v1_classify(w,&i) && bx_ntvdm_bop_provider_registry_v1_select(&i,&s) &&
        bx_ntvdm_dem_plane_v1_classify(&i,&s,&p) && p.service==expected;
}
int bx_ntvdm_legacy_plane_gate_v1_command(const bx_ntvdm_instruction_window_v1 *w,
    uint8_t expected) {
    bx_ntvdm_bop_ingress_v1 i; bx_ntvdm_bop_provider_selection_v1 s;
    bx_ntvdm_command_plane_record_v1 p;
    return bx_ntvdm_bop_ingress_v1_classify(w,&i) && bx_ntvdm_bop_provider_registry_v1_select(&i,&s) &&
        bx_ntvdm_command_plane_v1_classify(&i,&s,&p) && p.service==expected;
}
