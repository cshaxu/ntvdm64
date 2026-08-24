#include "bop_composition.h"

#include "adapter-bop/bop_ingress.h"
#include "opennt-bop/ingress/opennt_bop_route.h"

int app_bop_composition_v1_bind(void)
{
    return runtime_bop_ingress_v1_bind(
        runtime_opennt_bop_route_dispatch_v1, 0);
}

void app_bop_composition_v1_reset(void)
{
    runtime_bop_ingress_v1_unbind();
}
