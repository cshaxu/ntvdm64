#include "bx_ntvdm_command_bootstrap_provider_v1.h"
#include <string.h>

int bx_ntvdm_command_bootstrap_provider_v1_owns_service(uint8_t service)
{
    switch (service) {
    case 1u: case 2u: case 4u: case 5u: case 12u: case 13u: case 15u: case 16u:
        return 1;
    default:
        return 0;
    }
}

int bx_ntvdm_command_bootstrap_provider_v1_valid(
    const bx_ntvdm_command_bootstrap_provider_v1 *value)
{
    return value && value->magic == BX_NTVDM_COMMAND_BOOTSTRAP_PROVIDER_V1_MAGIC &&
        value->abi_version == BX_NTVDM_COMMAND_BOOTSTRAP_PROVIDER_V1_VERSION &&
        value->struct_bytes == sizeof(*value) && value->initialized == 1u &&
        bx_ntvdm_command_boot_input_v1_valid(&value->boot_input);
}

int bx_ntvdm_command_bootstrap_provider_v1_initialize(
    bx_ntvdm_command_bootstrap_provider_v1 *value,
    const bx_ntvdm_readonly_namespace_v1 *namespace_view)
{
    if (!value || !namespace_view) return 0;
    memset(value, 0, sizeof(*value));
    value->magic = BX_NTVDM_COMMAND_BOOTSTRAP_PROVIDER_V1_MAGIC;
    value->abi_version = BX_NTVDM_COMMAND_BOOTSTRAP_PROVIDER_V1_VERSION;
    value->struct_bytes = (uint32_t)sizeof(*value);
    bx_ntvdm_cmd_comspec_bootstrap_v1_initialize(&value->comspec);
    bx_ntvdm_cmd_get_next_state_v1_initialize(&value->get_next);
    bx_ntvdm_command_launch_plane_v1_clear(&value->set_info);
    if (!bx_ntvdm_command_boot_input_v1_initialize(&value->boot_input, namespace_view)) return 0;
    value->initialized = 1u;
    return bx_ntvdm_command_bootstrap_provider_v1_valid(value);
}
