#ifndef BX_NTVDM_BOP_SHIM_DEM_DRIVE_POLICY_SHIM_H
#define BX_NTVDM_BOP_SHIM_DEM_DRIVE_POLICY_SHIM_H

#include <stdint.h>

#include "bx_ntvdm_host_drive_policy.h"

/* Shared, session-private admission seam for imported DEM and COMMAND drive
 * discovery.  It is intentionally narrower than either owner's register or
 * memory shim, so those source domains never have to include one another. */
int bx_ntvdm_demdasd_drive_policy_bind(
    const bx_ntvdm_host_drive_snapshot_v1 *snapshot);
void bx_ntvdm_demdasd_drive_policy_reset(void);
int bx_ntvdm_demdasd_drive_policy_bound(void);
int bx_ntvdm_demdasd_drive_policy_admits(uint8_t drive);

#endif
