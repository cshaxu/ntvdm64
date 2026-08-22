#ifndef BX_NTVDM_HOST_SERVICE_CONTRACT_H
#define BX_NTVDM_HOST_SERVICE_CONTRACT_H

#include <stdint.h>

/*
 * Fixed-width transaction classification shared by the current mechanical
 * transaction ABI and the current OpenNT-derived BOP routes.  This record is
 * deliberately selector-neutral: it carries no provider, catalog, or legacy
 * composition dependency.  The retained `_v1` suffix denotes the frozen wire
 * layout, not a provider implementation selection.
 */
typedef enum bx_ntvdm_host_service_family_v1 {
    BX_NTVDM_HOST_SERVICE_FAMILY_NONE = 0,
    BX_NTVDM_HOST_SERVICE_FAMILY_DEM = 1,
    BX_NTVDM_HOST_SERVICE_FAMILY_COMMAND = 2
} bx_ntvdm_host_service_family_v1;

typedef struct bx_ntvdm_host_service_request_v1 {
    uint8_t selector;
    uint8_t service;
    uint16_t family;
} bx_ntvdm_host_service_request_v1;

#endif
