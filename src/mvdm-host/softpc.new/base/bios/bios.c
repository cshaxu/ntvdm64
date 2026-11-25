#include "insignia.h"
#include "host_def.h"
/*
 * VPC-XT Revision 1.0
 *
 * Title	: bios
 *
 * Description	: Vector of BOP calls which map to appropriate BIOS functions
 *
 * Author	: Rod MacGregor
 *
 * Notes	: hard disk int (0D) and command_check (B0) added DAR
 *
 * Mods: (r2.14): Replaced the entry against BOP 18 (not_supported())
 *                with the dummy routine rom_basic().
 *
 */

#ifdef SCCSID
#line 20
#error DIVERGENCE MVDM-HOST-DIV-518: SCCSID identification strings are not selected; see mirror register.
#endif
#line 22

#ifdef SEGMENTATION
#line 24
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
#include "bios.h"
#include CpuH
#include "host.h"
#include "cntlbop.h"

#include "host_hfx.h"
#include "hfx.h"
#include "virtual.h"
#include "gispsvga.h"
#ifdef SWIN_HFX
#line 53
#error DIVERGENCE MVDM-HOST-DIV-528: SWIN_HFX platform branch is not selected; see mirror register.
#endif
#line 55


#ifdef RDCHK
extern void get_lar IPT0();
#endif /* RDCHK */

#ifdef CPU_40_STYLE
#define SWAP_INSTANCE virtual_swap_instance()
#else
#line 64
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#endif
#line 66

/*
   Traps for BOP's requiring virtualisation.
 */
#ifdef SWIN_HFX
#line 71
#error DIVERGENCE MVDM-HOST-DIV-528: SWIN_HFX platform branch is not selected; see mirror register.
#endif
#line 73
LOCAL void v_mouse_install1	IFN0() { SWAP_INSTANCE; mouse_install1(); }
LOCAL void v_mouse_install2	IFN0() { SWAP_INSTANCE; mouse_install2(); }
LOCAL void v_mouse_io_interrupt	IFN0() { SWAP_INSTANCE; mouse_io_interrupt(); }
LOCAL void v_mouse_io_language  IFN0() { SWAP_INSTANCE; mouse_io_language(); }
LOCAL void v_mouse_video_io	IFN0() { SWAP_INSTANCE; mouse_video_io(); }
LOCAL void v_mouse_int1		IFN0() { SWAP_INSTANCE; mouse_int1(); }
LOCAL void v_mouse_int2		IFN0() { SWAP_INSTANCE; mouse_int2(); }
#if defined(XWINDOW) || defined(NTVDM)
LOCAL void v_host_mouse_install1 IFN0() {SWAP_INSTANCE; host_mouse_install1(); }
LOCAL void v_host_mouse_install2 IFN0() {SWAP_INSTANCE; host_mouse_install2(); }
#endif /* XWINDOW || NTVDM */
#ifdef HFX
#line 85
#error Unsupported excluded original configuration
#endif	/* HFX */
#line 88
LOCAL void v_mouse_EM_callback	IFN0() { SWAP_INSTANCE; mouse_EM_callback(); }

#ifdef PROFILE
#line 91
#error DIVERGENCE MVDM-HOST-DIV-531: PROFILE branch is not selected; see mirror register.
#endif
#line 94

#ifdef SIGNAL_PROFILING
#line 96
#error DIVERGENCE MVDM-HOST-DIV-529: SIGNAL_PROFILING platform branch is not selected; see mirror register.
#endif
#line 100

#if !defined(PROD) && defined(CPU_40_STYLE)
#line 102
#error DIVERGENCE MVDM-HOST-DIV-515: compound branch is not selected for this source; see mirror register.
#endif
#line 104

#if defined(CPU_40_STYLE) && !defined(NTVDM)
#line 106
#error DIVERGENCE MVDM-HOST-DIV-541: unselected compound host branch; see mirror register.
#endif
#line 108

#ifndef PROD
#line 110
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 113

#ifdef NTVDM
void rtc_int(void);
#endif

#ifndef MAC_LIKE

void (*BIOS[])() = {
			reset,	 	/* BOP 00 */
			dummy_int,   	/* BOP 01 */
			unexpected_int,	/* BOP 02 */
#ifdef DOS_APP_LIC
#line 125
#error DIVERGENCE MVDM-HOST-DIV-531: DOS_APP_LIC branch is not selected; see mirror register.
#else
#line 127
			illegal_bop,   	/* BOP 03 */
#endif	/* DOS_APP_LIC */
#line 129
#ifdef WDCTRL_BOP
#line 130
#error Unsupported excluded WDCTRL_BOP configuration
#else   /* WDCTRL_BOP */
#line 132
			illegal_bop,	/* BOP 04 */
#endif	/* WDCTRL_BOP */
#line 134
#if !defined(PROD) && defined(CPU_40_STYLE)
#line 135
#error DIVERGENCE MVDM-HOST-DIV-515: compound branch is not selected for this source; see mirror register.
#else
#line 137
			illegal_bop,	/* BOP 05 */
#endif	/* !PROD && CPU_40_STYLE */
#if defined(NEC_98)
#error Unsupported NEC PC-98 BIOS configuration
#else  // !NEC_98
#line 164
			illegal_op_int,	/* BOP 06 */
			illegal_dvr_bop,/* BOP 07 */
#if defined(NTVDM) || defined(CPU_40_STYLE)
			illegal_bop,   	/* BOP 08 */
#else /* !(NTVDM || CPU_40_STYLE) */
#line 169
#error DIVERGENCE MVDM-HOST-DIV-541: unselected compound host branch; see mirror register.
#endif /* !(NTVDM || CPU_40_STYLE) */
#line 171
			keyboard_int,  	/* BOP 09 */
			illegal_bop,   	/* BOP 0A */
			illegal_bop,   	/* BOP 0B */
			illegal_bop,   	/* BOP 0C */
			/* disk interrupts on vector 76 .. no BOP required though!
			 * diskbios() uses re-entrant CPU to get disk interrupt
			 * and the disk ISR is pure Intel assembler (with no BOPs)
		 	 */
			illegal_bop,   	/* BOP 0D */
			diskette_int,  	/* BOP 0E */
			illegal_bop,   	/* BOP 0F */
			video_io,	/* BOP 10 */
			equipment, 	/* BOP 11 */
			memory_size, 	/* BOP 12 */
			disk_io, 	/* BOP 13 */
			rs232_io,       /* BOP 14 */
			cassette_io,    /* BOP 15 */
			keyboard_io,	/* BOP 16 */
			printer_io,	/* BOP 17 */
			rom_basic,	/* BOP 18 */
#ifdef NTVDM
/* NT port, kill vdm instance, instead of warmbooting */
			terminate,
#else
#line 195
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 197
			time_of_day,	/* BOP 1A */
			illegal_bop,	/* BOP 1B */
			illegal_bop,	/* BOP 1C */
#endif // !NEC_98
			kb_idle_poll,	/* BOP 1D */
			illegal_bop,	/* BOP 1E */
			illegal_bop,	/* BOP 1F */
#if  defined(RDCHK) && !defined(PROD)
#line 205
#error Unsupported excluded original BIOS configuration
#else
#line 207
			illegal_bop,	/* BOP 20 */
#endif
#line 209
			Get_build_id,	/* BOP 21 */
#ifdef WIN_VTD
#line 211
#error Unsupported excluded original BIOS configuration
#else
#line 213
			illegal_bop,	/* BOP 22 */
#endif /* WIN_VTD */
#line 215
			illegal_bop,	/* BOP 23 */
			illegal_bop,	/* BOP 24 */
#if defined(CPU_40_STYLE) && !defined(NTVDM)
#line 218
#error DIVERGENCE MVDM-HOST-DIV-541: unselected compound host branch; see mirror register.
#else
#line 220
			illegal_bop,	/* BOP 25 */
#endif /* CPU_40_STYLE */
#line 222
			illegal_bop,	/* BOP 26 */
			illegal_bop,	/* BOP 27 */
			illegal_bop,	/* BOP 28 */
			illegal_bop,	/* BOP 29 */
			illegal_bop,	/* BOP 2A */
#ifndef NTVDM
#line 228
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#else /* NTVDM */
#line 231
                        illegal_bop,    /* BOP 2B */
                        illegal_bop,    /* BOP 2C */
#endif /* NTVDM */
#line 234
			illegal_bop,	/* BOP 2D */
#ifdef HFX
#line 236
#error Unsupported excluded original HFX BIOS configuration
#else
#line 239
			illegal_bop,	/* BOP 2E */
			illegal_bop,	/* BOP 2F */
#endif
#line 242
#ifdef DPMI
#line 243
#error Unsupported excluded original DPMI BIOS configuration
#else
#line 248
			illegal_bop,	/* BOP 30 */
			illegal_bop,	/* BOP 31 */
			illegal_bop,	/* BOP 32 */
			illegal_bop,	/* BOP 33 */
#endif /* DPMI */
#line 253
#ifdef NOVELL
#line 254
#error DIVERGENCE MVDM-HOST-DIV-528: NOVELL platform branch is not selected; see mirror register.










#else	/* NOVELL */
#line 275
			illegal_bop,	/* BOP 34 */
			illegal_bop,	/* BOP 35 */
			illegal_bop,	/* BOP 36 */
			illegal_bop,	/* BOP 37 */
			illegal_bop,	/* BOP 38 */
			illegal_bop,	/* BOP 39 */
			illegal_bop,	/* BOP 3A */
			illegal_bop,	/* BOP 3B */
			illegal_bop,	/* BOP 3C */
			illegal_bop,	/* BOP 3D */
			illegal_bop,	/* BOP 3E */
			illegal_bop,	/* BOP 3F */
#endif	/* NOVELL */
#if defined(NEC_98)
#error Unsupported NEC PC-98 BIOS configuration
#else  // !NEC_98
#line 291
			diskette_io,	/* BOP 40 */
#endif // !NEC_98
			illegal_bop,	/* BOP 41 */
#ifdef EGG
			ega_video_io,	/* BOP 42 */
#else
#line 297
#error DIVERGENCE MVDM-HOST-DIV-540: non-EGG branch is not selected; see mirror register.
#endif
#if defined(NEC_98)
#error Unsupported NEC PC-98 BIOS configuration
#else  // !NEC_98
#line 302
#ifdef JAPAN
#error Unsupported Japanese DOS/V BIOS configuration
#elif defined(KOREA) // !JAPAN
#error Unsupported Korean DOS BIOS configuration
#else // !KOREA
#line 310
			illegal_bop,	/* BOP 43 */
#endif // !KOREA
#endif // !NEC_98
			illegal_bop,	/* BOP 44 */
			illegal_bop,	/* BOP 45 */
			illegal_bop,	/* BOP 46 */
			illegal_bop,	/* BOP 47 */
			illegal_bop,	/* BOP 48 */
#ifdef DPMI
#line 319
#error Unsupported excluded original DPMI BIOS configuration
#else
#line 323
			illegal_bop,	/* BOP 49 */
			illegal_bop,	/* BOP 4A */
			illegal_bop,	/* BOP 4B */
#endif /* DPMI */
#line 327
			illegal_bop,	/* BOP 4C */
			illegal_bop,	/* BOP 4D */
			illegal_bop,	/* BOP 4E */
			illegal_bop,	/* BOP 4F */
#line 331
#ifdef NTVDM
/*
   Note that this precludes SMEG and NT existing together which seems
   reasonable given the Unix & X dependencies of SMEG
*/
			MS_bop_0,	/* BOP 50 - MS reserved */
			MS_bop_1,	/* BOP 51 - MS reserved */
			MS_bop_2,	/* BOP 52 - MS reserved */
			MS_bop_3,	/* BOP 53 - MS reserved */
			MS_bop_4,	/* BOP 54 - MS reserved */
			MS_bop_5,	/* BOP 55 - MS reserved */
			MS_bop_6,	/* BOP 56 - MS reserved */
			MS_bop_7,	/* BOP 57 - MS reserved */
			MS_bop_8,	/* BOP 58 - MS reserved */
			MS_bop_9,	/* BOP 59 - MS reserved */
			MS_bop_A,	/* BOP 5A - MS reserved */
			MS_bop_B,	/* BOP 5B - MS reserved */
			MS_bop_C,	/* BOP 5C - MS reserved */
			MS_bop_D,	/* BOP 5D - MS reserved */
			MS_bop_E,	/* BOP 5E - MS reserved */
			MS_bop_F,	/* BOP 5F - MS reserved */
#else
#line 353
#error Non-NTVDM BIOS or EMS provider is not supported
#endif /* NTVDM */
#line 383
			softpc_version,	/* BOP 60 */
			illegal_bop,	/* BOP 61 */
			illegal_bop,	/* BOP 62 */
#ifdef PTY
#line 387
#error Unsupported excluded original PTY BIOS configuration
#else
#line 389
			illegal_bop,	/* BOP 63 */
#endif
#line 391
			illegal_bop,	/* BOP 64 */
#ifdef PC_CONFIG
#line 393
#error DIVERGENCE MVDM-HOST-DIV-531: PC_CONFIG branch is not selected; see mirror register.
#else
#line 395
			illegal_bop,	/* BOP 65 */
#endif
#line 397
#ifdef LIM
			emm_init,	/* BOP 66 */
			emm_io,		/* BOP 67 */
			return_from_call, /* BOP 68 */
#else			
#line 402
#error DIVERGENCE MVDM-HOST-DIV-540: non-LIM branch is not selected; see mirror register.
#endif			
#line 406
#ifdef SUSPEND
#line 407
#error Unsupported excluded original SUSPEND BIOS configuration
#else
#line 410
			illegal_bop,	/* BOP 69 */
			illegal_bop,	/* BOP 6A */
#endif
#line 413
#ifdef GEN_DRVR
#line 414
#error Unsupported excluded original GEN_DRVR BIOS configuration
#else
#line 416
			illegal_bop,	/* BOP 6B */
#endif
#line 418
#ifdef SUSPEND
#line 419
#error Unsupported excluded original SUSPEND BIOS configuration
#else
#line 421
			illegal_bop,	/* BOP 6C */
#endif
#line 423
			illegal_bop,	/* BOP 6D */
			illegal_bop,	/* BOP 6E */
#ifdef CDROM
#line 426
#error Unsupported excluded original CDROM BIOS configuration
#else
#line 428
			illegal_bop,	/* BOP 6F */
#endif
#line 430

#if defined(NEC_98)
#error Unsupported NEC PC-98 BIOS configuration
#else  // !NEC_98
#line 441
#ifdef NTVDM
                        rtc_int,        /* BOP 70 */
#else
#line 444
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 446
                        re_direct,      /* BOP 71 */
			D11_int,	/* BOP 72 */
			D11_int,	/* BOP 73 */
			D11_int,	/* BOP 74 */
			int_287,	/* BOP 75 */
			D11_int,	/* BOP 76 */
			D11_int,	/* BOP 77 */
#endif // !NEC_98
#ifndef NTVDM
#line 455
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#else /* NTVDM */
#line 458
			illegal_bop,	/* BOP 78 */
			illegal_bop,	/* BOP 79 */
#endif /* NTVDM */
#line 461
			illegal_bop,	/* BOP 7A */
			illegal_bop,	/* BOP 7B */
			illegal_bop,	/* BOP 7C */
			illegal_bop,	/* BOP 7D */
			illegal_bop,	/* BOP 7E */
			illegal_bop,	/* BOP 7F */
			illegal_bop,    /* BOP 80 */
			illegal_bop,    /* BOP 81 */
			illegal_bop,    /* BOP 82 */
			illegal_bop,    /* BOP 83 */
			illegal_bop,    /* BOP 84 */
			illegal_bop,    /* BOP 85 */
			illegal_bop,    /* BOP 86 */
			illegal_bop,    /* BOP 87 */
			illegal_bop,    /* BOP 88 */
			illegal_bop,	/* BOP 89 */
			illegal_bop,	/* BOP 8A */
			illegal_bop,	/* BOP 8B */
			illegal_bop,	/* BOP 8C */
			illegal_bop,	/* BOP 8D */
			illegal_bop,	/* BOP 8E */
			illegal_bop,	/* BOP 8F */

#ifdef NTVDM
/* No bootstrap on NT */
			illegal_bop,	 /* BOP 90 */
			illegal_bop,	 /* BOP 91 */
			illegal_bop,	 /* BOP 92 */
#else
#line 490
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 494

#ifdef SWINAPI
#line 496
#error DIVERGENCE MVDM-HOST-DIV-529: SWINAPI platform branch is not selected; see mirror register.
#else /* SWINAPI */
#line 500

			illegal_bop,	/* BOP 93 */
			illegal_bop,	/* BOP 94 */
			illegal_bop,	/* BOP 95 */
#endif /* SWINAPI */
#line 505

			illegal_bop,	/* BOP 96 */
			illegal_bop,	/* BOP 97 */
#ifdef MSWDVR
#line 509
#error Unsupported excluded original BIOS configuration
#else
#line 514
			illegal_bop,	/* BOP 98 */
			illegal_bop,	/* BOP 99 */
			illegal_bop,	/* BOP 9A */
			illegal_bop,	/* BOP 9B */
#endif
#line 519
#if	defined(SOFTWIN_API) || defined(SWIN_HFX)
#line 520
#error Unsupported excluded original SoftWindows BIOS configuration
#else
#line 523
			illegal_bop,	/* BOP 9C */
			illegal_bop,	/* BOP 9D */
#endif	/* SOFTWIN_API or SWIN_HFX */
#line 526
#if	defined(SOFTWIN_API)
#line 527
#error DIVERGENCE MVDM-HOST-DIV-528: SOFTWIN_API platform branch is not selected; see mirror register.
#else
#line 529
			illegal_bop,	/* BOP 9E */
#endif	/* SOFTWIN_API */
#line 531
#ifdef SWIN_HAW
#line 532
#error DIVERGENCE MVDM-HOST-DIV-528: SWIN_HAW platform branch is not selected; see mirror register.
#else
#line 534
			illegal_bop,	/* BOP 9F */
#endif /* SWIN_HAW */
#line 536

#ifdef	NOVELL_IPX
#line 538
#error Unsupported excluded original BIOS configuration
#else	/* NOVELL_IPX */
#line 543
			illegal_bop,	/* BOP A0 */
			illegal_bop,	/* BOP A1 */
			illegal_bop,	/* BOP A2 */
			illegal_bop,	/* BOP A3 */
#endif	/* NOVELL_IPX */
#line 548

#ifdef	NOVELL_TCPIP
#line 550
#error Unsupported excluded original BIOS configuration
#else	/* NOVELL_TCPIP */
#line 553

			illegal_bop,	/* BOP A4 */
			illegal_bop,	/* BOP A5 */

#endif	/* NOVELL_TCPIP */
#line 558

#ifdef WINSOCK
#line 560
#error Unsupported excluded original BIOS configuration
#else /* WINSOCK */
#line 563
                        illegal_bop,    /* BOP A6 */
                        illegal_bop,    /* BOP A7 */
#endif /* WINSOCK */ 
#line 566
			illegal_bop,	/* BOP A8 */ 
			illegal_bop,	/* BOP A9 */
#ifdef SWIN_HFX
#line 569
#error DIVERGENCE MVDM-HOST-DIV-528: SWIN_HFX platform branch is not selected; see mirror register.
#else
#line 573
			illegal_bop,	/* BOP AA */
			illegal_bop,	/* BOP AB */
			illegal_bop,	/* BOP AC */
#endif
#line 577

#ifdef	MSWDVR
#line 579
#error Unsupported excluded original BIOS configuration
#else
#line 584
			illegal_bop,	/* BOP AD */
			illegal_bop,	/* BOP AE */
			illegal_bop,	/* BOP AF */
			illegal_bop,	/* BOP B0 */
#endif
#line 589

#ifdef CPU_40_STYLE
			virtual_device_trap,	/* BOP B1 */
#else
#line 593
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#endif
#line 595
			illegal_bop,	/* BOP B2 */
			illegal_bop,	/* BOP B3 */
			illegal_bop,	/* BOP B4 */
			illegal_bop,	/* BOP B5 */
			illegal_bop,	/* BOP B6 */
			illegal_bop,	/* BOP B7 */
			v_mouse_install1,	/* BOP B8 */ /* mouse_install1 */
			v_mouse_install2,	/* BOP B9 */ /* mouse_install2 */
			v_mouse_int1,		/* BOP BA */ /* mouse_int1 */
			v_mouse_int2,		/* BOP BB */ /* mouse_int2 */
			v_mouse_io_language,	/* BOP BC */ /* mouse_io_language */
			v_mouse_io_interrupt,	/* BOP BD */ /* mouse_io_interrupt */
			v_mouse_video_io,      	/* BOP BE */ /* mouse_video_io */
			v_mouse_EM_callback,	/* BOP BF */ /* mouse_EM_callback */
			illegal_bop,	/* BOP C0 */
			illegal_bop,	/* BOP C1 */
			illegal_bop,	/* BOP C2 */
			illegal_bop,	/* BOP C3 */
			illegal_bop,	/* BOP C4 */
			illegal_bop,	/* BOP C5 */
			illegal_bop,	/* BOP C6 */
			illegal_bop,	/* BOP C7 */
#if defined(XWINDOW) || defined(NTVDM)
			v_host_mouse_install1,	/* BOP C8 */ /* host_mouse_install1 */
			v_host_mouse_install2,	/* BOP C9 */ /* host_mouse_install2 */
#else
#line 621
#error Unsupported excluded original host mouse configuration
#endif /* defined(XWINDOW) || defined(NTVDM) */
#line 629
			illegal_bop,	/* BOP CA */
			illegal_bop,	/* BOP CB */
			illegal_bop,	/* BOP CC */
			illegal_bop,	/* BOP CD */
			illegal_bop,	/* BOP CE */
			illegal_bop,	/* BOP CF */
#ifdef PROFILE
#line 636
#error DIVERGENCE MVDM-HOST-DIV-531: PROFILE branch is not selected; see mirror register.
#else
#line 639
			illegal_bop,	/* BOP D0 */
			illegal_bop,	/* BOP D1 */
#endif
#line 642
			illegal_bop,	/* BOP D2 */
			illegal_bop,	/* BOP D3 */
#ifdef SIGNAL_PROFILING
#line 645
#error DIVERGENCE MVDM-HOST-DIV-529: SIGNAL_PROFILING platform branch is not selected; see mirror register.
#else
#line 649
			illegal_bop,	/* BOP D4 */
			illegal_bop,	/* BOP D5 */
			illegal_bop,	/* BOP D6 */
#endif
#line 653
			illegal_bop,	/* BOP D7 */
			illegal_bop,	/* BOP D8 */
			illegal_bop,	/* BOP D9 */
			illegal_bop,	/* BOP DA */
			illegal_bop,	/* BOP DB */
			illegal_bop,	/* BOP DC */
			illegal_bop,	/* BOP DD */
			illegal_bop,	/* BOP DE */
			illegal_bop,	/* BOP DF */
			illegal_bop,	/* BOP E0 */
			illegal_bop,	/* BOP E1 */
			illegal_bop,	/* BOP E2 */
			illegal_bop,	/* BOP E3 */
			illegal_bop,	/* BOP E4 */
			illegal_bop,	/* BOP E5 */
			illegal_bop,	/* BOP E6 */
			illegal_bop,	/* BOP E7 */
			illegal_bop,	/* BOP E8 */
			illegal_bop,	/* BOP E9 */
			illegal_bop,	/* BOP EA */
			illegal_bop,	/* BOP EB */
			illegal_bop,	/* BOP EC */
			illegal_bop,	/* BOP ED */
			illegal_bop,	/* BOP EE */
			illegal_bop,	/* BOP EF */
			illegal_bop,	/* BOP F0 */
			illegal_bop,	/* BOP F1 */
			illegal_bop,	/* BOP F2 */
			illegal_bop,	/* BOP F3 */
			illegal_bop,	/* BOP F4 */
			illegal_bop,	/* BOP F5 */
			illegal_bop,	/* BOP F6 */
			illegal_bop,	/* BOP F7 */

#ifndef PROD
#line 688
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#else
#line 691
			illegal_bop,	/* BOP F8 */
			illegal_bop,	/* BOP F9 */
#endif
#line 694
#ifndef GISP_CPU
			illegal_bop,	/* BOP FA */
			illegal_bop,	/* BOP FB */
			illegal_bop,	/* BOP FC */
#if defined(NTVDM) && defined(MONITOR)
                        switch_to_real_mode,	/* BOP FD */
#else
#line 701
#error DIVERGENCE MVDM-HOST-DIV-541: unselected compound host branch; see mirror register.
#endif	/* NTVDM && MONITOR */
#line 703
#if !defined(LDBIOS) && !defined(CPU_30_STYLE)
                        host_unsimulate,/* BOP FE */
#else
#line 706
#error Excluded unsupported LDBIOS or CPU30 BIOS continuation




#endif	/* !LDBIOS && !CPU_30_STYLE */
#line 712

#else /* ndef GISP_CPU */
#line 714
#error DIVERGENCE MVDM-HOST-DIV-523: GISP_CPU alternative is not selected; see mirror register.
#endif	/* GISP_CPU */
#line 726

                        control_bop     /* BOP FF */
			/* Don't put anymore entries after FF because
			   we only have a byte quantity */
		};
#endif /* MAC_LIKE */



#ifndef PROD
#line 736
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif	/* PROD */
#line 944
