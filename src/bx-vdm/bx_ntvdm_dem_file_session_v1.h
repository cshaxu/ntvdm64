#ifndef BX_NTVDM_DEM_FILE_SESSION_V1_H
#define BX_NTVDM_DEM_FILE_SESSION_V1_H

#include <stdint.h>
#include <windows.h>

#include "bx_ntvdm_dem_profile_consumer_v1.h"

#define BX_NTVDM_DEM_FILE_SESSION_V1_MAGIC 0x42584653u
#define BX_NTVDM_DEM_FILE_SESSION_V1_VERSION 1u
#define BX_NTVDM_DEM_FILE_SESSION_V1_MAX_TOKENS 64u

/* The guest-visible token is never a native handle.  Its backend kind is
 * checked before any backend-specific operation; later readonly/overlay/
 * virtual entries must not be accepted by the Direct Win32-handle paths. */
enum bx_ntvdm_dem_file_token_kind_v1 {
    BX_NTVDM_DEM_FILE_TOKEN_KIND_V1_NONE = 0u,
    BX_NTVDM_DEM_FILE_TOKEN_KIND_V1_DIRECT_WIN32_HANDLE = 1u,
    BX_NTVDM_DEM_FILE_TOKEN_KIND_V1_READONLY_NAMESPACE = 2u
};

/* The token is a copied guest-visible value, not a Win32 handle.  The low
 * 16 bits name one bounded slot and the high bits reject stale reuse. */
typedef struct bx_ntvdm_dem_file_token_slot_v1 {
    HANDLE handle;
    uint32_t generation;
    uint32_t in_use;
    uint16_t pdb_owner;
    uint16_t kind;
    uint16_t reserved0;
    uint32_t backend_token;
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
int bx_ntvdm_dem_file_session_v1_adopt_owned(
    bx_ntvdm_dem_file_session_v1 *session, HANDLE handle,
    uint16_t pdb_owner, uint32_t *token_out);
int bx_ntvdm_dem_file_session_v1_adopt_backend(
    bx_ntvdm_dem_file_session_v1 *session, uint32_t kind,
    uint32_t backend_token, uint16_t pdb_owner, uint32_t *token_out);
int bx_ntvdm_dem_file_session_v1_lookup(
    const bx_ntvdm_dem_file_session_v1 *session, uint32_t token,
    HANDLE *handle_out);
int bx_ntvdm_dem_file_session_v1_token_kind(
    const bx_ntvdm_dem_file_session_v1 *session, uint32_t token,
    uint32_t *kind_out);
int bx_ntvdm_dem_file_session_v1_lookup_backend(
    const bx_ntvdm_dem_file_session_v1 *session, uint32_t token,
    uint32_t expected_kind, uint32_t *backend_token_out);
int bx_ntvdm_dem_file_session_v1_release(
    bx_ntvdm_dem_file_session_v1 *session, uint32_t token);
int bx_ntvdm_dem_file_session_v1_release_backend(
    bx_ntvdm_dem_file_session_v1 *session, uint32_t token,
    uint32_t expected_kind);
int bx_ntvdm_dem_file_session_v1_release_owner(
    bx_ntvdm_dem_file_session_v1 *session, uint16_t pdb_owner,
    uint32_t *released_out);
void bx_ntvdm_dem_file_session_v1_teardown(
    bx_ntvdm_dem_file_session_v1 *session);

#ifdef __cplusplus
}
#endif

#endif
