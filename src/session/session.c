#include "session.h"

#include <intrin.h>
#include <string.h>

static __declspec(thread) session *thread_instance;

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
    mapping_manager_initialize(&instance->guest_memory_mappings,
        MAPPING_MANAGER_GUEST_MEMORY,
        MAPPING_MANAGER_RESERVE_ZERO | MAPPING_MANAGER_RESERVE_MAXIMUM);
    mapping_manager_initialize(&instance->host_resource_mappings,
        MAPPING_MANAGER_HOST_RESOURCE,
        MAPPING_MANAGER_RESERVE_ZERO | MAPPING_MANAGER_RESERVE_MAXIMUM);
    mapping_manager_initialize(&instance->completion_callback_mappings,
        MAPPING_MANAGER_COMPLETION_CALLBACK,
        MAPPING_MANAGER_RESERVE_ZERO | MAPPING_MANAGER_RESERVE_MAXIMUM);
}

int session_valid(const session *instance)
{
    return instance != NULL && instance->magic == SESSION_MAGIC &&
        instance->abi_version == SESSION_ABI_VERSION &&
        instance->struct_bytes == sizeof(*instance) &&
        instance->identity != 0u && instance->state <= SESSION_STATE_COMPLETED &&
        instance->teardown_count <= SESSION_MAX_TEARDOWNS &&
        session_binding_count(instance) <= INT32_MAX &&
        mapping_manager_valid(&instance->guest_memory_mappings,
            MAPPING_MANAGER_GUEST_MEMORY) &&
        mapping_manager_valid(&instance->host_resource_mappings,
            MAPPING_MANAGER_HOST_RESOURCE) &&
        mapping_manager_valid(&instance->completion_callback_mappings,
            MAPPING_MANAGER_COMPLETION_CALLBACK);
}

int session_activate(session *instance)
{
    if (!session_valid(instance) || instance->state != SESSION_STATE_READY)
        return 0;
    if (++instance->epoch == 0u) ++instance->epoch;
    instance->state = SESSION_STATE_ACTIVE;
    return 1;
}

int session_register_teardown(session *instance, session_teardown_fn function,
    void *context)
{
    uint32_t index;
    if (!session_valid(instance) || instance->state != SESSION_STATE_ACTIVE ||
        function == NULL) return 0;
    for (index = 0u; index < instance->teardown_count; ++index) {
        if (instance->teardowns[index].function == function &&
            instance->teardowns[index].context == context) return 1;
    }
    if (instance->teardown_count == SESSION_MAX_TEARDOWNS) return 0;
    instance->teardowns[instance->teardown_count].function = function;
    instance->teardowns[instance->teardown_count].context = context;
    ++instance->teardown_count;
    return 1;
}

int session_request_cancellation(session *instance, uint32_t reason)
{
    if (!session_valid(instance) || instance->state != SESSION_STATE_ACTIVE ||
        reason != SESSION_CANCELLATION_REQUESTED) return 0;
    instance->cancellation_reason = reason;
    instance->state = SESSION_STATE_CANCELLED;
    return 1;
}

void session_complete(session *instance, uint32_t completion_code)
{
    if (!session_valid(instance) || instance->state != SESSION_STATE_ACTIVE)
        return;
    instance->completion_code = completion_code;
    instance->state = SESSION_STATE_COMPLETED;
}

int session_dispose(session *instance)
{
    uint32_t index;
    if (!session_valid(instance) || session_binding_count(instance) != 0u)
        return 0;
    for (index = instance->teardown_count; index != 0u; --index) {
        session_teardown teardown = instance->teardowns[index - 1u];
        teardown.function(teardown.context);
    }
    mapping_manager_dispose(&instance->guest_memory_mappings);
    mapping_manager_dispose(&instance->host_resource_mappings);
    mapping_manager_dispose(&instance->completion_callback_mappings);
    memset(instance, 0, sizeof(*instance));
    return 1;
}

int session_thread_bind(session *instance)
{
    if (!session_valid(instance) || instance->state != SESSION_STATE_ACTIVE ||
        thread_instance != NULL) return 0;
    _InterlockedIncrement(&instance->binding_count);
    thread_instance = instance;
    return 1;
}

int session_thread_unbind(session *instance)
{
    if (instance == NULL || thread_instance != instance) return 0;
    thread_instance = NULL;
    _InterlockedDecrement(&instance->binding_count);
    return 1;
}

session *session_thread_current(void)
{
    return thread_instance;
}

mapping_manager *session_guest_memory_mappings(session *instance)
{
    return session_valid(instance) ? &instance->guest_memory_mappings : NULL;
}

mapping_manager *session_host_resource_mappings(session *instance)
{
    return session_valid(instance) ? &instance->host_resource_mappings : NULL;
}

mapping_manager *session_completion_callback_mappings(session *instance)
{
    return session_valid(instance) ? &instance->completion_callback_mappings : NULL;
}
