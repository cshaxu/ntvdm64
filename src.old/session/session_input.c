#include "session_input.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

static int replace_multisz(char **destination, uint32_t *destination_bytes,
    const char *source, uint32_t bytes)
{
    char *replacement;
    if (destination == NULL || destination_bytes == NULL || source == NULL ||
        bytes < 2u || bytes > UINT16_MAX || source[bytes - 2u] != '\0' ||
        source[bytes - 1u] != '\0') return 0;
    replacement = (char *)malloc(bytes);
    if (replacement == NULL) return 0;
    memcpy(replacement, source, bytes);
    free(*destination);
    *destination = replacement;
    *destination_bytes = bytes;
    return 1;
}

void session_input_initialize(session_input *input)
{
    if (input == NULL) return;
    memset(input, 0, sizeof(*input));
    input->magic = SESSION_INPUT_MAGIC;
    input->abi_version = SESSION_INPUT_VERSION;
    input->struct_bytes = (uint32_t)sizeof(*input);
}

void session_input_dispose(session_input *input)
{
    if (!session_input_valid(input)) return;
    free(input->environment);
    free(input->transformed_environment);
    free(input->published_directories);
    session_input_initialize(input);
}

int session_input_valid(const session_input *input)
{
    return input != NULL && input->magic == SESSION_INPUT_MAGIC &&
        input->abi_version == SESSION_INPUT_VERSION &&
        input->struct_bytes == sizeof(*input) &&
        input->environment_bytes <= UINT16_MAX &&
        input->transformed_environment_bytes <= UINT16_MAX &&
        input->published_directories_bytes <= UINT16_MAX;
}

int session_input_set_startup(session_input *input, const char *target,
    const char *arguments, uint16_t location, uint16_t text_code_page)
{
    size_t target_bytes, argument_bytes;
    if (!session_input_valid(input) || target == NULL || arguments == NULL ||
        input->ready != 0u) return 0;
    target_bytes = strlen(target);
    argument_bytes = strlen(arguments);
    if (target_bytes == 0u || target_bytes >= sizeof(input->target) ||
        argument_bytes >= sizeof(input->arguments)) return 0;
    memcpy(input->target, target, target_bytes + 1u);
    memcpy(input->arguments, arguments, argument_bytes + 1u);
    input->location = location;
    input->text_code_page = text_code_page;
    input->ready = 1u;
    return 1;
}

int session_input_set_environment(session_input *input, const char *environment,
    uint32_t bytes)
{
    return session_input_valid(input) && replace_multisz(&input->environment,
        &input->environment_bytes, environment, bytes);
}

int session_input_set_transformed_environment(session_input *input,
    const char *environment, uint32_t bytes)
{
    return session_input_valid(input) && replace_multisz(
        &input->transformed_environment, &input->transformed_environment_bytes,
        environment, bytes);
}

int session_input_set_published_directories(session_input *input,
    const char *directories, uint32_t bytes)
{
    return session_input_valid(input) && replace_multisz(
        &input->published_directories, &input->published_directories_bytes,
        directories, bytes);
}
