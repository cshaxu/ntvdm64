#include "bx_ntvdm_command_provider_v1.h"
#include <stdint.h>
int bx_ntvdm_command_provider_v1_dispatch(const bx_ntvdm_bop_ingress_v1 *i,
    const bx_ntvdm_bop_provider_selection_v1 *s, const bx_ntvdm_command_plane_record_v1 *p,
    const bx_ntvdm_exception_event_v1 *e, const bx_ntvdm_cpu_state_v1 *c, bx_ntvdm_cpu_result_v2 *r) {
    if(!r || !bx_ntvdm_bop_ingress_v1_valid(i) || !bx_ntvdm_bop_provider_selection_v1_valid(s) ||
       !bx_ntvdm_command_plane_v1_valid(p) || !bx_ntvdm_exception_event_v1_valid(e) || !bx_ntvdm_cpu_state_v1_valid(c)) return 0;
    bx_ntvdm_cpu_result_v2_pass_through(r);
    if(i->route!=BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED || i->family!=BX_NTVDM_BOP_FAMILY_COMMAND || i->service!=3u ||
       s->disposition!=BX_NTVDM_BOP_PROVIDER_DEFERRED || s->provider_family!=BX_NTVDM_BOP_PROVIDER_COMMAND ||
       s->precedence!=BX_NTVDM_BOP_PROVIDER_PRECEDENCE_ORIGINAL_OPENNT || p->service!=3u ||
       p->component!=BX_NTVDM_COMMAND_COMPONENT_ORIGINAL_NOOP || p->disposition!=BX_NTVDM_COMMAND_PLANE_ORIGINAL_NOOP ||
       e->kind!=BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION || e->vector!=6u || e->fault_rip>UINT64_MAX-4u) return 0;
    return bx_ntvdm_cpu_result_v2_resume(r,e->fault_rip+4u);
}
