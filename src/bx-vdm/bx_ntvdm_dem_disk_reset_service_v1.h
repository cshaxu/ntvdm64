#ifndef BX_NTVDM_DEM_DISK_RESET_SERVICE_V1_H
#define BX_NTVDM_DEM_DISK_RESET_SERVICE_V1_H
#include "bx_ntvdm_bop_ingress_v1.h"
#include "bx_ntvdm_cpu_result_v2.h"
#include "bx-mantle/bx_ntvdm_mechanical_action_v1.h"
int bx_ntvdm_dem_disk_reset_service_v1_prepare(uint64_t fd_access,
 const bx_ntvdm_bop_ingress_v1 *ingress,const bx_ntvdm_exception_event_v1 *event,
 const bx_ntvdm_cpu_state_v1 *cpu,uint32_t action_id,
 struct bx_ntvdm_mechanical_action_v1 *action,bx_ntvdm_cpu_result_v2 *result);
#endif