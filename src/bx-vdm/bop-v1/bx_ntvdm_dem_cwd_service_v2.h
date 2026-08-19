#ifndef BX_NTVDM_DEM_CWD_SERVICE_V2_H
#define BX_NTVDM_DEM_CWD_SERVICE_V2_H

#include "bop-v1/bx_ntvdm_dem_cwd_context_v1.h"
#include "bx_ntvdm_guest_gather_read_action_v1.h"
#include "bx_ntvdm_multi_write_transaction.h"

/* Source-derived shared ABI for demQueryCurrentDir (13h), demSetCurrentDir
 * (18h), and demSetDefaultDrive (1Ah).  Selector routing stays with DEM's
 * package plane; this service receives the already selected service byte. */
int bx_ntvdm_dem_cwd_service_v2_prepare(uint8_t service,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    bx_ntvdm_guest_gather_read_action_v1 *action);
int bx_ntvdm_dem_cwd_service_v2_complete(
    bx_ntvdm_dem_cwd_context_v1 *context,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t service,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_guest_gather_read_action_v1 *action,
    const uint8_t *bytes, uint64_t byte_count,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD]);

#endif
