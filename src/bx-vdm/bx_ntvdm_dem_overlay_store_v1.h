#ifndef BX_NTVDM_DEM_OVERLAY_STORE_V1_H
#define BX_NTVDM_DEM_OVERLAY_STORE_V1_H

#include <stdint.h>
#include <wchar.h>

#include "bx_ntvdm_dem_path_v1.h"

#define BX_NTVDM_DEM_OVERLAY_STORE_V1_MAGIC 0x42584f53u
#define BX_NTVDM_DEM_OVERLAY_STORE_V1_VERSION 1u

enum bx_ntvdm_dem_overlay_store_v1_state {
    BX_NTVDM_DEM_OVERLAY_STORE_V1_FILE = 1u,
    BX_NTVDM_DEM_OVERLAY_STORE_V1_TOMBSTONE = 2u,
    BX_NTVDM_DEM_OVERLAY_STORE_V1_DIRECTORY = 3u,
    BX_NTVDM_DEM_OVERLAY_STORE_V1_DIRECTORY_TOMBSTONE = 4u
};

typedef struct bx_ntvdm_dem_overlay_store_v1_entry {
    uint32_t state, attributes, byte_count, byte_capacity;
    uint8_t drive_index;
    uint8_t reserved0[3];
    wchar_t relative[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
    uint8_t *bytes;
} bx_ntvdm_dem_overlay_store_v1_entry;

/* Session-private COW state.  It has no guest pointer, host path or handle.
 * The base host namespace is intentionally not mutated by this component. */
typedef struct bx_ntvdm_dem_overlay_store_v1 {
    uint32_t magic, abi_version, struct_bytes, count, capacity;
    bx_ntvdm_dem_overlay_store_v1_entry *entries;
} bx_ntvdm_dem_overlay_store_v1;

int bx_ntvdm_dem_overlay_store_v1_initialize(bx_ntvdm_dem_overlay_store_v1 *store);
int bx_ntvdm_dem_overlay_store_v1_valid(const bx_ntvdm_dem_overlay_store_v1 *store);
void bx_ntvdm_dem_overlay_store_v1_teardown(bx_ntvdm_dem_overlay_store_v1 *store);
/* Copy-on-write input: an existing base file is copied by the view resolver,
 * then installed here before any mutation. */
int bx_ntvdm_dem_overlay_store_v1_put_file(bx_ntvdm_dem_overlay_store_v1 *store,
    uint8_t drive_index, const wchar_t *relative, uint32_t attributes,
    const uint8_t *bytes, uint32_t byte_count);
int bx_ntvdm_dem_overlay_store_v1_tombstone(bx_ntvdm_dem_overlay_store_v1 *store,
    uint8_t drive_index, const wchar_t *relative);
int bx_ntvdm_dem_overlay_store_v1_put_directory(bx_ntvdm_dem_overlay_store_v1 *store,
    uint8_t drive_index, const wchar_t *relative, uint32_t attributes);
int bx_ntvdm_dem_overlay_store_v1_tombstone_directory(
    bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive_index,
    const wchar_t *relative);
const bx_ntvdm_dem_overlay_store_v1_entry *bx_ntvdm_dem_overlay_store_v1_lookup(
    const bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive_index,
    const wchar_t *relative);

#endif
