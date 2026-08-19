#ifndef BX_NTVDM_DEM_OVERLAY_FILE_V1_H
#define BX_NTVDM_DEM_OVERLAY_FILE_V1_H

#include <stdint.h>
#include <wchar.h>

#include "bx_ntvdm_dem_overlay_store_v1.h"

#define BX_NTVDM_DEM_OVERLAY_FILE_V1_MAGIC 0x42584f46u
#define BX_NTVDM_DEM_OVERLAY_FILE_V1_VERSION 1u
#define BX_NTVDM_DEM_OVERLAY_FILE_V1_MAX_HANDLES 64u

/* This is a provider-private file object.  It deliberately has no BOP,
 * guest pointer, host pathname or Win32 HANDLE dependency.  A later view
 * resolver supplies a copied base file only on the first Overlay open. */
enum bx_ntvdm_dem_overlay_file_v1_access {
    BX_NTVDM_DEM_OVERLAY_FILE_V1_READ = 1u,
    BX_NTVDM_DEM_OVERLAY_FILE_V1_WRITE = 2u
};

enum bx_ntvdm_dem_overlay_file_v1_seek_origin {
    BX_NTVDM_DEM_OVERLAY_FILE_V1_SEEK_BEGIN = 0u,
    BX_NTVDM_DEM_OVERLAY_FILE_V1_SEEK_CURRENT = 1u,
    BX_NTVDM_DEM_OVERLAY_FILE_V1_SEEK_END = 2u
};

typedef struct bx_ntvdm_dem_overlay_file_v1_handle {
    uint32_t generation, in_use, access, share_access, position;
    uint8_t drive_index;
    uint8_t reserved0[3];
    wchar_t relative[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
} bx_ntvdm_dem_overlay_file_v1_handle;

typedef struct bx_ntvdm_dem_overlay_file_v1 {
    uint32_t magic, abi_version, struct_bytes, reserved0;
    bx_ntvdm_dem_overlay_store_v1 *store;
    bx_ntvdm_dem_overlay_file_v1_handle
        handles[BX_NTVDM_DEM_OVERLAY_FILE_V1_MAX_HANDLES];
} bx_ntvdm_dem_overlay_file_v1;

int bx_ntvdm_dem_overlay_file_v1_initialize(bx_ntvdm_dem_overlay_file_v1 *files,
    bx_ntvdm_dem_overlay_store_v1 *store);
int bx_ntvdm_dem_overlay_file_v1_valid(const bx_ntvdm_dem_overlay_file_v1 *files);
void bx_ntvdm_dem_overlay_file_v1_teardown(bx_ntvdm_dem_overlay_file_v1 *files);

/* `base_*` is an already-copied source supplied by the caller.  No host I/O
 * occurs here.  If `create_if_missing` is nonzero an absent path starts empty;
 * otherwise an absent path fails. */
int bx_ntvdm_dem_overlay_file_v1_open(bx_ntvdm_dem_overlay_file_v1 *files,
    uint8_t drive_index, const wchar_t *relative, uint32_t access,
    const uint8_t *base_bytes, uint32_t base_byte_count, uint32_t base_attributes,
    int base_exists, int create_if_missing, uint32_t *token_out);
int bx_ntvdm_dem_overlay_file_v1_open_shared(bx_ntvdm_dem_overlay_file_v1 *files,
    uint8_t drive_index, const wchar_t *relative, uint32_t access, uint32_t share_access,
    const uint8_t *base_bytes, uint32_t base_byte_count, uint32_t base_attributes,
    int base_exists, int create_if_missing, uint32_t *token_out);
int bx_ntvdm_dem_overlay_file_v1_read(bx_ntvdm_dem_overlay_file_v1 *files,
    uint32_t token, uint8_t *bytes, uint32_t capacity, uint32_t *byte_count_out);
int bx_ntvdm_dem_overlay_file_v1_write(bx_ntvdm_dem_overlay_file_v1 *files,
    uint32_t token, const uint8_t *bytes, uint32_t byte_count,
    uint32_t *written_out);
int bx_ntvdm_dem_overlay_file_v1_seek(bx_ntvdm_dem_overlay_file_v1 *files,
    uint32_t token, int32_t distance, uint32_t origin, uint32_t *position_out);
int bx_ntvdm_dem_overlay_file_v1_truncate(bx_ntvdm_dem_overlay_file_v1 *files,
    uint32_t token);
int bx_ntvdm_dem_overlay_file_v1_close(bx_ntvdm_dem_overlay_file_v1 *files,
    uint32_t token);
int bx_ntvdm_dem_overlay_file_v1_info(bx_ntvdm_dem_overlay_file_v1 *files,
    uint32_t token, uint32_t *attributes_out, uint32_t *size_out,
    uint16_t *time_out, uint16_t *date_out);
int bx_ntvdm_dem_overlay_file_v1_set_dos_time(bx_ntvdm_dem_overlay_file_v1 *files,
    uint32_t token, uint16_t dos_time, uint16_t dos_date);

#endif
