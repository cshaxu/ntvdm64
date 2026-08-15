#ifndef BX_NTVDM_DEM_PROCESS_OWNER_V1_H
#define BX_NTVDM_DEM_PROCESS_OWNER_V1_H

#include <stdint.h>

#include "bx_ntvdm_dem_dta_service.h"
#include "bx_ntvdm_guest_read_action_v1.h"

#define BX_NTVDM_DEM_PROCESS_OWNER_V1_MAGIC 0x4258504fu
#define BX_NTVDM_DEM_PROCESS_OWNER_V1_VERSION 1u

enum bx_ntvdm_dem_process_owner_v1_status {
    BX_NTVDM_DEM_PROCESS_OWNER_V1_ABSENT = 0u,
    BX_NTVDM_DEM_PROCESS_OWNER_V1_READ_REQUIRED = 1u,
    BX_NTVDM_DEM_PROCESS_OWNER_V1_VALID = 2u,
    BX_NTVDM_DEM_PROCESS_OWNER_V1_MALFORMED = 3u
};

/* A copied DOS process identity.  It intentionally carries neither a guest
 * pointer nor a host handle: demgset.c publishes CurrentPDB as a guest word,
 * and demfile.c consumes only that segment when no explicit PDB is supplied. */
typedef struct bx_ntvdm_dem_process_owner_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t status;
    uint16_t pdb_segment;
    uint16_t reserved0;
} bx_ntvdm_dem_process_owner_v1;

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_dem_process_owner_v1_clear(
    bx_ntvdm_dem_process_owner_v1 *owner);
int bx_ntvdm_dem_process_owner_v1_valid(
    const bx_ntvdm_dem_process_owner_v1 *owner);

/* Emit only the two-byte CurrentPDB read registered by demSetDTALocation.
 * An absent registration is an explicit outcome, not a request for a fallback
 * guest inspection. */
int bx_ntvdm_dem_process_owner_v1_prepare(
    const bx_ntvdm_dem_dta_registration_v1 *registration,
    bx_ntvdm_dem_process_owner_v1 *owner,
    bx_ntvdm_guest_read_action_v1 *action);

/* Accept only the exact action emitted above and copy its little-endian PDB
 * segment.  The caller owns any eventual file/search/lifecycle policy. */
int bx_ntvdm_dem_process_owner_v1_complete(
    const bx_ntvdm_dem_dta_registration_v1 *registration,
    const bx_ntvdm_guest_read_action_v1 *action,
    const uint8_t bytes[2], uint64_t byte_count,
    bx_ntvdm_dem_process_owner_v1 *owner);

#ifdef __cplusplus
}
#endif

#endif
