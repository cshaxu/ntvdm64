#include "host_handle_manager.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct runtime_host_handle_entry {
    HANDLE host_handle;
    uint32_t guest_handle;
    uint32_t ownership;
    void *data_value;
    runtime_session_data_release_fn data_release;
    struct runtime_host_handle_entry *next_host;
    struct runtime_host_handle_entry *next_guest;
};

static uint32_t host_bucket(HANDLE handle)
{
    return (uint32_t)(((uintptr_t)handle >> 3u) % RUNTIME_HOST_HANDLE_MANAGER_BUCKETS);
}

static uint32_t guest_bucket(uint32_t handle)
{
    return ((uint32_t)handle) % RUNTIME_HOST_HANDLE_MANAGER_BUCKETS;
}

static runtime_host_handle_entry *find_host(
    const runtime_host_handle_manager *manager, HANDLE handle)
{
    runtime_host_handle_entry *entry;
    if (!runtime_host_handle_manager_valid(manager)) return NULL;
    for (entry = manager->by_host[host_bucket(handle)]; entry != NULL;
        entry = entry->next_host)
        if (entry->host_handle == handle) return entry;
    return NULL;
}

runtime_host_handle_manager *runtime_host_handle_manager_session(void)
{
    return runtime_guest_pointer_manager_session_host_handle();
}

static runtime_host_handle_entry *find_guest(
    const runtime_host_handle_manager *manager, uint32_t handle)
{
    runtime_host_handle_entry *entry;
    if (!runtime_host_handle_manager_valid(manager) || handle == 0u ||
        handle == UINT32_MAX) return NULL;
    for (entry = manager->by_guest[guest_bucket(handle)]; entry != NULL;
        entry = entry->next_guest)
        if (entry->guest_handle == handle) return entry;
    return NULL;
}

static int data_valid(const runtime_guest_pointer_manager *manager)
{
    return manager == runtime_guest_pointer_manager_session_data() &&
        manager->instance_kind == RUNTIME_MAPPING_INSTANCE_SESSION_DATA &&
        manager->next_guest_handle >= 1u && manager->entry_count < UINT32_MAX;
}

static runtime_host_handle_entry *find_data(
    const runtime_guest_pointer_manager *manager, uint32_t guest_id)
{
    runtime_host_handle_entry *entry;
    if (!data_valid(manager) || guest_id == 0u || guest_id == UINT32_MAX) return NULL;
    for (entry = manager->by_guest[guest_bucket(guest_id)]; entry != NULL;
        entry = entry->next_guest)
        if (entry->guest_handle == guest_id) return entry;
    return NULL;
}

int runtime_host_handle_manager_initialize(
    runtime_host_handle_manager *manager)
{
    if (manager == NULL || manager != runtime_host_handle_manager_session()) return 0;
    if (manager->next_guest_handle == 0u) manager->next_guest_handle = 1u;
    return runtime_host_handle_manager_valid(manager);
}

int runtime_host_handle_manager_valid(
    const runtime_host_handle_manager *manager)
{
    return manager == runtime_host_handle_manager_session() &&
        manager->instance_kind == RUNTIME_MAPPING_INSTANCE_HOST_HANDLE &&
        manager->next_guest_handle >= 1u &&
        manager->entry_count < UINT32_MAX;
}

int runtime_host_handle_manager_publish(
    runtime_host_handle_manager *manager, HANDLE host_handle,
    uint32_t ownership, uint32_t *guest_handle_out, DWORD *error_out)
{
    runtime_host_handle_entry *entry;
    uint32_t guest_handle;
    uint32_t bucket;
    if (guest_handle_out != NULL) *guest_handle_out = 0u;
    if (error_out != NULL) *error_out = ERROR_INVALID_HANDLE;
    if (!runtime_host_handle_manager_valid(manager) || host_handle == NULL ||
        host_handle == INVALID_HANDLE_VALUE ||
        (ownership != RUNTIME_HOST_HANDLE_BORROWED &&
         ownership != RUNTIME_HOST_HANDLE_OWNED)) return 0;
    entry = find_host(manager, host_handle);
    if (entry != NULL) {
        if (guest_handle_out != NULL) *guest_handle_out = entry->guest_handle;
        if (error_out != NULL) *error_out = ERROR_SUCCESS;
        return 1;
    }
    if (manager->next_guest_handle == UINT32_MAX) {
        if (error_out != NULL) *error_out = ERROR_TOO_MANY_OPEN_FILES;
        return 0;
    }
    entry = (runtime_host_handle_entry *)calloc(1u, sizeof(*entry));
    if (entry == NULL) {
        if (error_out != NULL) *error_out = ERROR_NOT_ENOUGH_MEMORY;
        return 0;
    }
    guest_handle = manager->next_guest_handle++;
    entry->host_handle = host_handle;
    entry->guest_handle = guest_handle;
    entry->ownership = ownership;
    bucket = host_bucket(host_handle);
    entry->next_host = manager->by_host[bucket];
    manager->by_host[bucket] = entry;
    bucket = guest_bucket(guest_handle);
    entry->next_guest = manager->by_guest[bucket];
    manager->by_guest[bucket] = entry;
    ++manager->entry_count;
    if (guest_handle_out != NULL) *guest_handle_out = guest_handle;
    if (error_out != NULL) *error_out = ERROR_SUCCESS;
    return 1;
}

int runtime_host_handle_manager_lookup_handle(
    const runtime_host_handle_manager *manager, uint32_t guest_handle,
    HANDLE *host_handle_out)
{
    runtime_host_handle_entry *entry = find_guest(manager, guest_handle);
    if (host_handle_out != NULL) *host_handle_out = INVALID_HANDLE_VALUE;
    if (entry == NULL) return 0;
    if (host_handle_out != NULL) *host_handle_out = entry->host_handle;
    return 1;
}

int runtime_host_handle_manager_lookup_guest(
    const runtime_host_handle_manager *manager, HANDLE host_handle,
    uint32_t *guest_handle_out)
{
    runtime_host_handle_entry *entry = find_host(manager, host_handle);
    if (guest_handle_out != NULL) *guest_handle_out = 0u;
    if (entry == NULL) return 0;
    if (guest_handle_out != NULL) *guest_handle_out = entry->guest_handle;
    return 1;
}

static void unlink_entry(runtime_host_handle_manager *manager,
    runtime_host_handle_entry *entry)
{
    runtime_host_handle_entry **cursor;
    cursor = &manager->by_host[host_bucket(entry->host_handle)];
    while (*cursor != NULL && *cursor != entry) cursor = &(*cursor)->next_host;
    if (*cursor == entry) *cursor = entry->next_host;
    cursor = &manager->by_guest[guest_bucket(entry->guest_handle)];
    while (*cursor != NULL && *cursor != entry) cursor = &(*cursor)->next_guest;
    if (*cursor == entry) *cursor = entry->next_guest;
    --manager->entry_count;
}

int runtime_host_handle_manager_release(
    runtime_host_handle_manager *manager, uint32_t guest_handle,
    DWORD *error_out)
{
    runtime_host_handle_entry *entry;
    if (error_out != NULL) *error_out = ERROR_INVALID_HANDLE;
    if (!runtime_host_handle_manager_valid(manager) ||
        (entry = find_guest(manager, guest_handle)) == NULL) return 0;
    if (entry->ownership == RUNTIME_HOST_HANDLE_OWNED &&
        !CloseHandle(entry->host_handle)) {
        if (error_out != NULL) *error_out = GetLastError();
        return 0;
    }
    unlink_entry(manager, entry);
    free(entry);
    if (error_out != NULL) *error_out = ERROR_SUCCESS;
    return 1;
}

void runtime_host_handle_manager_reset(
    runtime_host_handle_manager *manager)
{
    uint32_t bucket;
    if (!runtime_host_handle_manager_valid(manager)) return;
    for (bucket = 0u; bucket < RUNTIME_HOST_HANDLE_MANAGER_BUCKETS; ++bucket) {
        runtime_host_handle_entry *entry = manager->by_guest[bucket];
        while (entry != NULL) {
            runtime_host_handle_entry *next = entry->next_guest;
            if (entry->ownership == RUNTIME_HOST_HANDLE_OWNED)
                CloseHandle(entry->host_handle);
            free(entry);
            entry = next;
        }
        manager->by_guest[bucket] = NULL;
        manager->by_host[bucket] = NULL;
    }
    manager->entry_count = 0u;
    manager->next_guest_handle = 1u;
}

int runtime_session_data_publish(runtime_guest_pointer_manager *manager,
    void *value, runtime_session_data_release_fn release, uint32_t *guest_id_out)
{
    runtime_host_handle_entry *entry;
    uint32_t guest_id, bucket;
    if (guest_id_out != NULL) *guest_id_out = 0u;
    if (!data_valid(manager) || value == NULL || manager->next_guest_handle == UINT32_MAX)
        return 0;
    entry = (runtime_host_handle_entry *)calloc(1u, sizeof(*entry));
    if (entry == NULL) return 0;
    guest_id = manager->next_guest_handle++;
    entry->guest_handle = guest_id;
    entry->data_value = value;
    entry->data_release = release;
    bucket = guest_bucket(guest_id);
    entry->next_guest = manager->by_guest[bucket];
    manager->by_guest[bucket] = entry;
    ++manager->entry_count;
    if (guest_id_out != NULL) *guest_id_out = guest_id;
    return 1;
}

int runtime_session_data_lookup(const runtime_guest_pointer_manager *manager,
    uint32_t guest_id, void **value_out)
{
    runtime_host_handle_entry *entry = find_data(manager, guest_id);
    if (value_out != NULL) *value_out = NULL;
    if (entry == NULL) return 0;
    if (value_out != NULL) *value_out = entry->data_value;
    return 1;
}

int runtime_session_data_release(runtime_guest_pointer_manager *manager,
    uint32_t guest_id)
{
    runtime_host_handle_entry *entry = find_data(manager, guest_id);
    runtime_host_handle_entry **cursor;
    if (entry == NULL) return 0;
    cursor = &manager->by_guest[guest_bucket(entry->guest_handle)];
    while (*cursor != NULL && *cursor != entry) cursor = &(*cursor)->next_guest;
    if (*cursor != entry) return 0;
    *cursor = entry->next_guest;
    if (entry->data_release != NULL) entry->data_release(entry->data_value);
    free(entry);
    --manager->entry_count;
    return 1;
}

void runtime_session_data_reset(runtime_guest_pointer_manager *manager)
{
    uint32_t bucket;
    if (!data_valid(manager)) return;
    for (bucket = 0u; bucket < RUNTIME_HOST_HANDLE_MANAGER_BUCKETS; ++bucket) {
        runtime_host_handle_entry *entry = manager->by_guest[bucket];
        while (entry != NULL) {
            runtime_host_handle_entry *next = entry->next_guest;
            if (entry->data_release != NULL) entry->data_release(entry->data_value);
            free(entry);
            entry = next;
        }
        manager->by_guest[bucket] = NULL;
        manager->by_host[bucket] = NULL;
    }
    manager->entry_count = 0u;
    manager->next_guest_handle = 1u;
}
