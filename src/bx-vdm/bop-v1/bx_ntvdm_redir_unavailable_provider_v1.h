#ifndef BX_NTVDM_REDIR_UNAVAILABLE_PROVIDER_V1_H
#define BX_NTVDM_REDIR_UNAVAILABLE_PROVIDER_V1_H

#include "bx_ntvdm_bop_provider_registry_v1.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Reserved for a source-proven terminal branch of OpenNT MS_bop_7 after
 * VdmRedir failed to load. It provides no redirector operation or host
 * capability and has no currently admitted runtime caller. */
int bx_ntvdm_redir_unavailable_provider_v1_dispatch(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_cpu_result_v2 *result);

#ifdef __cplusplus
}
#endif

#endif
