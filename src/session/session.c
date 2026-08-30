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
    instance->mechanical_resume_status = SESSION_MECHANICAL_STATUS_NONE;
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
        instance->machine_backend <= SESSION_MACHINE_BACKEND_SOFTPC &&
        instance->teardown_count <= SESSION_MAX_TEARDOWNS &&
        session_binding_count(instance) <= INT32_MAX &&
        mapping_manager_valid(&instance->guest_memory_mappings,
            MAPPING_MANAGER_GUEST_MEMORY) &&
        mapping_manager_valid(&instance->host_resource_mappings,
            MAPPING_MANAGER_HOST_RESOURCE) &&
        mapping_manager_valid(&instance->completion_callback_mappings,
            MAPPING_MANAGER_COMPLETION_CALLBACK) &&
        guest_memory_lease_context_valid(&instance->guest_memory_lease);
}

int session_activate(session *instance)
{
    if (!session_valid(instance) || instance->state != SESSION_STATE_READY)
        return 0;
    if (++instance->epoch == 0u) ++instance->epoch;
    instance->state = SESSION_STATE_ACTIVE;
    return 1;
}

int session_select_machine_backend(session *instance, uint32_t backend)
{
    if (!session_valid(instance) || instance->state != SESSION_STATE_READY ||
        instance->machine_backend != SESSION_MACHINE_BACKEND_NONE ||
        (backend != SESSION_MACHINE_BACKEND_BOCHS &&
         backend != SESSION_MACHINE_BACKEND_SOFTPC)) return 0;
    instance->machine_backend = backend;
    return 1;
}

uint32_t session_machine_backend(const session *instance)
{
    if (!session_valid(instance)) return SESSION_MACHINE_BACKEND_NONE;
    return instance->machine_backend;
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

int session_set_mechanical_resume_budget(session *instance, uint64_t budget)
{
    if (!session_valid(instance) || instance->state != SESSION_STATE_ACTIVE ||
        budget == 0u) return 0;
    instance->mechanical_resume_budget = budget;
    instance->mechanical_resume_status = SESSION_MECHANICAL_STATUS_NONE;
    return 1;
}

uint64_t session_mechanical_resume_budget(const session *instance)
{
    if (!session_valid(instance) || instance->state != SESSION_STATE_ACTIVE)
        return 0u;
    return instance->mechanical_resume_budget;
}

void session_record_mechanical_resume_status(session *instance,
    uint32_t status)
{
    if (!session_valid(instance) || instance->state != SESSION_STATE_ACTIVE)
        return;
    instance->mechanical_resume_status = status;
}

uint32_t session_mechanical_resume_status(const session *instance)
{
    if (!session_valid(instance)) return SESSION_MECHANICAL_STATUS_NONE;
    return instance->mechanical_resume_status;
}

int session_set_video_event_sink(session *instance, session_video_event_fn sink,
    void *context)
{
    if (!session_valid(instance) || instance->state != SESSION_STATE_READY)
        return 0;
    instance->video_event_sink = sink;
    instance->video_event_context = context;
    return 1;
}

int session_notify_video_event(session *instance,
    const session_video_event *event)
{
    if (!session_valid(instance) || instance->state != SESSION_STATE_ACTIVE ||
        event == NULL)
        return 0;
    if (event->kind == SESSION_VIDEO_EVENT_ACTIVE)
        instance->video_event_active = 1u;
    return instance->video_event_sink != NULL &&
        instance->video_event_sink(instance->video_event_context, event) != 0;
}

uint32_t session_video_event_active(const session *instance)
{
    return session_valid(instance) ? instance->video_event_active : 0u;
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
    session_guest_memory_end(instance);
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

static int session_set_root(session *instance, char *destination,
    const char *path)
{
    size_t length;
    if (!session_valid(instance) || instance->state != SESSION_STATE_READY ||
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

int session_set_dos_media_root(session *instance, const char *path)
{
    return session_set_root(instance, instance != NULL ? instance->dos_media_root : NULL,
        path);
}

const char *session_dos_media_root(const session *instance)
{
    return session_root(instance, instance != NULL ? instance->dos_media_root : NULL);
}

int session_set_win16_media_root(session *instance, const char *path)
{
    return session_set_root(instance,
        instance != NULL ? instance->win16_media_root : NULL, path);
}

const char *session_win16_media_root(const session *instance)
{
    return session_root(instance,
        instance != NULL ? instance->win16_media_root : NULL);
}

int session_guest_memory_begin(session *instance, void *context,
    guest_memory_read_fn read, guest_memory_write_fn write)
{
    if (!session_valid(instance) || instance->state != SESSION_STATE_ACTIVE)
        return 0;
    return guest_memory_lease_begin(&instance->guest_memory_lease, context,
        read, write);
}

void session_guest_memory_end(session *instance)
{
    if (instance == NULL) return;
    guest_memory_lease_end(&instance->guest_memory_lease);
}

int session_guest_memory_acquire(session *instance, uint32_t address,
    uint32_t byte_count, uint32_t access, guest_memory_lease **lease_out,
    uint8_t **bytes_out)
{
    if (!session_valid(instance) || instance->state != SESSION_STATE_ACTIVE)
        return 0;
    return guest_memory_lease_acquire(&instance->guest_memory_lease, address,
        byte_count, access, lease_out, bytes_out);
}

int session_guest_memory_release(session *instance, guest_memory_lease *lease,
    int commit)
{
    if (!session_valid(instance) || instance->state != SESSION_STATE_ACTIVE)
        return 0;
    return guest_memory_lease_release(&instance->guest_memory_lease, lease,
        commit);
}
