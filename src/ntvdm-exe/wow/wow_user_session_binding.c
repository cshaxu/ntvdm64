#include "include/wow_user_session_binding.h"

static __declspec(thread) wow_user_runtime_thread current_binding;
extern HANDLE hWOWDll;

static FARPROC lifecycle_operation(const char *name)
{
    if (!hWOWDll) { SetLastError(ERROR_INVALID_STATE); return NULL; }
    return GetProcAddress(hWOWDll, name);
}

static BOOL unbind_runtime(wow_user_runtime_thread *thread)
{
    if (thread->thread && thread->runtime->lifecycle) {
        typedef BOOL (WINAPI *unbind_fn)(wow_user_runtime_thread *);
        unbind_fn unbind = (unbind_fn)lifecycle_operation("wow_user_task_lifecycle_unbind");
        return unbind ? unbind(thread) : FALSE;
    }
    return wow_user_runtime_unbind(thread);
}


static int bind_current(void *context)
{
    wow_user_session_binding *binding = context;
    if (!binding || !binding->registered ||
            session_thread_current() != binding->session ||
            wow_user_runtime_current() != NULL)
        return 0;
    return wow_user_runtime_bind(&current_binding, binding->runtime, NULL, NULL);
}

static int unbind_current(void *context)
{
    wow_user_session_binding *binding = context;
    if (binding && wow_user_runtime_current() == &current_binding) {
        int result = 0;
        current_binding.terminal_exit = session_thread_is_finishing();
        __try {
            result = unbind_runtime(&current_binding);
        } __finally {
            current_binding.terminal_exit = FALSE;
        }
        return result;
    }
    return 1;
}

BOOL WINAPI wow_user_session_attach(wow_user_session_binding *binding,
    session *owner, wow_user_runtime *runtime)
{
    if (!binding || !owner || !runtime || binding->registered ||
            binding->session || binding->runtime || session_thread_current() != owner ||
            !wow_user_runtime_initialize(runtime)) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    binding->session = owner;
    binding->runtime = runtime;
    binding->registered = TRUE;
    if (!session_register_thread_hook(owner, bind_current, unbind_current, binding) ||
            !bind_current(binding)) {
        (void)session_unregister_thread_hook(owner, bind_current, unbind_current,
            binding);
        binding->registered = FALSE;
        binding->session = NULL;
        binding->runtime = NULL;
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    return TRUE;
}

BOOL WINAPI wow_user_session_detach(wow_user_session_binding *binding)
{
    session_binding_diagnostic threads;
    if (!binding || !binding->registered || !binding->session ||
            session_thread_current() != binding->session) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    /* The owner stops thread producers before detach, as it must before
     * session_dispose. Do not remove hooks or provider state while another
     * bound thread can still return through them. This is a precondition
     * check, not a replacement thread join or a new shutdown scheduler. */
    if (!session_binding_diagnostic_snapshot(binding->session, &threads) ||
            threads.total != 1u) {
        SetLastError(ERROR_BUSY);
        return FALSE;
    }
    /* Explicit detach is fallible: retain the registration when USER still
     * owns live task/window state, so the caller can complete cleanup/retry. */
    if (wow_user_runtime_current() == &current_binding &&
            !unbind_runtime(&current_binding))
        return FALSE;
    if (binding->runtime->lifecycle) {
        typedef BOOL (WINAPI *dispose_fn)(PVOID);
        dispose_fn dispose = (dispose_fn)lifecycle_operation("wow_user_task_lifecycle_dispose");
        if (!dispose || !dispose(binding->runtime->lifecycle)) return FALSE;
    }
    if (!session_unregister_thread_hook(binding->session, bind_current,
            unbind_current, binding)) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    binding->registered = FALSE;
    binding->session = NULL;
    binding->runtime = NULL;
    return TRUE;
}

BOOL WINAPI wow_user_worker_active(void)
{
    session *owner = session_thread_current();

    return owner != NULL && owner->state == SESSION_STATE_ACTIVE;
}
