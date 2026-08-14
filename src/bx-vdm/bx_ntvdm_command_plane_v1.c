#include "bx_ntvdm_command_plane_v1.h"
#include <string.h>

void bx_ntvdm_command_plane_v1_clear(bx_ntvdm_command_plane_record_v1 *r) {
    if (!r) return; memset(r, 0, sizeof(*r)); r->magic=BX_NTVDM_COMMAND_PLANE_V1_MAGIC;
    r->abi_version=BX_NTVDM_COMMAND_PLANE_V1_VERSION; r->struct_bytes=(uint32_t)sizeof(*r);
}
int bx_ntvdm_command_plane_v1_valid(const bx_ntvdm_command_plane_record_v1 *r) {
    return r && r->magic==BX_NTVDM_COMMAND_PLANE_V1_MAGIC && r->abi_version==1u &&
        r->struct_bytes==sizeof(*r) && r->service<17u &&
        r->component<=BX_NTVDM_COMMAND_COMPONENT_ORIGINAL_NOOP &&
        r->disposition<=BX_NTVDM_COMMAND_PLANE_ORIGINAL_NOOP && !r->reserved0;
}
static uint32_t component(uint8_t s) {
    switch(s) { case 0:return 1; case 1:case 2:case 4:case 5:return 2;
    case 12:case 13:return 3; case 6:case 7:case 8:case 10:case 11:return 4;
    case 9:case 14:return 5; case 15:return 6; case 16:return 7; case 3:return 8;
    default:return 0; }
}
int bx_ntvdm_command_plane_v1_classify(const bx_ntvdm_bop_ingress_v1 *i,
    const bx_ntvdm_bop_provider_selection_v1 *s, bx_ntvdm_command_plane_record_v1 *r) {
    uint32_t c; if(!r || !bx_ntvdm_bop_ingress_v1_valid(i) || !bx_ntvdm_bop_provider_selection_v1_valid(s)) return 0;
    bx_ntvdm_command_plane_v1_clear(r);
    if(i->route!=BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED || i->family!=BX_NTVDM_BOP_FAMILY_COMMAND ||
       s->disposition!=BX_NTVDM_BOP_PROVIDER_DEFERRED || s->provider_family!=BX_NTVDM_BOP_PROVIDER_COMMAND ||
       s->precedence!=BX_NTVDM_BOP_PROVIDER_PRECEDENCE_ORIGINAL_OPENNT || i->service>=17u || !(c=component(i->service))) return 0;
    r->service=i->service; r->component=c;
    r->disposition=c==BX_NTVDM_COMMAND_COMPONENT_ORIGINAL_NOOP ? BX_NTVDM_COMMAND_PLANE_ORIGINAL_NOOP :
        ((i->service==6u || i->service==8u || i->service==10u) ? BX_NTVDM_COMMAND_PLANE_EXPLICIT_UNAVAILABLE : BX_NTVDM_COMMAND_PLANE_DEFERRED);
    return bx_ntvdm_command_plane_v1_valid(r);
}
