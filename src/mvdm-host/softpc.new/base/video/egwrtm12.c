#include "insignia.h"
#include "host_def.h"

#if !defined(NTVDM) || (defined(NTVDM) && !defined(X86GFX) )

/*			INSIGNIA (SUB)MODULE SPECIFICATION
			-----------------------------


	THIS PROGRAM SOURCE FILE  IS  SUPPLIED IN CONFIDENCE TO THE
	CUSTOMER, THE CONTENTS  OR  DETAILS  OF  ITS OPERATION MUST
	NOT BE DISCLOSED TO ANY  OTHER PARTIES  WITHOUT THE EXPRESS
	AUTHORISATION FROM THE DIRECTORS OF INSIGNIA SOLUTIONS LTD.


DOCUMENT 			: name and number

RELATED DOCS		: include all relevant references

DESIGNER			: J Maiden 

REVISION HISTORY	:
First version		: J Maiden, SoftPC 2.0
Second version		: J Shanly, SoftPC 3.0

SUBMODULE NAME		: write mode 1 and 2

SOURCE FILE NAME	: ega_writem1.c

PURPOSE			: functions to write to EGA memory in write modes 1 & 2
		
		
SccsID = @(#)ega_wrtm12.c	1.20 3/9/94 Copyright Insignia Solutions Ltd.

[1.INTERMODULE INTERFACE SPECIFICATION]

[1.0 INCLUDE FILE NEEDED TO ACCESS THIS INTERFACE FROM OTHER SUBMODULES]

	INCLUDE FILE : ega_cpu.pi

[1.1    INTERMODULE EXPORTS]

	PROCEDURES() :	
			ega_mode1_chn_b_write();
			ega_mode1_chn_w_write();
			ega_mode1_chn_b_fill();
			ega_mode1_chn_w_fill();
			ega_mode1_chn_b_move();
			ega_mode1_chn_w_move();
			ega_mode2_chn_b_write();
			ega_mode2_chn_w_write();
			ega_mode2_chn_b_fill();
			ega_mode2_chn_w_fill();
			ega_mode2_chn_b_move();
			ega_mode2_chn_w_move();

	DATA 	     :	give type and name

-------------------------------------------------------------------------
[1.2 DATATYPES FOR [1.1] (if not basic C types)]

	STRUCTURES/TYPEDEFS/ENUMS: 
		
-------------------------------------------------------------------------
[1.3 INTERMODULE IMPORTS]
     (not o/s objects or standard libs)


-------------------------------------------------------------------------

[1.4 DESCRIPTION OF INTERMODULE INTERFACE]

[1.4.1 IMPORTED OBJECTS]

DATA OBJECTS	  :	struct EGA_CPU

FILES ACCESSED    :	NONE

DEVICES ACCESSED  :	NONE

SIGNALS CAUGHT	  :	NONE

SIGNALS ISSUED	  :	NONE


[1.4.2 EXPORTED OBJECTS]

/*=======================================================================
[3.INTERMODULE INTERFACE DECLARATIONS]
=========================================================================

[3.1 INTERMODULE IMPORTS]						*/

/* [3.1.1 #INCLUDES]                                                    */


#ifdef EGG

#include	"xt.h"
#include	"debug.h"
#include	"sas.h"
#include	TypesH
#include	CpuH
#include	"gmi.h"
#include	"egacpu.h"
#include	"egaports.h"
#include	"cpu_vid.h"
#include	"gfx_upd.h"

/* [3.1.2 DECLARATIONS]                                                 */


/* [3.2 INTERMODULE EXPORTS]						*/ 


/*
5.MODULE INTERNALS   :   (not visible externally, global internally)]     

[5.1 LOCAL DECLARATIONS]						*/

/* [5.1.1 #DEFINES]							*/

#ifdef SEGMENTATION
#line 124
#error Excluded unsupported SEGMENTATION configuration









#endif
#line 135

/* [5.1.2 TYPEDEF, STRUCTURE, ENUM DECLARATIONS]			*/


/* [5.1.3 PROCEDURE() DECLARATIONS]					*/


/* -----------------------------------------------------------------------
[5.2 LOCAL DEFINITIONS]

   [5.2.1 INTERNAL DATA DEFINITIONS 					*/

/* [5.2.2 INTERNAL PROCEDURE DEFINITIONS]				*/


/*
7.INTERMODULE INTERFACE IMPLEMENTATION :

/*
[7.1 INTERMODULE DATA DEFINITIONS]				*/

#line 156
#ifdef A_VID
#line 157
#error Excluded unsupported original A_VID writer-table profile
#line 204
#else
#line 205
#if defined(CPU_40_STYLE)
WRT_POINTERS mode1_handlers;
WRT_POINTERS mode2_handlers;
#else
#line 205
#error Excluded unsupported non-CPU40 writer-table profile
#endif
#line 256
#endif /* A_VID */
#line 257


GLOBAL VOID
copy_alternate_bytes IFN3(byte *, start, byte *, end, byte *, source)
{
	while (start <= end)
	{
		*start = *source;
		start += 4;       /* advance by longs, writing bytes */
		source += 4;
	}
}

GLOBAL VOID
fill_alternate_bytes IFN3(byte *, start, byte *, end, byte, value )
{
	while( start <= end )
	{
		*start = value;
		start += 4;	/* advance by longs, writing bytes */
	}
}

#ifdef  BIGEND
#line 281
#error DIVERGENCE MVDM-HOST-DIV-524: BIGEND platform branch is not selected; see mirror register.
#line 283
#else
#line 284
#define first_half(wd)      (wd & 0xff)
#define sec_half(wd)        ((wd & 0xff00) >> 8)
#line 286
#endif

GLOBAL VOID
fill_both_bytes IFN3(USHORT, data, USHORT *, dest, ULONG, len )
{
#line 291
	USHORT swapped;

#ifdef BIGEND
#line 294
#error DIVERGENCE MVDM-HOST-DIV-524: BIGEND platform branch is not selected; see mirror register.
#endif

	if( (ULONG) dest & 1 )
	{
		*((UTINY *) dest) = first_half(data);

		dest = (USHORT *) ((ULONG) dest + 1);
		len--;

		while( len-- )
		{
			*dest = data;
			dest += 2;
		}

		*((UTINY *) dest) = sec_half(data);
	}
	else 
	{
		while( len-- )
		{
#ifdef BIGEND
#line 317
#error DIVERGENCE MVDM-HOST-DIV-524: BIGEND platform branch is not selected; see mirror register.
#else
			*dest = data;
#endif
			dest += 2;
		}
	}
}


#ifdef SEGMENTATION
#line 328
#error Excluded unsupported SEGMENTATION configuration





#endif

VOID
ega_mode1_chn_b_write IFN2(ULONG, value, ULONG, offset )
{
	ULONG lsb;

	UNUSED(value);
	
	note_entrance0("ega_mode1_chn_b_write");

	lsb = offset & 1;
	offset = (offset & ~1u) << 2;

	if( lsb )	/* odd address, in plane 1 or 3 */
	{
		if( getVideoplane_enable() & 2 )
			EGA_plane01[offset + 1] = get_latch1; 

		if( getVideoplane_enable() & 8 )
			EGA_plane23[offset + 1] = get_latch3; 
	}
	else		/* even address, in plane 0 or 2 */
	{
		if( getVideoplane_enable() & 1 )
			EGA_plane01[offset] = get_latch0; 

		if( getVideoplane_enable() & 4 )
			EGA_plane23[offset] = get_latch2; 
	}

	update_alg.mark_byte( offset );
}

VOID
ega_mode1_chn_w_write IFN2(ULONG, value, ULONG, offset )
{
	ULONG lsb;

	UNUSED(value);

	note_entrance0("ega_mode1_chn_w_write");

	lsb = offset & 1;
	offset = (offset & ~1u) << 2;

	if( lsb )	/* odd address, low byte in planes 1 and 3 */
	{
		if( getVideoplane_enable() & 2 )
			EGA_plane01[offset + 1] = get_latch1; 

		if( getVideoplane_enable() & 1 )
			EGA_plane01[offset + 8] = get_latch0; 

		if( getVideoplane_enable() & 8 )
			EGA_plane23[offset + 1] = get_latch3; 

		if( getVideoplane_enable() & 4 )
			EGA_plane23[offset + 8] = get_latch2; 
	}
	else		/* even address, low byte in planes 0 and 2 */
	{
		if( getVideoplane_enable() & 1 )
			EGA_plane01[offset] = get_latch0; 

		if( getVideoplane_enable() & 2 )
			EGA_plane01[offset + 1] = get_latch1; 

		if( getVideoplane_enable() & 4 )
			EGA_plane23[offset] = get_latch2; 

		if( getVideoplane_enable() & 8 )
			EGA_plane23[offset + 1] = get_latch3; 
	}

	update_alg.mark_word( offset );
}

#ifndef CPU_40_STYLE
#line 414
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#endif /* CPU_40_STYLE */

#line 654
VOID
ega_mode2_chn_b_write IFN2(ULONG, value, ULONG, offset )
{
	ULONG	value1;
	ULONG lsb;

	note_entrance0("ega_mode2_chn_b_write");

	lsb = offset & 1;
	offset = (offset & ~1u) << 2;

	if( EGA_CPU.fun_or_protection )
	{
		if( lsb )	/* odd address, applies to planes 1 and 3 */
		{
			if( getVideoplane_enable() & 2 )
			{
				value1 = value & 2 ? 0xff : 0;
				EGA_plane01[offset + 1] = do_logicals( value1, get_latch1 );
			}

			if( getVideoplane_enable() & 8 )
			{
				value1 = value & 8 ? 0xff : 0;
				EGA_plane23[offset + 1] = do_logicals( value1, get_latch3 );
			}
		}
		else		/* even address, applies to planes 0 and 2 */
		{
			if( getVideoplane_enable() & 1 )
			{
				value1 = value & 1 ? 0xff : 0;
				EGA_plane01[offset] = do_logicals( value1, get_latch0 );
			}

			if( getVideoplane_enable() & 4 )
			{
				value1 = value & 4 ? 0xff : 0;
				EGA_plane23[offset] = do_logicals( value1, get_latch2 );
			}
		}
	}
	else	/* no difficult function or protection stuff */
	{
		if( lsb )	/* odd address, applies to planes 1 and 3 */
		{
			if( getVideoplane_enable() & 2 )
				EGA_plane01[offset + 1] = value & 2 ? 0xff : 0;

			if( getVideoplane_enable() & 8 )
				EGA_plane23[offset + 1] = value & 8 ? 0xff : 0;
		}
		else		/* even address, applies to planes 0 and 2 */
		{
			if( getVideoplane_enable() & 1 )
				EGA_plane01[offset] = value & 1 ? 0xff : 0;

			if( getVideoplane_enable() & 8 )
				EGA_plane23[offset] = value & 4 ? 0xff : 0;
		}
	}

	update_alg.mark_byte( offset );
}

VOID
ega_mode2_chn_w_write IFN2(ULONG, value, ULONG, offset )
{
	ULONG value2;
	ULONG lsb;
	ULONG low, high;

	low = value & 0xff;
	high = value >> 8;

	note_entrance0("ega_mode2_chn_w_write");

	lsb = offset & 1;
	offset = (offset & ~1u) << 2;

	if( EGA_CPU.fun_or_protection )
	{
		if( lsb )	/* odd address, low byte in planes 1 and 3 */
		{
			if( getVideoplane_enable() & 2 )
			{
				value2 = low & 2 ? 0xff : 0;
				EGA_plane01[offset + 1] = do_logicals( value2, get_latch1 );
			}

			if( getVideoplane_enable() & 1 )
			{
				value2 = high & 1 ? 0xff : 0;
				EGA_plane01[offset + 8] = do_logicals( value2, get_latch0 );
			}

			if( getVideoplane_enable() & 8 )
			{
				value2 = low & 8 ? 0xff : 0;
				EGA_plane23[offset + 1] = do_logicals( value2, get_latch3 );
			}

			if( getVideoplane_enable() & 4 )
			{
				value2 = high & 4 ? 0xff : 0;
				EGA_plane23[offset + 8] = do_logicals( value2, get_latch2 );
			}
		}
		else		/* even address, low byte in planes 0 and 2 */
		{
			if( getVideoplane_enable() & 1 )
			{
				value2 = low & 1 ? 0xff : 0;
				EGA_plane01[offset] = do_logicals( value2, get_latch0 );
			}

			if( getVideoplane_enable() & 2 )
			{
				value2 = high & 2 ? 0xff : 0;
				EGA_plane01[offset + 1] = do_logicals( value2, get_latch1 );
			}

			if( getVideoplane_enable() & 4 )
			{
				value2 = low & 4 ? 0xff : 0;
				EGA_plane23[offset] = do_logicals( value2, get_latch2 );
			}

			if( getVideoplane_enable() & 8 )
			{
				value2 = high & 8 ? 0xff : 0;
				EGA_plane23[offset + 1] = do_logicals( value2, get_latch3 );
			}
		}
	}
	else	/* easy no function or bit prot case */
	{
		if( lsb )	/* odd address, low byte in planes 1 and 3 */
		{
			if( getVideoplane_enable() & 2 )
				EGA_plane01[offset + 1] = low & 2 ? 0xff : 0;

			if( getVideoplane_enable() & 1 )
				EGA_plane01[offset + 8] = high & 1 ? 0xff : 0;

			if( getVideoplane_enable() & 8 )
				EGA_plane23[offset + 1] = low & 8 ? 0xff : 0;

			if( getVideoplane_enable() & 4 )
				EGA_plane23[offset + 8] = high & 4 ? 0xff : 0;
		}
		else		/* even address, low byte in planes 0 and 2 */
		{
			if( getVideoplane_enable() & 1 )
				EGA_plane01[offset] = low & 1 ? 0xff : 0;

			if( getVideoplane_enable() & 2 )
				EGA_plane01[offset + 1] = high & 2 ? 0xff : 0;

			if( getVideoplane_enable() & 4 )
				EGA_plane23[offset] = low & 4 ? 0xff : 0;

			if( getVideoplane_enable() & 8 )
				EGA_plane23[offset + 1] = high & 8 ? 0xff : 0;
		}
	}

	update_alg.mark_word( offset );
}

VOID
ega_mode2_chn_b_fill IFN3(ULONG, value, ULONG, offset, ULONG, count )
{
	ULONG low_offset;		/* distance into regen buffer of write start and end */
	ULONG high_offset;	/* distance into regen buffer of write start and end */
	ULONG new_value;

	note_entrance0("ega_mode2_chn_b_fill");

	/*
	 *	Complicated by possibility that only one of a chained pair of
	 *	planes is write enabled, needing alternate bytes to be written.
	 */ 

	/* starting on odd address makes it difficult, go to next one */

	if(( (ULONG) offset & 1 ) && count )
	{
		ega_mode2_chn_b_write(value, offset++ );
		count--;
	}

	/* ending on even address makes it difficult, retreat to previous one */

	if(( (ULONG)( offset + count - 1 ) & 1 ) == 0 && count )
	{
		ega_mode2_chn_b_write(value, offset + count - 1 );
		count--;
	}

	low_offset = (offset >> 1) << 2;				/* start of write */
	high_offset = ((offset + count - 1) >> 1) << 2;		/* end of write */

	switch( getVideoplane_enable() & 3 )
	{
		case 1:	/* just plane 0, ie even addresses to be written */
			value = value & 1 ? 0xff : 0;

			if( EGA_CPU.fun_or_protection )
				value = do_logicals( value, get_latch0 );

			fill_alternate_bytes( &EGA_plane01[low_offset],
									&EGA_plane01[high_offset], value );
			break;

		case 2:	/* just plane 1, ie odd addresses to be written */
			value = value & 2 ? 0xff : 0;

			if( EGA_CPU.fun_or_protection )
				value = do_logicals( value, get_latch1 );

			fill_alternate_bytes( &EGA_plane01[low_offset + 1],
									&EGA_plane01[high_offset], value );
			break;

		case 3:	/* sensible case is to have both chained planes write enabled */
			new_value = ( value & 1 ? 0xff : 0) | (value & 2 ? 0xff00: 0);

			if( EGA_CPU.fun_or_protection )
				new_value = do_logicals( new_value, get_latch01);

			fill_both_bytes( new_value, (USHORT *)&EGA_plane01[low_offset], count >> 1 );
			break;

	}	/* end of switch on plane01 enabled */

	switch( getVideoplane_enable() & 0xc )	/* isolate 2 bits for planes2 and 3 */
	{
		case 4:	/* just plane 2, ie even addresses to be written */
			value = value & 4 ? 0xff : 0;

			if( EGA_CPU.fun_or_protection )
				value = do_logicals( value, get_latch2 );

			fill_alternate_bytes( &EGA_plane23[low_offset],
								&EGA_plane23[high_offset], value );
			break;

		case 8:	/* just plane 3, ie odd addresses to be written */
			value = value & 8 ? 0xff : 0;

			if( EGA_CPU.fun_or_protection )
				value = do_logicals( value, get_latch3 );

			fill_alternate_bytes( &EGA_plane23[low_offset + 1],
								&EGA_plane23[high_offset], value );
			break;

		case 12:	/* sensible case is to have both chained planes write enabled */
			new_value = ( value & 4 ? 0xff : 0) | (value & 8 ? 0xff00: 0);

			if( EGA_CPU.fun_or_protection )
				new_value = do_logicals( new_value, get_latch23);

			fill_both_bytes( new_value, (USHORT *)&EGA_plane23[low_offset], count >> 1 );
			break;
	}	/* end of switch on plane23 enabled */

	update_alg.mark_fill( offset, offset + count - 1 );
}

VOID
ega_mode2_chn_w_fill IFN3(ULONG, value, ULONG, offset, ULONG, count )
{
	ULONG	low_offset;		/* distance into regen buffer of write start and end */
	ULONG	high_offset;	/* distance into regen buffer of write start and end */
	ULONG	value1;

	note_entrance0("ega_mode2_chn_w_fill");

	/*
	 *	Complicated by possibility that only one of a chained pair of
	 *	planes is write enabled, needing alternate bytes to be written.
	 */ 

	/* starting on odd address makes it difficult, go to next one */

	if(( (ULONG) offset & 1 ) && count )
	{
		ega_mode2_chn_b_write( value, offset++);
		count--;

		if( count )
		{
			ega_mode2_chn_b_write( value >> 8, offset + count - 1 );
			count--;
		}

		value = ( value << 8 ) | ( value >> 8 );
	}

	low_offset = (offset >> 1) << 2;				/* start of write */
	high_offset = ((offset + count - 1) >> 1) << 2;		/* end of write */

	switch( getVideoplane_enable() & 3 )
	{
		case 1:	/* just plane 0, ie even addresses to be written */
			value1 = value & 1 ? 0xff : 0;

			if( EGA_CPU.fun_or_protection )
				value1 = do_logicals( value1, get_latch0 );

			fill_alternate_bytes( &EGA_plane01[low_offset],
									&EGA_plane01[high_offset], value1 );
			break;

		case 2:	/* just plane 1, ie odd addresses to be written */
			value1 = ( value >> 8 ) & 2 ? 0xff : 0;

			if( EGA_CPU.fun_or_protection )
				value1 = do_logicals( value1, get_latch1 );

			fill_alternate_bytes( &EGA_plane01[low_offset + 1],
									&EGA_plane01[high_offset], value1 );
			break;

		case 3:	/* sensible case is to have both chained planes write enabled */
			/* get a word pattern for filling */
			value1 = ( value & 1 ? 0xff : 0 ) | (( value >> 8 ) & 2 ? 0xff00: 0 );

			if( EGA_CPU.fun_or_protection )
				value1 = do_logicals( value1, get_latch01 );

			fill_both_bytes( value1, (USHORT *)&EGA_plane01[low_offset], count >> 1 );
			break;
	}	/* end of switch on plane01 enabled */

	switch( getVideoplane_enable() & 0xc )	/* isolate 2 bits for planes 2 and 3 */
	{
		case 4:	/* just plane 2, ie even addresses to be written */
			value1 = value & 4 ? 0xff : 0;

			if( EGA_CPU.fun_or_protection )
				value1 = do_logicals( value1, get_latch2 );

			fill_alternate_bytes( &EGA_plane23[low_offset],
									&EGA_plane23[high_offset], value1 );
			break;

		case 8:	/* just plane 3, ie odd addresses to be written */
			value1 = ( value >> 8 ) & 8 ? 0xff : 0;

			if( EGA_CPU.fun_or_protection )
				value1 = do_logicals( value1, get_latch3 );

			fill_alternate_bytes( &EGA_plane23[low_offset + 1],
									&EGA_plane23[high_offset], value1 );
			break;

		case 12:	/* sensible case is to have both chained planes write enabled */
			/* get a word pattern for filling */
			value1 = ( value & 4 ? 0xff : 0 ) | (( value >> 8 ) & 8 ? 0xff00: 0 );

			if( EGA_CPU.fun_or_protection )
				value1 = do_logicals( value1, get_latch23);

			fill_both_bytes( value1, (USHORT *)&EGA_plane23[low_offset], count >> 1 );
			break;
	}	/* end of switch on plane23 enabled */

	/* the 3rd parameter is needed by GORE. */
	update_alg.mark_wfill( offset, offset + count - 1, 0 );
}

#ifndef CPU_40_STYLE
#line 1028
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#endif /* CPU_40_STYLE */

#line 1216
#endif

#line 1218
#endif	/* !NTVDM | (NTVDM & !X86GFX) */
