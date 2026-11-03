#include "insignia.h"
#include "host_def.h"
/*
 * VPC-XT Revision 1.0
 *
 * Title	: memory_size
 *
 * Description	: Returns the memory size of the virtual PC memory
 *
 * Author	: Henry Nash
 *
 * Notes	: None
 *
 */

#ifdef SCCSID
#line 17
#error DIVERGENCE MVDM-HOST-DIV-518: SCCSID identification strings are not selected; see mirror register.
#endif
#line 19

#ifdef SEGMENTATION
#line 21
#error Excluded unsupported SEGMENTATION configuration
#endif
#line 28


/*
 *    O/S include files.
 */
#include <stdio.h>
#include TypesH

/*
 * SoftPC include files
 */
#include "xt.h"
#include CpuH
#include "bios.h"
#include "sas.h"

void memory_size()
{
    word memory_size;
    
    /*
     * Return the memory size in AX.  This is read in from the BIOS, as
     * certain applications can write to this area.
     */
    sas_loadw(MEMORY_VAR, &memory_size);
    
    setAX(memory_size);
}
