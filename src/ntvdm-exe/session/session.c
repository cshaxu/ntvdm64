#include "session.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

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
        instance->host_context.threads.hook_count <= COMPAT_HOST_THREAD_HOOK_LIMIT &&
        compat_host_thread_count(&instance->host_context) <= INT32_MAX &&
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
    return session_valid(instance) &&
        compat_host_thread_register(&instance->host_context, bind, unbind, context);
}

int session_unregister_thread_hook(session *instance, session_thread_bind_fn bind,
    session_thread_unbind_fn unbind, void *context)
{
    return session_valid(instance) &&
        compat_host_thread_unregister(&instance->host_context, bind, unbind, context);
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
    if (compat_host_thread_count(&instance->host_context) != 0u)
        return SESSION_DISPOSE_REASON_BINDING_COUNT;
    if (instance->host_context.escape_stack != NULL)
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

int session_thread_bind_owned_source(session *instance, uint32_t binding_owner,
    const char *source_name)
{
    return session_valid(instance) &&
        compat_host_thread_bind(&instance->host_context, binding_owner, source_name);
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
    return instance != NULL && compat_host_thread_unbind(&instance->host_context);
}

int session_thread_finish(session *instance)
{
    return instance != NULL && compat_host_thread_finish(&instance->host_context);
}

int session_thread_is_finishing(void)
{
    return compat_host_thread_is_finishing();
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
    diagnostic_out->total = compat_host_thread_count(&instance->host_context);
    diagnostic_out->softpc_entry = compat_host_thread_owner_count(&instance->host_context,
        COMPAT_HOST_THREAD_MACHINE_ENTRY);
    diagnostic_out->original_worker = compat_host_thread_owner_count(&instance->host_context,
        COMPAT_HOST_THREAD_ORIGINAL_WORKER);
    diagnostic_out->unspecified = compat_host_thread_owner_count(&instance->host_context,
        COMPAT_HOST_THREAD_UNSPECIFIED);
    memcpy(diagnostic_out->original_worker_source, instance->host_context.threads.original_worker_source,
        sizeof(diagnostic_out->original_worker_source));
    return 1;
}

int session_arm_termination_escape(session *instance)
{
    if (!session_valid(instance) || session_thread_current() != instance)
        return 0;
    return compat_host_escape_arm(&instance->host_context,
        &instance->termination_escape, &instance->completion_code);
}

void session_disarm_termination_escape(session *instance)
{
    if (session_valid(instance))
        compat_host_escape_disarm(&instance->host_context);
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
