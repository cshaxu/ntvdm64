#include "bx_ntvdm_dem_overlay_store_v1.h"

#include <string.h>
#include <windows.h>

static int valid_path(uint8_t drive, const wchar_t *relative)
{ return drive < 26u && relative != 0 && relative[0] != L'\0' &&
    wcslen(relative) < BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE; }

static void timestamp(bx_ntvdm_dem_overlay_store_v1_entry *entry)
{
    SYSTEMTIME now;
    FILETIME utc, local;
    WORD date = 0u, time = 0u;
    if (entry == 0) return;
    GetLocalTime(&now);
    if (SystemTimeToFileTime(&now, &utc) && FileTimeToLocalFileTime(&utc, &local))
        (void)FileTimeToDosDateTime(&local, &date, &time);
    entry->dos_time = time; entry->dos_date = date;
}

static int locate(const bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive,
    const wchar_t *relative, uint32_t *index_out)
{
    uint32_t index;
    if (!bx_ntvdm_dem_overlay_store_v1_valid(store) || !valid_path(drive, relative)) return 0;
    for (index = 0u; index < store->count; ++index)
        if (store->entries[index].drive_index == drive &&
            _wcsicmp(store->entries[index].relative, relative) == 0) {
            if (index_out != 0) *index_out = index;
            return 1;
        }
    return 0;
}

static int prefix(const wchar_t *path, const wchar_t *candidate)
{
    size_t length;
    if (path == 0 || candidate == 0) return 0;
    length = wcslen(candidate);
    return _wcsnicmp(path, candidate, length) == 0 &&
        (path[length] == L'\0' || path[length] == L'\\');
}

int bx_ntvdm_dem_overlay_store_v1_valid(const bx_ntvdm_dem_overlay_store_v1 *store)
{
    uint32_t index;
    if (store == 0 || store->magic != BX_NTVDM_DEM_OVERLAY_STORE_V1_MAGIC ||
        store->abi_version != BX_NTVDM_DEM_OVERLAY_STORE_V1_VERSION ||
        store->struct_bytes != sizeof(*store) || store->count > store->capacity ||
        (store->capacity == 0u) != (store->entries == 0) ||
        store->relocation_count > store->relocation_capacity ||
        (store->relocation_capacity == 0u) != (store->relocations == 0)) return 0;
    for (index = 0u; index < store->count; ++index) {
        const bx_ntvdm_dem_overlay_store_v1_entry *entry = &store->entries[index];
        if (!valid_path(entry->drive_index, entry->relative) ||
            (entry->state != BX_NTVDM_DEM_OVERLAY_STORE_V1_FILE &&
             entry->state != BX_NTVDM_DEM_OVERLAY_STORE_V1_TOMBSTONE &&
             entry->state != BX_NTVDM_DEM_OVERLAY_STORE_V1_DIRECTORY &&
             entry->state != BX_NTVDM_DEM_OVERLAY_STORE_V1_DIRECTORY_TOMBSTONE) ||
            ((entry->state == BX_NTVDM_DEM_OVERLAY_STORE_V1_TOMBSTONE ||
              entry->state == BX_NTVDM_DEM_OVERLAY_STORE_V1_DIRECTORY ||
              entry->state == BX_NTVDM_DEM_OVERLAY_STORE_V1_DIRECTORY_TOMBSTONE) &&
             (entry->bytes != 0 || entry->byte_count != 0u || entry->byte_capacity != 0u)) ||
            (entry->state == BX_NTVDM_DEM_OVERLAY_STORE_V1_FILE &&
             (entry->byte_count > entry->byte_capacity ||
              (entry->byte_count != 0u && entry->bytes == 0)))) return 0;
    }
    for (index = 0u; index < store->relocation_count; ++index) {
        const bx_ntvdm_dem_overlay_store_v1_relocation *entry =
            &store->relocations[index];
        if (!valid_path(entry->drive_index, entry->destination) ||
            !valid_path(entry->drive_index, entry->source) ||
            _wcsicmp(entry->destination, entry->source) == 0 ||
            prefix(entry->destination, entry->source)) return 0;
    }
    return 1;
}

int bx_ntvdm_dem_overlay_store_v1_initialize(bx_ntvdm_dem_overlay_store_v1 *store)
{
    if (store == 0) return 0;
    memset(store, 0, sizeof(*store));
    store->magic = BX_NTVDM_DEM_OVERLAY_STORE_V1_MAGIC;
    store->abi_version = BX_NTVDM_DEM_OVERLAY_STORE_V1_VERSION;
    store->struct_bytes = sizeof(*store);
    return bx_ntvdm_dem_overlay_store_v1_valid(store);
}

void bx_ntvdm_dem_overlay_store_v1_teardown(bx_ntvdm_dem_overlay_store_v1 *store)
{
    uint32_t index;
    if (store == 0) return;
    for (index = 0u; index < store->count; ++index)
        if (store->entries[index].bytes != 0) HeapFree(GetProcessHeap(), 0u, store->entries[index].bytes);
    if (store->entries != 0) HeapFree(GetProcessHeap(), 0u, store->entries);
    if (store->relocations != 0) HeapFree(GetProcessHeap(), 0u, store->relocations);
    memset(store, 0, sizeof(*store));
}

static bx_ntvdm_dem_overlay_store_v1_entry *entry_for(bx_ntvdm_dem_overlay_store_v1 *store,
    uint8_t drive, const wchar_t *relative)
{
    uint32_t index;
    if (locate(store, drive, relative, &index)) return &store->entries[index];
    if (store->count == store->capacity) {
        uint32_t next = store->capacity == 0u ? 8u : store->capacity * 2u;
        void *grown;
        if (next < store->capacity || (size_t)next > SIZE_MAX / sizeof(*store->entries))
            return 0;
        grown = store->entries == 0 ? HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
            (size_t)next * sizeof(*store->entries)) : HeapReAlloc(GetProcessHeap(),
            HEAP_ZERO_MEMORY, store->entries, (size_t)next * sizeof(*store->entries));
        if (grown == 0) return 0;
        store->entries = (bx_ntvdm_dem_overlay_store_v1_entry *)grown;
        store->capacity = next;
    }
    index = store->count++;
    store->entries[index].drive_index = drive;
    wcscpy_s(store->entries[index].relative, BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE, relative);
    return &store->entries[index];
}

int bx_ntvdm_dem_overlay_store_v1_put_file(bx_ntvdm_dem_overlay_store_v1 *store,
    uint8_t drive, const wchar_t *relative, uint32_t attributes,
    const uint8_t *bytes, uint32_t byte_count)
{
    bx_ntvdm_dem_overlay_store_v1_entry *entry;
    uint8_t *copy = 0;
    if (!bx_ntvdm_dem_overlay_store_v1_valid(store) || !valid_path(drive, relative) ||
        (byte_count != 0u && bytes == 0)) return 0;
    if (byte_count != 0u && (copy = (uint8_t *)HeapAlloc(GetProcessHeap(), 0u, byte_count)) == 0) return 0;
    if (byte_count != 0u) memcpy(copy, bytes, byte_count);
    entry = entry_for(store, drive, relative);
    if (entry == 0) { if (copy) HeapFree(GetProcessHeap(), 0u, copy); return 0; }
    if (entry->bytes) HeapFree(GetProcessHeap(), 0u, entry->bytes);
    entry->state = BX_NTVDM_DEM_OVERLAY_STORE_V1_FILE;
    entry->attributes = attributes;
    entry->bytes = copy; entry->byte_count = byte_count; entry->byte_capacity = byte_count;
    timestamp(entry);
    return bx_ntvdm_dem_overlay_store_v1_valid(store);
}

int bx_ntvdm_dem_overlay_store_v1_copy_file(
    bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive,
    const wchar_t *source, const wchar_t *destination)
{
    const bx_ntvdm_dem_overlay_store_v1_entry *entry;
    if (!bx_ntvdm_dem_overlay_store_v1_valid(store) || !valid_path(drive, source) ||
        !valid_path(drive, destination) || _wcsicmp(source, destination) == 0) return 0;
    entry = bx_ntvdm_dem_overlay_store_v1_lookup(store, drive, source);
    if (entry == 0 || entry->state != BX_NTVDM_DEM_OVERLAY_STORE_V1_FILE) return 0;
    return bx_ntvdm_dem_overlay_store_v1_put_file(store, drive, destination,
        entry->attributes, entry->bytes, entry->byte_count);
}

int bx_ntvdm_dem_overlay_store_v1_set_attributes(
    bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive,
    const wchar_t *relative, uint32_t attributes)
{
    uint32_t index;
    if (!locate(store, drive, relative, &index)) return 0;
    if (store->entries[index].state != BX_NTVDM_DEM_OVERLAY_STORE_V1_FILE &&
        store->entries[index].state != BX_NTVDM_DEM_OVERLAY_STORE_V1_DIRECTORY) return 0;
    store->entries[index].attributes = attributes;
    return bx_ntvdm_dem_overlay_store_v1_valid(store);
}

int bx_ntvdm_dem_overlay_store_v1_tombstone(bx_ntvdm_dem_overlay_store_v1 *store,
    uint8_t drive, const wchar_t *relative)
{
    bx_ntvdm_dem_overlay_store_v1_entry *entry;
    if (!bx_ntvdm_dem_overlay_store_v1_valid(store) || !valid_path(drive, relative)) return 0;
    entry = entry_for(store, drive, relative); if (entry == 0) return 0;
    if (entry->bytes) HeapFree(GetProcessHeap(), 0u, entry->bytes);
    entry->bytes = 0; entry->byte_count = entry->byte_capacity = entry->attributes = 0u;
    entry->dos_time = entry->dos_date = 0u;
    entry->state = BX_NTVDM_DEM_OVERLAY_STORE_V1_TOMBSTONE;
    return bx_ntvdm_dem_overlay_store_v1_valid(store);
}

int bx_ntvdm_dem_overlay_store_v1_put_directory(bx_ntvdm_dem_overlay_store_v1 *store,
    uint8_t drive, const wchar_t *relative, uint32_t attributes)
{
    bx_ntvdm_dem_overlay_store_v1_entry *entry;
    if (!bx_ntvdm_dem_overlay_store_v1_valid(store) || !valid_path(drive, relative)) return 0;
    entry = entry_for(store, drive, relative); if (!entry) return 0;
    if (entry->bytes) HeapFree(GetProcessHeap(), 0u, entry->bytes);
    entry->bytes = 0; entry->byte_count = entry->byte_capacity = 0u;
    entry->attributes = attributes; entry->state = BX_NTVDM_DEM_OVERLAY_STORE_V1_DIRECTORY;
    timestamp(entry);
    return bx_ntvdm_dem_overlay_store_v1_valid(store);
}

int bx_ntvdm_dem_overlay_store_v1_tombstone_directory(
    bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive, const wchar_t *relative)
{
    bx_ntvdm_dem_overlay_store_v1_entry *entry;
    if (!bx_ntvdm_dem_overlay_store_v1_valid(store) || !valid_path(drive, relative)) return 0;
    entry = entry_for(store, drive, relative); if (!entry) return 0;
    if (entry->bytes) HeapFree(GetProcessHeap(), 0u, entry->bytes);
    entry->bytes = 0; entry->byte_count = entry->byte_capacity = entry->attributes = 0u;
    entry->dos_time = entry->dos_date = 0u;
    entry->state = BX_NTVDM_DEM_OVERLAY_STORE_V1_DIRECTORY_TOMBSTONE;
    return bx_ntvdm_dem_overlay_store_v1_valid(store);
}

const bx_ntvdm_dem_overlay_store_v1_entry *bx_ntvdm_dem_overlay_store_v1_lookup(
    const bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive, const wchar_t *relative)
{ uint32_t index; return locate(store, drive, relative, &index) ? &store->entries[index] : 0; }

int bx_ntvdm_dem_overlay_store_v1_has_descendant(
    const bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive,
    const wchar_t *relative)
{
    uint32_t index; size_t length;
    if (!bx_ntvdm_dem_overlay_store_v1_valid(store) || !valid_path(drive, relative)) return 0;
    length = wcslen(relative);
    for (index = 0u; index < store->count; ++index) {
        const bx_ntvdm_dem_overlay_store_v1_entry *entry = &store->entries[index];
        if (entry->drive_index == drive && _wcsnicmp(entry->relative, relative, length) == 0 &&
            entry->relative[length] == L'\\' &&
            entry->state != BX_NTVDM_DEM_OVERLAY_STORE_V1_TOMBSTONE &&
            entry->state != BX_NTVDM_DEM_OVERLAY_STORE_V1_DIRECTORY_TOMBSTONE) return 1;
    }
    return 0;
}

static void remove_prefix(bx_ntvdm_dem_overlay_store_v1 *store,
    uint8_t drive, const wchar_t *relative)
{
    uint32_t read, write = 0u;
    for (read = 0u; read < store->count; ++read) {
        bx_ntvdm_dem_overlay_store_v1_entry *entry = &store->entries[read];
        if (entry->drive_index == drive && prefix(entry->relative, relative)) {
            if (entry->bytes != 0) HeapFree(GetProcessHeap(), 0u, entry->bytes);
            continue;
        }
        if (write != read) store->entries[write] = *entry;
        ++write;
    }
    store->count = write;
}

int bx_ntvdm_dem_overlay_store_v1_move_private_subtree(
    bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive,
    const wchar_t *source, const wchar_t *destination)
{
    uint32_t index;
    size_t source_length, destination_length;
    if (!bx_ntvdm_dem_overlay_store_v1_valid(store) || !valid_path(drive, source) ||
        !valid_path(drive, destination) || _wcsicmp(source, destination) == 0 ||
        prefix(destination, source)) return 0;
    source_length = wcslen(source);
    destination_length = wcslen(destination);
    for (index = 0u; index < store->count; ++index) {
        const bx_ntvdm_dem_overlay_store_v1_entry *entry = &store->entries[index];
        if (entry->drive_index == drive && prefix(entry->relative, source) &&
            destination_length + wcslen(entry->relative + source_length) >=
                BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE) return 0;
    }
    /* The visible destination was preflighted by the caller.  Discard only
     * its private hidden residue before changing source paths, so the move
     * has no duplicate key and cannot expose an old tombstone there. */
    remove_prefix(store, drive, destination);
    for (index = 0u; index < store->count; ++index) {
        bx_ntvdm_dem_overlay_store_v1_entry *entry = &store->entries[index];
        if (entry->drive_index == drive && prefix(entry->relative, source)) {
            wchar_t moved[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
            const wchar_t *suffix = entry->relative + source_length;
            wcscpy_s(moved, BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE, destination);
            wcscat_s(moved, BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE, suffix);
            wcscpy_s(entry->relative, BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE, moved);
        }
    }
    return bx_ntvdm_dem_overlay_store_v1_valid(store);
}

int bx_ntvdm_dem_overlay_store_v1_add_relocation(
    bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive,
    const wchar_t *destination, const wchar_t *source)
{
    bx_ntvdm_dem_overlay_store_v1_relocation *entry;
    wchar_t effective_source[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
    uint32_t index;
    if (!bx_ntvdm_dem_overlay_store_v1_valid(store) ||
        !valid_path(drive, destination) || !valid_path(drive, source) ||
        _wcsicmp(destination, source) == 0 || prefix(destination, source)) return 0;
    for (index = 0u; index < store->relocation_count; ++index) {
        entry = &store->relocations[index];
        if (entry->drive_index == drive &&
            _wcsicmp(entry->destination, destination) == 0) return 0;
    }
    /* Normalize through the existing private maps before recording the next
     * move.  It makes an A->B then B->A cycle impossible to install and
     * keeps future lookup to one effective source chain. */
    if (!bx_ntvdm_dem_overlay_store_v1_resolve_relocation(store, drive, source,
            effective_source) || _wcsicmp(destination, effective_source) == 0 ||
        prefix(destination, effective_source)) return 0;
    if (store->relocation_count == store->relocation_capacity) {
        uint32_t next = store->relocation_capacity == 0u ? 8u :
            store->relocation_capacity * 2u;
        void *grown;
        if (next < store->relocation_capacity ||
            (size_t)next > SIZE_MAX / sizeof(*store->relocations)) return 0;
        grown = store->relocations == 0 ? HeapAlloc(GetProcessHeap(),
            HEAP_ZERO_MEMORY, (size_t)next * sizeof(*store->relocations)) :
            HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, store->relocations,
                (size_t)next * sizeof(*store->relocations));
        if (grown == 0) return 0;
        store->relocations = (bx_ntvdm_dem_overlay_store_v1_relocation *)grown;
        store->relocation_capacity = next;
    }
    entry = &store->relocations[store->relocation_count++];
    entry->drive_index = drive;
    wcscpy_s(entry->destination, BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE, destination);
    wcscpy_s(entry->source, BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE, effective_source);
    return bx_ntvdm_dem_overlay_store_v1_valid(store);
}

int bx_ntvdm_dem_overlay_store_v1_resolve_relocation(
    const bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive,
    const wchar_t *visible, wchar_t effective[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE])
{
    wchar_t current[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
    uint32_t hop;
    if (effective != 0) effective[0] = L'\0';
    if (!bx_ntvdm_dem_overlay_store_v1_valid(store) || !valid_path(drive, visible) ||
        effective == 0) return 0;
    wcscpy_s(current, BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE, visible);
    /* One extra pass is the terminal no-match check after the last allowed
     * relocation.  Reaching a further match means a malformed cycle. */
    for (hop = 0u; hop <= store->relocation_count; ++hop) {
        const bx_ntvdm_dem_overlay_store_v1_relocation *best = 0;
        uint32_t index;
        for (index = 0u; index < store->relocation_count; ++index) {
            const bx_ntvdm_dem_overlay_store_v1_relocation *candidate =
                &store->relocations[index];
            if (candidate->drive_index == drive && prefix(current, candidate->destination) &&
                (best == 0 || wcslen(candidate->destination) > wcslen(best->destination)))
                best = candidate;
        }
        if (best == 0) {
            wcscpy_s(effective, BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE, current);
            return 1;
        }
        {
            wchar_t next[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
            const wchar_t *suffix = current + wcslen(best->destination);
            size_t source_length = wcslen(best->source);
            size_t suffix_length = wcslen(suffix);
            if (source_length + suffix_length >= BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE)
                return 0;
            memcpy(next, best->source, source_length * sizeof(*next));
            memcpy(next + source_length, suffix,
                (suffix_length + 1u) * sizeof(*next));
            memcpy(current, next, (source_length + suffix_length + 1u) * sizeof(*current));
        }
    }
    return 0;
}
