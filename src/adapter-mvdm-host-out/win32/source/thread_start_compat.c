#include "thread_start_compat.h"

#include "session/session.h"

#undef CreateThread
#undef ExitThread

typedef struct _OPENNT_CDECL_THREAD_CONTEXT {
    OPENNT_CDECL_THREAD_START_ROUTINE start_routine;
    LPVOID parameter;
    session *owner;
} OPENNT_CDECL_THREAD_CONTEXT;

typedef struct _OPENNT_VOID_CDECL_THREAD_CONTEXT {
    OPENNT_VOID_CDECL_THREAD_START_ROUTINE start_routine;
    session *owner;
} OPENNT_VOID_CDECL_THREAD_CONTEXT;

typedef struct _OPENNT_VOID_CDECL_PARAMETER_THREAD_CONTEXT {
    OPENNT_VOID_CDECL_PARAMETER_THREAD_START_ROUTINE start_routine;
    LPVOID parameter;
    session *owner;
} OPENNT_VOID_CDECL_PARAMETER_THREAD_CONTEXT;

VOID WINAPI opennt_exit_thread(DWORD exit_code)
{
    session *owner = session_thread_current();
    if (owner != NULL) (void)session_thread_unbind(owner);
    ExitThread(exit_code);
}

static DWORD WINAPI opennt_cdecl_thread_thunk(LPVOID parameter)
{
    OPENNT_CDECL_THREAD_CONTEXT *context = (OPENNT_CDECL_THREAD_CONTEXT *)parameter;
    OPENNT_CDECL_THREAD_START_ROUTINE start_routine = context->start_routine;
    LPVOID start_parameter = context->parameter;
    session *owner = context->owner;
    int did_bind = 0;
    DWORD result;

    HeapFree(GetProcessHeap(), 0, context);
    /* The original SoftPC caller has no session parameter.  Capture only the
     * creator's process-local binding and establish it for this worker; no
     * guest pointer, MVDM field, or callback ABI is changed. */
    if (owner != NULL) {
        if (!session_thread_bind_owned(owner,
                SESSION_THREAD_BINDING_ORIGINAL_WORKER)) return ERROR_INVALID_STATE;
        did_bind = 1;
    }
    result = start_routine(start_parameter);
    if (did_bind) (void)session_thread_unbind(owner);
    return result;
}

static DWORD WINAPI opennt_void_cdecl_thread_thunk(LPVOID parameter)
{
    OPENNT_VOID_CDECL_THREAD_CONTEXT *context =
        (OPENNT_VOID_CDECL_THREAD_CONTEXT *)parameter;
    OPENNT_VOID_CDECL_THREAD_START_ROUTINE start_routine =
        context->start_routine;
    session *owner = context->owner;
    int did_bind = 0;

    HeapFree(GetProcessHeap(), 0, context);
    if (owner != NULL) {
        if (!session_thread_bind_owned(owner,
                SESSION_THREAD_BINDING_ORIGINAL_WORKER)) return ERROR_INVALID_STATE;
        did_bind = 1;
    }
    start_routine();
    if (did_bind) (void)session_thread_unbind(owner);
    return 0;
}

static DWORD WINAPI opennt_void_cdecl_parameter_thread_thunk(LPVOID parameter)
{
    OPENNT_VOID_CDECL_PARAMETER_THREAD_CONTEXT *context =
        (OPENNT_VOID_CDECL_PARAMETER_THREAD_CONTEXT *)parameter;
    OPENNT_VOID_CDECL_PARAMETER_THREAD_START_ROUTINE start_routine =
        context->start_routine;
    LPVOID start_parameter = context->parameter;
    session *owner = context->owner;
    int did_bind = 0;

    HeapFree(GetProcessHeap(), 0, context);
    if (owner != NULL) {
        if (!session_thread_bind_owned(owner,
                SESSION_THREAD_BINDING_ORIGINAL_WORKER)) return ERROR_INVALID_STATE;
        did_bind = 1;
    }
    start_routine(start_parameter);
    if (did_bind) (void)session_thread_unbind(owner);
    return 0;
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
    context->owner = session_thread_current();
    thread = CreateThread(attributes, stack_bytes, opennt_cdecl_thread_thunk,
        context, flags, thread_id);
    if (thread == NULL) {
        HeapFree(GetProcessHeap(), 0, context);
    }
    return thread;
}

HANDLE opennt_create_void_cdecl_thread(
    LPSECURITY_ATTRIBUTES attributes,
    SIZE_T stack_bytes,
    OPENNT_VOID_CDECL_THREAD_START_ROUTINE start_routine,
    LPVOID parameter,
    DWORD flags,
    LPDWORD thread_id)
{
    OPENNT_VOID_CDECL_THREAD_CONTEXT *context;
    HANDLE thread;

    if (start_routine == NULL || parameter != NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }
    context = (OPENNT_VOID_CDECL_THREAD_CONTEXT *)HeapAlloc(
        GetProcessHeap(), 0, sizeof(*context));
    if (context == NULL) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }
    context->start_routine = start_routine;
    context->owner = session_thread_current();
    thread = CreateThread(attributes, stack_bytes, opennt_void_cdecl_thread_thunk,
        context, flags, thread_id);
    if (thread == NULL) HeapFree(GetProcessHeap(), 0, context);
    return thread;
}

HANDLE opennt_create_void_cdecl_parameter_thread(
    LPSECURITY_ATTRIBUTES attributes,
    SIZE_T stack_bytes,
    OPENNT_VOID_CDECL_PARAMETER_THREAD_START_ROUTINE start_routine,
    LPVOID parameter,
    DWORD flags,
    LPDWORD thread_id)
{
    OPENNT_VOID_CDECL_PARAMETER_THREAD_CONTEXT *context;
    HANDLE thread;

    if (start_routine == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }
    context = (OPENNT_VOID_CDECL_PARAMETER_THREAD_CONTEXT *)HeapAlloc(
        GetProcessHeap(), 0, sizeof(*context));
    if (context == NULL) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }
    context->start_routine = start_routine;
    context->parameter = parameter;
    context->owner = session_thread_current();
    thread = CreateThread(attributes, stack_bytes,
        opennt_void_cdecl_parameter_thread_thunk, context, flags, thread_id);
    if (thread == NULL) HeapFree(GetProcessHeap(), 0, context);
    return thread;
}
