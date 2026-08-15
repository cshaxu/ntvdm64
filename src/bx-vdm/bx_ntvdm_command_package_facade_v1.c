#include "bx_ntvdm_command_package_facade_v1.h"
#include <string.h>

static int existing_provider(uint8_t service)
{
    /* T202/S6 selects the COMMAND session as the one package owner for every
     * callable cmddisp.c slot.  Per-service capability disposition happens
     * inside that session; no service may be intercepted by a detached
     * facade leaf before the provider can record its source-shaped outcome. */
    return service < 17u;
}
void bx_ntvdm_command_package_route_v1_clear(bx_ntvdm_command_package_route_v1 *route)
{
    if (!route) return;
    memset(route, 0, sizeof(*route));
    route->magic=BX_NTVDM_COMMAND_PACKAGE_ROUTE_V1_MAGIC;
    route->abi_version=BX_NTVDM_COMMAND_PACKAGE_ROUTE_V1_VERSION;
    route->struct_bytes=(uint32_t)sizeof(*route);
    bx_ntvdm_command_plane_v1_clear(&route->plane);
}
int bx_ntvdm_command_package_route_v1_valid(const bx_ntvdm_command_package_route_v1 *route)
{
    return route && route->magic==BX_NTVDM_COMMAND_PACKAGE_ROUTE_V1_MAGIC &&
        route->abi_version==BX_NTVDM_COMMAND_PACKAGE_ROUTE_V1_VERSION &&
        route->struct_bytes==sizeof(*route) && bx_ntvdm_command_plane_v1_valid(&route->plane) &&
        route->disposition<=BX_NTVDM_COMMAND_PACKAGE_EXISTING_PROVIDER && !route->reserved0;
}
int bx_ntvdm_command_package_facade_v1_classify(const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection, bx_ntvdm_command_package_route_v1 *route)
{
    if (!route) return 0;
    bx_ntvdm_command_package_route_v1_clear(route);
    if (!bx_ntvdm_command_plane_v1_classify(ingress, selection, &route->plane)) return 0;
    if (existing_provider((uint8_t)route->plane.service))
        route->disposition=BX_NTVDM_COMMAND_PACKAGE_EXISTING_PROVIDER;
    else route->disposition=BX_NTVDM_COMMAND_PACKAGE_DEFERRED;
    return bx_ntvdm_command_package_route_v1_valid(route);
}
int bx_ntvdm_command_package_facade_v1_dispatch(const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection, const bx_ntvdm_command_package_route_v1 *route,
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_cpu_result_v2 *result)
{
    if (!result || !bx_ntvdm_command_package_route_v1_valid(route)) return 0;
    bx_ntvdm_cpu_result_v2_pass_through(result);
    (void)ingress; (void)selection; (void)event; (void)cpu_before;
    return 0;
}
