#include "mvdm_softpc_physical_mapping.h"

#include <stdlib.h>

#include "session/session.h"

typedef struct physical_mapping_record {
    session *owner;
    uint32_t identifier;
    uint32_t source_size;
    uint32_t prepared_size;
    uint32_t guest_base;
    uint32_t active;
    struct physical_mapping_record *next;
} physical_mapping_record;

static physical_mapping_record *records;

static int add_overflow(uint32_t left, uint32_t right, uint32_t *sum)
{
    if (left > UINT32_MAX - right) return 1;
    *sum = left + right;
    return 0;
}

static physical_mapping_record *find_identifier(session *owner,
    uint32_t identifier)
{
    physical_mapping_record *record;
    for (record = records; record != NULL; record = record->next) {
        if (record->owner == owner && record->identifier == identifier)
            return record;
    }
    return NULL;
}

static void remove_record(physical_mapping_record *target)
{
    physical_mapping_record **cursor = &records;
    while (*cursor != NULL && *cursor != target) cursor = &(*cursor)->next;
    if (*cursor == target) *cursor = target->next;
    free(target);
}

static void release_owner(void *context)
{
    session *owner = (session *)context;
    physical_mapping_record *record = records;
    while (record != NULL) {
        physical_mapping_record *next = record->next;
        if (record->owner == owner) {
            (void)mapping_manager_release(session_guest_memory_mappings(owner),
                record->identifier);
            remove_record(record);
        }
        record = next;
    }
}

int mvdm_softpc_physical_mapping_publish(void *host_bytes,
    uint32_t byte_count, uint32_t *identifier_out)
{
    session *owner = session_thread_current();
    mapping_manager *mappings;
    physical_mapping_record *record;
    uint32_t identifier;
    int teardown_registered = 0;

    if (identifier_out != NULL) *identifier_out = 0u;
    if (owner == NULL || !session_valid(owner) || host_bytes == NULL ||
        byte_count == 0u) return 0;
    mappings = session_guest_memory_mappings(owner);
    if (mappings == NULL || !mapping_manager_publish(mappings,
            (uintptr_t)host_bytes, &identifier)) return 0;
    record = find_identifier(owner, identifier);
    if (record != NULL) {
        if (record->source_size != byte_count || record->active != 0u) return 0;
        if (identifier_out != NULL) *identifier_out = identifier;
        return 1;
    }
    for (record = records; record != NULL; record = record->next) {
        if (record->owner == owner) {
            teardown_registered = 1;
            break;
        }
    }
    record = (physical_mapping_record *)calloc(1u, sizeof(*record));
    if (record == NULL) {
        (void)mapping_manager_release(mappings, identifier);
        return 0;
    }
    record->owner = owner;
    record->identifier = identifier;
    record->source_size = byte_count;
    record->next = records;
    records = record;
    if (!teardown_registered &&
        !session_register_teardown(owner, release_owner, owner)) {
        remove_record(record);
        (void)mapping_manager_release(mappings, identifier);
        return 0;
    }
    if (identifier_out != NULL) *identifier_out = identifier;
    return 1;
}

int mvdm_softpc_physical_mapping_prepare(uint32_t identifier,
    uint32_t byte_count, uint32_t *alignment_out)
{
    session *owner = session_thread_current();
    physical_mapping_record *record;
    uintptr_t native_value;
    uint32_t alignment;
    uint32_t total;

    if (alignment_out != NULL) *alignment_out = 0u;
    if (owner == NULL || !session_valid(owner) || byte_count == 0u ||
        (record = find_identifier(owner, identifier)) == NULL ||
        record->active != 0u || record->source_size != byte_count ||
        !mapping_manager_lookup_value(session_guest_memory_mappings(owner),
            identifier, &native_value)) return 0;
    alignment = (uint32_t)(native_value & 3u);
    if (add_overflow(byte_count, alignment, &total)) return 0;
    if (add_overflow(total, UINT32_C(4095), &total)) return 0;
    record->prepared_size = total & ~UINT32_C(4095);
    if (alignment_out != NULL) *alignment_out = alignment;
    return 1;
}

void mvdm_softpc_physical_mapping_set(uint32_t identifier,
    uint32_t intel_address, uint32_t byte_count)
{
    session *owner = session_thread_current();
    physical_mapping_record *record;
    uintptr_t ignored;

    if (owner == NULL || !session_valid(owner)) return;
    record = find_identifier(owner, identifier);
    if (record != NULL && record->active == 0u &&
        record->prepared_size == byte_count &&
        mapping_manager_lookup_value(session_guest_memory_mappings(owner),
            identifier, &ignored)) {
        record->guest_base = intel_address;
        record->active = 1u;
        return;
    }
    for (record = records; record != NULL; record = record->next) {
        if (record->owner == owner && record->active != 0u &&
            record->guest_base == intel_address && record->prepared_size == byte_count) {
            (void)mapping_manager_release(session_guest_memory_mappings(owner),
                record->identifier);
            remove_record(record);
            return;
        }
    }
}

int mvdm_softpc_physical_mapping_resolve(uint32_t intel_address,
    uint8_t **host_byte_out)
{
    session *owner = session_thread_current();
    physical_mapping_record *record;
    uintptr_t native_value;

    if (host_byte_out != NULL) *host_byte_out = NULL;
    if (owner == NULL || !session_valid(owner) || host_byte_out == NULL) return 0;
    for (record = records; record != NULL; record = record->next) {
        uint32_t offset;
        uint32_t alignment;
        if (record->owner != owner || record->active == 0u ||
            intel_address < record->guest_base) continue;
        offset = intel_address - record->guest_base;
        if (!mapping_manager_lookup_value(session_guest_memory_mappings(owner),
                record->identifier, &native_value)) return 0;
        alignment = (uint32_t)(native_value & (uintptr_t)3u);
        if (offset < alignment || offset - alignment >= record->source_size)
            continue;
        *host_byte_out = (uint8_t *)(native_value -
            (native_value & (uintptr_t)3u) + offset);
        return 1;
    }
    return 0;
}

void mvdm_softpc_physical_mapping_cancel(uint32_t identifier)
{
    session *owner = session_thread_current();
    physical_mapping_record *record;
    if (owner == NULL || !session_valid(owner) ||
        (record = find_identifier(owner, identifier)) == NULL ||
        record->active != 0u) return;
    (void)mapping_manager_release(session_guest_memory_mappings(owner), identifier);
    remove_record(record);
}

/* Historical SoftPC spelling retained at the original call sites. The
 * adapter owns the source-derived CCPU page-binding behavior. */
void VdmSetPhysRecStructs(uint32_t host_address, uint32_t intel_address,
    uint32_t byte_count)
{
    mvdm_softpc_physical_mapping_set(host_address, intel_address, byte_count);
}
