#include "bx_ntvdm_host_service_catalog.h"
#include "bx_ntvdm_bop_ingress_v1.h"

int bx_ntvdm_host_service_catalog_v1_lookup(
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_host_service_request_v1 *request)
{
    bx_ntvdm_bop_ingress_v1 ingress;

    if (request == 0) return 0;
    request->selector = 0u;
    request->service = 0u;
    request->family = BX_NTVDM_HOST_SERVICE_FAMILY_NONE;
    if (!bx_ntvdm_bop_ingress_v1_classify(window, &ingress) ||
        ingress.route != BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED) return 0;
    if (ingress.family == BX_NTVDM_BOP_FAMILY_DEM) {
        request->family = BX_NTVDM_HOST_SERVICE_FAMILY_DEM;
    } else if (ingress.family == BX_NTVDM_BOP_FAMILY_COMMAND) {
        request->family = BX_NTVDM_HOST_SERVICE_FAMILY_COMMAND;
    } else {
        return 0;
    }
    request->selector = ingress.selector;
    request->service = ingress.service;
    return 1;
}
