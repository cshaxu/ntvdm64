#include "session_resource_abi.h"

#include <string.h>

void runtime_session_resources_initialize(
    runtime_session_resources *resources)
{
    if (resources == 0) return;
    memset(resources, 0, sizeof(*resources));
    resources->magic = RUNTIME_SESSION_RESOURCE_ABI_MAGIC;
    resources->abi_version = RUNTIME_SESSION_RESOURCE_ABI_VERSION;
    resources->struct_bytes = sizeof(*resources);
}

int runtime_session_resources_valid(
    const runtime_session_resources *resources)
{
    unsigned int index;
    uint32_t expected_kind;
    if (resources == 0 || resources->magic != RUNTIME_SESSION_RESOURCE_ABI_MAGIC ||
        resources->abi_version != RUNTIME_SESSION_RESOURCE_ABI_VERSION ||
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
