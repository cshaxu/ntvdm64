#include "bx_ntvdm_system_plane_v1.h"
#include <string.h>

void bx_ntvdm_system_plane_v1_clear(bx_ntvdm_system_plane_record_v1 *r) {
    if (!r) return; memset(r,0,sizeof(*r)); r->magic=BX_NTVDM_SYSTEM_PLANE_V1_MAGIC;
    r->abi_version=BX_NTVDM_SYSTEM_PLANE_V1_VERSION; r->struct_bytes=(uint32_t)sizeof(*r);
}
int bx_ntvdm_system_plane_v1_valid(const bx_ntvdm_system_plane_record_v1 *r) {
    return r && r->magic==BX_NTVDM_SYSTEM_PLANE_V1_MAGIC && r->abi_version==1u &&
        r->struct_bytes==sizeof(*r) && (r->provider_family==BX_NTVDM_BOP_PROVIDER_REDIR || r->provider_family==BX_NTVDM_BOP_PROVIDER_DEBUGGER) &&
        r->component>=BX_NTVDM_SYSTEM_COMPONENT_REDIR_LIFECYCLE && r->component<=BX_NTVDM_SYSTEM_COMPONENT_DEBUG_ATTACH &&
        r->disposition<=BX_NTVDM_SYSTEM_EXPLICIT_UNAVAILABLE && !r->reserved0;
}
static uint32_t redir_component(uint8_t n) { if(n<2)return 1; if(n<9)return 2; if(n<15)return 3; if(n==15)return 1;
    if(n>=16&&n<=26)return 4; if(n>=27&&n<=34)return 5; if(n>=35&&n<=40)return 6; return 7; }
static uint32_t debug_component(uint8_t n) { if(n<5)return 8; if(n<10)return 9; if(n<14)return 10; return 11; }
int bx_ntvdm_system_plane_v1_classify(const bx_ntvdm_bop_ingress_v1 *i,
    const bx_ntvdm_bop_provider_selection_v1 *s, bx_ntvdm_system_plane_record_v1 *r) {
    if(!r || !bx_ntvdm_bop_ingress_v1_valid(i) || !bx_ntvdm_bop_provider_selection_v1_valid(s)) return 0;
    bx_ntvdm_system_plane_v1_clear(r);
    if(i->route!=BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED || s->disposition!=BX_NTVDM_BOP_PROVIDER_DEFERRED ||
       s->precedence!=BX_NTVDM_BOP_PROVIDER_PRECEDENCE_ORIGINAL_OPENNT ||
       !((i->family==BX_NTVDM_BOP_FAMILY_REDIR && s->provider_family==BX_NTVDM_BOP_PROVIDER_REDIR && i->service<49u) ||
         (i->family==BX_NTVDM_BOP_FAMILY_DEBUGGER && s->provider_family==BX_NTVDM_BOP_PROVIDER_DEBUGGER && i->service<16u))) return 0;
    r->service=i->service; r->provider_family=s->provider_family;
    r->component=i->family==BX_NTVDM_BOP_FAMILY_REDIR?redir_component(i->service):debug_component(i->service);
    r->disposition=i->family==BX_NTVDM_BOP_FAMILY_REDIR?BX_NTVDM_SYSTEM_EXPLICIT_UNAVAILABLE:BX_NTVDM_SYSTEM_DEFERRED;
    return bx_ntvdm_system_plane_v1_valid(r);
}
