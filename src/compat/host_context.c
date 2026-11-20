#include "include/host_context.h"
#include <string.h>
#include <stdlib.h>

static __declspec(thread) compat_host_context *current_context;

int compat_host_context_valid(const compat_host_context *context)
{
    return context != NULL && context->cleanup_count <= COMPAT_HOST_CLEANUP_LIMIT;
}

compat_host_context *__cdecl compat_host_context_current(void)
{
    return current_context;
}

int compat_host_context_bind(compat_host_context *context)
{
    if (!compat_host_context_valid(context) || current_context != NULL) return 0;
    current_context = context;
    return 1;
}

int compat_host_context_unbind(compat_host_context *context)
{
    if (context == NULL || current_context != context) return 0;
    current_context = NULL;
    return 1;
}

int compat_host_context_register(compat_host_context *context,
    compat_host_cleanup_fn function, void *argument)
{
    uint32_t index;
    if (!compat_host_context_valid(context) || context->state != COMPAT_HOST_CONTEXT_ACTIVE || function == NULL)
        return 0;
    for (index = 0u; index < context->cleanup_count; ++index) {
        if (context->cleanups[index].function == function &&
            context->cleanups[index].context == argument) return 1;
    }
    if (context->cleanup_count >= COMPAT_HOST_CLEANUP_LIMIT) return 0;
    context->cleanups[context->cleanup_count].function = function;
    context->cleanups[context->cleanup_count].context = argument;
    ++context->cleanup_count;
    return 1;
}

void compat_host_context_dispose(compat_host_context *context)
{
    uint32_t index;
    for (index = context->cleanup_count; index != 0u; --index) {
        compat_host_cleanup cleanup = context->cleanups[index - 1u];
        cleanup.function(cleanup.context);
    }
    compat_host_text_clear(context);
    memset(context, 0, sizeof(*context));
}

int compat_host_text_acquire_writable(compat_host_context *instance,
    uint32_t columns, uint32_t rows, uint32_t cell_bytes, uint8_t **bytes_out)
{
    uint64_t required;
    uint8_t *replacement;

    if (bytes_out != NULL) *bytes_out = NULL;
    if (!compat_host_context_valid(instance) || instance->state != COMPAT_HOST_CONTEXT_ACTIVE ||
        bytes_out == NULL || columns == 0u || rows == 0u ||
        (cell_bytes != 2u && cell_bytes != 4u)) return 0;
    required = (uint64_t)columns * (uint64_t)rows * cell_bytes;
    if (required == 0u || required > UINT32_MAX) return 0;
    if (instance->presentation_text_storage != NULL &&
        instance->presentation_text_columns == columns &&
        instance->presentation_text_rows == rows &&
        instance->presentation_text_bytes == required) {
        *bytes_out = instance->presentation_text_storage;
        return 1;
    }
    replacement = (uint8_t *)calloc(1u, (size_t)required);
    if (replacement == NULL) return 0;
    compat_host_text_clear(instance);
    instance->presentation_text_storage = replacement;
    instance->presentation_text_columns = columns;
    instance->presentation_text_rows = rows;
    instance->presentation_text_bytes = (uint32_t)required;
    *bytes_out = replacement;
    return 1;
}

int compat_host_text_snapshot(const compat_host_context *instance,
    uint8_t *destination, uint32_t destination_bytes, uint32_t *columns_out,
    uint32_t *rows_out, uint32_t *bytes_out)
{
    if (columns_out != NULL) *columns_out = 0u;
    if (rows_out != NULL) *rows_out = 0u;
    if (bytes_out != NULL) *bytes_out = 0u;
    if (!compat_host_context_valid(instance) || destination == NULL ||
        instance->presentation_text_storage == NULL ||
        destination_bytes < instance->presentation_text_bytes) return 0;
    memcpy(destination, instance->presentation_text_storage,
        instance->presentation_text_bytes);
    if (columns_out != NULL) *columns_out = instance->presentation_text_columns;
    if (rows_out != NULL) *rows_out = instance->presentation_text_rows;
    if (bytes_out != NULL) *bytes_out = instance->presentation_text_bytes;
    return 1;
}

int compat_host_text_describe(const compat_host_context *instance,
    uint32_t *columns_out, uint32_t *rows_out, uint32_t *bytes_out)
{
    if (columns_out != NULL) *columns_out = 0u;
    if (rows_out != NULL) *rows_out = 0u;
    if (bytes_out != NULL) *bytes_out = 0u;
    if (!compat_host_context_valid(instance) || instance->presentation_text_storage == NULL)
        return 0;
    if (columns_out != NULL) *columns_out = instance->presentation_text_columns;
    if (rows_out != NULL) *rows_out = instance->presentation_text_rows;
    if (bytes_out != NULL) *bytes_out = instance->presentation_text_bytes;
    return 1;
}

void compat_host_text_clear(compat_host_context *instance)
{
    if (instance == NULL) return;
    free(instance->presentation_text_storage);
    instance->presentation_text_storage = NULL;
    instance->presentation_text_columns = 0u;
    instance->presentation_text_rows = 0u;
    instance->presentation_text_bytes = 0u;
}
