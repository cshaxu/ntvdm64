#include "bx_ntvdm_command_host_context_v1.h"

#include <string.h>

static int environment_valid(const uint8_t *environment, uint32_t bytes)
{
    uint32_t offset = 0u;
    if (bytes == 0u) return 1;
    if (environment == 0 || bytes < 2u ||
        bytes > BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES ||
        environment[bytes - 1u] != 0u || environment[bytes - 2u] != 0u)
        return 0;
    while (offset < bytes - 1u) {
        uint32_t start = offset;
        while (offset < bytes && environment[offset] != 0u) {
            ++offset;
        }
        if (offset == start) return offset == bytes - 1u;
        ++offset;
    }
    return offset == bytes - 1u;
}

int bx_ntvdm_command_host_context_v1_valid(
    const bx_ntvdm_command_host_context_v1 *context)
{
    return context != 0 &&
        context->magic == BX_NTVDM_COMMAND_HOST_CONTEXT_V1_MAGIC &&
        context->abi_version == BX_NTVDM_COMMAND_HOST_CONTEXT_V1_VERSION &&
        context->struct_bytes == sizeof(*context) &&
        context->selected_drive < 26u && context->directory_bytes >= 4u &&
        context->directory_bytes <= BX_NTVDM_COMMAND_HOST_CONTEXT_V1_DIRECTORY_BYTES &&
        context->selected_directory[0] == (uint8_t)('A' + context->selected_drive) &&
        context->selected_directory[1] == ':' && context->selected_directory[2] == '\\' &&
        context->selected_directory[context->directory_bytes - 1u] == '\0' &&
        context->processor_bytes <= BX_NTVDM_COMMAND_HOST_CONTEXT_V1_PROCESSOR_BYTES &&
        (context->processor_bytes == 0u ||
         (context->processor_bytes >= 2u &&
          context->processor[context->processor_bytes - 1u] == '\0')) &&
        environment_valid(context->environment, context->environment_bytes);
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

int bx_ntvdm_command_host_context_v1_set_environment(
    bx_ntvdm_command_host_context_v1 *context, const uint8_t *environment,
    uint32_t environment_bytes)
{
    if (!bx_ntvdm_command_host_context_v1_valid(context) ||
        !environment_valid(environment, environment_bytes)) return 0;
    memset(context->environment, 0, sizeof(context->environment));
    if (environment_bytes != 0u)
        memcpy(context->environment, environment, environment_bytes);
    context->environment_bytes = environment_bytes;
    return bx_ntvdm_command_host_context_v1_valid(context);
}

int bx_ntvdm_command_host_context_v1_set_processor(
    bx_ntvdm_command_host_context_v1 *context, const uint8_t *processor,
    uint32_t processor_bytes)
{
    if (!bx_ntvdm_command_host_context_v1_valid(context) || processor == 0 ||
        processor_bytes < 2u ||
        processor_bytes > BX_NTVDM_COMMAND_HOST_CONTEXT_V1_PROCESSOR_BYTES ||
        processor[processor_bytes - 1u] != '\0') return 0;
    memset(context->processor, 0, sizeof(context->processor));
    memcpy(context->processor, processor, processor_bytes);
    context->processor_bytes = processor_bytes;
    return bx_ntvdm_command_host_context_v1_valid(context);
}
