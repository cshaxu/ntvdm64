#include "bx_ntvdm_xms_dpmi_plane_v1.h"
#include <string.h>

void bx_ntvdm_xms_dpmi_plane_v1_clear(bx_ntvdm_xms_dpmi_plane_record_v1 *r) {
    if (!r) return; memset(r, 0, sizeof(*r)); r->magic=BX_NTVDM_XMS_DPMI_PLANE_V1_MAGIC;
    r->abi_version=BX_NTVDM_XMS_DPMI_PLANE_V1_VERSION; r->struct_bytes=(uint32_t)sizeof(*r);
}
int bx_ntvdm_xms_dpmi_plane_v1_valid(const bx_ntvdm_xms_dpmi_plane_record_v1 *r) {
    return r && r->magic==BX_NTVDM_XMS_DPMI_PLANE_V1_MAGIC && r->abi_version==1u &&
        r->struct_bytes==sizeof(*r) && (r->provider_family==BX_NTVDM_BOP_PROVIDER_XMS ||
        r->provider_family==BX_NTVDM_BOP_PROVIDER_DPMI) && r->component &&
        r->component<=BX_NTVDM_XMS_DPMI_COMPONENT_VDD && r->machine_owner &&
        r->machine_owner<=BX_NTVDM_XMS_DPMI_MACHINE_OWNER_COMPOSITION &&
        r->disposition<=BX_NTVDM_XMS_DPMI_EXPLICIT_UNAVAILABLE;
}
static uint32_t xms_component(uint8_t n) { switch(n) {
case 0:return BX_NTVDM_XMS_DPMI_COMPONENT_A20; case 1:case 2:case 3:case 4:case 5:case 10:case 11:return BX_NTVDM_XMS_DPMI_COMPONENT_EXTENDED_MEMORY;
case 6:case 7:case 8:return BX_NTVDM_XMS_DPMI_COMPONENT_UMB; case 9:return BX_NTVDM_XMS_DPMI_COMPONENT_FIRMWARE_INTERRUPT; default:return 0; }}
static uint32_t dpmi_component(uint8_t n) { switch(n) {
case 0:case 1:case 3:case 4:case 5:case 15:case 17:return BX_NTVDM_XMS_DPMI_COMPONENT_DESCRIPTOR_MODE;
case 2:case 10:case 20:case 21:case 22:case 23:case 24:return BX_NTVDM_XMS_DPMI_COMPONENT_INTERRUPT_FAULT;
case 6:return BX_NTVDM_XMS_DPMI_COMPONENT_DOS_BRIDGE; case 7:case 8:case 9:case 11:case 16:case 19:return BX_NTVDM_XMS_DPMI_COMPONENT_EXTENDED_MEMORY;
case 12:case 13:return BX_NTVDM_XMS_DPMI_COMPONENT_SESSION; case 14:return BX_NTVDM_XMS_DPMI_COMPONENT_DEBUG; case 18:return BX_NTVDM_XMS_DPMI_COMPONENT_VDD; default:return 0; }}
static uint32_t owner(uint32_t c) { switch(c) {
case BX_NTVDM_XMS_DPMI_COMPONENT_A20:case BX_NTVDM_XMS_DPMI_COMPONENT_EXTENDED_MEMORY:case BX_NTVDM_XMS_DPMI_COMPONENT_UMB:return BX_NTVDM_XMS_DPMI_MACHINE_OWNER_BOCHS_MEMORY;
case BX_NTVDM_XMS_DPMI_COMPONENT_DESCRIPTOR_MODE:case BX_NTVDM_XMS_DPMI_COMPONENT_DEBUG:return BX_NTVDM_XMS_DPMI_MACHINE_OWNER_BOCHS_CPU;
case BX_NTVDM_XMS_DPMI_COMPONENT_FIRMWARE_INTERRUPT:case BX_NTVDM_XMS_DPMI_COMPONENT_INTERRUPT_FAULT:return BX_NTVDM_XMS_DPMI_MACHINE_OWNER_BOCHS_INTERRUPT;
default:return BX_NTVDM_XMS_DPMI_MACHINE_OWNER_COMPOSITION; }}
int bx_ntvdm_xms_dpmi_plane_v1_classify(const bx_ntvdm_bop_ingress_v1 *i,
    const bx_ntvdm_bop_provider_selection_v1 *s, bx_ntvdm_xms_dpmi_plane_record_v1 *r) {
    uint32_t c; if(!r || !bx_ntvdm_bop_ingress_v1_valid(i) || !bx_ntvdm_bop_provider_selection_v1_valid(s)) return 0;
    bx_ntvdm_xms_dpmi_plane_v1_clear(r);
    if(i->route!=BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED || s->disposition!=BX_NTVDM_BOP_PROVIDER_DEFERRED ||
       s->precedence!=BX_NTVDM_BOP_PROVIDER_PRECEDENCE_ORIGINAL_OPENNT ||
       !((i->family==BX_NTVDM_BOP_FAMILY_XMS && s->provider_family==BX_NTVDM_BOP_PROVIDER_XMS && i->service<12u) ||
         (i->family==BX_NTVDM_BOP_FAMILY_DPMI && s->provider_family==BX_NTVDM_BOP_PROVIDER_DPMI && i->service<25u))) return 0;
    c=i->family==BX_NTVDM_BOP_FAMILY_XMS?xms_component(i->service):dpmi_component(i->service); if(!c) return 0;
    r->service=i->service; r->provider_family=s->provider_family; r->component=c; r->machine_owner=owner(c);
    /* No DPMI component is admitted in the current CLI profile.  Preserve the
     * original service identity and mechanical owner, but make every selected
     * DPMI request terminal at the typed boundary rather than returning it to
     * the CPU as an unhandled #UD. */
    r->disposition=i->family==BX_NTVDM_BOP_FAMILY_DPMI ?
        BX_NTVDM_XMS_DPMI_EXPLICIT_UNAVAILABLE : BX_NTVDM_XMS_DPMI_DEFERRED;
    return bx_ntvdm_xms_dpmi_plane_v1_valid(r);
}
