#include "bx_ntvdm_command_launch_execution_provider_v1.h"
#include <string.h>

int bx_ntvdm_command_launch_execution_provider_v1_owns_service(uint8_t service)
{
    return service == 6u || service == 7u || service == 8u || service == 10u;
}
int bx_ntvdm_command_launch_execution_provider_v1_valid(
    const bx_ntvdm_command_launch_execution_provider_v1 *value)
{
    return value && value->magic == BX_NTVDM_COMMAND_LAUNCH_EXECUTION_PROVIDER_V1_MAGIC &&
        value->abi_version == BX_NTVDM_COMMAND_LAUNCH_EXECUTION_PROVIDER_V1_VERSION &&
        value->struct_bytes == sizeof(*value) && value->initialized == 1u &&
        bx_ntvdm_command_execution_lifecycle_v1_valid(&value->execution) &&
        bx_ntvdm_command_stream_child_v1_valid(&value->stream_child);
}
int bx_ntvdm_command_launch_execution_provider_v1_initialize(
    bx_ntvdm_command_launch_execution_provider_v1 *value)
{
    if (!value) return 0;
    memset(value, 0, sizeof(*value));
    value->magic = BX_NTVDM_COMMAND_LAUNCH_EXECUTION_PROVIDER_V1_MAGIC;
    value->abi_version = BX_NTVDM_COMMAND_LAUNCH_EXECUTION_PROVIDER_V1_VERSION;
    value->struct_bytes = (uint32_t)sizeof(*value);
    bx_ntvdm_command_execution_lifecycle_v1_initialize(&value->execution);
    bx_ntvdm_command_stream_child_v1_initialize(&value->stream_child);
    value->initialized = 1u;
    return bx_ntvdm_command_launch_execution_provider_v1_valid(value);
}

int bx_ntvdm_command_launch_execution_provider_v1_direct_allowed(
    const bx_ntvdm_command_launch_execution_provider_v1 *value,
    const bx_ntvdm_command_profile_consumer_v1 *profile)
{
    uint32_t policy = 0u;
    return bx_ntvdm_command_launch_execution_provider_v1_valid(value) &&
        profile != 0 && bx_ntvdm_command_profile_consumer_v1_resolve(profile,
            BX_NTVDM_MUTATION_CLASS_V1_HOST_GLOBAL, &policy) &&
        policy == BX_NTVDM_MUTATION_POLICY_V1_DIRECT_HOST;
}
