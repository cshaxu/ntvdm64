#include "bx_ntvdm_session_resource_abi.h"

#include <string.h>

void bx_ntvdm_session_resources_v1_initialize(
    bx_ntvdm_session_resources_v1 *resources)
{
    if (resources == 0) return;
    memset(resources, 0, sizeof(*resources));
    resources->magic = BX_NTVDM_SESSION_RESOURCE_ABI_MAGIC;
    resources->abi_version = BX_NTVDM_SESSION_RESOURCE_ABI_VERSION;
    resources->struct_bytes = sizeof(*resources);
}

int bx_ntvdm_session_resources_v1_valid(
    const bx_ntvdm_session_resources_v1 *resources)
{
    unsigned int index;
    uint32_t expected_kind;
    if (resources == 0 || resources->magic != BX_NTVDM_SESSION_RESOURCE_ABI_MAGIC ||
        resources->abi_version != BX_NTVDM_SESSION_RESOURCE_ABI_VERSION ||
        resources->struct_bytes != sizeof(*resources) || resources->flags != 0u)
        return 0;
    for (index = 0u; index < 3u; ++index) {
        expected_kind = index + 1u;
        if (resources->resources[index].kind != expected_kind ||
            resources->resources[index].flags != 0u ||
            resources->resources[index].byte_count == 0u) return 0;
    }
    return 1;
}
