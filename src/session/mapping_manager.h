#ifndef MAPPING_MANAGER_H
#define MAPPING_MANAGER_H

#include <stdint.h>

#define MAPPING_MANAGER_MAGIC UINT32_C(0x4d415050)
#define MAPPING_MANAGER_VERSION UINT32_C(1)
#define MAPPING_MANAGER_BUCKETS 37u

enum mapping_manager_kind {
    MAPPING_MANAGER_GUEST_MEMORY = 1u,
    MAPPING_MANAGER_HOST_RESOURCE = 2u,
    MAPPING_MANAGER_COMPLETION_CALLBACK = 3u
};

enum mapping_manager_reserved_identifier {
    MAPPING_MANAGER_RESERVE_ZERO = 1u,
    MAPPING_MANAGER_RESERVE_MAXIMUM = 2u
};

typedef struct mapping_record mapping_record;

typedef struct mapping_manager {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t kind;
    uint32_t reserved_identifiers;
    uint32_t next_identifier;
    uint32_t active_count;
    uint32_t tombstone_count;
    uint32_t exhausted;
    mapping_record *by_identifier[MAPPING_MANAGER_BUCKETS];
    mapping_record *by_value[MAPPING_MANAGER_BUCKETS];
} mapping_manager;

void mapping_manager_initialize(mapping_manager *manager, uint32_t kind,
    uint32_t reserved_identifiers);
int mapping_manager_valid(const mapping_manager *manager, uint32_t kind);
int mapping_manager_publish(mapping_manager *manager, uintptr_t native_value,
    uint32_t *identifier_out);
int mapping_manager_lookup_value(const mapping_manager *manager,
    uint32_t identifier, uintptr_t *native_value_out);
int mapping_manager_lookup_identifier(const mapping_manager *manager,
    uintptr_t native_value, uint32_t *identifier_out);
int mapping_manager_release(mapping_manager *manager, uint32_t identifier);
void mapping_manager_dispose(mapping_manager *manager);

#if defined(SESSION_MAPPING_TESTING)
void mapping_manager_test_set_next_identifier(mapping_manager *manager,
    uint32_t next_identifier);
#endif

#endif
