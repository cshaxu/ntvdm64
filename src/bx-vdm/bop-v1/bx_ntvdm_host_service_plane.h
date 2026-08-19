#ifndef BX_NTVDM_HOST_SERVICE_PLANE_H
#define BX_NTVDM_HOST_SERVICE_PLANE_H

#include <stdint.h>

#include "bx_ntvdm_host_transaction_abi.h"
#include "bx_ntvdm_session_resource_abi.h"

typedef enum bx_ntvdm_host_service_plane_result_v1 {
    BX_NTVDM_HOST_SERVICE_PLANE_UNHANDLED = 0u,
    BX_NTVDM_HOST_SERVICE_PLANE_READY = 1u,
    BX_NTVDM_HOST_SERVICE_PLANE_REJECTED = 2u
} bx_ntvdm_host_service_plane_result_v1;

/* Same-island staging only. This structure is not a Bochs/guest ABI. */
typedef struct bx_ntvdm_host_service_payloads_v1 {
    const uint8_t *ntio_bytes;
    uint64_t ntio_byte_count;
    const uint8_t *ntdos_bytes;
    uint64_t ntdos_byte_count;
    const uint8_t *command_bytes;
    uint64_t command_byte_count;
} bx_ntvdm_host_service_payloads_v1;

#ifdef __cplusplus
extern "C" {
#endif

bx_ntvdm_host_service_plane_result_v1 bx_ntvdm_host_service_plane_v1_prepare(
    const bx_ntvdm_host_service_request_v1 *request,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_session_resources_v1 *resources,
    const bx_ntvdm_host_service_payloads_v1 *payloads,
    uint64_t aperture_bytes,
    bx_ntvdm_host_transaction_v1 *transaction);

#ifdef __cplusplus
}
#endif

#endif
