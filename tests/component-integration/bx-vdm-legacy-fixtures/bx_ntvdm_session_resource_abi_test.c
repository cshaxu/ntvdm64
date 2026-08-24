#include "session_resource_abi.h"

int main(void)
{
    runtime_session_resources_v1 resources;
    unsigned int index;

    runtime_session_resources_v1_initialize(&resources);
    if (sizeof(resources) != 160u ||
        runtime_session_resources_v1_valid(&resources)) return 1;
    for (index = 0u; index < 3u; ++index) {
        resources.resources[index].kind = index + 1u;
        resources.resources[index].byte_count = 1u;
    }
    if (!runtime_session_resources_v1_valid(&resources)) return 2;
    resources.resources[1].kind = RUNTIME_SESSION_RESOURCE_NTIO;
    if (runtime_session_resources_v1_valid(&resources)) return 3;
    resources.resources[1].kind = RUNTIME_SESSION_RESOURCE_NTDOS;
    resources.resources[2].flags = 1u;
    if (runtime_session_resources_v1_valid(&resources)) return 4;
    return runtime_session_resources_v1_valid(0) ? 5 : 0;
}
