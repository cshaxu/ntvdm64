#include "thread.h"

typedef struct win32_thread_context {
    win32_thread_entry entry;
    win32_thread_hooks hooks;
    LPVOID parameter;
} win32_thread_context;

static DWORD WINAPI win32_thread_thunk(LPVOID opaque)
{
    win32_thread_context *allocated = (win32_thread_context *)opaque;
    win32_thread_context context = *allocated;
    DWORD result = 0;
    int bound = 0;
    HeapFree(GetProcessHeap(), 0, allocated);
    if (context.hooks.owner != NULL) {
        if (!context.hooks.enter(context.hooks.owner, context.hooks.source_name))
            return ERROR_INVALID_STATE;
        bound = 1;
    }
    __try {
        if (context.entry.cdecl_entry) result = context.entry.cdecl_entry(context.parameter);
        else if (context.entry.winapi_entry) result = context.entry.winapi_entry(context.parameter);
        else if (context.entry.void_entry) context.entry.void_entry();
        else context.entry.void_parameter_entry(context.parameter);
    } __finally {
        if (bound) context.hooks.finish();
    }
    return result;
}

HANDLE win32_thread_create(LPSECURITY_ATTRIBUTES attributes, SIZE_T stack_bytes,
    win32_thread_entry entry, LPVOID parameter, DWORD flags, LPDWORD thread_id,
    const win32_thread_hooks *hooks)
{
    win32_thread_context *context;
    HANDLE thread;
    unsigned count = (entry.cdecl_entry != NULL) + (entry.winapi_entry != NULL) +
        (entry.void_entry != NULL) + (entry.void_parameter_entry != NULL);
    if (count != 1u || (entry.void_entry && parameter) || hooks == NULL ||
        (hooks->owner && (!hooks->enter || !hooks->finish))) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }
    context = (win32_thread_context *)HeapAlloc(GetProcessHeap(), 0, sizeof(*context));
    if (!context) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
    context->entry = entry;
    context->hooks = *hooks;
    context->parameter = parameter;
    thread = CreateThread(attributes, stack_bytes, win32_thread_thunk, context, flags, thread_id);
    if (!thread) HeapFree(GetProcessHeap(), 0, context);
    return thread;
}
