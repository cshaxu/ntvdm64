#ifndef BX_NTVDM_BOP_OPENNT_DEMDISP_H
#define BX_NTVDM_BOP_OPENNT_DEMDISP_H

#include <stddef.h>
#include <stdint.h>

#include "../../../bx_ntvdm_cpu_result_v2.h"
#include "../../../bx_ntvdm_exception_abi.h"

typedef struct bx_ntvdm_opennt_dem_service_entry {
    uint8_t service;
    const char *handler;
    const char *owner_file;
} bx_ntvdm_opennt_dem_service_entry;

size_t bx_ntvdm_opennt_dem_service_count(void);
const bx_ntvdm_opennt_dem_service_entry *bx_ntvdm_opennt_dem_service_at(
    uint8_t service);
int bx_ntvdm_opennt_dem_not_yet_implemented(
    const bx_ntvdm_exception_event_v1 *boundary,
    bx_ntvdm_cpu_result_v2 *result);
int bx_ntvdm_opennt_dem_set_v86_kernel_addr(
    const bx_ntvdm_exception_event_v1 *boundary,
    bx_ntvdm_cpu_result_v2 *result);

#endif
