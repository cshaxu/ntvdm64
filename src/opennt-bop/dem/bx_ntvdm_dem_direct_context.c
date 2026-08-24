#include "bx_ntvdm_dem_direct_context.h"

int runtime_dem_direct_context_valid(
    const runtime_dem_direct_context *context)
{
    return context != 0 &&
        context->magic == RUNTIME_DEM_DIRECT_CONTEXT_MAGIC &&
        context->abi_version == RUNTIME_DEM_DIRECT_CONTEXT_VERSION &&
        context->struct_bytes == sizeof(*context) && context->reserved0 == 0u &&
        context->state != 0 && context->publish_handle != 0 &&
        context->lookup_handle != 0 &&
        context->release_handle != 0 && context->query_attributes != 0 &&
        context->set_attributes != 0;
}
