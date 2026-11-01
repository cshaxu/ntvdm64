/* demdata.c - All the VDM instance Data.
 *
 * Sudeepb 06-Apr-1991 Created
 */

#include "dem.h"

/** VDM's DTA Address **/
/* DIVERGENCE MVDM-HOST-DIV-005: numeric location of the four-byte DTA
 * address cell in NTDOS; never a retained native GetVDMAddr pointer. */
mvdm_guest_location dta_location;


/** VDM's Current PDB **/

/* DIVERGENCE MVDM-HOST-DIV-006: numeric location of the current two-byte
 * PDB segment in NTDOS, not a retained native GetVDMAddr pointer. */
mvdm_guest_location current_pdb_location;

/** SFT Chain Head **/

/* DIVERGENCE MVDM-HOST-DIV-007: never retain the native pointer returned by
 * GetVDMAddr.  The adapter reconstructs bounded SFT/JFT shadows from this
 * original numeric location for each VDD operation. */
mvdm_guest_location sft_head_location;

//
// address of extended error information in DOS data segment
//

/* DIVERGENCE MVDM-HOST-DIV-005: do not retain a native GetVDMAddr result
 * beyond the source callback.  This remains the original DS:CX location. */
mvdm_guest_location extended_error_location;
