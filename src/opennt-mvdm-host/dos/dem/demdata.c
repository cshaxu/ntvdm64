/* demdata.c - All the VDM instance Data.
 *
 * Sudeepb 06-Apr-1991 Created
 */

#include "dem.h"

/** VDM's DTA Address **/
ULONG  UNALIGNED *pulDTALocation; // Address in ntdos where DTA address is kept.


/** VDM's Current PDB **/

PUSHORT pusCurrentPDB;

/** SFT Chain Head **/

PDOSSF pSFTHead = NULL;

//
// address of extended error information in DOS data segment
//

/* DIVERGENCE MVDM-HOST-DIV-005: do not retain a native GetVDMAddr result
 * beyond the source callback.  This remains the original DS:CX location. */
mvdm_guest_location extended_error_location;
