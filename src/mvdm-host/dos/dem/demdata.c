/* demdata.c - All the VDM instance Data.
 *
 * Sudeepb 06-Apr-1991 Created
 */

#include "dem.h"

/** VDM's DTA Address **/
/* DIVERGENCE MVDM-HOST-DIV-005: numeric location of the four-byte DTA
 * address cell in NTDOS; never a retained native GetVDMAddr pointer. */
mvdm_guest_location dta_location;
#line 10


/** VDM's Current PDB **/

/* DIVERGENCE MVDM-HOST-DIV-006: numeric location of the current two-byte
 * PDB segment in NTDOS, not a retained native GetVDMAddr pointer. */
mvdm_guest_location current_pdb_location;
#line 15

/** SFT Chain Head **/


mvdm_guest_location sft_head_location;
#line 19

//
// address of extended error information in DOS data segment
//

#line 24
PDEMEXTERR pExtendedError;
