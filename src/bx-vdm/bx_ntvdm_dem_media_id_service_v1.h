#ifndef BX_NTVDM_DEM_MEDIA_ID_SERVICE_V1_H
#define BX_NTVDM_DEM_MEDIA_ID_SERVICE_V1_H
#include "bx_ntvdm_host_volume_snapshot_v1.h"
#include "bx_ntvdm_multi_write_transaction.h"
int bx_ntvdm_dem_media_id_service_v1_prepare(const bx_ntvdm_host_volume_snapshot_v1 *,const bx_ntvdm_exception_event_v1 *,const bx_ntvdm_cpu_state_v1 *,const bx_ntvdm_instruction_window_v1 *,bx_ntvdm_multi_write_transaction_v1 *,uint8_t[23]);
int bx_ntvdm_dem_media_id_service_v1_snapshot_failure(const bx_ntvdm_host_volume_snapshot_v1 *,const bx_ntvdm_exception_event_v1 *,const bx_ntvdm_cpu_state_v1 *,const bx_ntvdm_instruction_window_v1 *,bx_ntvdm_cpu_result_v2 *);
int bx_ntvdm_dem_media_id_service_v1_set_failure(const bx_ntvdm_exception_event_v1 *,const bx_ntvdm_cpu_state_v1 *,const bx_ntvdm_instruction_window_v1 *,bx_ntvdm_cpu_result_v2 *);
#endif
