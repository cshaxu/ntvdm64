#include "bx_ntvdm_command_package_facade_v1.h"
#include <string.h>

static int existing_provider(uint8_t service)
{
    switch (service) {
    case 1u: case 2u: case 4u: case 5u: case 12u: case 13u: case 14u:
        return 1;
    default: return 0;
    }
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
    if (route->plane.disposition==BX_NTVDM_COMMAND_PLANE_ORIGINAL_NOOP)
        route->disposition=BX_NTVDM_COMMAND_PACKAGE_ORIGINAL_NOOP;
    else if (route->plane.disposition==BX_NTVDM_COMMAND_PLANE_EXPLICIT_UNAVAILABLE)
        route->disposition=BX_NTVDM_COMMAND_PACKAGE_EXPLICIT_UNAVAILABLE;
    else if (existing_provider((uint8_t)route->plane.service))
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
    if (route->disposition==BX_NTVDM_COMMAND_PACKAGE_ORIGINAL_NOOP)
        return bx_ntvdm_command_provider_v1_dispatch(ingress,selection,&route->plane,event,cpu_before,result);
    if (route->disposition==BX_NTVDM_COMMAND_PACKAGE_EXPLICIT_UNAVAILABLE)
        return bx_ntvdm_cpu_result_v2_stop(result);
    return 0;
}
