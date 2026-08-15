#ifndef BX_NTVDM_DEM_FILE_SESSION_V1_H
#define BX_NTVDM_DEM_FILE_SESSION_V1_H

#include <stdint.h>
#include <windows.h>

#include "bx_ntvdm_dem_profile_consumer_v1.h"

#define BX_NTVDM_DEM_FILE_SESSION_V1_MAGIC 0x42584653u
#define BX_NTVDM_DEM_FILE_SESSION_V1_VERSION 1u
#define BX_NTVDM_DEM_FILE_SESSION_V1_MAX_TOKENS 64u

/* The token is a copied guest-visible value, not a Win32 handle.  The low
 * 16 bits name one bounded slot and the high bits reject stale reuse. */
typedef struct bx_ntvdm_dem_file_token_slot_v1 {
    HANDLE handle;
    uint32_t generation;
    uint32_t in_use;
} bx_ntvdm_dem_file_token_slot_v1;

typedef struct bx_ntvdm_dem_file_session_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t reserved0;
    bx_ntvdm_dem_profile_consumer_v1 profile;
    bx_ntvdm_dem_file_token_slot_v1
        slots[BX_NTVDM_DEM_FILE_SESSION_V1_MAX_TOKENS];
} bx_ntvdm_dem_file_session_v1;

#ifdef __cplusplus
extern "C" {
#endif

int bx_ntvdm_dem_file_session_v1_initialize(
    bx_ntvdm_dem_file_session_v1 *session,
    const bx_ntvdm_mutation_profile_v1 *profile);
int bx_ntvdm_dem_file_session_v1_valid(
    const bx_ntvdm_dem_file_session_v1 *session);

/* Takes ownership of an adapter-internal live handle and returns an opaque
 * token.  This interface is deliberately not a BOP/service interface. */
int bx_ntvdm_dem_file_session_v1_adopt(
    bx_ntvdm_dem_file_session_v1 *session, HANDLE handle,
    uint32_t *token_out);
int bx_ntvdm_dem_file_session_v1_lookup(
    const bx_ntvdm_dem_file_session_v1 *session, uint32_t token,
    HANDLE *handle_out);
int bx_ntvdm_dem_file_session_v1_release(
    bx_ntvdm_dem_file_session_v1 *session, uint32_t token);
void bx_ntvdm_dem_file_session_v1_teardown(
    bx_ntvdm_dem_file_session_v1 *session);

#ifdef __cplusplus
}
#endif

#endif
