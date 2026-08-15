#include "bx_ntvdm_command_host_context_v1.h"

#include <string.h>

int bx_ntvdm_command_host_context_v1_valid(
    const bx_ntvdm_command_host_context_v1 *context)
{
    return context != 0 &&
        context->magic == BX_NTVDM_COMMAND_HOST_CONTEXT_V1_MAGIC &&
        context->abi_version == BX_NTVDM_COMMAND_HOST_CONTEXT_V1_VERSION &&
        context->struct_bytes == sizeof(*context) &&
        context->selected_drive < 26u && context->directory_bytes >= 4u &&
        context->directory_bytes <= BX_NTVDM_COMMAND_HOST_CONTEXT_V1_DIRECTORY_BYTES &&
        context->reserved0 == 0u &&
        context->selected_directory[0] == (uint8_t)('A' + context->selected_drive) &&
        context->selected_directory[1] == ':' && context->selected_directory[2] == '\\' &&
        context->selected_directory[context->directory_bytes - 1u] == '\0';
}

int bx_ntvdm_command_host_context_v1_initialize(
    bx_ntvdm_command_host_context_v1 *context, uint32_t selected_drive,
    const uint8_t *directory, uint32_t directory_bytes)
{
    if (context == 0 || directory == 0 || selected_drive >= 26u ||
        directory_bytes >= BX_NTVDM_COMMAND_HOST_CONTEXT_V1_DIRECTORY_BYTES)
        return 0;
    memset(context, 0, sizeof(*context));
    context->magic = BX_NTVDM_COMMAND_HOST_CONTEXT_V1_MAGIC;
    context->abi_version = BX_NTVDM_COMMAND_HOST_CONTEXT_V1_VERSION;
    context->struct_bytes = (uint32_t)sizeof(*context);
    context->selected_drive = selected_drive;
    context->directory_bytes = directory_bytes + 1u;
    memcpy(context->selected_directory, directory, directory_bytes);
    context->selected_directory[directory_bytes] = '\0';
    return bx_ntvdm_command_host_context_v1_valid(context);
}
