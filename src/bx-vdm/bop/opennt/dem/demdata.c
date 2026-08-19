/* OpenNT source mirror: base/mvdm/dos/dem/demdata.c. */

#include "dem_shared.h"

/* Divergence: CCPU/SAS guest pointers are not host-dereferenceable in bx-vdm.
 * These preserve the original DEM state identities as opaque typed values. */
uintptr_t *pulDTALocation;
uint16_t *pusCurrentPDB;
void *pSFTHead;
void *pExtendedError;
