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
/* DIVERGENCE MVDM-HOST-DIV-527: SUN_VA branch is not selected; see mirror register. */
#endif

#ifdef HOST_SAS

#undef sas_load
#undef sas_loadw
#undef sas_store
#undef sas_storew
#undef sas_fills
#undef sas_fillsw
#undef sas_hw_at
#undef sas_w_at
#undef sas_dw_at
#undef sas_loads
#undef sas_stores
#undef sas_move_bytes_forward
#undef sas_move_words_forward
#undef sas_move_bytes_backward
#undef sas_move_words_backward
#undef get_byte_addr
#undef inc_M_ptr
#undef M_get_dw_ptr

IMPORT	VOID	sas_load();
IMPORT	VOID	sas_store();
#ifdef SUN_VA
/* DIVERGENCE MVDM-HOST-DIV-527: SUN_VA branch is not selected; see mirror register. */
#else
IMPORT	VOID	sas_loadw();
IMPORT	VOID	sas_storew();
#endif
IMPORT	VOID	sas_fills();
IMPORT	VOID	sas_fillsw();
IMPORT	half_word	sas_hw_at();
IMPORT	word	sas_w_at();
IMPORT	double_word	sas_dw_at();
IMPORT	VOID	sas_loads();
IMPORT	VOID	sas_stores();
IMPORT	VOID	sas_move_bytes_forward();
IMPORT	VOID	sas_move_words_forward();
IMPORT	VOID	sas_move_bytes_backward();
IMPORT	VOID	sas_move_words_backward();
IMPORT	host_addr Start_of_M_area;

LOCAL	host_addr	forward_get_addr(addr)
host_addr	addr;
{
	return( (host_addr)((long)Start_of_M_area + (long)addr));
}

LOCAL	host_addr	forward_inc_M_ptr(p, off)
host_addr	p;
host_addr	off;
{
	return( (host_addr)((long)p + (long)off) );
}

GLOBAL    SAS_FUNCTIONS host_sas_funcs =
{
	sas_load,
#ifdef SUN_VA
/* DIVERGENCE MVDM-HOST-DIV-527: SUN_VA branch is not selected; see mirror register. */
#else
	sas_loadw,
#endif
	sas_store,
#ifdef SUN_VA
/* DIVERGENCE MVDM-HOST-DIV-527: SUN_VA branch is not selected; see mirror register. */
#else
	sas_storew,
#endif
	sas_fills,
	sas_fillsw,
	sas_hw_at,
	sas_w_at,
	sas_dw_at,
	sas_loads,
	sas_stores,
	sas_move_bytes_forward,
	sas_move_words_forward,
	sas_move_bytes_backward,
	sas_move_words_backward,
	forward_get_addr,
	forward_inc_M_ptr,
	forward_get_addr,
};

#endif /* HOST_SAS */

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
/* DIVERGENCE MVDM-HOST-DIV-527: SUN_VA branch is not selected; see mirror register. */
#endif /* SUN_VA */

//UTINY *host_sas_term()
//{
//    if(reserve_for_M) free(reserve_for_M);
//
//    return(reserve_for_M = NULL);
//}

