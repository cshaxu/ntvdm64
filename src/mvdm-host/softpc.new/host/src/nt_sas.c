#include "insignia.h"
#include "host_def.h"
/*
 * VPC-XT Revision 1.0
 *
 * Title	: Sun4 SAS initialization
 *
 * Description	: Initialize the host side of sas.
 *
 * Author	: A. Guthrie
 *
 * Notes	: None
 */

static char SccsID[]="@(#)sun4_sas.c	1.3 5/7/91 Copyright Insignia Solutions Ltd.";

#include <sys/types.h>
#include "xt.h"
#include "sas.h"
#include "debug.h"

LOCAL    UTINY *reserve_for_M = NULL;

//
// Temporary pointer to the start of M.
//

#ifdef SUN_VA
#error DIVERGENCE MVDM-HOST-DIV-527: SUN_VA branch is not selected; see mirror register.
#endif

#ifdef HOST_SAS
#error Unsupported excluded original configuration
#endif /* HOST_SAS */
#line 118

/*
	Host_sas_init: allocate intel memory space
*/

#define SIXTY_FOUR_K (1024*64) /* For scratch buffer */

//UTINY *host_sas_init(size)
//sys_addr size;
//{
//    return(NULL);
//}

#ifdef SUN_VA
#error DIVERGENCE MVDM-HOST-DIV-527: SUN_VA branch is not selected; see mirror register.
#endif /* SUN_VA */

//UTINY *host_sas_term()
//{
//    if(reserve_for_M) free(reserve_for_M);
//
//    return(reserve_for_M = NULL);
//}

