#include "insignia.h"
#include "host_def.h"

#if !(defined(NTVDM) && defined(MONITOR))

/*			INSIGNIA (SUB)MODULE SPECIFICATION
			-----------------------------


	THIS PROGRAM SOURCE FILE  IS  SUPPLIED IN CONFIDENCE TO THE
	CUSTOMER, THE CONTENTS  OR  DETAILS  OF  ITS OPERATION MUST
	NOT BE DISCLOSED TO ANY  OTHER PARTIES  WITHOUT THE EXPRESS
	AUTHORISATION FROM THE DIRECTORS OF INSIGNIA SOLUTIONS LTD.

DOCUMENT 		: name and number

RELATED DOCS		: include all relevant references

DESIGNER		: 

REVISION HISTORY	:
First version		: August 1988, J. Maiden
Second version		: February 1991, John Shanly, SoftPC 3.0

SUBMODULE NAME		: ega		

SOURCE FILE NAME	: ega_read.c

PURPOSE			: emulation of EGA read operations 

SccsID = @(#)ega_read.c	1.32 09/07/94 Copyright Insignia Solutions Ltd.
		

[1.INTERMODULE INTERFACE SPECIFICATION]

[1.0 INCLUDE FILE NEEDED TO ACCESS THIS INTERFACE FROM OTHER SUBMODULES]

	INCLUDE FILE : ega_read.gi

[1.1    INTERMODULE EXPORTS]

	PROCEDURES() :
			void ega_read_init()
			void ega_read_term()
			void ega_read_routines_update()
	DATA 	     :	
-------------------------------------------------------------------------
[1.2 DATATYPES FOR [1.1] (if not basic C types)]
-------------------------------------------------------------------------
[1.3 INTERMODULE IMPORTS]
	 (not o/s objects or standard libs)

	PROCEDURES() : 

	DATA 	     : 	give name, and source module name

-------------------------------------------------------------------------

[1.4 DESCRIPTION OF INTERMODULE INTERFACE]

[1.4.1 IMPORTED OBJECTS]

[1.4.2 EXPORTED OBJECTS]
=========================================================================
PROCEDURE	 	 : 	ega_read_init

PURPOSE		  : 	initialize EGA read aspects.
		
PARAMETERS	  :	none

GLOBALS		  :	none

DESCRIPTION	  : 	initialize ega read data and code to sensible state.

ERROR INDICATIONS :	none.

ERROR RECOVERY	  :	none.
=========================================================================
PROCEDURE	 	 : 	ega_read_term

PURPOSE		  : 	terminate EGA read aspects.
		
PARAMETERS	  :	none

GLOBALS		  :	none

DESCRIPTION	  : 	set things up so that read processing is effectively turned off.

ERROR INDICATIONS :	none.

ERROR RECOVERY	  :	none.
=========================================================================

PROCEDURE	 	 : 	ega_read_routines_update

PURPOSE		  : 	Update read state to match registers.
		
PARAMETERS	  :	none

GLOBALS		  :	none

DESCRIPTION	  : 	Examines RAM enabled/disabled bit, read mode, chained/unchained
				and either mapped plane or colour compare and colour don't
				care states.  Sets global variables that allow byte_read,
				word_read and string_read to yield the data that would be
				read from M.

ERROR INDICATIONS :	none.

ERROR RECOVERY	  :	none.

=========================================================================

/*=======================================================================
[3.INTERMODULE INTERFACE DECLARATIONS]
=========================================================================

[3.1 INTERMODULE IMPORTS]						*/

/* [3.1.1 #INCLUDES]                                                    */


#ifdef EGG

#include	"xt.h"
#include	CpuH
#include	"debug.h"
#include	"sas.h"
#include	"gmi.h"
#include	"gvi.h"
#include	"ios.h"
#include	"egacpu.h"
#include	"egaports.h"
#include	"egaread.h"
#include	"ga_mark.h"
#include	"ga_defs.h"
#include	"cpu_vid.h"
/* DIVERGENCE(MVDM-HOST-DIV-067): the selected C_VID function has an
 * original `void (IUH)` shape; do not use an implicit C declaration. */
IMPORT void setReadPointers IPT1(IUH, readset);

/* [3.1.2 DECLARATIONS]                                                 */

/* [3.2 INTERMODULE EXPORTS]						*/ 

/*
5.0 MODULE INTERNALS   :   (not visible externally, global internally)]     

[5.1 LOCAL DECLARATIONS]						*/

/* [5.1.1 #DEFINES]							*/

#ifdef SEGMENTATION
/*
 * The following #include specifies the code segment into which this
 * module will by placed by the MPW C compiler on the Mac II running
 * MultiFinder.
 */
#include "SOFTPC_EGA.seg"
#endif

#if defined(EGA_DUMP) || defined(EGA_STAT)
#define	change_read_pointers(ptr)	dump_change_read_pointers(&ptr)
#else
#ifdef EGATEST
#error DIVERGENCE MVDM-HOST-DIV-529: EGATEST platform branch is not selected; see mirror register.
#else
#define	change_read_pointers(ptr)	read_glue_ptrs = ptr
#endif /* EGATEST */
#endif

/* [5.1.2 TYPEDEF, STRUCTURE, ENUM DECLARATIONS]			*/


/* [5.1.3 PROCEDURE() DECLARATIONS]					*/


/* -----------------------------------------------------------------------
[5.2 LOCAL DEFINITIONS]

	[5.2.1 INTERNAL DATA DEFINITIONS 					*/

extern IU32 glue_b_read();
extern IU32 glue_w_read();
extern void glue_str_read();

#ifndef REAL_VGA
READ_POINTERS read_glue_ptrs;

/* If we have no string operations, the host allocates storage for "read_pointers" */

#ifndef	NO_STRING_OPERATIONS
GLOBAL READ_POINTERS read_pointers;
#endif	/* NO_STRING_OPERATIONS */


#ifndef CPU_40_STYLE	/* i.e. EVID without introducing an EVID define */
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#ifndef EGATEST
#error Excluded original conditional branch
#ifndef A3CPU
#error Excluded original conditional branch
#ifndef JOKER
#error Excluded original conditional branch
#ifndef	NO_STRING_OPERATIONS
#error Excluded original conditional branch
#endif
#endif
#endif
#endif
#ifndef	NO_STRING_OPERATIONS
#error Excluded original conditional branch
#endif
#ifndef	NO_STRING_OPERATIONS
#error Excluded original conditional branch
#endif
#ifndef	NO_STRING_OPERATIONS
#error Excluded original conditional branch
#endif
#ifdef VGG
#error Excluded original conditional branch
#ifndef	NO_STRING_OPERATIONS
#error Excluded original conditional branch
#endif
#endif
#ifndef	NO_STRING_OPERATIONS
#error Excluded original conditional branch
#endif
#ifdef VGG
#error Excluded original conditional branch
#ifndef	NO_STRING_OPERATIONS
#error Excluded original conditional branch
#endif
#endif
#ifdef A_VID
#error Excluded original conditional branch
#ifndef	NO_STRING_OPERATIONS
#error Excluded original conditional branch
#endif
#ifndef	NO_STRING_OPERATIONS
#error Excluded original conditional branch
#endif
#else
#error Excluded original conditional branch
#ifndef	NO_STRING_OPERATIONS
#error Excluded original conditional branch
#endif
#ifndef	NO_STRING_OPERATIONS
#error Excluded original conditional branch
#endif
#endif
#ifdef A3CPU
#error Excluded original conditional branch
#ifdef C_VID
#error Excluded original conditional branch
#endif
#else
#error Excluded original conditional branch
#ifdef C_VID
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#endif
#ifndef GISP_CPU
#error Excluded original conditional branch
#if (defined(A_VID) && defined(A2CPU) && !defined(A3CPU)) || (defined(A3CPU) && defined(C_VID))
#error Excluded original conditional branch
#ifndef	NO_STRING_OPERATIONS
#error Excluded original conditional branch
#endif
#endif
#endif
#else	/* CPU_40_STYLE - evid */
#line 200

#ifdef C_VID
/* C_Evid glue */
/* DIVERGENCE MVDM-HOST-DIV-089: retain the concrete original C-video read
 * glue contracts instead of implicit host-int declarations. */
extern IU32 read_byte_ev_glue IPT1(IU32, eaOff);
extern IU32 read_word_ev_glue IPT1(IU32, eaOff);
extern void read_str_fwd_ev_glue IPT3(IU8 *, dest, IU32, eaOff, IU32, count);
READ_POINTERS Glue_reads = 
{
	read_byte_ev_glue,
	read_word_ev_glue,
	read_str_fwd_ev_glue
};
#else
#error DIVERGENCE MVDM-HOST-DIV-539: non-C_VID branch is not selected; see mirror register.
#endif /* C_VID */
READ_POINTERS	simple_reads;
READ_POINTERS	pointers_mode0_nch;
READ_POINTERS	pointers_mode1_nch;
READ_POINTERS	pointers_mode0_ch4;
READ_POINTERS	pointers_mode1_ch4;
READ_POINTERS	pointers_mode0_ch2;
READ_POINTERS	pointers_mode1_ch2;
GLOBAL READ_POINTERS C_vid_reads;
GLOBAL READ_POINTERS A_vid_reads;

ULONG EasVal;
IU32 latchval;	/* for get_latch() etc macros */
#endif	/* CPU_40_STYLE - evid */
#line 230

READ_STATE read_state;

#ifndef	NO_STRING_OPERATIONS
GLOBAL void (*string_read_ptr)();
#endif	/* NO_STRING_OPERATIONS */
#line 236

/* comparison masks for read mode 1 */
ULONG comp0, comp1, comp2, comp3;

/* colour comparison don't care masks for read mode 1 */
ULONG dont_care0, dont_care1, dont_care2, dont_care3;

/* [5.2.2 INTERNAL PROCEDURE DEFINITIONS]				*/

/* Used to correct writes to M when in mode 0 */

GLOBAL IU32
rdm0_byte_ch2 IFN1(ULONG, offset )
{
	IU32 lsb;
	UTINY temp;

	lsb = offset & 0x1;
	offset = ( offset >> 1 ) << 2;
	setVideolatches(*(IU32 *)( EGA_planes + offset ));

	offset |= lsb;

  	temp = EGA_CPU.read_mapped_plane_ch2[offset];

#ifdef C_VID
	EasVal = temp;
#endif
#line 264
	return( temp );
}

/* Used to correct writes to M when in mode 0 */

GLOBAL IU32
rdm0_word_ch2 IFN1(ULONG, offset )
{
	IU32 temp;
	IU32 lsb;

	setVideolatches(*(IU32 *)( EGA_planes +
		((( offset + 1 ) >> 1 ) << 2 )));

	lsb = offset & 0x1;
	offset = ( offset >> 1 ) << 2;

	if( lsb )
	{
		temp = EGA_CPU.read_mapped_plane_ch2[offset + 1];
		temp |= ( EGA_CPU.read_mapped_plane_ch2[offset + 4] << 8 );
	}
	else
	{
		temp = EGA_CPU.read_mapped_plane_ch2[offset];
		temp |= ( EGA_CPU.read_mapped_plane_ch2[offset + 1] << 8 );
	}

#ifdef C_VID
	EasVal = temp;
#endif
#line 295
	return( temp );
}

/* Used to correct writes to M when in mode 0 */

GLOBAL void
rdm0_string_ch2 IFN3(UTINY *, dest, ULONG, offset, ULONG, count )
{
	ULONG lsb;
	ULONG inc;
	UTINY *planes;

	if( getDF() )
		setVideolatches(*(IU32 *)( EGA_planes + (( offset >> 1 ) << 2 )));
	else
		setVideolatches(*(IU32 *)( EGA_planes + ((( offset + count - 1 ) >> 1 ) << 2 )));

	lsb = offset & 0x1;
	offset = ( offset >> 1 ) << 2;

	if( lsb )
	{
		offset += 1;
		inc = 3;
	}
	else
		inc = 1;

	planes = EGA_CPU.read_mapped_plane_ch2;

    while( count-- )
    {
#ifdef BACK_M
#error DIVERGENCE MVDM-HOST-DIV-525: BACK_M platform branch is not selected; see mirror register.
#else
#line 330
        *dest++ = *(planes + offset);
#endif
#line 332
		offset += inc;
		inc ^= 0x2;
    }
}

/* Used to correct writes to M when in mode 1 */

GLOBAL IU32
rdm1_byte_ch2 IFN1(ULONG, offset )
{
	IU32 temp, lsb;

	lsb = offset & 0x1;
	offset = ( offset >> 1 ) << 2;
	setVideolatches(*(IU32 *)( EGA_planes + offset ));

	if( lsb )
	{
		offset += 1;

		temp = (IU32)((( EGA_plane01[offset] ^ comp1 ) | dont_care1 )
					& (( EGA_plane23[offset] ^ comp3 ) | dont_care3 ));
	}
	else
	{
		temp = (IU32)((( EGA_plane01[offset] ^ comp0 ) | dont_care0 )
					& (( EGA_plane23[offset] ^ comp2 ) | dont_care2 ));
	}

#ifdef C_VID
	EasVal = temp;
#endif
#line 364
	return( temp );
}

GLOBAL IU32
rdm1_word_ch2 IFN1(ULONG, offset )		/* used to correct writes to M when in mode 1 */
{
	IU32 temp1, temp2, lsb;

	setVideolatches(*(IU32 *)( EGA_planes + ((( offset + 1 ) >> 1 ) << 2 )));

	lsb = offset & 0x1;
	offset = ( offset >> 1 ) << 2;

	if( lsb )
	{
		offset += 1;
		temp1 = (( EGA_plane01[offset] ^ comp1 ) | dont_care1 )
					& (( EGA_plane23[offset] ^ comp3 ) | dont_care3 );

		offset += 3;
		temp2 = (( EGA_plane01[offset] ^ comp0 ) | dont_care0 )
					& (( EGA_plane23[offset] ^ comp2 ) | dont_care2 );
	}
	else
	{
		temp1 = (( EGA_plane01[offset] ^ comp0 ) | dont_care0 )
					& (( EGA_plane23[offset] ^ comp2 ) | dont_care2 );

		offset += 1;
		temp2 = (( EGA_plane01[offset] ^ comp1 ) | dont_care1 )
					& (( EGA_plane23[offset] ^ comp3 ) | dont_care3 );
	}

	temp1 |= temp2 << 8;

#ifdef C_VID
	EasVal = temp1;
#endif
#line 402
	return( temp1 );
}

GLOBAL void
rdm1_string_ch2 IFN3(UTINY *, dest, ULONG, offset, ULONG, count )	/* used to correct writes to M when in mode 1 */
{
	UTINY *p01, *p23;
	ULONG tcount, lsb;

#ifdef BACK_M
#error DIVERGENCE MVDM-HOST-DIV-525: BACK_M platform branch is not selected; see mirror register.
#else
#line 414
#define	PLUS +
#define	MINUS -
#endif
#line 417

	if( getDF() )
		setVideolatches(*(IU32 *)( EGA_planes + (( offset >> 1 ) << 2 )));
	else
		setVideolatches(*(IU32 *)( EGA_planes + ((( offset + count - 1 ) >> 1 ) << 2 )));

	dest = dest PLUS count;

	lsb = offset & 0x1;
	offset = ( offset >> 1 ) << 2;

	/* Two streams of source data */

	p01 = &EGA_plane01[offset];
	p23 = &EGA_plane23[offset];

	offset = 0;

	if( lsb )
	{
		*(dest MINUS count) = (( *(p01 + 1) ^ comp1 ) | dont_care1 )
							& (( *(p23 + 1) ^ comp3 ) | dont_care3 );
		count--;
		offset += 4;
	}

	tcount = count & ~1;

	while( tcount-- )
	{
		*(dest MINUS tcount) = (( *(p01 + offset) ^ comp0) | dont_care0 )
							& (( *(p23 + offset) ^ comp2 ) | dont_care2 );

		tcount--;
		offset += 1;

		*(dest MINUS tcount) = (( *(p01 + offset) ^ comp1) | dont_care1 )
							& (( *(p23 + offset) ^ comp3 ) | dont_care3 );

		offset += 3;
	}	

	if( count & 1 )
	{
		*(dest MINUS count) = (( *(p01 + offset) ^ comp0 ) | dont_care0 )
							& (( *(p23 + offset) ^ comp2 ) | dont_care2 );
	}
}

#ifdef SEGMENTATION
/*
 * The following #include specifies the code segment into which this
 * module will by placed by the MPW C compiler on the Mac II running
 * MultiFinder.
 */
#include "SOFTPC_GRAPHICS.seg"
#endif
#line 474

#if !(defined(NTVDM) && defined(MONITOR))
GLOBAL void
Glue_set_vid_rd_ptrs IFN1(READ_POINTERS *, handler )
{
#ifndef CPU_40_STYLE	/* EVID */
#ifdef A3CPU
#error DIVERGENCE MVDM-HOST-DIV-522: A3CPU platform branch is not selected; see mirror register.
#ifdef C_VID
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#else					/* A3CPU */
#line 483
#ifdef C_VID

	C_vid_reads.b_read = handler->b_read;
	C_vid_reads.w_read = handler->w_read;

#ifndef	NO_STRING_OPERATIONS
	C_vid_reads.str_read = handler->str_read;
#endif	/* NO_STRING_OPERATIONS */

#else

	A_vid_reads = *handler;

#if	0
#error DIVERGENCE MVDM-HOST-DIV-496: original literal #if 0 excludes this legacy branch; see mirror register.
#ifndef	NO_STRING_OPERATIONS
#error Excluded original conditional branch
#endif
#endif	/* 0 */
#line 499

#endif	/* C_VID */
#line 501
#endif	/* A3CPU */
#line 502
#endif	/* CPU_40_STYLE - EVID */
#line 503
}	
#endif /* !(NTVDM && MONITOR) */
#line 505

#ifdef SEGMENTATION
/*
 * The following #include specifies the code segment into which this
 * module will by placed by the MPW C compiler on the Mac II running
 * MultiFinder.
 */
#include "SOFTPC_EGA.seg"
#endif
#line 514

GLOBAL void
update_shift_count IFN0()

{
	switch( EGA_CPU.chain )
	{
		case UNCHAINED:

 			/*
 			 *	Interleaved - need a shift count for accessing the mapped plane
 			 */

#ifdef BIGEND
#error DIVERGENCE MVDM-HOST-DIV-524: BIGEND platform branch is not selected; see mirror register.
#else
#line 530
 			setVideoread_shift_count(getVideoread_mapped_plane() << 3);
#endif /* BIGEND */
#line 532


			break;

		case CHAIN2:

 			/*
 			 *	Planar - need an offset for accessing the mapped plane
 			 */

 			EGA_CPU.read_mapped_plane_ch2 = EGA_planes +
		 					(getVideoread_mapped_plane() & 2) * EGA_PLANE_SIZE;

			break;

#ifdef	VGG
		case CHAIN4:

 			/*
 			 *	Interleaved - doesn't need any magic numbers
 			 */

			break;
#endif	/* VGG */
#line 556
	}
}

void
ega_read_routines_update IFN0()

{
	LOCAL BOOL ram_off = TRUE;	/* optimised to avoid updates if ram disabled */
	LOCAL READ_POINTERS *read_ptrs;

	/* ram disabled and not now being enabled	*/

	if( ram_off && (!EGA_CPU.ram_enabled ))
		return;

	if( !EGA_CPU.ram_enabled )	/* video off, just return 0xff */
	{
#ifdef CPU_40_STYLE
		SetReadPointers(2);
#else  /* CPU_40_STYLE */
#line 576
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#ifndef GISP_CPU
#error Excluded original conditional branch
#ifdef A3CPU
#error Excluded original conditional branch
#ifdef C_VID
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#else
#error Excluded original conditional branch
#endif
#endif
#ifndef	NO_STRING_OPERATIONS
#error Excluded original conditional branch
#endif
#endif  /* CPU_40_STYLE */
#line 578

		ram_off = TRUE;	/* prevent recalcs until ram enabled again */

		return;
	}

	ram_off = FALSE;

	if( read_state.mode == 0 )    /* read mode 0 */
	{
		/* chained in write mode implies chained for reading too */

		switch( EGA_CPU.chain )
		{
			case UNCHAINED:
#ifdef CPU_40_STYLE
				SetReadPointers(0);
#else  /* CPU_40_STYLE */
#line 596
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#ifndef	NO_STRING_OPERATIONS
#error Excluded original conditional branch
#endif
#endif	/* CPU_40_STYLE */
#line 598
				break;
				
			case CHAIN2:
#ifdef CPU_40_STYLE
				SetReadPointers(0);
#else  /* CPU_40_STYLE */
#line 604
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#ifndef	NO_STRING_OPERATIONS
#error Excluded original conditional branch
#endif
#endif	/* CPU_40_STYLE */
#line 606
				EGA_CPU.read_mapped_plane_ch2 = EGA_planes +
							(getVideoread_mapped_plane() & 2)*EGA_PLANE_SIZE;
				break;
				
#ifdef	VGG
			case CHAIN4:
#ifdef CPU_40_STYLE
				SetReadPointers(0);
#else  /* CPU_40_STYLE */
#line 615
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#ifndef	NO_STRING_OPERATIONS
#error Excluded original conditional branch
#endif
#endif	/* CPU_40_STYLE */
#line 617
				break;
#endif	/* VGG */
#line 619
		}
	}
	else   /* read mode 1 */
	{
		switch( EGA_CPU.chain )
		{
			case UNCHAINED:
				/* preserve or complement values by xor with comps later */

				setVideodont_care(~sr_lookup[read_state.colour_dont_care]);
				setVideocolour_comp(~sr_lookup[read_state.colour_compare]);

#ifdef CPU_40_STYLE
				SetReadPointers(1);
#else  /* CPU_40_STYLE */
#line 634
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#ifndef	NO_STRING_OPERATIONS
#error Excluded original conditional branch
#endif
#endif	/* CPU_40_STYLE */
#line 636
				break;

			case CHAIN2:
				dont_care0 = read_state.colour_dont_care & 1 ? 0 : 0xff;
				dont_care1 = read_state.colour_dont_care & 2 ? 0 : 0xff;
				dont_care2 = read_state.colour_dont_care & 4 ? 0 : 0xff;
				dont_care3 = read_state.colour_dont_care & 8 ? 0 : 0xff;

				comp0 = read_state.colour_compare & 1 ? 0 : 0xff;
				comp1 = read_state.colour_compare & 2 ? 0 : 0xff;
				comp2 = read_state.colour_compare & 4 ? 0 : 0xff;
				comp3 = read_state.colour_compare & 8 ? 0 : 0xff;

#ifdef CPU_40_STYLE
				SetReadPointers(1);
#else  /* CPU_40_STYLE */
#line 652
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#ifndef	NO_STRING_OPERATIONS
#error Excluded original conditional branch
#endif
#endif	/* CPU_40_STYLE */
#line 654

				break;

#ifdef	VGG
			case CHAIN4:
				setVideodont_care(( read_state.colour_dont_care & 1 ) ? 0 : 0xff);
				setVideocolour_comp(( read_state.colour_compare & 1 ) ? 0 : 0xff);

#ifdef CPU_40_STYLE
				SetReadPointers(1);
#else  /* CPU_40_STYLE */
#line 665
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#ifndef	NO_STRING_OPERATIONS
#error Excluded original conditional branch
#endif
#endif	/* CPU_40_STYLE */
#line 667
				break;
#endif	/* VGG */
#line 669
		}
	}

	update_shift_count();
	update_banking();

#ifndef CPU_40_STYLE
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#ifndef GISP_CPU
#error Excluded original conditional branch
#ifdef A3CPU
#error Excluded original conditional branch
#ifdef C_VID
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#else
#error Excluded original conditional branch
#endif
#endif
#endif	/* CPU_40_STYLE */
#line 678
}

void
ega_read_init IFN0()

{
	read_state.mode = 0;
	read_state.colour_compare = 0x0f;		/* looking for bright white */
	read_state.colour_dont_care = 0xf;		/* all planes significant */

#ifdef CPU_40_STYLE
	SetReadPointers(2);
#else  /* CPU_40_STYLE */
#line 691
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#ifndef	NO_STRING_OPERATIONS
#error Excluded original conditional branch
#endif
#endif	/* CPU_40_STYLE */
#line 693

	setVideoread_mapped_plane(0);

	ega_read_routines_update();			/* initialise M */

#if defined(EGA_DUMP) || defined(EGA_STAT)
	dump_read_pointers_init();
#endif
#line 701

#if !defined(EGATEST) && !defined(A3CPU)
	read_pointers = Glue_reads;
#endif /* EGATEST */
#line 705

#ifndef CPU_40_STYLE
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#ifndef GISP_CPU
#error Excluded original conditional branch
#ifdef A3CPU
#error Excluded original conditional branch
#ifdef C_VID
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#else
#error Excluded original conditional branch
#endif
#endif
#endif	/* CPU_40_STYLE */
#line 709
}

void
ega_read_term IFN0()

{
	/*
	 *	Turn off read calculations for non EGA/VGA adaptors
	 */

#ifdef CPU_40_STYLE
		SetReadPointers(3);
#else  /* CPU_40_STYLE */
#line 722
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#ifndef GISP_CPU
#error Excluded original conditional branch
#ifdef A3CPU
#error Excluded original conditional branch
#ifdef C_VID
#error Excluded original conditional branch
#else
#error Excluded original conditional branch
#endif
#else
#error Excluded original conditional branch
#endif
#endif
#endif	/* CPU_40_STYLE */
#line 724
}

#endif /* REAL_VGA */
#line 727
#endif /* EGG */
#line 728

#endif	/* !(NTVDM && MONITOR) */
#line 730
