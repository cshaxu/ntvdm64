#include "include/host_context.h"
#include <string.h>

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
    if (!compat_host_context_valid(context) || !context->registration_open || function == NULL)
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
    memset(context, 0, sizeof(*context));
}
