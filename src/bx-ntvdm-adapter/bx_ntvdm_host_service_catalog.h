#ifndef BX_NTVDM_HOST_SERVICE_CATALOG_H
#define BX_NTVDM_HOST_SERVICE_CATALOG_H

#include <stdint.h>

#include "bx_ntvdm_instruction_window_abi.h"

typedef enum bx_ntvdm_host_service_family_v1 {
    BX_NTVDM_HOST_SERVICE_FAMILY_NONE = 0u,
    BX_NTVDM_HOST_SERVICE_FAMILY_DEM = 1u,
    BX_NTVDM_HOST_SERVICE_FAMILY_COMMAND = 2u
} bx_ntvdm_host_service_family_v1;

typedef struct bx_ntvdm_host_service_request_v1 {
    uint8_t selector;
    uint8_t service;
    uint16_t family;
} bx_ntvdm_host_service_request_v1;

#ifdef __cplusplus
extern "C" {
#endif

/* Recognizes only catalogued historical host-service ranges. It performs no
 * dispatch, state mutation, guest-memory read, host I/O, or IP advance. */
int bx_ntvdm_host_service_catalog_v1_lookup(
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_host_service_request_v1 *request);

#ifdef __cplusplus
}
#endif

#endif
