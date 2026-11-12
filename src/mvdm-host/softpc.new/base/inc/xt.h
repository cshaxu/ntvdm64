#ifndef _XT_H
#define _XT_H
/*[
	Name:		xt.h
	Derived From:	VPC-XT Revision 1.0 (xt.h)
	Author:		Henry Nash
	Created On:	
	Sccs ID:	@(#)xt.h	1.19 05/15/95
	Purpose:	General include file for VPC-XT
	Notes:		This file should be included by all source modules.
			It includes the host specific general include file.

	(c)Copyright Insignia Solutions Ltd., 1990. All rights reserved.

]*/

/*
 * Useful defines
 */
#define HALF_WORD_SIZE 		sizeof (half_word)
#define WORD_SIZE 		sizeof (word)
#define DOUBLE_WORD_SIZE 	sizeof (double_word)

/*
 * Used for specifying 8, 16 bit or 32 bit sizes.
 */

typedef enum {EIGHT_BIT, SIXTEEN_BIT, THIRTY_TWO_BIT} SIZE_SPECIFIER;


#ifndef TRUE
#line 32
#error Unsupported excluded original configuration
#endif /* ! TRUE */
#line 35

#undef SUCCESS
#undef FAILURE
#define SUCCESS 0
#define FAILURE	~SUCCESS

#ifndef	NULL
#line 42
#error Unsupported excluded original configuration
#endif
#line 44

#ifndef NULL_STRING
#define NULL_STRING	""
#endif

#ifdef SOFTWINDOWS
#line 50
#error DIVERGENCE MVDM-HOST-DIV-529: SOFTWINDOWS platform branch is not selected; see mirror register.
#else
#line 52
#define SPC_PRODUCT_NAME "SoftPC"
#endif
#line 54

/***********************************************************************\
* host_gen.h is guarenteed to be included early in every C source file.	*
* It should contain those defines which are common to all versions	*
* built for a given host, to reduce the overhead in the "m" script.	*
* Bod 15/3/89.								*
\***********************************************************************/

#include "host_gen.h"

#ifndef MAXPATHLEN
#define MAXPATHLEN    1024
#endif


/*
 * Effective address calculation stuff
 */

#ifdef CPU_30_STYLE
#line 74
#error Unsupported excluded original configuration
#else /* CPU_30_STYLE */
#line 93
/* ========================================================== */

#ifdef A2CPU
#line 96
#error DIVERGENCE MVDM-HOST-DIV-520: A2CPU platform branch is not selected; see mirror register.
#endif /* A2CPU */
#line 104

#ifdef CCPU
extern sys_addr effective_addr IPT2( word, seg, word, ofs);
#endif /* CCPU */

/* ========================================================== */
#endif /* CPU_30_STYLE */
#line 111


#ifdef CCPU
/*
 * CCPU has no descriptor cache - so this should just fail.
 */
#define Cpu_find_dcache_entry(seg, base)	((IBOOL)FALSE)
#else	/* not CCPU */
extern IBOOL Cpu_find_dcache_entry IPT2(word, seg, double_word *, base);
#endif

/*
 * Global Flags and Variables
 */

extern char **pargv;			/* Pointer to argv		*/
extern int *pargc;			/* Pointer to argc		*/
extern int verbose;			/* FALSE => only report errors  */
extern IU32 io_verbose;			/* TRUE => report io errors   	*/
extern IBOOL Running_SoftWindows;	/* Are we SoftWindows?		*/
extern CHAR *SPC_Product_Name;		/* "SoftPC" or "SoftWindows"	*/

/*
 * The Parity Lookup table
 */

#ifndef CPU_30_STYLE

#line 139
extern half_word pf_table[]; /* shouldn't this be in host_cpu.h ? */

#endif /* CPU_30_STYLE */

/*
 * External function declarations.
 */
 
#ifdef ANSI
#line 148
extern void applInit(int, char *[]);
extern void applClose(void);
extern void terminate(void);
extern void host_terminate(void);
#else
extern void applInit();
extern void applClose();
extern void terminate();
extern void host_terminate();
#endif /* ANSI */

#ifdef SPC386
extern IBOOL CsIsBig IPT1(IU16, csVal);	/* is this a 32 bit code segment? */
#endif /* SPC386 */

extern void exitswin IPT0();

#endif /* _XT_H */
