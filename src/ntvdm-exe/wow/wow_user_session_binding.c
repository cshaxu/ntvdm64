#include "wow_user_session_binding.h"

static __declspec(thread) wow_user_runtime_thread current_binding;

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
    if (binding && wow_user_runtime_current() == &current_binding)
        return wow_user_runtime_unbind(&current_binding);
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
    if (!binding || !binding->registered || !binding->session ||
            session_thread_current() != binding->session) {
        SetLastError(ERROR_INVALID_STATE);
        return FALSE;
    }
    /* Explicit detach is fallible: retain the registration when USER still
     * owns live task/window state, so the caller can complete cleanup/retry. */
    if (wow_user_runtime_current() == &current_binding &&
            !wow_user_runtime_unbind(&current_binding))
        return FALSE;
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
