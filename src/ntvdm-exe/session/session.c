#include "session.h"

#include <intrin.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

static __declspec(thread) uint32_t thread_binding_owner;
static __declspec(thread) int thread_finishing;
/* A saved C stack belongs to its arming thread, not every session binding. */
static __declspec(thread) session *thread_termination_escape;

static uint32_t session_binding_count(const session *instance)
{
    return (uint32_t)_InterlockedCompareExchange(
        (volatile long *)&instance->binding_count, 0, 0);
}

void session_initialize(session *instance, uint32_t identity)
{
    if (instance == NULL || identity == 0u) return;
    memset(instance, 0, sizeof(*instance));
    instance->magic = SESSION_MAGIC;
    instance->abi_version = SESSION_ABI_VERSION;
    instance->struct_bytes = (uint32_t)sizeof(*instance);
    instance->identity = identity;
    instance->host_context.guest_memory = &instance->guest_memory_lease;
}

int session_valid(const session *instance)
{
    return instance != NULL && instance->magic == SESSION_MAGIC &&
        instance->abi_version == SESSION_ABI_VERSION &&
        instance->struct_bytes == sizeof(*instance) &&
        instance->identity != 0u && instance->host_context.state <= SESSION_STATE_COMPLETED &&
        instance->machine_backend <= SESSION_MACHINE_BACKEND_SOFTPC &&
        compat_host_context_valid(&instance->host_context) &&
        instance->thread_hook_count <= SESSION_MAX_THREAD_HOOKS &&
        instance->termination_armed <= 1u &&
        session_binding_count(instance) <= INT32_MAX &&
        guest_memory_lease_context_valid(&instance->guest_memory_lease);
}

int session_activate(session *instance)
{
    if (!session_valid(instance) || instance->host_context.state != SESSION_STATE_READY)
        return 0;
    if (++instance->host_context.epoch == 0u) ++instance->host_context.epoch;
    instance->host_context.state = SESSION_STATE_ACTIVE;
    return 1;
}

int session_select_machine_backend(session *instance, uint32_t backend)
{
    if (!session_valid(instance) || instance->host_context.state != SESSION_STATE_READY ||
        instance->machine_backend != SESSION_MACHINE_BACKEND_NONE ||
        backend != SESSION_MACHINE_BACKEND_SOFTPC) return 0;
    instance->machine_backend = backend;
    return 1;
}

uint32_t session_machine_backend(const session *instance)
{
    if (!session_valid(instance)) return SESSION_MACHINE_BACKEND_NONE;
    return instance->machine_backend;
}

int session_register_thread_hook(session *instance, session_thread_bind_fn bind,
    session_thread_unbind_fn unbind, void *context)
{
    uint32_t index;
    if (!session_valid(instance) || instance->host_context.state != SESSION_STATE_ACTIVE ||
        bind == NULL || unbind == NULL) return 0;
    for (index = 0u; index < instance->thread_hook_count; ++index) {
        session_thread_hook *hook = &instance->thread_hooks[index];
        if (hook->bind == bind && hook->unbind == unbind &&
            hook->context == context) return 1;
    }
    if (instance->thread_hook_count == SESSION_MAX_THREAD_HOOKS) return 0;
    instance->thread_hooks[instance->thread_hook_count].bind = bind;
    instance->thread_hooks[instance->thread_hook_count].unbind = unbind;
    instance->thread_hooks[instance->thread_hook_count].context = context;
    ++instance->thread_hook_count;
    return 1;
}

int session_unregister_thread_hook(session *instance, session_thread_bind_fn bind,
    session_thread_unbind_fn unbind, void *context)
{
    uint32_t index;
    if (!session_valid(instance) || bind == NULL || unbind == NULL) return 0;
    for (index = 0u; index < instance->thread_hook_count; ++index) {
        session_thread_hook *hook = &instance->thread_hooks[index];
        if (hook->bind == bind && hook->unbind == unbind &&
            hook->context == context) {
            --instance->thread_hook_count;
            instance->thread_hooks[index] =
                instance->thread_hooks[instance->thread_hook_count];
            memset(&instance->thread_hooks[instance->thread_hook_count], 0,
                sizeof(instance->thread_hooks[instance->thread_hook_count]));
            return 1;
        }
    }
    return 0;
}

int session_request_cancellation(session *instance, uint32_t reason)
{
    if (!session_valid(instance) || instance->host_context.state != SESSION_STATE_ACTIVE ||
        reason != SESSION_CANCELLATION_REQUESTED) return 0;
    instance->cancellation_reason = reason;
    instance->host_context.state = SESSION_STATE_CANCELLED;
    return 1;
}

void session_complete(session *instance, uint32_t completion_code)
{
    if (!session_valid(instance) || instance->host_context.state != SESSION_STATE_ACTIVE)
        return;
    instance->completion_code = completion_code;
    instance->host_context.state = SESSION_STATE_COMPLETED;
}

static uint32_t session_dispose_reason(const session *instance)
{
    if (!session_valid(instance)) return SESSION_DISPOSE_REASON_INVALID;
    if (session_binding_count(instance) != 0u)
        return SESSION_DISPOSE_REASON_BINDING_COUNT;
    if (instance->termination_armed != 0u)
        return SESSION_DISPOSE_REASON_TERMINATION_ARMED;
    return SESSION_DISPOSE_REASON_NONE;
}

int session_dispose_with_reason(session *instance, uint32_t *reason_out,
    void (*close_console)(session *))
{
    uint32_t reason = session_dispose_reason(instance);

    if (reason == SESSION_DISPOSE_REASON_NONE && instance->console_client && !close_console)
        reason = SESSION_DISPOSE_REASON_INVALID;
    if (reason_out != NULL) *reason_out = reason;
    if (reason != SESSION_DISPOSE_REASON_NONE) return 0;
    if (instance->console_client) {
        /* Bootstrap opens the Console before the guest registers its resources.
         * Keep it alive for their cleanup, then stop it before freeing text. */
        compat_host_context_release_resources(&instance->host_context);
        close_console(instance);
    }
    compat_host_context_dispose(&instance->host_context);
    session_guest_memory_end(instance);
    memset(instance, 0, sizeof(*instance));
    return 1;
}

int session_dispose(session *instance)
{
    return session_dispose_with_reason(instance, NULL, NULL);
}

static volatile long *session_binding_owner_counter(session *instance,
    uint32_t binding_owner)
{
    switch (binding_owner) {
    case SESSION_THREAD_BINDING_SOFTPC_ENTRY:
        return &instance->binding_softpc_entry_count;
    case SESSION_THREAD_BINDING_ORIGINAL_WORKER:
        return &instance->binding_original_worker_count;
    case SESSION_THREAD_BINDING_UNSPECIFIED:
        return &instance->binding_unspecified_count;
    default:
        return NULL;
    }
}

static void session_record_original_worker_source(session *instance,
    const char *source_name)
{
    size_t index;

    memset(instance->original_worker_source, 0,
        sizeof(instance->original_worker_source));
    if (source_name == NULL) return;
    for (index = 0u; index + 1u < sizeof(instance->original_worker_source) &&
            source_name[index] != '\0'; ++index) {
        instance->original_worker_source[index] = source_name[index];
    }
}

int session_thread_bind_owned_source(session *instance, uint32_t binding_owner,
    const char *source_name)
{
    uint32_t index;
    volatile long *owner_counter;
    if (!session_valid(instance) || instance->host_context.state != SESSION_STATE_ACTIVE ||
        session_thread_current() != NULL ||
        (owner_counter = session_binding_owner_counter(instance, binding_owner)) == NULL)
        return 0;
    _InterlockedIncrement(&instance->binding_count);
    _InterlockedIncrement(owner_counter);
    (void)compat_host_context_bind(&instance->host_context);
    thread_binding_owner = binding_owner;
    if (binding_owner == SESSION_THREAD_BINDING_ORIGINAL_WORKER)
        session_record_original_worker_source(instance, source_name);
    for (index = 0u; index < instance->thread_hook_count; ++index) {
        session_thread_hook *hook = &instance->thread_hooks[index];
        if (!hook->bind(hook->context)) {
            while (index != 0u) {
                --index;
                if (!instance->thread_hooks[index].unbind(
                        instance->thread_hooks[index].context))
                    return 0;
            }
            (void)compat_host_context_unbind(&instance->host_context);
            thread_binding_owner = SESSION_THREAD_BINDING_UNSPECIFIED;
            _InterlockedDecrement(owner_counter);
            _InterlockedDecrement(&instance->binding_count);
            return 0;
        }
    }
    return 1;
}

int session_thread_bind_owned(session *instance, uint32_t binding_owner)
{
    return session_thread_bind_owned_source(instance, binding_owner, NULL);
}

int session_thread_bind(session *instance)
{
    return session_thread_bind_owned(instance,
        SESSION_THREAD_BINDING_UNSPECIFIED);
}

int session_thread_unbind(session *instance)
{
    uint32_t index;
    volatile long *owner_counter;
    if (instance == NULL || session_thread_current() != instance) return 0;
    owner_counter = session_binding_owner_counter(instance, thread_binding_owner);
    if (owner_counter == NULL) return 0;
    for (index = instance->thread_hook_count; index != 0u; --index) {
        session_thread_hook *hook = &instance->thread_hooks[index - 1u];
        if (!hook->unbind(hook->context)) return 0;
    }
    (void)compat_host_context_unbind(&instance->host_context);
    thread_binding_owner = SESSION_THREAD_BINDING_UNSPECIFIED;
    _InterlockedDecrement(owner_counter);
    _InterlockedDecrement(&instance->binding_count);
    return 1;
}

int session_thread_finish(session *instance)
{
    int result = 0;
    if (instance == NULL || session_thread_current() != instance || thread_finishing)
        return 0;
    thread_finishing = 1;
    __try {
        result = session_thread_unbind(instance);
    } __finally {
        thread_finishing = 0;
    }
    return result;
}

int session_thread_is_finishing(void)
{
    return thread_finishing;
}

session *session_thread_current(void)
{
    compat_host_context *context = compat_host_context_current();
    return context == NULL ? NULL :
        (session *)((char *)context - offsetof(session, host_context));
}

int session_binding_diagnostic_snapshot(const session *instance,
    session_binding_diagnostic *diagnostic_out)
{
    if (!session_valid(instance) || diagnostic_out == NULL) return 0;
    diagnostic_out->total = (uint32_t)_InterlockedCompareExchange(
        (volatile long *)&instance->binding_count, 0, 0);
    diagnostic_out->softpc_entry = (uint32_t)_InterlockedCompareExchange(
        (volatile long *)&instance->binding_softpc_entry_count, 0, 0);
    diagnostic_out->original_worker = (uint32_t)_InterlockedCompareExchange(
        (volatile long *)&instance->binding_original_worker_count, 0, 0);
    diagnostic_out->unspecified = (uint32_t)_InterlockedCompareExchange(
        (volatile long *)&instance->binding_unspecified_count, 0, 0);
    memcpy(diagnostic_out->original_worker_source,
        instance->original_worker_source,
        sizeof(diagnostic_out->original_worker_source));
    return 1;
}

int session_arm_termination_escape(session *instance)
{
    if (!session_valid(instance) || instance->host_context.state != SESSION_STATE_ACTIVE ||
        session_thread_current() != instance || instance->termination_armed != 0u ||
        thread_termination_escape != NULL)
        return 0;
    thread_termination_escape = instance;
    instance->termination_armed = 1u;
    return 1;
}

void session_disarm_termination_escape(session *instance)
{
    if (instance != NULL && instance == thread_termination_escape &&
        session_valid(instance)) {
        instance->termination_armed = 0u;
        thread_termination_escape = NULL;
    }
}

int session_terminate_current(uint32_t completion_code)
{
    session *instance = session_thread_current();
    if (instance != thread_termination_escape ||
        !session_valid(instance) || instance->host_context.state != SESSION_STATE_ACTIVE ||
        instance->termination_armed == 0u)
        return 0;
    instance->completion_code = completion_code;
    instance->host_context.state = SESSION_STATE_COMPLETED;
    longjmp(instance->termination_escape, 1);
}

static int session_set_root(session *instance, char *destination,
    const char *path)
{
    size_t length;
    if (!session_valid(instance) || instance->host_context.state != SESSION_STATE_READY ||
        destination == NULL || path == NULL || path[0] == '\0') return 0;
    length = strlen(path);
    if (length >= SESSION_FIRMWARE_ROOT_BYTES) return 0;
    memcpy(destination, path, length + 1u);
    return 1;
}

static const char *session_root(const session *instance, const char *root)
{
    if (!session_valid(instance) || root == NULL || root[0] == '\0')
        return NULL;
    return root;
}

int session_set_firmware_root(session *instance, const char *path)
{
    return session_set_root(instance, instance != NULL ? instance->firmware_root : NULL,
        path);
}

const char *session_firmware_root(const session *instance)
{
    return session_root(instance, instance != NULL ? instance->firmware_root : NULL);
}

int session_set_mvdm_system_root(session *instance, const char *path)
{
    return session_set_root(instance, instance != NULL ? instance->host_context.mvdm_system_root : NULL,
        path);
}

const char *session_mvdm_system_root(const session *instance)
{
    return session_root(instance,
        instance != NULL ? instance->host_context.mvdm_system_root : NULL);
}

int session_guest_memory_begin(session *instance)
{
    if (!session_valid(instance) || instance->host_context.state != SESSION_STATE_ACTIVE)
        return 0;
    return guest_memory_lease_begin(&instance->guest_memory_lease);
}

void session_guest_memory_end(session *instance)
{
    if (instance == NULL) return;
    guest_memory_lease_end(&instance->guest_memory_lease);
}
