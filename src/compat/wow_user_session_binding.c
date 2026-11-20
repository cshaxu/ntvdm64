#include "include/wow_user_session_binding.h"

static __declspec(thread) wow_user_runtime_thread current_binding;

static BOOL unbind_runtime(wow_user_runtime_thread *thread)
{
    if (thread->thread && thread->runtime->lifecycle) {
        const wow_user_runtime_lifecycle_ops *ops = thread->runtime->lifecycle_ops;
        if (!ops || !ops->unbind) {
            SetLastError(ops ? ERROR_PROC_NOT_FOUND : ERROR_INVALID_STATE);
            return FALSE;
        }
        return ops->unbind(thread);
    }
    return wow_user_runtime_unbind(thread);
}


static int bind_current(void *context)
{
    wow_user_session_binding *binding = context;
    if (!binding || !binding->registered ||
            compat_host_context_current() != binding->host ||
            wow_user_runtime_current() != NULL)
        return 0;
    return wow_user_runtime_bind(&current_binding, binding->runtime, NULL, NULL);
}

static int unbind_current(void *context)
{
    wow_user_session_binding *binding = context;
    if (binding && wow_user_runtime_current() == &current_binding) {
        int result = 0;
        current_binding.terminal_exit = compat_host_thread_is_finishing();
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
    compat_host_context *owner, wow_user_runtime *runtime)
{
    if (!binding || !owner || !runtime || binding->registered ||
            binding->host || binding->runtime || compat_host_context_current() != owner ||
            !wow_user_runtime_initialize(runtime)) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    binding->host = owner;
    binding->runtime = runtime;
    binding->registered = TRUE;
    if (!compat_host_thread_register(owner, bind_current, unbind_current, binding) ||
            !bind_current(binding)) {
        (void)compat_host_thread_unregister(owner, bind_current, unbind_current,
            binding);
        binding->registered = FALSE;
        binding->host = NULL;
        binding->runtime = NULL;
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    return TRUE;
}

BOOL WINAPI wow_user_session_detach(wow_user_session_binding *binding)
{
    if (!binding || !binding->registered || !binding->host ||
            compat_host_context_current() != binding->host) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    /* The owner stops thread producers before detach, as it must before
     * session_dispose. Do not remove hooks or provider state while another
     * bound thread can still return through them. This is a precondition
     * check, not a replacement thread join or a new shutdown scheduler. */
    if (compat_host_thread_count(binding->host) != 1u) {
        SetLastError(ERROR_BUSY);
        return FALSE;
    }
    /* Explicit detach is fallible: retain the registration when USER still
     * owns live task/window state, so the caller can complete cleanup/retry. */
    if (wow_user_runtime_current() == &current_binding &&
            !unbind_runtime(&current_binding))
        return FALSE;
    if (binding->runtime->lifecycle) {
        const wow_user_runtime_lifecycle_ops *ops = binding->runtime->lifecycle_ops;
        if (!ops || !ops->dispose) {
            SetLastError(ops ? ERROR_PROC_NOT_FOUND : ERROR_INVALID_STATE);
            return FALSE;
        }
        if (!ops->dispose(binding->runtime->lifecycle)) return FALSE;
    }
    if (!compat_host_thread_unregister(binding->host, bind_current,
            unbind_current, binding)) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    binding->registered = FALSE;
    binding->host = NULL;
    binding->runtime = NULL;
    return TRUE;
}
