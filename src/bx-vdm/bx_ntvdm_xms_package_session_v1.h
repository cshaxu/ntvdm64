#ifndef BX_NTVDM_XMS_PACKAGE_SESSION_V1_H
#define BX_NTVDM_XMS_PACKAGE_SESSION_V1_H

#include "bx_ntvdm_xms_dpmi_plane_v1.h"

#define BX_NTVDM_XMS_PACKAGE_SESSION_V1_MAGIC 0x42585853u
#define BX_NTVDM_XMS_PACKAGE_SESSION_V1_VERSION 1u
#define BX_NTVDM_XMS_PACKAGE_SESSION_V1_SLOTS 32u

typedef struct bx_ntvdm_xms_package_allocation_v1 {
    uint32_t mantle_handle, kib;
    uint16_t base_kib, reserved0;
} bx_ntvdm_xms_package_allocation_v1;

/* Adapter-owned translation from original XMS KiB base values to opaque
 * mantle handles.  The mantle remains owner of allocation/move mechanics. */
typedef struct bx_ntvdm_xms_package_session_v1 {
    uint32_t magic, abi_version, struct_bytes, initialized;
    uint32_t reserved_mantle_handle;
    bx_ntvdm_xms_package_allocation_v1 allocations[BX_NTVDM_XMS_PACKAGE_SESSION_V1_SLOTS];
} bx_ntvdm_xms_package_session_v1;

#ifdef __cplusplus
extern "C" {
#endif
int bx_ntvdm_xms_package_session_v1_initialize(bx_ntvdm_xms_package_session_v1 *session);
int bx_ntvdm_xms_package_session_v1_valid(const bx_ntvdm_xms_package_session_v1 *session);
int bx_ntvdm_xms_package_session_v1_dispatch(bx_ntvdm_xms_package_session_v1 *session,
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);
#ifdef __cplusplus
}
#endif

#endif
