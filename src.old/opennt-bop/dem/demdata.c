/* demdata.c - All the VDM instance Data.
 *
 * Sudeepb 06-Apr-1991 Created
 */

/* DIVERGENCE(BOP-DIV-092): direct import from src/opennt/base/mvdm/dos/dem/demdata.c.
 * The historical dem.h closure is represented by the declaration
 * only opennt_dem_common_compat.h; the original state identities and types remain. */
#include "opennt-bop-overlay/dem/opennt_dem_common_compat.h"

/** VDM's DTA Address **/
ULONG  UNALIGNED *pulDTALocation;

/** VDM's Current PDB **/
PUSHORT pusCurrentPDB;

/** SFT Chain Head **/
PDOSSF pSFTHead = NULL;

//
// address of extended error information in DOS data segment
//
PDEMEXTERR pExtendedError;
