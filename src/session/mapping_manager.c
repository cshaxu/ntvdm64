#include "mapping_manager.h"

#include <stdlib.h>
#include <string.h>

struct mapping_record {
    uint32_t identifier;
    uintptr_t native_value;
    uint32_t active;
    struct mapping_record *next_identifier;
    struct mapping_record *next_value;
};

static uint32_t identifier_bucket(uint32_t identifier)
{
    return identifier % MAPPING_MANAGER_BUCKETS;
}

static uint32_t value_bucket(uintptr_t native_value)
{
    return (uint32_t)((native_value >> 3u) % MAPPING_MANAGER_BUCKETS);
}

static int manager_kind_valid(uint32_t kind)
{
    return kind == MAPPING_MANAGER_GUEST_MEMORY ||
        kind == MAPPING_MANAGER_HOST_RESOURCE ||
        kind == MAPPING_MANAGER_COMPLETION_CALLBACK;
}

static int identifier_reserved(const mapping_manager *manager,
    uint32_t identifier)
{
    return ((manager->reserved_identifiers & MAPPING_MANAGER_RESERVE_ZERO) != 0u &&
            identifier == 0u) ||
        ((manager->reserved_identifiers & MAPPING_MANAGER_RESERVE_MAXIMUM) != 0u &&
            identifier == UINT32_MAX);
}

static mapping_record *find_identifier(const mapping_manager *manager,
    uint32_t identifier)
{
    mapping_record *record;
    for (record = manager->by_identifier[identifier_bucket(identifier)];
        record != NULL; record = record->next_identifier) {
        if (record->identifier == identifier) return record;
    }
    return NULL;
}

static mapping_record *find_value(const mapping_manager *manager,
    uintptr_t native_value)
{
    mapping_record *record;
    for (record = manager->by_value[value_bucket(native_value)]; record != NULL;
        record = record->next_value) {
        if (record->native_value == native_value) return record;
    }
    return NULL;
}

static int allocate_identifier(mapping_manager *manager, uint32_t *identifier_out)
{
    uint32_t identifier;
    if (manager->exhausted != 0u || identifier_out == NULL) return 0;
    for (;;) {
        identifier = manager->next_identifier;
        if (identifier == UINT32_MAX) manager->exhausted = 1u;
        else ++manager->next_identifier;
        if (!identifier_reserved(manager, identifier)) {
            *identifier_out = identifier;
            return 1;
        }
        if (manager->exhausted != 0u) return 0;
    }
}

void mapping_manager_initialize(mapping_manager *manager, uint32_t kind,
    uint32_t reserved_identifiers)
{
    if (manager == NULL || !manager_kind_valid(kind) ||
        (reserved_identifiers & ~(MAPPING_MANAGER_RESERVE_ZERO |
            MAPPING_MANAGER_RESERVE_MAXIMUM)) != 0u) return;
    memset(manager, 0, sizeof(*manager));
    manager->magic = MAPPING_MANAGER_MAGIC;
    manager->abi_version = MAPPING_MANAGER_VERSION;
    manager->struct_bytes = (uint32_t)sizeof(*manager);
    manager->kind = kind;
    manager->reserved_identifiers = reserved_identifiers;
}

int mapping_manager_valid(const mapping_manager *manager, uint32_t kind)
{
    return manager != NULL && manager->magic == MAPPING_MANAGER_MAGIC &&
        manager->abi_version == MAPPING_MANAGER_VERSION &&
        manager->struct_bytes == sizeof(*manager) && manager->kind == kind &&
        manager_kind_valid(kind) &&
        (manager->reserved_identifiers & ~(MAPPING_MANAGER_RESERVE_ZERO |
            MAPPING_MANAGER_RESERVE_MAXIMUM)) == 0u;
}

int mapping_manager_publish(mapping_manager *manager, uintptr_t native_value,
    uint32_t *identifier_out)
{
    mapping_record *record;
    uint32_t identifier;
    uint32_t bucket;
    if (identifier_out != NULL) *identifier_out = 0u;
    if (!mapping_manager_valid(manager, manager == NULL ? 0u : manager->kind) ||
        native_value == 0u) return 0;
    record = find_value(manager, native_value);
    if (record != NULL) {
        if (identifier_out != NULL) *identifier_out = record->identifier;
        return 1;
    }
    if (!allocate_identifier(manager, &identifier)) return 0;
    record = (mapping_record *)calloc(1u, sizeof(*record));
    if (record == NULL) return 0;
    record->identifier = identifier;
    record->native_value = native_value;
    record->active = 1u;
    bucket = identifier_bucket(identifier);
    record->next_identifier = manager->by_identifier[bucket];
    manager->by_identifier[bucket] = record;
    bucket = value_bucket(native_value);
    record->next_value = manager->by_value[bucket];
    manager->by_value[bucket] = record;
    ++manager->active_count;
    if (identifier_out != NULL) *identifier_out = identifier;
    return 1;
}

int mapping_manager_lookup_value(const mapping_manager *manager,
    uint32_t identifier, uintptr_t *native_value_out)
{
    mapping_record *record;
    if (native_value_out != NULL) *native_value_out = (uintptr_t)0u;
    if (!mapping_manager_valid(manager, manager == NULL ? 0u : manager->kind) ||
        identifier_reserved(manager, identifier) ||
        (record = find_identifier(manager, identifier)) == NULL || record->active == 0u)
        return 0;
    if (native_value_out != NULL) *native_value_out = record->native_value;
    return 1;
}

int mapping_manager_lookup_identifier(const mapping_manager *manager,
    uintptr_t native_value, uint32_t *identifier_out)
{
    mapping_record *record;
    if (identifier_out != NULL) *identifier_out = 0u;
    if (!mapping_manager_valid(manager, manager == NULL ? 0u : manager->kind) ||
        native_value == 0u || (record = find_value(manager, native_value)) == NULL)
        return 0;
    if (identifier_out != NULL) *identifier_out = record->identifier;
    return 1;
}

int mapping_manager_release(mapping_manager *manager, uint32_t identifier)
{
    mapping_record *record;
    mapping_record **cursor;
    if (!mapping_manager_valid(manager, manager == NULL ? 0u : manager->kind) ||
        identifier_reserved(manager, identifier) ||
        (record = find_identifier(manager, identifier)) == NULL || record->active == 0u)
        return 0;
    cursor = &manager->by_value[value_bucket(record->native_value)];
    while (*cursor != NULL && *cursor != record) cursor = &(*cursor)->next_value;
    if (*cursor != record) return 0;
    *cursor = record->next_value;
    record->next_value = NULL;
    record->native_value = (uintptr_t)0u;
    record->active = 0u;
    --manager->active_count;
    ++manager->tombstone_count;
    return 1;
}

void mapping_manager_dispose(mapping_manager *manager)
{
    uint32_t bucket;
    if (!mapping_manager_valid(manager, manager == NULL ? 0u : manager->kind)) return;
    for (bucket = 0u; bucket < MAPPING_MANAGER_BUCKETS; ++bucket) {
        mapping_record *record = manager->by_identifier[bucket];
        while (record != NULL) {
            mapping_record *next = record->next_identifier;
            free(record);
            record = next;
        }
    }
    memset(manager, 0, sizeof(*manager));
}

#if defined(SESSION_MAPPING_TESTING)
void mapping_manager_test_set_next_identifier(mapping_manager *manager,
    uint32_t next_identifier)
{
    if (!mapping_manager_valid(manager, manager == NULL ? 0u : manager->kind) ||
        manager->active_count != 0u || manager->tombstone_count != 0u) return;
    manager->next_identifier = next_identifier;
    manager->exhausted = 0u;
}
#endif
