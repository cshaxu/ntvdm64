#ifndef RUNTIME_OPENNT_BOP_DEM_DIRECT_CONTEXT_H
#define RUNTIME_OPENNT_BOP_DEM_DIRECT_CONTEXT_H

/*
 * Direct-only DEM host-composition seam.
 *
 * This is not an OpenNT DEM provider or dispatcher.  A mirror file uses it
 * only where the historical CCPU/SAS composition supplied a native HANDLE or
 * an OEM pathname helper that cannot cross the modern guest ABI unchanged.
 */

#include <stdint.h>
#include <windows.h>

typedef struct runtime_dem_direct_context {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t reserved0;
    void *state;
    int (*publish_handle)(void *state, HANDLE handle, uint32_t *token_out,
        DWORD *error_out);
    int (*lookup_handle)(void *state, uint32_t token, HANDLE *handle_out);
    int (*release_handle)(void *state, uint32_t token, DWORD *error_out);
    int (*query_attributes)(void *state, uint8_t drive,
        const wchar_t *relative_path, DWORD *attributes_out, DWORD *error_out);
    int (*set_attributes)(void *state, uint8_t drive,
        const wchar_t *relative_path, DWORD attributes, DWORD *error_out);
} runtime_dem_direct_context;

#define RUNTIME_DEM_DIRECT_CONTEXT_MAGIC 0x42584443u
#define RUNTIME_DEM_DIRECT_CONTEXT_VERSION 1u

int runtime_dem_direct_context_valid(
    const runtime_dem_direct_context *context);

#endif
