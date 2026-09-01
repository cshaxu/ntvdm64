#include "session.h"

#include <intrin.h>
#include <stdlib.h>
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
        instance->thread_hook_count <= SESSION_MAX_THREAD_HOOKS &&
        instance->termination_armed <= 1u &&
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
        backend != SESSION_MACHINE_BACKEND_SOFTPC) return 0;
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

int session_register_thread_hook(session *instance, session_thread_bind_fn bind,
    session_thread_unbind_fn unbind, void *context)
{
    uint32_t index;
    if (!session_valid(instance) || instance->state != SESSION_STATE_ACTIVE ||
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
    if (!session_valid(instance) || (instance->state != SESSION_STATE_ACTIVE &&
        instance->state != SESSION_STATE_COMPLETED))
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

int session_presentation_text_acquire_writable(session *instance,
    uint32_t columns, uint32_t rows, uint8_t **bytes_out)
{
    uint64_t required;
    uint8_t *replacement;

    if (bytes_out != NULL) *bytes_out = NULL;
    if (!session_valid(instance) || instance->state != SESSION_STATE_ACTIVE ||
        bytes_out == NULL || columns == 0u || rows == 0u) return 0;
    required = (uint64_t)columns * (uint64_t)rows * 2u;
    if (required == 0u || required > UINT32_MAX) return 0;
    if (instance->presentation_text_storage != NULL &&
        instance->presentation_text_columns == columns &&
        instance->presentation_text_rows == rows) {
        *bytes_out = instance->presentation_text_storage;
        return 1;
    }
    replacement = (uint8_t *)calloc(1u, (size_t)required);
    if (replacement == NULL) return 0;
    session_presentation_text_clear(instance);
    instance->presentation_text_storage = replacement;
    instance->presentation_text_columns = columns;
    instance->presentation_text_rows = rows;
    instance->presentation_text_bytes = (uint32_t)required;
    *bytes_out = replacement;
    return 1;
}

int session_presentation_text_snapshot(const session *instance,
    uint8_t *destination, uint32_t destination_bytes, uint32_t *columns_out,
    uint32_t *rows_out, uint32_t *bytes_out)
{
    if (columns_out != NULL) *columns_out = 0u;
    if (rows_out != NULL) *rows_out = 0u;
    if (bytes_out != NULL) *bytes_out = 0u;
    if (!session_valid(instance) || destination == NULL ||
        instance->presentation_text_storage == NULL ||
        destination_bytes < instance->presentation_text_bytes) return 0;
    memcpy(destination, instance->presentation_text_storage,
        instance->presentation_text_bytes);
    if (columns_out != NULL) *columns_out = instance->presentation_text_columns;
    if (rows_out != NULL) *rows_out = instance->presentation_text_rows;
    if (bytes_out != NULL) *bytes_out = instance->presentation_text_bytes;
    return 1;
}

int session_presentation_text_describe(const session *instance,
    uint32_t *columns_out, uint32_t *rows_out, uint32_t *bytes_out)
{
    if (columns_out != NULL) *columns_out = 0u;
    if (rows_out != NULL) *rows_out = 0u;
    if (bytes_out != NULL) *bytes_out = 0u;
    if (!session_valid(instance) || instance->presentation_text_storage == NULL)
        return 0;
    if (columns_out != NULL) *columns_out = instance->presentation_text_columns;
    if (rows_out != NULL) *rows_out = instance->presentation_text_rows;
    if (bytes_out != NULL) *bytes_out = instance->presentation_text_bytes;
    return 1;
}

void session_presentation_text_clear(session *instance)
{
    if (instance == NULL) return;
    free(instance->presentation_text_storage);
    instance->presentation_text_storage = NULL;
    instance->presentation_text_columns = 0u;
    instance->presentation_text_rows = 0u;
    instance->presentation_text_bytes = 0u;
}

int session_presentation_graphics_acquire_writable(session *instance,
    uint32_t width, uint32_t height, uint32_t bits_per_pixel,
    uint32_t stride, uint8_t **bytes_out)
{
    uint64_t required;
    uint8_t *replacement;

    if (bytes_out != NULL) *bytes_out = NULL;
    if (!session_valid(instance) || instance->state != SESSION_STATE_ACTIVE ||
        bytes_out == NULL || width == 0u || height == 0u ||
        bits_per_pixel == 0u || stride == 0u) return 0;
    required = (uint64_t)stride * (uint64_t)height;
    if (required == 0u || required > UINT32_MAX) return 0;
    if (instance->presentation_graphics_storage != NULL &&
        instance->presentation_graphics_width == width &&
        instance->presentation_graphics_height == height &&
        instance->presentation_graphics_bits_per_pixel == bits_per_pixel &&
        instance->presentation_graphics_stride == stride) {
        *bytes_out = instance->presentation_graphics_storage;
        return 1;
    }
    replacement = (uint8_t *)calloc(1u, (size_t)required);
    if (replacement == NULL) return 0;
    session_presentation_graphics_clear(instance);
    instance->presentation_graphics_storage = replacement;
    instance->presentation_graphics_width = width;
    instance->presentation_graphics_height = height;
    instance->presentation_graphics_bits_per_pixel = bits_per_pixel;
    instance->presentation_graphics_stride = stride;
    instance->presentation_graphics_bytes = (uint32_t)required;
    *bytes_out = replacement;
    return 1;
}

int session_presentation_graphics_snapshot(const session *instance,
    uint8_t *destination, uint32_t destination_bytes, uint32_t *width_out,
    uint32_t *height_out, uint32_t *bits_per_pixel_out, uint32_t *stride_out,
    uint32_t *bytes_out)
{
    if (width_out != NULL) *width_out = 0u;
    if (height_out != NULL) *height_out = 0u;
    if (bits_per_pixel_out != NULL) *bits_per_pixel_out = 0u;
    if (stride_out != NULL) *stride_out = 0u;
    if (bytes_out != NULL) *bytes_out = 0u;
    if (!session_valid(instance) || destination == NULL ||
        instance->presentation_graphics_storage == NULL ||
        destination_bytes < instance->presentation_graphics_bytes) return 0;
    memcpy(destination, instance->presentation_graphics_storage,
        instance->presentation_graphics_bytes);
    if (width_out != NULL) *width_out = instance->presentation_graphics_width;
    if (height_out != NULL) *height_out = instance->presentation_graphics_height;
    if (bits_per_pixel_out != NULL)
        *bits_per_pixel_out = instance->presentation_graphics_bits_per_pixel;
    if (stride_out != NULL) *stride_out = instance->presentation_graphics_stride;
    if (bytes_out != NULL) *bytes_out = instance->presentation_graphics_bytes;
    return 1;
}

int session_presentation_graphics_describe(const session *instance,
    uint32_t *width_out, uint32_t *height_out, uint32_t *bits_per_pixel_out,
    uint32_t *stride_out, uint32_t *bytes_out)
{
    if (width_out != NULL) *width_out = 0u;
    if (height_out != NULL) *height_out = 0u;
    if (bits_per_pixel_out != NULL) *bits_per_pixel_out = 0u;
    if (stride_out != NULL) *stride_out = 0u;
    if (bytes_out != NULL) *bytes_out = 0u;
    if (!session_valid(instance) ||
        instance->presentation_graphics_storage == NULL) return 0;
    if (width_out != NULL) *width_out = instance->presentation_graphics_width;
    if (height_out != NULL) *height_out = instance->presentation_graphics_height;
    if (bits_per_pixel_out != NULL)
        *bits_per_pixel_out = instance->presentation_graphics_bits_per_pixel;
    if (stride_out != NULL) *stride_out = instance->presentation_graphics_stride;
    if (bytes_out != NULL) *bytes_out = instance->presentation_graphics_bytes;
    return 1;
}

void session_presentation_graphics_clear(session *instance)
{
    if (instance == NULL) return;
    free(instance->presentation_graphics_storage);
    instance->presentation_graphics_storage = NULL;
    instance->presentation_graphics_width = 0u;
    instance->presentation_graphics_height = 0u;
    instance->presentation_graphics_bits_per_pixel = 0u;
    instance->presentation_graphics_stride = 0u;
    instance->presentation_graphics_bytes = 0u;
    instance->presentation_graphics_palette_entries = 0u;
    instance->presentation_graphics_mutex_identifier = 0u;
    memset(instance->presentation_graphics_palette_rgb, 0,
        sizeof(instance->presentation_graphics_palette_rgb));
}

int session_presentation_graphics_set_palette(session *instance,
    const uint32_t *rgb, uint32_t entries)
{
    if (!session_valid(instance) || instance->state != SESSION_STATE_ACTIVE ||
        rgb == NULL || entries == 0u ||
        entries > SESSION_PRESENTATION_PALETTE_ENTRIES) return 0;
    memcpy(instance->presentation_graphics_palette_rgb, rgb,
        (size_t)entries * sizeof(*rgb));
    instance->presentation_graphics_palette_entries = entries;
    return 1;
}

int session_presentation_graphics_palette_snapshot(const session *instance,
    uint32_t *rgb, uint32_t capacity, uint32_t *entries_out)
{
    uint32_t entries;
    if (entries_out != NULL) *entries_out = 0u;
    if (!session_valid(instance) || rgb == NULL ||
        (entries = instance->presentation_graphics_palette_entries) == 0u ||
        capacity < entries) return 0;
    memcpy(rgb, instance->presentation_graphics_palette_rgb,
        (size_t)entries * sizeof(*rgb));
    if (entries_out != NULL) *entries_out = entries;
    return 1;
}

int session_presentation_graphics_set_mutex_identifier(session *instance,
    uint32_t identifier)
{
    if (!session_valid(instance) || instance->state != SESSION_STATE_ACTIVE)
        return 0;
    instance->presentation_graphics_mutex_identifier = identifier;
    return 1;
}

uint32_t session_presentation_graphics_mutex_identifier(const session *instance)
{
    return session_valid(instance) ?
        instance->presentation_graphics_mutex_identifier : 0u;
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

int session_dispose_with_reason(session *instance, uint32_t *reason_out)
{
    uint32_t index;
    uint32_t reason = session_dispose_reason(instance);

    if (reason_out != NULL) *reason_out = reason;
    if (reason != SESSION_DISPOSE_REASON_NONE) return 0;
    for (index = instance->teardown_count; index != 0u; --index) {
        session_teardown teardown = instance->teardowns[index - 1u];
        teardown.function(teardown.context);
    }
    session_presentation_text_clear(instance);
    session_presentation_graphics_clear(instance);
    session_guest_memory_end(instance);
    mapping_manager_dispose(&instance->guest_memory_mappings);
    mapping_manager_dispose(&instance->host_resource_mappings);
    mapping_manager_dispose(&instance->completion_callback_mappings);
    memset(instance, 0, sizeof(*instance));
    return 1;
}

int session_dispose(session *instance)
{
    return session_dispose_with_reason(instance, NULL);
}

int session_thread_bind(session *instance)
{
    uint32_t index;
    if (!session_valid(instance) || instance->state != SESSION_STATE_ACTIVE ||
        thread_instance != NULL) return 0;
    _InterlockedIncrement(&instance->binding_count);
    thread_instance = instance;
    for (index = 0u; index < instance->thread_hook_count; ++index) {
        session_thread_hook *hook = &instance->thread_hooks[index];
        if (!hook->bind(hook->context)) {
            while (index != 0u) {
                --index;
                instance->thread_hooks[index].unbind(
                    instance->thread_hooks[index].context);
            }
            thread_instance = NULL;
            _InterlockedDecrement(&instance->binding_count);
            return 0;
        }
    }
    return 1;
}

int session_thread_unbind(session *instance)
{
    uint32_t index;
    if (instance == NULL || thread_instance != instance) return 0;
    for (index = instance->thread_hook_count; index != 0u; --index) {
        session_thread_hook *hook = &instance->thread_hooks[index - 1u];
        hook->unbind(hook->context);
    }
    thread_instance = NULL;
    _InterlockedDecrement(&instance->binding_count);
    return 1;
}

session *session_thread_current(void)
{
    return thread_instance;
}

int session_arm_termination_escape(session *instance)
{
    if (!session_valid(instance) || instance->state != SESSION_STATE_ACTIVE ||
        thread_instance != instance || instance->termination_armed != 0u)
        return 0;
    instance->termination_armed = 1u;
    return 1;
}

void session_disarm_termination_escape(session *instance)
{
    if (instance != NULL && session_valid(instance))
        instance->termination_armed = 0u;
}

int session_terminate_current(uint32_t completion_code)
{
    session *instance = thread_instance;
    if (!session_valid(instance) || instance->state != SESSION_STATE_ACTIVE ||
        instance->termination_armed == 0u)
        return 0;
    instance->completion_code = completion_code;
    instance->state = SESSION_STATE_COMPLETED;
    longjmp(instance->termination_escape, 1);
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

int session_set_mvdm_system_root(session *instance, const char *path)
{
    return session_set_root(instance, instance != NULL ? instance->mvdm_system_root : NULL,
        path);
}

const char *session_mvdm_system_root(const session *instance)
{
    return session_root(instance,
        instance != NULL ? instance->mvdm_system_root : NULL);
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
