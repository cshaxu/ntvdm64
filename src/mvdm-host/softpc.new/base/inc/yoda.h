
/*
 * VPC-XT Revision 0.1
 *
 * Title	: yoda.h
 *
 * Description	: The force is with you include file
 *		  (ps yoda debugging file)
 *
 * Author	: Henry Nash
 *		  Phil Bousfield
 *
 * Notes	: This file contains the debugger call definitions
 */

/* SccsID[]="@(#)yoda.h	1.6 06/30/95 Copyright Insignia Solutions Ltd."; */

/*
 * ============================================================================
 * Structure/Data definitions
 * ============================================================================
 */

/* None */

/*
 * ============================================================================
 * External declarations and macros
 * ============================================================================
 */


/*  
 * If the PROD flag is set then turn the debugger calls into null macros
 * otherwise they are external functions - see yoda.c
 */

#ifdef PROD
#define check_I() 		/*	*/
#define check_D(address, value) /*address, value*/
#define force_yoda() 		/*	*/
#else
#error DIVERGENCE MVDM-HOST-DIV-617: unsupported alternate host profile
#endif
#line 47

/*
 * Interface definitions and enums - non-prod only.
 */

#ifndef PROD
#error DIVERGENCE MVDM-HOST-DIV-504: non-PROD header branch is not selected; see mirror register.



#else /* !PROD */
#line 55

#endif /* !PROD else*/
#line 57
