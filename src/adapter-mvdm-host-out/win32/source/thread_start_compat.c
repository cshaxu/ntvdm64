#include "thread_start_compat.h"

#undef CreateThread

typedef struct _OPENNT_CDECL_THREAD_CONTEXT {
    OPENNT_CDECL_THREAD_START_ROUTINE start_routine;
    LPVOID parameter;
} OPENNT_CDECL_THREAD_CONTEXT;

static DWORD WINAPI opennt_cdecl_thread_thunk(LPVOID parameter)
{
    OPENNT_CDECL_THREAD_CONTEXT *context = (OPENNT_CDECL_THREAD_CONTEXT *)parameter;
    OPENNT_CDECL_THREAD_START_ROUTINE start_routine = context->start_routine;
    LPVOID start_parameter = context->parameter;

    HeapFree(GetProcessHeap(), 0, context);
    return start_routine(start_parameter);
}

HANDLE opennt_create_cdecl_thread(
    LPSECURITY_ATTRIBUTES attributes,
    SIZE_T stack_bytes,
    OPENNT_CDECL_THREAD_START_ROUTINE start_routine,
    LPVOID parameter,
    DWORD flags,
    LPDWORD thread_id)
{
    OPENNT_CDECL_THREAD_CONTEXT *context;
    HANDLE thread;

    if (start_routine == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }

    context = (OPENNT_CDECL_THREAD_CONTEXT *)HeapAlloc(
        GetProcessHeap(), 0, sizeof(*context));
    if (context == NULL) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }

    context->start_routine = start_routine;
    context->parameter = parameter;
    thread = CreateThread(attributes, stack_bytes, opennt_cdecl_thread_thunk,
        context, flags, thread_id);
    if (thread == NULL) {
        HeapFree(GetProcessHeap(), 0, context);
    }
    return thread;
}
