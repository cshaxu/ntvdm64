#include <stdio.h>

#include "bx_ntvdm_dem_process_owner_v1.h"

int main(void)
{
    bx_ntvdm_dem_dta_registration_v1 registration = { 0u, 0x200u, 0u, 0u };
    bx_ntvdm_dem_process_owner_v1 owner;
    bx_ntvdm_guest_read_action_v1 action;
    const uint8_t owner_bytes[2] = { 0x34u, 0x12u };

    if (!bx_ntvdm_dem_process_owner_v1_prepare(&registration, &owner, &action) ||
        owner.status != BX_NTVDM_DEM_PROCESS_OWNER_V1_READ_REQUIRED ||
        action.disposition != BX_NTVDM_GUEST_READ_ACTION_V1_NEED_READ ||
        action.guest_read.address != 0x200u || action.guest_read.length != 2u ||
        !bx_ntvdm_dem_process_owner_v1_complete(&registration, &action,
            owner_bytes, sizeof(owner_bytes), &owner) ||
        owner.status != BX_NTVDM_DEM_PROCESS_OWNER_V1_VALID ||
        owner.pdb_segment != 0x1234u) return 1;

    registration.current_pdb = 0u;
    if (!bx_ntvdm_dem_process_owner_v1_prepare(&registration, &owner, &action) ||
        owner.status != BX_NTVDM_DEM_PROCESS_OWNER_V1_ABSENT ||
        action.disposition != BX_NTVDM_GUEST_READ_ACTION_V1_PASS_THROUGH) return 2;

    registration.current_pdb = 0xfffffu;
    if (!bx_ntvdm_dem_process_owner_v1_prepare(&registration, &owner, &action) ||
        owner.status != BX_NTVDM_DEM_PROCESS_OWNER_V1_MALFORMED ||
        action.disposition != BX_NTVDM_GUEST_READ_ACTION_V1_PASS_THROUGH) return 3;

    registration.current_pdb = 0x200u;
    if (!bx_ntvdm_dem_process_owner_v1_prepare(&registration, &owner, &action)) return 4;
    action.guest_read.address = 0x202u;
    if (!bx_ntvdm_dem_process_owner_v1_complete(&registration, &action,
            owner_bytes, sizeof(owner_bytes), &owner) ||
        owner.status != BX_NTVDM_DEM_PROCESS_OWNER_V1_MALFORMED) return 5;

    if (!bx_ntvdm_dem_process_owner_v1_prepare(&registration, &owner, &action) ||
        !bx_ntvdm_dem_process_owner_v1_complete(&registration, &action,
            owner_bytes, 1u, &owner) ||
        owner.status != BX_NTVDM_DEM_PROCESS_OWNER_V1_MALFORMED) return 6;

    if (!bx_ntvdm_dem_process_owner_v1_prepare(&registration, &owner, &action) ||
        !bx_ntvdm_dem_process_owner_v1_complete(&registration, &action,
            (const uint8_t[]){ 0u, 0u }, 2u, &owner) ||
        owner.status != BX_NTVDM_DEM_PROCESS_OWNER_V1_MALFORMED) return 7;

    puts("bx-ntvdm DEM CurrentPDB owner helper: bounded copied identity verified");
    return 0;
}
