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

RELATED DOCS	: include all relevant references

DESIGNER		: J.Roper

REVISION HISTORY	:
First version		: 7/22/88 W.Gulland

SUBMODULE NAME		: ega_write		

SOURCE FILE NAME	: ega_write.c

PURPOSE			: control the way writes to EGA memory is emulated.
			  This module looks at the state of the EGA when it is changed 
			  via writes to the EGA registers, and works out what to do about it. 
		
		
SccsID = @(#)ega_write.c	1.40 12/15/95 Copyright Insignia Solutions Ltd.

/*=======================================================================
[3.INTERMODULE INTERFACE DECLARATIONS]
=========================================================================

[3.1 INTERMODULE IMPORTS]						*/

/* [3.1.1 #INCLUDES]                                                    */


#include <stdio.h>
#include TypesH
#include FCntlH

#ifdef EGG
#include	"xt.h"
#include	CpuH
#include	"debug.h"
#include	"gmi.h"
#include	"gvi.h"
#include	"egacpu.h"
#include	"egaports.h"
#include	"cpu_vid.h"
#include	"video.h"


/* [3.1.2 DECLARATIONS]                                                 */
#if defined(EGA_DUMP) || defined(EGA_STAT)
#line 62
#error Unsupported excluded EGA diagnostics configuration
#endif
#line 64

extern WRT_POINTERS mode0_gen_handlers, mode0_copy_handlers;
extern WRT_POINTERS mode1_handlers, mode2_handlers;

/* [3.2 INTERMODULE EXPORTS]						*/ 

/*
5.MODULE INTERNALS   :   (not visible externally, global internally)]     

[5.1 LOCAL DECLARATIONS]						*/
#ifdef SEGMENTATION
#line 75
#error Excluded unsupported SEGMENTATION configuration





#endif
#line 82

#ifndef REAL_VGA

#ifdef V7VGA
IMPORT UTINY fg_bg_control;
GLOBAL UTINY Last_v7_fg_bg;			/* used by {ev}ga_mask_register_changed() */
#endif

#ifndef CPU_40_STYLE	/* EVID without introducing EVID define */
#line 91
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.











































#else	/* CPU_40_STYLE - EVID */
#line 302
WRT_POINTERS *mode_chain_handler_table[] = { 0 };
#ifdef C_VID

/* C_Evid glue */
extern void  write_byte_ev_glue IPT2(IU32, eaOff, IU8, eaVal);
extern void  write_word_ev_glue IPT2(IU32, eaOff, IU16, eaVal);
extern void  fill_byte_ev_glue IPT3(IU32, eaOff, IU8, eaVal, IU32, count);
/* DIVERGENCE MVDM-HOST-DIV-074: match the original glue body's IU16 word value. */
extern void  fill_word_ev_glue IPT3(IU32, eaOff, IU16, eaVal, IU32, count);
extern void  move_byte_fwd_ev_glue IPT4(IU32, eaOff, IHPE, fromOff, IU32, count, IBOOL, srcInRAM);
extern void  move_word_fwd_ev_glue IPT4(IU32, eaOff, IHPE, fromOff, IU32, count, IBOOL, srcInRAM);
/* DIVERGENCE MVDM-HOST-DIV-088: use the original C-video selector body's
 * exact no-argument contract rather than an implicit host-int call. */
extern void setWritePointers IPT0();
MEM_HANDLERS Glue_writes =
{
	write_byte_ev_glue,
	write_word_ev_glue,
	fill_byte_ev_glue,
	fill_word_ev_glue,
	move_byte_fwd_ev_glue,
	move_word_fwd_ev_glue,
};
#else	/* C_VID */
#line 322
#error DIVERGENCE MVDM-HOST-DIV-539: non-C_VID branch is not selected; see mirror register.
#endif	/* CVID */
#line 325
WRT_POINTERS dth_md0_writes;
WRT_POINTERS dth_md2_writes;
WRT_POINTERS simple_writes;
WRT_POINTERS dth_md3_writes;
#endif	/* CPU_40_STYLE - EVID */
#line 330

/* DIVERGENCE(MVDM-HOST-DIV-136): preserve the original writer table order,
 * with the exact original provider parameter contracts. */
IMPORT VOID ega_copy_b_write IPT2(ULONG, value, ULONG, offset);
IMPORT VOID ega_mode0_chn_b_write IPT2(ULONG, value, ULONG, offset);
IMPORT VOID ega_mode1_chn_b_write IPT2(ULONG, value, ULONG, offset);
IMPORT VOID ega_mode2_chn_b_write IPT2(ULONG, value, ULONG, offset);

IMPORT VOID ega_copy_w_write IPT2(ULONG, value, ULONG, offset);
IMPORT VOID ega_mode0_chn_w_write IPT2(ULONG, value, ULONG, offset);
IMPORT VOID ega_mode1_chn_w_write IPT2(ULONG, value, ULONG, offset);
IMPORT VOID ega_mode2_chn_w_write IPT2(ULONG, value, ULONG, offset);

/* Handy array to extract all 4 plane values in one go. */

ULONG sr_lookup[16] =
{
#ifdef LITTLEND
	0x00000000,0x000000ff,0x0000ff00,0x0000ffff,
	0x00ff0000,0x00ff00ff,0x00ffff00,0x00ffffff,
	0xff000000,0xff0000ff,0xff00ff00,0xff00ffff,
	0xffff0000,0xffff00ff,0xffffff00,0xffffffff
#endif
#line 147
#ifdef BIGEND
#line 352
#error DIVERGENCE MVDM-HOST-DIV-524: BIGEND platform branch is not selected; see mirror register.
#endif
#line 357
};

GLOBAL VOID
stub IFN0()
{
	/*
	 * For VGA write modes we don't do because they represent
	 * unlikely combinations of registers.
	 */
}

GLOBAL ULONG calc_data_xor;
GLOBAL ULONG calc_latch_xor;

#ifdef SEGMENTATION
#line 372
#error Excluded unsupported SEGMENTATION configuration





#endif
#line 379

#if !(defined(NTVDM) && defined(MONITOR))
GLOBAL VOID
Glue_set_vid_wrt_ptrs IFN1(WRT_POINTERS *, handler )
{

#line 385
#ifndef CPU_40_STYLE	/* EVID */
#line 386
#error Non-CPU40 video glue is not supported
#endif 	/* CPU_40_STYLE - EVID */
#line 443
}
#endif /* !(NTVDM && MONITOR) */
#line 213

#ifdef SEGMENTATION
#line 447
#error Excluded unsupported SEGMENTATION configuration





#endif
#line 454

/*  Initialize the write module */

VOID
ega_write_init IFN0()
{
	WRT_POINTERS *handler;

	note_entrance0("ega_write_init");

	EGA_CPU.saved_state = 0;
	EGA_CPU.write_mode = 0;
	EGA_CPU.chain = UNCHAINED;
	setVideochain(EGA_CPU.chain);
	setVideowrmode(EGA_CPU.write_mode);
	setVideowrstate(0);

	handler = &mode_chain_handler_table[0][0];

#ifdef CPU_40_STYLE
	/* ensure correct write mode in place for initial font writes */
	SetWritePointers();
#endif
#line 245

#ifdef	JOKER
#line 479
#error DIVERGENCE MVDM-HOST-DIV-531: JOKER branch is not selected; see mirror register.
#else	/* not JOKER */
#line 483

#if defined(EGA_DUMP) || defined(EGA_STAT)
#line 485
#error Unsupported excluded EGA diagnostics configuration
#else
#line 487
#ifdef EGATEST
#line 488
#error DIVERGENCE MVDM-HOST-DIV-529: EGATEST platform branch is not selected; see mirror register.
#else
#line 490
#ifndef GISP_CPU
#ifdef A3CPU
#line 492
#error DIVERGENCE MVDM-HOST-DIV-522: A3CPU platform branch is not selected; see mirror register.





#else
#line 499
	gmi_define_mem(VIDEO,&Glue_writes);
	Glue_set_vid_wrt_ptrs( handler );
#endif /* A3CPU */
#line 502
#endif /* GISP_CPU */
#line 264
#endif /* EGATEST */
#line 504
#endif /* EGA_DUMP || EGA_STAT */
#line 505
#endif /* JOKER */
#line 506

	ega_write_routines_update(WRITE_MODE);
	ega_write_routines_update(RAM_MOVED);
	ega_write_routines_update(RAM_ENABLED);
	ega_write_routines_update(SET_RESET);
	ega_write_routines_update(ENABLE_SET_RESET);
	ega_write_routines_update(FUNCTION);
}

VOID
ega_write_term IFN0()
{
	/*
	 * ensure that if you are an EGA and then change to a VGA (or vice
	 * versa) the write mode will be changed by the new adaptor. Otherwise
	 * this gives a 'drunken' font
	 */

	EGA_CPU.write_mode = 0;
	EGA_CPU.ega_state.mode_0.lookup =
				(EGA_CPU.ega_state.mode_0.lookup == 0) ? 1 : 0;
	setVideowrmode(EGA_CPU.write_mode);

	ega_write_routines_update(WRITE_MODE);
}


/* analyze the write state, and update the routines if necesary */

VOID
ega_write_routines_update IFN1(CHANGE_TYPE, reason )
{
	ULONG state;
	ULONG mode_and_chain;
	WRT_POINTERS *handler;
#ifndef PROD
#line 542
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 544

	note_entrance1("ega_write_routines_update(%d)",reason);

	switch( reason )
	{
		case FUNCTION:
			switch (write_state.func)
			{
				case 0:	/* Assign */
					setVideodata_and_mask(0xffffffff);
					setVideodata_xor_mask(~(getVideobit_prot_mask()));
					setVideolatch_xor_mask(getVideobit_prot_mask());
					EGA_CPU.calc_data_xor = 0xffffffff;
					EGA_CPU.calc_latch_xor = 0xffffffff;
					break;

				case 1:	/* AND */
					setVideodata_and_mask(0xffffffff);
					setVideodata_xor_mask(~(getVideobit_prot_mask()));
					setVideolatch_xor_mask(0);
					EGA_CPU.calc_data_xor = 0xffffffff;
					EGA_CPU.calc_latch_xor = 0x00000000;
					break;

				case 2:	/* OR */
					setVideodata_and_mask(0);
					setVideodata_xor_mask(0xffffffff);
					setVideolatch_xor_mask(
						getVideobit_prot_mask());
					EGA_CPU.calc_data_xor = 0x00000000;
					EGA_CPU.calc_latch_xor = 0xffffffff;
					break;

				case 3:	/* XOR */
					setVideodata_and_mask(0xffffffff);
					setVideodata_xor_mask(0xffffffff);
					setVideolatch_xor_mask(
						getVideobit_prot_mask());
					EGA_CPU.calc_data_xor = 0x00000000;
					EGA_CPU.calc_latch_xor = 0xffffffff;
					break;
			}

			setVideocalc_data_xor(EGA_CPU.calc_data_xor);
			setVideocalc_latch_xor(EGA_CPU.calc_latch_xor);
			break;

		case WRITE_MODE:
			/* write mode 3 has set/reset enabled for all planes
			 * so recalulate the mask ignoring the sr_enable register
			 * otherwise set the mask in case mode 3 last time.
			 */
			if( EGA_CPU.write_mode == 3) {
				setVideosr_nmask(0);
				setVideosr_masked_val(sr_lookup[EGA_CPU.set_reset]);
			} else {
				setVideosr_nmask(~sr_lookup[EGA_CPU.sr_enable]);
				setVideosr_masked_val(sr_lookup[EGA_CPU.set_reset & EGA_CPU.sr_enable]);
			}
			break;

		case SET_RESET:
			EGA_CPU.sr_value= sr_lookup[EGA_CPU.set_reset];
			if( EGA_CPU.write_mode == 3) {
				setVideosr_masked_val(sr_lookup[EGA_CPU.set_reset]);
			} else {
				setVideosr_masked_val(sr_lookup[EGA_CPU.set_reset & EGA_CPU.sr_enable]);
			}
			break;

		case ENABLE_SET_RESET:
			if( EGA_CPU.write_mode == 3) {
				setVideosr_nmask(0);
				setVideosr_masked_val(sr_lookup[EGA_CPU.set_reset]);
			} else {
				setVideosr_nmask(~sr_lookup[EGA_CPU.sr_enable]);
				setVideosr_masked_val(sr_lookup[EGA_CPU.set_reset & EGA_CPU.sr_enable]);
			}
			break;

		case PLANES_ENABLED:
			if (EGA_CPU.chain == CHAIN2)
			{
				if( getVideoplane_enable() & 0xc )
				{
					setVideorplane(EGA_plane23);
					setVideowplane(EGA_plane23);
				}
				else
				{
					setVideorplane(EGA_plane01);
					setVideowplane(EGA_plane01);
				}
			}
			break;

		case CHAINED:
			switch( EGA_CPU.chain )
			{
				case UNCHAINED:
				case CHAIN4:
					update_banking();
					break;

				case CHAIN2:
					if( getVideoplane_enable() & 0xc )
					{
						setVideorplane(EGA_plane23);
						setVideowplane(EGA_plane23);
					}
					else
					{
						setVideorplane(EGA_plane01);
						setVideowplane(EGA_plane01);
					}
					break;
			}

			break;

		case RAM_MOVED:
		case RAM_ENABLED:
		case BIT_PROT:
			/* No action required */
			break;
				
/*
 * Rotates are only partially supported in Avid and Cvid.
 * Mode 0 unchained byte writes are supported.  Word writes are also
 * supported in this case, as they use the byte write routines.
 *
 * Manage Your Money is the only application currently known to use rotates,
 * as of 22 Jan 1993.
 */
		case ROTATION:
			if (getVideorotate() > 0)
			{
#ifdef CPU_40_STYLE
				/* Write pointer change required but probably
				 * no state change otherwise.
				 */
				SetWritePointers();
#endif
#line 687
				always_trace3("Possible unsupported data rotate mode %d chain %d rotate by %d",
				               EGA_CPU.write_mode,
					       EGA_CPU.chain, getVideorotate());
			}
			break;
				
		default:
			assert0( NO, "Bad reason in ega_write_routines_update" );
			break;
	}

	/*
	 * Now select the right set of write routines according to the current state.
	 */

	switch( EGA_CPU.write_mode )
	{
		case 0:
			state = EGA_CPU.ega_state.mode_0.lookup;
			break;

		case 1:
			state = EGA_CPU.ega_state.mode_1.lookup;
			break;

		case 2:
			state = EGA_CPU.ega_state.mode_2.lookup;
			break;

#ifdef VGG
		case 3:
			state = EGA_CPU.ega_state.mode_3.lookup;
			break;
#endif /* VGG */
#line 482

		default:
			assert1( NO, "Bad write mode %d\n", EGA_CPU.write_mode );
			break;
	}

#ifdef VGG
	mode_and_chain = (EGA_CPU.chain << 2) + EGA_CPU.write_mode;
#else
#line 730
#error DIVERGENCE MVDM-HOST-DIV-540: non-VGG branch is not selected; see mirror register.
#endif /* VGG */
#line 732

	if(( EGA_CPU.saved_mode_chain != mode_and_chain ) 
		|| ( EGA_CPU.saved_state != state )
#ifdef V7VGA
		|| ( Last_v7_fg_bg != fg_bg_control)
#endif /* V7VGA */
#line 738
		)
	{
		setVideowrmode(EGA_CPU.write_mode);	/* reset for 'copy case' below */

		if( EGA_CPU.chain == CHAIN2 )
			switch (EGA_CPU.write_mode)
			{
				case 0:
					if( state == 0 )	/* basic text */
					{
						handler = &mode0_copy_handlers;
#ifdef CPU_40_STYLE
						setVideowrmode(4);	/* indicate 'copy case' */
#endif	/* CPU_40_STYLE */
#line 752
						bios_ch2_byte_wrt_fn = ega_copy_b_write;
						bios_ch2_word_wrt_fn = ega_copy_w_write;
					}
					else
					{
						handler = &mode0_gen_handlers;
						bios_ch2_byte_wrt_fn = ega_mode0_chn_b_write;
						bios_ch2_word_wrt_fn = ega_mode0_chn_w_write;
					}
					break;

				case 1:
					handler = &mode1_handlers;
					bios_ch2_byte_wrt_fn = ega_mode1_chn_b_write;
					bios_ch2_word_wrt_fn = ega_mode1_chn_w_write;
					break;

				case 2:
				case 3:	/* We don't support mode 3, chain 2 - JS */
					handler = &mode2_handlers;
					bios_ch2_byte_wrt_fn = ega_mode2_chn_b_write;
					bios_ch2_word_wrt_fn = ega_mode2_chn_w_write;
					break;
			}
		else
		{
#ifdef	V7VGA
			/*
			 *	Is it the V7VGA foreground dithering extension ?
			 */

			if( fg_bg_control & 0x8 )
			{
				setVideodither(1);	/* enable Evid dither fns */
				switch( EGA_CPU.write_mode )
				{
					case 0:
						handler = &dth_md0_writes;
						break;
					
					case 1:

						/*
						 *	No fg dither variant for write mode 1
						 */

						handler = &mode_chain_handler_table[mode_and_chain][state];
						break;
					
					case 2:
						handler = &dth_md2_writes;
						break;
					
					case 3:
						handler = &dth_md3_writes;
						break;
				}
			}
			else
#endif	/* V7VGA */
#line 812
				setVideodither(0);	/* disable Evid dither fns */

				handler = &mode_chain_handler_table[mode_and_chain][state];
		}

#ifdef CPU_40_STYLE
		SetWritePointers();
#else  /* CPU_40_STYLE */
#line 820
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#endif	/* CPU_40_STYLE */
#line 844

#ifndef PROD
#line 846
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 848

		set_mark_funcs();

		EGA_CPU.saved_state = state;
		EGA_CPU.saved_mode_chain = mode_and_chain;
#ifdef V7VGA
		Last_v7_fg_bg = fg_bg_control;
#endif
#line 595
	}
}
#endif /* REAL VGA */
#line 598
#endif /* EGG */
#line 599

#endif	/* !(NTVDM && MONITOR) */
#line 601
