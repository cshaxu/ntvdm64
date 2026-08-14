#ifndef BX_NTVDM_DEM_PACKAGE_FACADE_V1_H
#define BX_NTVDM_DEM_PACKAGE_FACADE_V1_H

#include "bx_ntvdm_dem_provider_v1.h"

#define BX_NTVDM_DEM_PACKAGE_ROUTE_V1_MAGIC 0x42584631u
#define BX_NTVDM_DEM_PACKAGE_ROUTE_V1_VERSION 1u

typedef enum bx_ntvdm_dem_package_disposition_v1 {
    BX_NTVDM_DEM_PACKAGE_DEFERRED = 0u,
    BX_NTVDM_DEM_PACKAGE_ORIGINAL_NOOP = 1u,
    BX_NTVDM_DEM_PACKAGE_FASTREAD_COMPATIBILITY = 2u,
    BX_NTVDM_DEM_PACKAGE_EXISTING_PROVIDER = 3u,
    BX_NTVDM_DEM_PACKAGE_EXPLICIT_SOURCE_FAILURE = 4u
} bx_ntvdm_dem_package_disposition_v1;

/* One package admission record.  It chooses a disposition, never a legacy
 * dispatcher callback. */
typedef struct bx_ntvdm_dem_package_route_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    bx_ntvdm_dem_plane_record_v1 plane;
    uint32_t disposition;
    uint32_t reserved0;
} bx_ntvdm_dem_package_route_v1;

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_dem_package_route_v1_clear(bx_ntvdm_dem_package_route_v1 *route);
int bx_ntvdm_dem_package_route_v1_valid(const bx_ntvdm_dem_package_route_v1 *route);
int bx_ntvdm_dem_package_facade_v1_classify(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    bx_ntvdm_dem_package_route_v1 *route);
/* Only demNotYetImplemented is complete at this layer. Other records carry no
 * CPU or RAM result; the session selects their component provider or the
 * explicit source-derived CLI failure boundary. */
int bx_ntvdm_dem_package_facade_v1_dispatch(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_dem_package_route_v1 *route,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_cpu_result_v2 *result);

#ifdef __cplusplus
}
#endif

#endif
