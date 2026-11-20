#include "include/host_context.h"
#include <intrin.h>
#include <string.h>
#include <stdlib.h>

static __declspec(thread) compat_host_context *current_context;
static __declspec(thread) compat_host_context *escape_context;
static __declspec(thread) uint32_t thread_owner;
static __declspec(thread) int thread_finishing;

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

uint32_t compat_host_thread_count(const compat_host_context *context)
{
    return (uint32_t)_InterlockedCompareExchange((volatile long *)&context->threads.count, 0, 0);
}

uint32_t compat_host_thread_owner_count(const compat_host_context *context, uint32_t owner)
{
    if (owner > COMPAT_HOST_THREAD_ORIGINAL_WORKER) return 0;
    return (uint32_t)_InterlockedCompareExchange(
        (volatile long *)&context->threads.owner_count[owner], 0, 0);
}

int compat_host_thread_register(compat_host_context *context,
    compat_host_thread_hook_fn bind, compat_host_thread_hook_fn unbind, void *argument)
{
    uint32_t index;
    if (!compat_host_context_valid(context) || context->state != COMPAT_HOST_CONTEXT_ACTIVE ||
        context->threads.hook_count > COMPAT_HOST_THREAD_HOOK_LIMIT ||
        !bind || !unbind) return 0;
    for (index = 0; index < context->threads.hook_count; ++index) {
        compat_host_thread_hook *hook = &context->threads.hooks[index];
        if (hook->bind == bind && hook->unbind == unbind && hook->context == argument)
            return 1;
    }
    if (context->threads.hook_count == COMPAT_HOST_THREAD_HOOK_LIMIT) return 0;
    context->threads.hooks[index].bind = bind;
    context->threads.hooks[index].unbind = unbind;
    context->threads.hooks[index].context = argument;
    ++context->threads.hook_count;
    return 1;
}

int compat_host_thread_unregister(compat_host_context *context,
    compat_host_thread_hook_fn bind, compat_host_thread_hook_fn unbind, void *argument)
{
    uint32_t index;
    if (!compat_host_context_valid(context) ||
        context->threads.hook_count > COMPAT_HOST_THREAD_HOOK_LIMIT || !bind || !unbind) return 0;
    for (index = 0; index < context->threads.hook_count; ++index) {
        compat_host_thread_hook *hook = &context->threads.hooks[index];
        if (hook->bind == bind && hook->unbind == unbind && hook->context == argument) {
            uint32_t last = --context->threads.hook_count;
            *hook = context->threads.hooks[last];
            memset(&context->threads.hooks[last], 0, sizeof(*hook));
            return 1;
        }
    }
    return 0;
}

static void release_thread_binding(compat_host_context *context)
{
    (void)compat_host_context_unbind(context);
    _InterlockedDecrement(&context->threads.owner_count[thread_owner - 1]);
    _InterlockedDecrement(&context->threads.count);
    thread_owner = 0;
}

int compat_host_thread_bind(compat_host_context *context, uint32_t owner, const char *source)
{
    uint32_t index;
    if (owner > COMPAT_HOST_THREAD_ORIGINAL_WORKER || !context ||
        context->state != COMPAT_HOST_CONTEXT_ACTIVE ||
        context->threads.hook_count > COMPAT_HOST_THREAD_HOOK_LIMIT ||
        compat_host_thread_count(context) > INT32_MAX || !compat_host_context_bind(context))
        return 0;
    _InterlockedIncrement(&context->threads.count);
    _InterlockedIncrement(&context->threads.owner_count[owner]);
    thread_owner = owner + 1;
    if (owner == COMPAT_HOST_THREAD_ORIGINAL_WORKER) {
        char *name = context->threads.original_worker_source;
        memset(name, 0, COMPAT_HOST_THREAD_SOURCE_BYTES);
        for (index = 0; source && index + 1 < COMPAT_HOST_THREAD_SOURCE_BYTES && source[index]; ++index)
            name[index] = source[index];
    }
    for (index = 0; index < context->threads.hook_count; ++index) {
        compat_host_thread_hook *hook = &context->threads.hooks[index];
        if (!hook->bind(hook->context)) {
            while (index != 0) {
                hook = &context->threads.hooks[--index];
                if (!hook->unbind(hook->context)) return 0;
            }
            release_thread_binding(context);
            return 0;
        }
    }
    return 1;
}

int compat_host_thread_unbind(compat_host_context *context)
{
    uint32_t index;
    if (!context || current_context != context || !thread_owner) return 0;
    for (index = context->threads.hook_count; index != 0; --index) {
        compat_host_thread_hook *hook = &context->threads.hooks[index - 1];
        if (!hook->unbind(hook->context)) return 0;
    }
    release_thread_binding(context);
    return 1;
}

int compat_host_thread_finish(compat_host_context *context)
{
    int result = 0;
    if (!context || current_context != context || thread_finishing) return 0;
    thread_finishing = 1;
    __try {
        result = compat_host_thread_unbind(context);
    } __finally {
        thread_finishing = 0;
    }
    return result;
}

int compat_host_thread_is_finishing(void)
{
    return thread_finishing;
}

int compat_host_escape_arm(compat_host_context *context, jmp_buf *stack, uint32_t *result)
{
    if (!compat_host_context_valid(context) || context != current_context ||
        context->state != COMPAT_HOST_CONTEXT_ACTIVE || context->escape_stack ||
        escape_context || !stack || !result) return 0;
    context->escape_stack = stack;
    context->escape_result = result;
    escape_context = context;
    return 1;
}

void compat_host_escape_disarm(compat_host_context *context)
{
    if (context == NULL || context != escape_context) return;
    context->escape_stack = NULL;
    context->escape_result = NULL;
    escape_context = NULL;
}

int compat_host_escape_terminate_current(uint32_t result)
{
    compat_host_context *context = current_context;
    if (!context || context != escape_context || !compat_host_context_valid(context) ||
        context->state != COMPAT_HOST_CONTEXT_ACTIVE || !context->escape_stack)
        return 0;
    *context->escape_result = result;
    context->state = COMPAT_HOST_CONTEXT_COMPLETED;
    longjmp(*context->escape_stack, 1);
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

void compat_host_context_release_resources(compat_host_context *context)
{
    uint32_t index;
    for (index = context->cleanup_count; index != 0u; --index) {
        compat_host_cleanup cleanup = context->cleanups[index - 1u];
        cleanup.function(cleanup.context);
    }
    context->cleanup_count = 0u;
}

void compat_host_context_dispose(compat_host_context *context)
{
    compat_host_context_release_resources(context);
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
