#include "insignia.h"
#include "host_def.h"
/*
 * SoftPC Revision 3.0
 *
 * Title	: equipment.c
 *
 * Description	: BIOS equipment function.  Returns a word containing
 *		  a bit pattern representing the equipment supported
 *		  by the virtual bios.
 *
 * Author	: Henry Nash / David Rees
 *
 * Notes	: Now reads the word from the appropriate place within
 *                the BIOS data area (40:10H).
 */

/*
 * static char SccsID[]="@(#)equipment.c	1.6 08/03/93 Copyright Insignia Solutions Ltd.";
 */


#ifdef SEGMENTATION
#error Excluded unsupported SEGMENTATION configuration
#endif
#line 31


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
#include "equip.h"

void equipment()
{
    EQUIPMENT_WORD equip_flag;

    sas_loadw(EQUIP_FLAG, &equip_flag.all);
    setAX(equip_flag.all);
}
