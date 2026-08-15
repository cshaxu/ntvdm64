#include "bx_ntvdm_dem_process_owner_v1.h"

#define BX_NTVDM_DEM_PROCESS_OWNER_V1_REAL_APERTURE UINT64_C(0x100000)
#define BX_NTVDM_DEM_PROCESS_OWNER_V1_CURRENT_PDB_BYTES UINT64_C(2)

static int current_pdb_range(const bx_ntvdm_dem_dta_registration_v1 *registration)
{
    return registration != 0 && registration->current_pdb != 0u &&
        registration->current_pdb <= BX_NTVDM_DEM_PROCESS_OWNER_V1_REAL_APERTURE -
            BX_NTVDM_DEM_PROCESS_OWNER_V1_CURRENT_PDB_BYTES;
}

void bx_ntvdm_dem_process_owner_v1_clear(
    bx_ntvdm_dem_process_owner_v1 *owner)
{
    if (owner == 0) return;
    owner->magic = BX_NTVDM_DEM_PROCESS_OWNER_V1_MAGIC;
    owner->abi_version = BX_NTVDM_DEM_PROCESS_OWNER_V1_VERSION;
    owner->struct_bytes = (uint32_t)sizeof(*owner);
    owner->status = BX_NTVDM_DEM_PROCESS_OWNER_V1_ABSENT;
    owner->pdb_segment = 0u;
    owner->reserved0 = 0u;
}

int bx_ntvdm_dem_process_owner_v1_valid(
    const bx_ntvdm_dem_process_owner_v1 *owner)
{
    return owner != 0 && owner->magic == BX_NTVDM_DEM_PROCESS_OWNER_V1_MAGIC &&
        owner->abi_version == BX_NTVDM_DEM_PROCESS_OWNER_V1_VERSION &&
        owner->struct_bytes == sizeof(*owner) &&
        owner->status <= BX_NTVDM_DEM_PROCESS_OWNER_V1_MALFORMED &&
        owner->reserved0 == 0u &&
        ((owner->status == BX_NTVDM_DEM_PROCESS_OWNER_V1_VALID &&
          owner->pdb_segment != 0u) ||
         (owner->status != BX_NTVDM_DEM_PROCESS_OWNER_V1_VALID &&
          owner->pdb_segment == 0u));
}

int bx_ntvdm_dem_process_owner_v1_prepare(
    const bx_ntvdm_dem_dta_registration_v1 *registration,
    bx_ntvdm_dem_process_owner_v1 *owner,
    bx_ntvdm_guest_read_action_v1 *action)
{
    if (owner == 0 || action == 0) return 0;
    bx_ntvdm_dem_process_owner_v1_clear(owner);
    bx_ntvdm_guest_read_action_v1_pass_through(action);
    if (registration == 0 || registration->current_pdb == 0u)
        return bx_ntvdm_dem_process_owner_v1_valid(owner) &&
            bx_ntvdm_guest_read_action_v1_valid(action);
    if (!current_pdb_range(registration)) {
        owner->status = BX_NTVDM_DEM_PROCESS_OWNER_V1_MALFORMED;
        return bx_ntvdm_dem_process_owner_v1_valid(owner) &&
            bx_ntvdm_guest_read_action_v1_valid(action);
    }
    owner->status = BX_NTVDM_DEM_PROCESS_OWNER_V1_READ_REQUIRED;
    return bx_ntvdm_guest_read_action_v1_need_read(action,
        registration->current_pdb,
        BX_NTVDM_DEM_PROCESS_OWNER_V1_CURRENT_PDB_BYTES) &&
        bx_ntvdm_dem_process_owner_v1_valid(owner);
}

int bx_ntvdm_dem_process_owner_v1_complete(
    const bx_ntvdm_dem_dta_registration_v1 *registration,
    const bx_ntvdm_guest_read_action_v1 *action,
    const uint8_t bytes[2], uint64_t byte_count,
    bx_ntvdm_dem_process_owner_v1 *owner)
{
    uint16_t pdb_segment;
    if (owner == 0) return 0;
    bx_ntvdm_dem_process_owner_v1_clear(owner);
    if (!current_pdb_range(registration) || action == 0 || bytes == 0 ||
        !bx_ntvdm_guest_read_action_v1_valid(action) ||
        action->disposition != BX_NTVDM_GUEST_READ_ACTION_V1_NEED_READ ||
        action->guest_read.address != registration->current_pdb ||
        action->guest_read.length != BX_NTVDM_DEM_PROCESS_OWNER_V1_CURRENT_PDB_BYTES ||
        byte_count != BX_NTVDM_DEM_PROCESS_OWNER_V1_CURRENT_PDB_BYTES) {
        owner->status = BX_NTVDM_DEM_PROCESS_OWNER_V1_MALFORMED;
        return bx_ntvdm_dem_process_owner_v1_valid(owner);
    }
    pdb_segment = (uint16_t)((uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8));
    if (pdb_segment == 0u) {
        owner->status = BX_NTVDM_DEM_PROCESS_OWNER_V1_MALFORMED;
        return bx_ntvdm_dem_process_owner_v1_valid(owner);
    }
    owner->status = BX_NTVDM_DEM_PROCESS_OWNER_V1_VALID;
    owner->pdb_segment = pdb_segment;
    return bx_ntvdm_dem_process_owner_v1_valid(owner);
}
