#include "bx_ntvdm_dem_overlay_store_v1.h"

#include <string.h>
#include <windows.h>

static int valid_path(uint8_t drive, const wchar_t *relative)
{ return drive < 26u && relative != 0 && relative[0] != L'\0' &&
    wcslen(relative) < BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE; }

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

int bx_ntvdm_dem_overlay_store_v1_valid(const bx_ntvdm_dem_overlay_store_v1 *store)
{
    uint32_t index;
    if (store == 0 || store->magic != BX_NTVDM_DEM_OVERLAY_STORE_V1_MAGIC ||
        store->abi_version != BX_NTVDM_DEM_OVERLAY_STORE_V1_VERSION ||
        store->struct_bytes != sizeof(*store) || store->count > store->capacity ||
        (store->capacity == 0u) != (store->entries == 0)) return 0;
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
    entry->state = BX_NTVDM_DEM_OVERLAY_STORE_V1_DIRECTORY_TOMBSTONE;
    return bx_ntvdm_dem_overlay_store_v1_valid(store);
}

const bx_ntvdm_dem_overlay_store_v1_entry *bx_ntvdm_dem_overlay_store_v1_lookup(
    const bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive, const wchar_t *relative)
{ uint32_t index; return locate(store, drive, relative, &index) ? &store->entries[index] : 0; }
