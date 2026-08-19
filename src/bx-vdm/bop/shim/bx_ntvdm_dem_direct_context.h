#ifndef BX_NTVDM_BOP_SHIM_DEM_DIRECT_CONTEXT_H
#define BX_NTVDM_BOP_SHIM_DEM_DIRECT_CONTEXT_H

/*
 * Direct-only DEM host-composition seam.
 *
 * This is not an OpenNT DEM provider or dispatcher.  A mirror file uses it
 * only where the historical CCPU/SAS composition supplied a native HANDLE or
 * an OEM pathname helper that cannot cross the modern guest ABI unchanged.
 */

#include <stdint.h>
#include <windows.h>

typedef struct bx_ntvdm_dem_direct_context {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t reserved0;
    void *state;
    int (*lookup_handle)(void *state, uint32_t token, HANDLE *handle_out);
    int (*release_handle)(void *state, uint32_t token, DWORD *error_out);
    int (*query_attributes)(void *state, uint8_t drive,
        const wchar_t *relative_path, DWORD *attributes_out, DWORD *error_out);
    int (*set_attributes)(void *state, uint8_t drive,
        const wchar_t *relative_path, DWORD attributes, DWORD *error_out);
} bx_ntvdm_dem_direct_context;

#define BX_NTVDM_DEM_DIRECT_CONTEXT_MAGIC 0x42584443u
#define BX_NTVDM_DEM_DIRECT_CONTEXT_VERSION 1u

int bx_ntvdm_dem_direct_context_valid(
    const bx_ntvdm_dem_direct_context *context);

#endif
