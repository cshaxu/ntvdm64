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

/* A lazy directory move never copies an admitted host-base tree.  This record
 * keeps the visible destination prefix and the private effective-source
 * prefix; both remain DOS-relative values inside the provider. */
typedef struct bx_ntvdm_dem_overlay_store_v1_relocation {
    uint8_t drive_index;
    uint8_t reserved0[3];
    wchar_t destination[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
    wchar_t source[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
} bx_ntvdm_dem_overlay_store_v1_relocation;

/* Session-private COW state.  It has no guest pointer, host path or handle.
 * The base host namespace is intentionally not mutated by this component. */
typedef struct bx_ntvdm_dem_overlay_store_v1 {
    uint32_t magic, abi_version, struct_bytes, count, capacity;
    bx_ntvdm_dem_overlay_store_v1_entry *entries;
    uint32_t relocation_count, relocation_capacity;
    bx_ntvdm_dem_overlay_store_v1_relocation *relocations;
} bx_ntvdm_dem_overlay_store_v1;

int bx_ntvdm_dem_overlay_store_v1_initialize(bx_ntvdm_dem_overlay_store_v1 *store);
int bx_ntvdm_dem_overlay_store_v1_valid(const bx_ntvdm_dem_overlay_store_v1 *store);
void bx_ntvdm_dem_overlay_store_v1_teardown(bx_ntvdm_dem_overlay_store_v1 *store);
/* Copy-on-write input: an existing base file is copied by the view resolver,
 * then installed here before any mutation. */
int bx_ntvdm_dem_overlay_store_v1_put_file(bx_ntvdm_dem_overlay_store_v1 *store,
    uint8_t drive_index, const wchar_t *relative, uint32_t attributes,
    const uint8_t *bytes, uint32_t byte_count);
/* Copies one existing private file node to another private path. This is a
 * store-only transaction primitive; source and destination host paths are
 * never consulted or changed. */
int bx_ntvdm_dem_overlay_store_v1_copy_file(
    bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive_index,
    const wchar_t *source, const wchar_t *destination);
int bx_ntvdm_dem_overlay_store_v1_set_attributes(
    bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive_index,
    const wchar_t *relative, uint32_t attributes);
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
int bx_ntvdm_dem_overlay_store_v1_has_descendant(
    const bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive_index,
    const wchar_t *relative);

/* Adds a private lazy directory relocation.  `destination` is the path DOS
 * sees after rename; `source` is the effective pre-rename tree.  It has no
 * host-side effect and rejects self/ancestor cycles. */
int bx_ntvdm_dem_overlay_store_v1_add_relocation(
    bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive_index,
    const wchar_t *destination, const wchar_t *source);

/* Resolves the longest matching visible destination prefix through any
 * private relocation chain.  The output remains a bounded DOS-relative path;
 * a cycle or invalid model is rejected. */
int bx_ntvdm_dem_overlay_store_v1_resolve_relocation(
    const bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive_index,
    const wchar_t *visible_relative,
    wchar_t effective_relative[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE]);

#endif
