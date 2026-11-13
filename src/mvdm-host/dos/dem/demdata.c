/* demdata.c - All the VDM instance Data.
 *
 * Sudeepb 06-Apr-1991 Created
 */

#include "dem.h"

/** VDM's DTA Address **/
#line 9
ULONG  UNALIGNED *pulDTALocation; // Address in ntdos where DTA address is kept.
#line 10


/** VDM's Current PDB **/

PUSHORT pusCurrentPDB;
#line 15

/** SFT Chain Head **/

PDOSSF pSFTHead = NULL;
#line 19

//
// address of extended error information in DOS data segment
//

#line 24
PDEMEXTERR pExtendedError;
