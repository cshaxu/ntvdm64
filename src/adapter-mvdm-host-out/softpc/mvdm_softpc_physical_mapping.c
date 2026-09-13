#include "mvdm_softpc_physical_mapping.h"

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <windows.h>

#include "session/session.h"

typedef struct physical_mapping_record {
    session *owner;
    uintptr_t normal_base;
    uint32_t page_count;
    uint32_t *pages;
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

void mvdm_softpc_mapping_observe(unsigned slot, const char *event,
    uint32_t a, uint32_t b, uint32_t c)
{
    static volatile LONG observed[96];
    if (slot < 96 && !observed[slot] &&
        InterlockedCompareExchange(&observed[slot], 1, 0) == 0)
        mapping_observe(event, a, b, c);
}

static physical_mapping_record *find_owner(session *owner)
{
    physical_mapping_record *record;
    for (record = records; record != NULL; record = record->next)
        if (record->owner == owner) return record;
    return NULL;
}

static void remove_record(physical_mapping_record *target)
{
    physical_mapping_record **cursor = &records;
    while (*cursor != NULL && *cursor != target) cursor = &(*cursor)->next;
    if (*cursor == target) *cursor = target->next;
    free(target->pages);
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

/* The original void setter cannot report allocation failure. Allocate all
 * translation slots before InitIntelMemory publishes success. This is the
 * bounded substitute for the unavailable PhysicalPageREC implementation,
 * not an extra protocol imposed on VdmAddVirtualMemory callers. */
int mvdm_softpc_physical_mapping_initialize(void *normal_base, uint32_t size)
{
    session *owner = session_thread_current();
    physical_mapping_record *record;
    if (owner == NULL || !session_valid(owner) || normal_base == NULL ||
        size == 0 || (size & 4095u) != 0 || find_owner(owner) != NULL)
        return 0;
    record = (physical_mapping_record *)calloc(1, sizeof(*record));
    if (record == NULL) return 0;
    record->pages = (uint32_t *)calloc(size >> 12, sizeof(*record->pages));
    if (record->pages == NULL) { free(record); return 0; }
    record->owner = owner;
    record->normal_base = (uintptr_t)normal_base;
    record->page_count = size >> 12;
    if (!session_register_teardown(owner, release_owner, owner)) {
        free(record->pages);
        free(record);
        return 0;
    }
    record->next = records;
    records = record;
    MAPPING_OBSERVE("pages.initialized", size, record->page_count, 0);
    return 1;
}

void mvdm_softpc_physical_mapping_release(void)
{
    session *owner = session_thread_current();
    if (owner != NULL) release_owner(owner);
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
    physical_mapping_record *record = find_owner(session_thread_current());
    uint32_t host;
    if (host_byte_out != NULL) *host_byte_out = NULL;
    if (record == NULL || host_byte_out == NULL ||
        (intel_address >> 12) >= record->page_count) return 0;
    host = record->pages[intel_address >> 12];
    if (host == 0) return 0;
    *host_byte_out = (uint8_t *)((uintptr_t)host + (intel_address & 4095u));
    MAPPING_OBSERVE("resolve.external-hit", intel_address, host, 0);
    return 1;
}

/* Original nt_mem passes a DWORD-aligned host start and a page-rounded span.
 * Removal supplies the ordinary backing address, not a magic zero identity.
 * All slots already exist: this operation cannot fail from heap exhaustion. */
void VdmSetPhysRecStructs(uint32_t host_address, uint32_t intel_address,
    uint32_t byte_count)
{
    physical_mapping_record *record = find_owner(session_thread_current());
    uint32_t first = intel_address >> 12, count = byte_count >> 12, i;
    int normal;
    MAPPING_OBSERVE("VdmSetPhysRecStructs.call", host_address, intel_address, byte_count);
    if (record == NULL || host_address == 0 || (intel_address & 4095u) ||
        (byte_count & 4095u) || count == 0 || first >= record->page_count ||
        count > record->page_count - first ||
        byte_count - 1u > UINT32_MAX - host_address) return;
    normal = (uintptr_t)host_address == record->normal_base + intel_address;
    for (i = 0; i < count; ++i)
        record->pages[first + i] = normal ? 0 : host_address + (i << 12);
    if (normal) {
        MAPPING_OBSERVE("set.normal-restored", intel_address, byte_count, 0);
    } else {
        MAPPING_OBSERVE("set.activated", host_address, intel_address, byte_count);
    }
}
