#ifndef BX_NTVDM_MACHINE_BOP_FACADE_V1_H
#define BX_NTVDM_MACHINE_BOP_FACADE_V1_H

#include "bx_ntvdm_bop_provider_registry_v1.h"

typedef enum bx_ntvdm_machine_bop_route_v1 {
    BX_NTVDM_MACHINE_BOP_NONE = 0u,
    BX_NTVDM_MACHINE_BOP_MEMORY = 1u,
    BX_NTVDM_MACHINE_BOP_HANDOFF = 2u,
    BX_NTVDM_MACHINE_BOP_DEFERRED_STOP = 3u
} bx_ntvdm_machine_bop_route_v1;

#ifdef __cplusplus
extern "C" {
#endif
int bx_ntvdm_machine_bop_facade_v1_classify(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection, uint32_t *route);
int bx_ntvdm_machine_bop_facade_v1_dispatch(uint32_t route,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before, bx_ntvdm_cpu_result_v2 *result);
#ifdef __cplusplus
}
#endif
#endif
