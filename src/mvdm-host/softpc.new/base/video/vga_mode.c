#include "insignia.h"
#include "host_def.h"
/*			INSIGNIA (SUB)MODULE SPECIFICATION
			-----------------------------


	THIS PROGRAM SOURCE FILE  IS  SUPPLIED IN CONFIDENCE TO THE
	CUSTOMER, THE CONTENTS  OR  DETAILS  OF  ITS OPERATION MUST
	NOT BE DISCLOSED TO ANY  OTHER PARTIES  WITHOUT THE EXPRESS
	AUTHORISATION FROM THE DIRECTORS OF INSIGNIA SOLUTIONS LTD.

DESIGNER		: S.Frost

REVISION HISTORY	:
First version		: Feb/Mar 1990. Simon Frost

SOURCE FILE NAME	: vga_mode.c

PURPOSE			: To decide which mode the VGA is in according to
			  variables set via vga_ports.c and to choose the
			  appropriate update and paint routines accordingly.
			  Borrows heavily from ega_mode.c...

static char SccsID[]="@(#)vga_mode.c	1.35 06/01/95 Copyright Insignia Solutions Ltd.";




[1.INTERMODULE INTERFACE SPECIFICATION]

[1.0 INCLUDE FILE NEEDED TO ACCESS THIS INTERFACE FROM OTHER SUBMODULES]

	INCLUDE FILE : ega_mode.gi

[1.1    INTERMODULE EXPORTS]

	PROCEDURES() :	choose_vga_display_mode

	DATA 	     :	uses EGA_GRAPH.display_state which is set via vga_ports.c, to
			determine what memory organisation the display side is in, and
			hence what sort of update and paint routines to use.

-------------------------------------------------------------------------
[1.2 DATATYPES FOR [1.1] (if not basic C types)]

	STRUCTURES/TYPEDEFS/ENUMS: 

uses	enum DISPLAY_STATE which is declared in ega_graph.pi.

uses	EGA_GRAPH structure for global variables set by the ports and
	used by the display.

-------------------------------------------------------------------------
[1.3 INTERMODULE IMPORTS]
     (not o/s objects or standard libs)

	PROCEDURES() : vote_vga_mode()
			host_set_paint_routine(DISPLAY_MODE)

	DATA 	     : EGA_GRAPH struct.

-------------------------------------------------------------------------

[1.4 DESCRIPTION OF INTERMODULE INTERFACE]

[1.4.1 IMPORTED OBJECTS]

[1.4.2 EXPORTED OBJECTS]

=========================================================================
PROCEDURE	  : 	choose_vga_display_mode

PURPOSE		  : 	To decide which memory organisation is being used by
			the vga, and to pick the best update and paint routines
			accordingly.  The paint routines are host specific,
			and so the memory organisation is indicated by an enum
			(called DISPLAY_MODE), describing each sort of memory
			organisation.

PARAMETERS	  :	none 

GLOBALS		  :	uses EGA_GRAPH struct, specially display_state to
			decide which mode is being used.

=========================================================================


/*=======================================================================
[3.INTERMODULE INTERFACE DECLARATIONS]
=========================================================================

[3.1 INTERMODULE IMPORTS]						*/


#ifndef REAL_VGA
#ifdef	VGG

/* [3.1.1 #INCLUDES]                                                    */

#include	"xt.h"
#include	"error.h"
#include	"config.h"
#include	"gvi.h"
#include	"gmi.h"
#include	"gfx_upd.h"
#include	"egagraph.h"
#include	"vgaports.h"
#include	"egacpu.h"
#include	"egaports.h"
#include	"debug.h"
#include	"host_gfx.h"

#ifdef GORE
#line 114
#error DIVERGENCE MVDM-HOST-DIV-525: GORE platform branch is not selected; see mirror register.
#endif /* GORE */
#line 116

/* [3.1.2 DECLARATIONS]                                                 */

/* [3.2 INTERMODULE EXPORTS]						*/ 

#include	"egamode.h"

#ifdef GISP_SVGA 
#line 124
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#endif /* GISP_SVGA */
#line 127

/*
5.MODULE INTERNALS   :   (not visible externally, global internally)]     

[5.1 LOCAL DECLARATIONS]						*/

/* [5.1.1 #DEFINES]							*/
#ifdef SEGMENTATION
#line 135
#error Excluded unsupported SEGMENTATION configuration
#endif
#line 142

/* [5.1.2 TYPEDEF, STRUCTURE, ENUM DECLARATIONS]			*/

/* [5.1.3 PROCEDURE() DECLARATIONS]					*/

/*
=========================================================================
PROCEDURE	  : 	set_up_screen_ptr()

PURPOSE		  : 	Decide which plane the information must come from for displaying

PARAMETERS	  :	none 

GLOBALS		  :	uses EGA_GRAPH struct, plane_mask to decide which planes are enabled

=========================================================================
*/

LOCAL VOID
set_up_screen_ptr()
{
	if( get_chain4_mode() )
	{
		if (all_planes_enabled())
			set_screen_ptr(EGA_plane0123);
		else
			assert0(NO,"No planes enabled for chain-4 mode\n");
	}
	else
		if( get_memory_chained() )
		{
			if( plane01_enabled() )
				set_screen_ptr(EGA_plane01);
			else
				if( plane23_enabled() )
					set_screen_ptr(EGA_plane23);
				else
					assert0(NO,"No planes enabled for chain mode");
		}
		else
			set_screen_ptr(EGA_planes);
}

/* -----------------------------------------------------------------------
[5.2 LOCAL DEFINITIONS]

   [5.2.1 INTERNAL DATA DEFINITIONS 					*/

IMPORT	DISPLAY_MODE	choose_mode[];

/* [5.2.2 INTERNAL PROCEDURE DEFINITIONS]				*/

#ifdef GISP_SVGA
#line 195
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#endif /* GISP_SVGA */
#line 197

static	void	set_update_routine(mode)
DISPLAY_MODE	mode;
{
	static int last_height = 200;

	if (last_height != get_screen_height()) {
		last_height = get_screen_height();
	}
	note_display_state1("VGA set_update_routine(%s)", get_mode_string(mode) );

#if defined(NTVDM) && defined(MONITOR)
#line 209
#error DIVERGENCE MVDM-HOST-DIV-541: unselected compound host branch; see mirror register.
#endif	/* MONITOR */
#line 242
/* NTVDM monitor: All text monitor cases dealt with. For frozen graphics */
/* now fall through to do decode to correct paint routines per mode */

	switch (mode) {
		case EGA_TEXT_40_SP_WR:
		case EGA_TEXT_80_SP_WR:
		case CGA_TEXT_40_SP_WR:
		case CGA_TEXT_80_SP_WR:
			assert0( is_it_text(), "In text memory mode, but not in alpha mode !!" );
#ifdef GISP_SVGA
#line 252
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#else /* GISP_SVGA */
			set_gfx_update_routines( ega_wrap_split_text_update, SIMPLE_MARKING, NO_SCROLL );
#endif /* GISP_SVGA */
#line 257
			host_update_fonts();
			break;
		case EGA_TEXT_40_SP:
		case EGA_TEXT_80_SP:
		case CGA_TEXT_40_SP:
		case CGA_TEXT_80_SP:
			assert0( is_it_text(), "In text memory mode, but not in alpha mode !!" );
#ifdef GISP_SVGA
#line 265
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#else /* GISP_SVGA */
			set_gfx_update_routines( ega_split_text_update, SIMPLE_MARKING, NO_SCROLL );
#endif /* GISP_SVGA */
#line 270
			host_update_fonts();
			break;
		case EGA_TEXT_40_WR:
		case EGA_TEXT_80_WR:
			assert0( is_it_text(), "In text memory mode, but not in alpha mode !!" );
#ifdef GISP_SVGA
#line 276
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#else /* GISP_SVGA */
			set_gfx_update_routines( ega_wrap_text_update, SIMPLE_MARKING, NO_SCROLL );
#endif /* GISP_SVGA */
#line 281
			host_update_fonts();
			break;
		case EGA_TEXT_40:
		case EGA_TEXT_80:
			assert0( is_it_text(), "In text memory mode, but not in alpha mode !!" );
#if defined(NTVDM) && !defined(MONITOR)   /* Only get here for NTVDM Riscs */
			{
	    		    extern void jazz_text_update();
	    		    set_gfx_update_routines( jazz_text_update, SIMPLE_MARKING, TEXT_SCROLL );
			}
#else
#line 292
#error Unsupported excluded original configuration
#endif	/* NTVDM */
#line 300
			break;
		case CGA_TEXT_40_WR:
		case CGA_TEXT_80_WR:
			assert0( is_it_text(), "In text memory mode, but not in alpha mode !!" );
#ifdef GISP_SVGA
#line 305
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#endif /* GISP_SVGA */
#line 307
			set_gfx_update_routines( ega_wrap_text_update, SIMPLE_MARKING, NO_SCROLL );
			host_update_fonts();
			break;
		case CGA_TEXT_40:
		case CGA_TEXT_80:
			assert0( is_it_text(), "In text memory mode, but not in alpha mode !!" );
#ifdef GISP_SVGA
#line 314
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#endif /* GISP_SVGA */
#line 316
			assert1( get_screen_height() == 200, "screen height %d for text mode", get_screen_height() );
			set_gfx_update_routines( text_update, SIMPLE_MARKING, TEXT_SCROLL );
			host_update_fonts();
			break;
		case CGA_MED:
			assert0( !is_it_text(), "In graphics memory mode, but not in graphics mode !!" );
#ifdef GISP_SVGA
#line 323
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#endif /* GISP_SVGA */
#line 326
			set_gfx_update_routines( cga_med_graph_update, CGA_GRAPHICS_MARKING, CGA_GRAPH_SCROLL );
			break;
		case CGA_HI:
			assert0( !is_it_text(), "In graphics memory mode, but not in graphics mode !!" );
#ifdef GISP_SVGA
#line 331
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#endif /* GISP_SVGA */
#line 334
			set_gfx_update_routines( cga_hi_graph_update, CGA_GRAPHICS_MARKING, CGA_GRAPH_SCROLL );
			break;
		case EGA_HI_WR:
		case EGA_MED_WR:
		case EGA_LO_WR:
			assert0( !is_it_text(), "In graphics memory mode, but not in graphics mode !!" );
#ifdef GISP_SVGA
#line 341
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#endif /* GISP_SVGA */
#line 344
			set_gfx_update_routines( ega_wrap_graph_update, EGA_GRAPHICS_MARKING, NO_SCROLL );
			break;
		case EGA_HI:
		case EGA_MED:
		case EGA_LO:
			assert0( !is_it_text(), "In graphics memory mode, but not in graphics mode !!" );
#ifdef GISP_SVGA
#line 351
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#endif /* GISP_SVGA */
#line 354
#ifdef GORE
#line 355
#error DIVERGENCE MVDM-HOST-DIV-525: GORE platform branch is not selected; see mirror register.
#else
#line 360
			if (get_256_colour_mode())
			    set_gfx_update_routines( vga_graph_update, EGA_GRAPHICS_MARKING, VGA_GRAPH_SCROLL );
			else
			    set_gfx_update_routines( ega_graph_update, EGA_GRAPHICS_MARKING, EGA_GRAPH_SCROLL );
#endif /* GORE */
#line 365
			break;
		case EGA_HI_SP_WR:
		case EGA_MED_SP_WR:
		case EGA_LO_SP_WR:
			assert0( !is_it_text(), "In graphics memory mode, but not in graphics mode !!" );
#ifdef GISP_SVGA
#line 371
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#endif /* GISP_SVGA */
#line 374
			set_gfx_update_routines( ega_wrap_split_graph_update, EGA_GRAPHICS_MARKING, NO_SCROLL );
			break;

		case EGA_HI_SP:
		case EGA_MED_SP:
		case EGA_LO_SP:
			assert0( !is_it_text(), "In graphics memory mode, but not in graphics mode !!" );
#ifdef GISP_SVGA
#line 382
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#endif /* GISP_SVGA */
#line 384
			if (get_256_colour_mode())
				set_gfx_update_routines(vga_split_graph_update,
					EGA_GRAPHICS_MARKING, NO_SCROLL);
			else
				set_gfx_update_routines(ega_split_graph_update,
					EGA_GRAPHICS_MARKING, NO_SCROLL);
			break;

		case TEXT_40_FUN:
		case TEXT_80_FUN:
			assert1(NO,"Funny memory organisation selected %s", get_mode_string(mode) );
#ifdef GISP_SVGA
#line 396
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#endif /* GISP_SVGA */
#line 398
			do_display_trace("dumping EGA_GRAPH struct ...", dump_EGA_GRAPH());
			set_gfx_update_routines( text_update, SIMPLE_MARKING, NO_SCROLL );
			host_update_fonts();
			break;
		case CGA_HI_FUN:
			assert1(NO,"Funny memory organisation selected %s", get_mode_string(mode) );
#ifdef GISP_SVGA
#line 405
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#endif /* GISP_SVGA */
#line 407
			do_display_trace("dumping EGA_GRAPH struct ...", dump_EGA_GRAPH());
			set_gfx_update_routines( cga_hi_graph_update, CGA_GRAPHICS_MARKING, NO_SCROLL );
			break;
		case CGA_MED_FUN:
			assert1(NO,"Funny memory organisation selected %s", get_mode_string(mode) );
#ifdef GISP_SVGA
#line 413
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#endif /* GISP_SVGA */
#line 416
			do_display_trace("dumping EGA_GRAPH struct ...", dump_EGA_GRAPH());
			set_gfx_update_routines( cga_med_graph_update, CGA_GRAPHICS_MARKING, NO_SCROLL );
			break;
		case EGA_HI_FUN:
		case EGA_MED_FUN:
		case EGA_LO_FUN:
			assert1(NO,"Funny memory organisation selected %s", get_mode_string(mode) );
#ifdef GISP_SVGA
#line 424
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#endif /* GISP_SVGA */
#line 427
			do_display_trace("dumping EGA_GRAPH struct ...", dump_EGA_GRAPH());
#ifdef GORE
#line 429
#error DIVERGENCE MVDM-HOST-DIV-525: GORE platform branch is not selected; see mirror register.
#else
#line 431
			set_gfx_update_routines( ega_graph_update, EGA_GRAPHICS_MARKING, NO_SCROLL );
#endif /* GORE */
#line 433
			break;
		case DUMMY_FUN:
			assert0(NO,"Using the dummy mode!!");
#ifdef GISP_SVGA
#line 437
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#endif /* GISP_SVGA */
#line 439
			set_gfx_update_routines( dummy_calc, SIMPLE_MARKING, NO_SCROLL );
			break;
		default:
			assert1(NO,"Bad display mode %d", (int) mode );
#ifdef GISP_SVGA
#line 444
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#endif /* GISP_SVGA */
#line 446
			break;
	}
}


/*
7.INTERMODULE INTERFACE IMPLEMENTATION :

[7.1 INTERMODULE DATA DEFINITIONS]				*/

/*
[7.2 INTERMODULE PROCEDURE DEFINITIONS]				*/



boolean	choose_vga_display_mode()
{
	DISPLAY_MODE	mode;

	note_entrance0("choose vga display mode");

	/*
	 * offset_per_line depends upon whether chained addressing is being
	 * used. This is because we interleave the planes, rather than
	 * anything the EGA does.
	 */

	/* Packed V7 scanout keeps byte units when CPU chain4 is disabled. */
	if( get_chain4_mode()
#ifdef V7VGA
		|| (get_256_colour_mode() && get_seq_chain4_mode())
#endif
	)
#line 474
	{
		set_offset_per_line_recal(get_actual_offset_per_line() << 2);
	}
	else
		if( get_memory_chained() && !get_256_colour_mode() )
		{
			set_offset_per_line_recal(get_actual_offset_per_line() << 1);
		}
		else
		{
			set_offset_per_line_recal(get_actual_offset_per_line());
		}

	/*
	 * It is possible that the display hardware will wrap the plane addressing. This occurs
	 * when the screen_start plus the screen_length are longer than the plane length.
	 * When in chained mode there is two planes length before wrapping occurs.
	 * When in chain 4 mode there is 4 planes length before wrapping occurs.
	 *
	 * V7VGA: No wrapping can occur with either of the sequential chain variants.
	 */

#ifdef V7VGA
	if( !( get_seq_chain4_mode() || get_seq_chain_mode() ))
#endif /* V7VGA */
		if (get_chain4_mode() )
		{
			set_screen_can_wrap( (get_screen_start()<<2)
						+ get_screen_length() > 4*EGA_PLANE_DISP_SIZE );
		}
		else
			if ( get_memory_chained() && !get_256_colour_mode() )
			{
				set_screen_can_wrap( (get_screen_start()<<1)
							+ get_screen_length() > 2*EGA_PLANE_DISP_SIZE );
			}
			else
			{
				set_screen_can_wrap( get_screen_start()
							+ get_screen_length() > EGA_PLANE_DISP_SIZE );
			}

	/*
	 * split screen comes into operation when screen_split is less than screen height
	 * split screen used is used as part of munge_index.
	 */

	set_split_screen_used( get_screen_split() < get_screen_height() );

	/*
	 * For the purposes of choosing a mode set up boolean values for chars per line (to help
	 * select the correct text mode), and screen height (to select EGA resolution).
	 */

	set_200_scan_lines( (get_screen_height()/get_pc_pix_height()) == 200 );

	/*
	 * Set up the appropriate update routine according to the memory organisation selected
	 * and return an indication of whether more than 1 plane can be used by the display.
	 *
	 * Note that in chained mode plane01 is considered to be one plane. Similarly for plane23
	 *
	 * We have to be careful that a nasty program, such as EGA-PICS, hasn't set up a ridiculously big
	 * screen size for the CGA modes (presumably caused by us being unlucky when the timer tick goes off).
	 */
	if(is_it_cga() && get_screen_length() > 0x4000)
#ifdef V7VGA
		/* For the 2 & 4 colour modes 63h & 64h */
		mode = EGA_HI;
#else
#line 544
#error DIVERGENCE MVDM-HOST-DIV-538: non-V7VGA branch is not selected; see mirror register.
#endif /* V7VGA */
#line 546
	else
		mode = choose_mode[get_munged_index()];

	/*
	 * Now set up screen pointers appropriately.
	 */

	set_up_screen_ptr();

	set_update_routine(mode);

	/*
	 * set up the paint routine to correspond with the memory organisation and the update routine
	 * (this bit is host specific)
	 */

	host_set_paint_routine(mode,get_screen_height());

	/*
	 * The screen needs refreshing, because the update and paint routines have changed.
	 * Indicate to the update routines that the next time they are called, they must update
	 * the whole screen
	 */

	screen_refresh_required();
	return TRUE;
}

#endif /* EGG */
#endif /* REAL_VGA */
