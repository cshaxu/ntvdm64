#include "bop_composition_v1.h"

#include "adapter-bop/bx_ntvdm_bop_ingress_v1.h"
#include "opennt-bop/ingress/opennt_bop_route.h"

int ntdos64_bop_composition_v1_bind(void)
{
    return bx_ntvdm_bop_ingress_v1_bind(
        bx_ntvdm_opennt_bop_route_dispatch_v1, 0);
}

void ntdos64_bop_composition_v1_reset(void)
{
    bx_ntvdm_bop_ingress_v1_unbind();
}
