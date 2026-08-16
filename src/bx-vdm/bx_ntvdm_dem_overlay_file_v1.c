#include "bx_ntvdm_dem_overlay_file_v1.h"

#include <limits.h>
#include <string.h>
#include <windows.h>

static uint32_t token_for(uint32_t index, uint32_t generation)
{ return (generation << 16) | (index + 1u); }

static int valid_path(uint8_t drive, const wchar_t *relative)
{ return drive < 26u && relative != 0 && relative[0] != L'\0' &&
    wcslen(relative) < BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE; }

static int decode(uint32_t token, uint32_t *index_out, uint32_t *generation_out)
{
    uint32_t index;
    if (token == 0u || index_out == 0 || generation_out == 0) return 0;
    index = (token & 0xffffu) - 1u;
    if (index >= BX_NTVDM_DEM_OVERLAY_FILE_V1_MAX_HANDLES || token >> 16 == 0u) return 0;
    *index_out = index; *generation_out = token >> 16;
    return 1;
}

static bx_ntvdm_dem_overlay_file_v1_handle *handle_for(
    bx_ntvdm_dem_overlay_file_v1 *files, uint32_t token, uint32_t required_access)
{
    uint32_t index, generation;
    bx_ntvdm_dem_overlay_file_v1_handle *handle;
    if (!bx_ntvdm_dem_overlay_file_v1_valid(files) || !decode(token, &index, &generation)) return 0;
    handle = &files->handles[index];
    if (!handle->in_use || handle->generation != generation ||
        (handle->access & required_access) != required_access) return 0;
    return handle;
}

static void clear_handle(bx_ntvdm_dem_overlay_file_v1_handle *handle)
{
    memset(handle->relative, 0, sizeof(handle->relative));
    handle->in_use = handle->access = handle->position = 0u;
    handle->drive_index = 0u;
    ++handle->generation;
    if (handle->generation == 0u) handle->generation = 1u;
}

int bx_ntvdm_dem_overlay_file_v1_valid(const bx_ntvdm_dem_overlay_file_v1 *files)
{
    uint32_t index;
    if (files == 0 || files->magic != BX_NTVDM_DEM_OVERLAY_FILE_V1_MAGIC ||
        files->abi_version != BX_NTVDM_DEM_OVERLAY_FILE_V1_VERSION ||
        files->struct_bytes != sizeof(*files) || files->reserved0 != 0u ||
        !bx_ntvdm_dem_overlay_store_v1_valid(files->store)) return 0;
    for (index = 0u; index < BX_NTVDM_DEM_OVERLAY_FILE_V1_MAX_HANDLES; ++index) {
        const bx_ntvdm_dem_overlay_file_v1_handle *handle = &files->handles[index];
        if (handle->generation == 0u || handle->in_use > 1u ||
            (handle->in_use && (!valid_path(handle->drive_index, handle->relative) ||
             handle->access == 0u || (handle->access & ~3u) != 0u))) return 0;
    }
    return 1;
}

int bx_ntvdm_dem_overlay_file_v1_initialize(bx_ntvdm_dem_overlay_file_v1 *files,
    bx_ntvdm_dem_overlay_store_v1 *store)
{
    uint32_t index;
    if (files == 0 || !bx_ntvdm_dem_overlay_store_v1_valid(store)) return 0;
    memset(files, 0, sizeof(*files));
    files->magic = BX_NTVDM_DEM_OVERLAY_FILE_V1_MAGIC;
    files->abi_version = BX_NTVDM_DEM_OVERLAY_FILE_V1_VERSION;
    files->struct_bytes = sizeof(*files); files->store = store;
    for (index = 0u; index < BX_NTVDM_DEM_OVERLAY_FILE_V1_MAX_HANDLES; ++index)
        files->handles[index].generation = 1u;
    return bx_ntvdm_dem_overlay_file_v1_valid(files);
}

void bx_ntvdm_dem_overlay_file_v1_teardown(bx_ntvdm_dem_overlay_file_v1 *files)
{ if (files != 0) memset(files, 0, sizeof(*files)); }

int bx_ntvdm_dem_overlay_file_v1_open(bx_ntvdm_dem_overlay_file_v1 *files,
    uint8_t drive, const wchar_t *relative, uint32_t access,
    const uint8_t *base_bytes, uint32_t base_byte_count, uint32_t base_attributes,
    int base_exists, int create_if_missing, uint32_t *token_out)
{
    uint32_t index;
    const bx_ntvdm_dem_overlay_store_v1_entry *entry;
    if (token_out != 0) *token_out = 0u;
    if (!bx_ntvdm_dem_overlay_file_v1_valid(files) || token_out == 0 ||
        !valid_path(drive, relative) || access == 0u || (access & ~3u) != 0u ||
        (!base_exists && base_byte_count != 0u) ||
        (base_byte_count != 0u && base_bytes == 0)) return 0;
    entry = bx_ntvdm_dem_overlay_store_v1_lookup(files->store, drive, relative);
    if (entry != 0 && entry->state == BX_NTVDM_DEM_OVERLAY_STORE_V1_TOMBSTONE) {
        if (!create_if_missing || !bx_ntvdm_dem_overlay_store_v1_put_file(files->store,
                drive, relative, base_attributes, 0, 0u)) return 0;
        entry = bx_ntvdm_dem_overlay_store_v1_lookup(files->store, drive, relative);
    }
    if (entry == 0 && !base_exists && !create_if_missing) return 0;
    if (entry == 0 && !bx_ntvdm_dem_overlay_store_v1_put_file(files->store, drive,
            relative, base_attributes, base_exists ? base_bytes : 0,
            base_exists ? base_byte_count : 0u)) return 0;
    for (index = 0u; index < BX_NTVDM_DEM_OVERLAY_FILE_V1_MAX_HANDLES; ++index) {
        bx_ntvdm_dem_overlay_file_v1_handle *handle = &files->handles[index];
        if (!handle->in_use) {
            handle->in_use = 1u; handle->access = access; handle->position = 0u;
            handle->drive_index = drive;
            wcscpy_s(handle->relative, BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE, relative);
            *token_out = token_for(index, handle->generation);
            return bx_ntvdm_dem_overlay_file_v1_valid(files);
        }
    }
    return 0;
}

int bx_ntvdm_dem_overlay_file_v1_read(bx_ntvdm_dem_overlay_file_v1 *files,
    uint32_t token, uint8_t *bytes, uint32_t capacity, uint32_t *byte_count_out)
{
    bx_ntvdm_dem_overlay_file_v1_handle *handle = handle_for(files, token,
        BX_NTVDM_DEM_OVERLAY_FILE_V1_READ);
    const bx_ntvdm_dem_overlay_store_v1_entry *entry;
    uint32_t count;
    if (byte_count_out != 0) *byte_count_out = 0u;
    if (handle == 0 || byte_count_out == 0 || (capacity != 0u && bytes == 0)) return 0;
    entry = bx_ntvdm_dem_overlay_store_v1_lookup(files->store, handle->drive_index, handle->relative);
    if (entry == 0 || entry->state != BX_NTVDM_DEM_OVERLAY_STORE_V1_FILE ||
        handle->position > entry->byte_count) return 0;
    count = entry->byte_count - handle->position;
    if (count > capacity) count = capacity;
    if (count != 0u) memcpy(bytes, entry->bytes + handle->position, count);
    handle->position += count; *byte_count_out = count;
    return 1;
}

int bx_ntvdm_dem_overlay_file_v1_write(bx_ntvdm_dem_overlay_file_v1 *files,
    uint32_t token, const uint8_t *bytes, uint32_t byte_count, uint32_t *written_out)
{
    bx_ntvdm_dem_overlay_file_v1_handle *handle = handle_for(files, token,
        BX_NTVDM_DEM_OVERLAY_FILE_V1_WRITE);
    const bx_ntvdm_dem_overlay_store_v1_entry *entry;
    uint8_t *copy;
    uint32_t end, write_end;
    if (written_out != 0) *written_out = 0u;
    if (handle == 0 || written_out == 0 || (byte_count != 0u && bytes == 0)) return 0;
    if (byte_count > UINT32_MAX - handle->position) return 0;
    entry = bx_ntvdm_dem_overlay_store_v1_lookup(files->store, handle->drive_index, handle->relative);
    if (entry == 0 || entry->state != BX_NTVDM_DEM_OVERLAY_STORE_V1_FILE) return 0;
    write_end = handle->position + byte_count;
    end = entry->byte_count > write_end ? entry->byte_count : write_end;
    copy = end == 0u ? 0 : (uint8_t *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, end);
    if (end != 0u && copy == 0) return 0;
    if (entry->byte_count != 0u) memcpy(copy, entry->bytes, entry->byte_count);
    if (byte_count != 0u) memcpy(copy + handle->position, bytes, byte_count);
    if (!bx_ntvdm_dem_overlay_store_v1_put_file(files->store, handle->drive_index,
            handle->relative, entry->attributes, copy, end)) {
        if (copy != 0) HeapFree(GetProcessHeap(), 0u, copy); return 0;
    }
    if (copy != 0) HeapFree(GetProcessHeap(), 0u, copy);
    handle->position = write_end; *written_out = byte_count;
    return 1;
}

int bx_ntvdm_dem_overlay_file_v1_seek(bx_ntvdm_dem_overlay_file_v1 *files,
    uint32_t token, int32_t distance, uint32_t origin, uint32_t *position_out)
{
    bx_ntvdm_dem_overlay_file_v1_handle *handle = handle_for(files, token, 0u);
    const bx_ntvdm_dem_overlay_store_v1_entry *entry;
    int64_t base, target;
    if (position_out != 0) *position_out = 0u;
    if (handle == 0 || position_out == 0) return 0;
    entry = bx_ntvdm_dem_overlay_store_v1_lookup(files->store, handle->drive_index, handle->relative);
    if (entry == 0 || entry->state != BX_NTVDM_DEM_OVERLAY_STORE_V1_FILE) return 0;
    if (origin == BX_NTVDM_DEM_OVERLAY_FILE_V1_SEEK_BEGIN) base = 0;
    else if (origin == BX_NTVDM_DEM_OVERLAY_FILE_V1_SEEK_CURRENT) base = handle->position;
    else if (origin == BX_NTVDM_DEM_OVERLAY_FILE_V1_SEEK_END) base = entry->byte_count;
    else return 0;
    target = base + distance;
    if (target < 0 || target > UINT32_MAX) return 0;
    handle->position = (uint32_t)target; *position_out = handle->position;
    return 1;
}

int bx_ntvdm_dem_overlay_file_v1_truncate(bx_ntvdm_dem_overlay_file_v1 *files,
    uint32_t token)
{
    bx_ntvdm_dem_overlay_file_v1_handle *handle = handle_for(files, token,
        BX_NTVDM_DEM_OVERLAY_FILE_V1_WRITE);
    const bx_ntvdm_dem_overlay_store_v1_entry *entry;
    if (handle == 0) return 0;
    entry = bx_ntvdm_dem_overlay_store_v1_lookup(files->store, handle->drive_index, handle->relative);
    return entry != 0 && entry->state == BX_NTVDM_DEM_OVERLAY_STORE_V1_FILE &&
        bx_ntvdm_dem_overlay_store_v1_put_file(files->store, handle->drive_index,
            handle->relative, entry->attributes, entry->bytes, handle->position);
}

int bx_ntvdm_dem_overlay_file_v1_close(bx_ntvdm_dem_overlay_file_v1 *files,
    uint32_t token)
{
    bx_ntvdm_dem_overlay_file_v1_handle *handle = handle_for(files, token, 0u);
    if (handle == 0) return 0;
    clear_handle(handle);
    return bx_ntvdm_dem_overlay_file_v1_valid(files);
}
