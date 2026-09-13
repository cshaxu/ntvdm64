#include "mvdm_softpc_physical_mapping.h"

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <windows.h>

#include "session/session.h"

typedef struct physical_mapping_record {
    session *owner;
    uint32_t identifier;
    uint32_t source_size;
    uint32_t prepared_size;
    uint32_t guest_base;
    uint32_t active;
    uintptr_t host_address;
    struct physical_mapping_record *next;
} physical_mapping_record;

typedef struct physical_alias_record {
    session *owner;
    uint32_t destination_base;
    uint32_t source_base;
    uint32_t byte_count;
    struct physical_alias_record *next;
} physical_alias_record;

static physical_mapping_record *records;
static physical_alias_record *aliases;

/* Temporary T406 observation: one record per call-site per process, not a
 * call counter or a correctness assertion. No guest bytes are read. Preserve
 * both error channels, close each handle, and never make logging a condition
 * of the mapping operation. Remove after consumer coverage is established. */
static void mapping_observe(const char *event, uint32_t a, uint32_t b,
    uint32_t c)
{
    DWORD saved_error = GetLastError();
    int saved_errno = errno;
    FILETIME created = {0}, exited, kernel, user;
    char path[MAX_PATH];
    char line[256];
    HANDLE file;
    DWORD written;
    int length;

    (void)GetProcessTimes(GetCurrentProcess(), &created, &exited, &kernel, &user);
    (void)snprintf(path, sizeof(path),
        "O:\\ntvdm64\\logs\\physical-mapping-%lu-%08lx%08lx.log",
        GetCurrentProcessId(), created.dwHighDateTime, created.dwLowDateTime);
    (void)CreateDirectoryA("O:\\ntvdm64\\logs", NULL);
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file != INVALID_HANDLE_VALUE) {
        length = snprintf(line, sizeof(line),
            "mapping-observation-v1 pid=%lu tid=%lu tick=%lu event=%s "
            "a=%08lx b=%08lx c=%08lx first-site-hit-only\r\n",
            GetCurrentProcessId(), GetCurrentThreadId(), GetTickCount(), event,
            (unsigned long)a, (unsigned long)b, (unsigned long)c);
        if (length > 0 && length < sizeof(line))
            (void)WriteFile(file, line, (DWORD)length, &written, NULL);
        CloseHandle(file);
    }
    errno = saved_errno;
    SetLastError(saved_error);
}

/* The volatile fast path avoids atomic operations and I/O on repeated SAS
 * accesses. Each expansion has a separate, bounded diagnostic-only flag. */
#define MAPPING_OBSERVE(event, a, b, c) do { \
    static volatile LONG observed; \
    if (!observed && InterlockedCompareExchange(&observed, 1, 0) == 0) \
        mapping_observe(event, (a), (b), (c)); \
} while (0)

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

static void remove_alias(physical_alias_record *target)
{
    physical_alias_record **cursor = &aliases;
    while (*cursor != NULL && *cursor != target) cursor = &(*cursor)->next;
    if (*cursor == target) *cursor = target->next;
    free(target);
}

static int page_span(uint32_t page, uint32_t count, uint32_t *base_out,
    uint32_t *size_out)
{
    uint32_t base;
    uint32_t size;
    if (page > UINT32_MAX / UINT32_C(4096) || count == 0u ||
        count > UINT32_MAX / UINT32_C(4096)) return 0;
    base = page * UINT32_C(4096);
    size = count * UINT32_C(4096);
    if (base > UINT32_MAX - size) return 0;
    if (base_out != NULL) *base_out = base;
    if (size_out != NULL) *size_out = size;
    return 1;
}

static void release_owner(void *context)
{
    session *owner = (session *)context;
    physical_mapping_record *record = records;
    while (record != NULL) {
        physical_mapping_record *next = record->next;
        if (record->owner == owner) {
            remove_record(record);
        }
        record = next;
    }
    {
        physical_alias_record *alias = aliases;
        while (alias != NULL) {
            physical_alias_record *next = alias->next;
            if (alias->owner == owner) remove_alias(alias);
            alias = next;
        }
    }
}

int mvdm_softpc_physical_mapping_publish(void *host_bytes,
    uint32_t byte_count, uint32_t *identifier_out)
{
    session *owner = session_thread_current();
    physical_mapping_record *record;
    uint32_t identifier;
    int teardown_registered = 0;

    MAPPING_OBSERVE("publish.call", (uint32_t)(uintptr_t)host_bytes, byte_count, 0);
    if (identifier_out != NULL) *identifier_out = 0u;
    if (owner == NULL || !session_valid(owner) || host_bytes == NULL ||
        byte_count == 0u) return 0;
    if ((uintptr_t)host_bytes > UINT32_MAX) return 0;
    identifier = (uint32_t)(uintptr_t)host_bytes;
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
    if (!teardown_registered) {
        physical_alias_record *alias;
        for (alias = aliases; alias != NULL; alias = alias->next) {
            if (alias->owner == owner) {
                teardown_registered = 1;
                break;
            }
        }
    }
    record = (physical_mapping_record *)calloc(1u, sizeof(*record));
    if (record == NULL) {
        return 0;
    }
    record->owner = owner;
    record->identifier = identifier;
    record->host_address = (uintptr_t)host_bytes;
    record->source_size = byte_count;
    record->next = records;
    records = record;
    if (!teardown_registered &&
        !session_register_teardown(owner, release_owner, owner)) {
        remove_record(record);
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
    uint32_t alignment;
    uint32_t total;

    MAPPING_OBSERVE("prepare.call", identifier, byte_count, 0);
    if (alignment_out != NULL) *alignment_out = 0u;
    if (owner == NULL || !session_valid(owner) || byte_count == 0u ||
        (record = find_identifier(owner, identifier)) == NULL ||
        record->active != 0u || record->source_size != byte_count) {
        MAPPING_OBSERVE("prepare.rejected", identifier, byte_count, 0);
        return 0;
    }
    alignment = (uint32_t)(record->host_address & 3u);
    if (add_overflow(byte_count, alignment, &total)) return 0;
    if (add_overflow(total, UINT32_C(4095), &total)) return 0;
    record->prepared_size = total & ~UINT32_C(4095);
    MAPPING_OBSERVE("prepare.ready", identifier, record->prepared_size, alignment);
    if (alignment_out != NULL) *alignment_out = alignment;
    return 1;
}

void mvdm_softpc_physical_mapping_set(uint32_t identifier,
    uint32_t intel_address, uint32_t byte_count)
{
    session *owner = session_thread_current();
    physical_mapping_record *record;

    if (owner == NULL || !session_valid(owner)) return;
    record = find_identifier(owner, identifier);
    if (record != NULL && record->active == 0u &&
        record->prepared_size == byte_count) {
        record->guest_base = intel_address;
        record->active = 1u;
        MAPPING_OBSERVE("set.activated", identifier, intel_address, byte_count);
        return;
    }
    for (record = records; record != NULL; record = record->next) {
        if (record->owner == owner && record->active != 0u &&
            record->guest_base == intel_address && record->prepared_size == byte_count) {
            remove_record(record);
            MAPPING_OBSERVE("set.removed", identifier, intel_address, byte_count);
            return;
        }
    }
    MAPPING_OBSERVE("set.no-match", identifier, intel_address, byte_count);
}

int32_t VdmMapDosMemory(uint32_t dos_intel_page, uint32_t vdm_intel_page,
    uint32_t page_count)
{
    session *owner = session_thread_current();
    physical_alias_record *record;
    physical_mapping_record *mapping;
    uint32_t destination_base;
    uint32_t source_base;
    uint32_t byte_count;
    int teardown_registered = 0;

    MAPPING_OBSERVE("VdmMapDosMemory.call", dos_intel_page, vdm_intel_page, page_count);
    if (owner == NULL || !session_valid(owner) ||
        !page_span(dos_intel_page, page_count, &destination_base, &byte_count) ||
        !page_span(vdm_intel_page, page_count, &source_base, NULL))
        return (int32_t)0xc000000du; /* STATUS_INVALID_PARAMETER */
    for (record = aliases; record != NULL; record = record->next) {
        if (record->owner == owner && record->destination_base == destination_base &&
            record->byte_count == byte_count) {
            record->source_base = source_base;
            MAPPING_OBSERVE("map.replaced", destination_base, source_base, byte_count);
            return 0;
        }
    }
    for (mapping = records; mapping != NULL; mapping = mapping->next) {
        if (mapping->owner == owner) {
            teardown_registered = 1;
            break;
        }
    }
    if (!teardown_registered) {
        for (record = aliases; record != NULL; record = record->next) {
            if (record->owner == owner) {
                teardown_registered = 1;
                break;
            }
        }
    }
    record = (physical_alias_record *)calloc(1u, sizeof(*record));
    if (record == NULL) return (int32_t)0xc0000017u; /* STATUS_NO_MEMORY */
    record->owner = owner;
    record->destination_base = destination_base;
    record->source_base = source_base;
    record->byte_count = byte_count;
    record->next = aliases;
    aliases = record;
    if (!teardown_registered &&
        !session_register_teardown(owner, release_owner, owner)) {
        remove_alias(record);
        return (int32_t)0xc0000001u; /* STATUS_UNSUCCESSFUL */
    }
    MAPPING_OBSERVE("map.created", destination_base, source_base, byte_count);
    return 0;
}

int32_t VdmUnmapDosMemory(uint32_t dos_intel_page, uint32_t page_count)
{
    session *owner = session_thread_current();
    physical_alias_record *record;
    uint32_t destination_base;
    uint32_t byte_count;
    MAPPING_OBSERVE("VdmUnmapDosMemory.call", dos_intel_page, page_count, 0);
    if (owner == NULL || !session_valid(owner) ||
        !page_span(dos_intel_page, page_count, &destination_base, &byte_count))
        return (int32_t)0xc000000du; /* STATUS_INVALID_PARAMETER */
    for (record = aliases; record != NULL; record = record->next) {
        if (record->owner == owner && record->destination_base == destination_base &&
            record->byte_count == byte_count) {
            remove_alias(record);
            MAPPING_OBSERVE("unmap.removed", destination_base, byte_count, 0);
            return 0;
        }
    }
    MAPPING_OBSERVE("unmap.not-found", destination_base, byte_count, 0);
    return (int32_t)0xc0000225u; /* STATUS_NOT_FOUND */
}

int mvdm_softpc_physical_mapping_translate(uint32_t intel_address,
    uint32_t *translated_address_out)
{
    session *owner = session_thread_current();
    physical_alias_record *record;
    MAPPING_OBSERVE("translate.observer-active", intel_address, 0, 0);
    if (translated_address_out != NULL) *translated_address_out = intel_address;
    if (owner == NULL || !session_valid(owner) || translated_address_out == NULL)
        return 0;
    for (record = aliases; record != NULL; record = record->next) {
        uint32_t offset;
        if (record->owner != owner || intel_address < record->destination_base)
            continue;
        offset = intel_address - record->destination_base;
        if (offset >= record->byte_count) continue;
        *translated_address_out = record->source_base + offset;
        MAPPING_OBSERVE("translate.alias-hit", intel_address, *translated_address_out, 0);
        return 1;
    }
    return 0;
}

int mvdm_softpc_physical_mapping_resolve(uint32_t intel_address,
    uint8_t **host_byte_out)
{
    session *owner = session_thread_current();
    physical_mapping_record *record;

    if (host_byte_out != NULL) *host_byte_out = NULL;
    if (owner == NULL || !session_valid(owner) || host_byte_out == NULL) return 0;
    for (record = records; record != NULL; record = record->next) {
        uint32_t offset;
        uint32_t alignment;
        if (record->owner != owner || record->active == 0u ||
            intel_address < record->guest_base) continue;
        offset = intel_address - record->guest_base;
        alignment = (uint32_t)(record->host_address & (uintptr_t)3u);
        if (offset < alignment || offset - alignment >= record->source_size)
            continue;
        *host_byte_out = (uint8_t *)(record->host_address -
            (record->host_address & (uintptr_t)3u) + offset);
        MAPPING_OBSERVE("resolve.external-hit", intel_address, record->guest_base, 0);
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
    remove_record(record);
}

/* Historical SoftPC spelling retained at the original call sites. The
 * adapter owns the source-derived CCPU page-binding behavior. */
void VdmSetPhysRecStructs(uint32_t host_address, uint32_t intel_address,
    uint32_t byte_count)
{
    MAPPING_OBSERVE("VdmSetPhysRecStructs.call", host_address, intel_address, byte_count);
    mvdm_softpc_physical_mapping_set(host_address, intel_address, byte_count);
}
