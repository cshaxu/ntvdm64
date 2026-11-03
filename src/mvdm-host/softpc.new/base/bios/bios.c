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
#error DIVERGENCE MVDM-HOST-DIV-518: SCCSID identification strings are not selected; see mirror register.
#endif

#ifdef SEGMENTATION
#error Excluded unsupported SEGMENTATION configuration





#endif


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
#error DIVERGENCE MVDM-HOST-DIV-528: SWIN_HFX platform branch is not selected; see mirror register.
#endif


#ifdef RDCHK
extern void get_lar();
#endif /* RDCHK */

#ifdef CPU_40_STYLE
#define SWAP_INSTANCE virtual_swap_instance()
#else
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#endif

/*
   Traps for BOP's requiring virtualisation.
 */
#ifdef SWIN_HFX
#error DIVERGENCE MVDM-HOST-DIV-528: SWIN_HFX platform branch is not selected; see mirror register.
#endif
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
#error Unsupported excluded original configuration
#endif	/* HFX */
#line 88
LOCAL void v_mouse_EM_callback	IFN0() { SWAP_INSTANCE; mouse_EM_callback(); }

#ifdef PROFILE
#error DIVERGENCE MVDM-HOST-DIV-531: PROFILE branch is not selected; see mirror register.
#endif

#ifdef SIGNAL_PROFILING
#error DIVERGENCE MVDM-HOST-DIV-529: SIGNAL_PROFILING platform branch is not selected; see mirror register.
#endif

#if !defined(PROD) && defined(CPU_40_STYLE)
#error DIVERGENCE MVDM-HOST-DIV-515: compound branch is not selected for this source; see mirror register.
#endif

#if defined(CPU_40_STYLE) && !defined(NTVDM)
#error DIVERGENCE MVDM-HOST-DIV-541: unselected compound host branch; see mirror register.
#endif

#ifndef PROD
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif

#ifdef NTVDM
void rtc_int(void);
#endif

#ifndef MAC_LIKE

void (*BIOS[])() = {
			reset,	 	/* BOP 00 */
			dummy_int,   	/* BOP 01 */
			unexpected_int,	/* BOP 02 */
#ifdef DOS_APP_LIC
#error DIVERGENCE MVDM-HOST-DIV-531: DOS_APP_LIC branch is not selected; see mirror register.
#else
			illegal_bop,   	/* BOP 03 */
#endif	/* DOS_APP_LIC */
#ifdef WDCTRL_BOP
			wdctrl_bop,   	/* BOP 04 */
#else   /* WDCTRL_BOP */
			illegal_bop,	/* BOP 04 */
#endif	/* WDCTRL_BOP */
#if !defined(PROD) && defined(CPU_40_STYLE)
#error DIVERGENCE MVDM-HOST-DIV-515: compound branch is not selected for this source; see mirror register.
#else
			illegal_bop,	/* BOP 05 */
#endif	/* !PROD && CPU_40_STYLE */
			illegal_op_int,	/* BOP 06 */
			illegal_dvr_bop,/* BOP 07 */
#if defined(NTVDM) || defined(CPU_40_STYLE)
			illegal_bop,   	/* BOP 08 */
#else /* !(NTVDM || CPU_40_STYLE) */
#error DIVERGENCE MVDM-HOST-DIV-541: unselected compound host branch; see mirror register.
#endif /* !(NTVDM || CPU_40_STYLE) */
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
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
			time_of_day,	/* BOP 1A */
			illegal_bop,	/* BOP 1B */
			illegal_bop,	/* BOP 1C */
			kb_idle_poll,	/* BOP 1D */
			illegal_bop,	/* BOP 1E */
			illegal_bop,	/* BOP 1F */
#if  defined(RDCHK) && !defined(PROD)
			get_lar,        /* BOP 20, used to debug read checks */
#else
			illegal_bop,	/* BOP 20 */
#endif
			Get_build_id,	/* BOP 21 */
#ifdef WIN_VTD
			VtdTickSync,	/* BOP 22 */
#else
			illegal_bop,	/* BOP 22 */
#endif /* WIN_VTD */
			illegal_bop,	/* BOP 23 */
			illegal_bop,	/* BOP 24 */
#if defined(CPU_40_STYLE) && !defined(NTVDM)
#error DIVERGENCE MVDM-HOST-DIV-541: unselected compound host branch; see mirror register.
#else
			illegal_bop,	/* BOP 25 */
#endif /* CPU_40_STYLE */
			illegal_bop,	/* BOP 26 */
			illegal_bop,	/* BOP 27 */
			illegal_bop,	/* BOP 28 */
			illegal_bop,	/* BOP 29 */
			illegal_bop,	/* BOP 2A */
#ifndef NTVDM
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#else /* NTVDM */
                        illegal_bop,    /* BOP 2B */
                        illegal_bop,    /* BOP 2C */
#endif /* NTVDM */
			illegal_bop,	/* BOP 2D */
#ifdef HFX
			v_test_for_us,	/* BOP 2E */ /* test_for_us */
			v_redirector,	/* BOP 2F */ /* redirector */
#else
			illegal_bop,	/* BOP 2E */
			illegal_bop,	/* BOP 2F */
#endif
#ifdef DPMI
			DPMI_2F,	/* BOP 30 */
			DPMI_31,	/* BOP 31 */
			DPMI_general,	/* BOP 32 */
			DPMI_int,	/* BOP 33 */
#else
			illegal_bop,	/* BOP 30 */
			illegal_bop,	/* BOP 31 */
			illegal_bop,	/* BOP 32 */
			illegal_bop,	/* BOP 33 */
#endif /* DPMI */
#ifdef NOVELL
#error DIVERGENCE MVDM-HOST-DIV-528: NOVELL platform branch is not selected; see mirror register.










#else	/* NOVELL */
#line 225
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
#line 238
			diskette_io,	/* BOP 40 */
			illegal_bop,	/* BOP 41 */
#ifdef EGG
			ega_video_io,	/* BOP 42 */
#else
#line 243
#error DIVERGENCE MVDM-HOST-DIV-540: non-EGG branch is not selected; see mirror register.
#endif
#line 245
			illegal_bop,	/* BOP 43 */
			illegal_bop,	/* BOP 44 */
			illegal_bop,	/* BOP 45 */
			illegal_bop,	/* BOP 46 */
			illegal_bop,	/* BOP 47 */
			illegal_bop,	/* BOP 48 */
#ifdef DPMI
			DPMI_r0_int,	/* BOP 49 */
			DPMI_exc,	/* BOP 4A */
			DPMI_4B,	/* BOP 4B */
#else
#line 256
			illegal_bop,	/* BOP 49 */
			illegal_bop,	/* BOP 4A */
			illegal_bop,	/* BOP 4B */
#endif /* DPMI */
#line 260
			illegal_bop,	/* BOP 4C */
			illegal_bop,	/* BOP 4D */
			illegal_bop,	/* BOP 4E */
			illegal_bop,	/* BOP 4F */
#line 288
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
#line 310
#error Non-NTVDM BIOS or EMS provider is not supported
#endif /* NTVDM */
#line 340
			softpc_version,	/* BOP 60 */
			illegal_bop,	/* BOP 61 */
			illegal_bop,	/* BOP 62 */
#ifdef PTY
			com_bop_pty,	/* BOP 63 */
#else
#line 322
			illegal_bop,	/* BOP 63 */
#endif
#line 324
			illegal_bop,	/* BOP 64 */
#ifdef PC_CONFIG
#error DIVERGENCE MVDM-HOST-DIV-531: PC_CONFIG branch is not selected; see mirror register.
#else
#line 328
			illegal_bop,	/* BOP 65 */
#endif
#line 330
#ifdef LIM
			emm_init,	/* BOP 66 */
			emm_io,		/* BOP 67 */
			return_from_call, /* BOP 68 */
#else			
#line 335
#error DIVERGENCE MVDM-HOST-DIV-540: non-LIM branch is not selected; see mirror register.
#endif			
#line 337
#ifdef SUSPEND
			suspend_softpc,	/* BOP 69 */
			terminate,	/* BOP 6A */
#else
#line 341
			illegal_bop,	/* BOP 69 */
			illegal_bop,	/* BOP 6A */
#endif
#line 344
#ifdef GEN_DRVR
			gen_driver_io,	/* BOP 6B */
#else
#line 347
			illegal_bop,	/* BOP 6B */
#endif
#line 349
#ifdef SUSPEND
			send_script,	/* BOP 6C */
#else
#line 352
			illegal_bop,	/* BOP 6C */
#endif
#line 354
			illegal_bop,	/* BOP 6D */
			illegal_bop,	/* BOP 6E */
#ifdef CDROM
			bcdrom_io,	/* BOP 6F */
#else
#line 359
			illegal_bop,	/* BOP 6F */
#endif
#line 361

#ifdef NTVDM
                        rtc_int,        /* BOP 70 */
#else
#line 365
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 367
                        re_direct,      /* BOP 71 */
			D11_int,	/* BOP 72 */
			D11_int,	/* BOP 73 */
			D11_int,	/* BOP 74 */
			int_287,	/* BOP 75 */
			D11_int,	/* BOP 76 */
			D11_int,	/* BOP 77 */
#ifndef NTVDM
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#else /* NTVDM */
#line 377
			illegal_bop,	/* BOP 78 */
			illegal_bop,	/* BOP 79 */
#endif /* NTVDM */
#line 380
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
#line 409
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif
#line 411

#ifdef SWINAPI
#error DIVERGENCE MVDM-HOST-DIV-529: SWINAPI platform branch is not selected; see mirror register.
#else /* SWINAPI */
#line 415

			illegal_bop,	/* BOP 93 */
			illegal_bop,	/* BOP 94 */
			illegal_bop,	/* BOP 95 */
#endif /* SWINAPI */
#line 420

			illegal_bop,	/* BOP 96 */
			illegal_bop,	/* BOP 97 */
#ifdef MSWDVR
			ms_windows,		/* BOP 98 */
			msw_mouse,	      /* BOP 99 */
			msw_copy,		/* BOP 9A */
			msw_keybd,		/* BOP 9B */
#else
#line 429
			illegal_bop,	/* BOP 98 */
			illegal_bop,	/* BOP 99 */
			illegal_bop,	/* BOP 9A */
			illegal_bop,	/* BOP 9B */
#endif
#line 434
#if	defined(SOFTWIN_API) || defined(SWIN_HFX)
			SoftWindowsInit,	/* BOP 9C */
			SoftWindowsTerm,	/* BOP 9D */
#else
#line 438
			illegal_bop,	/* BOP 9C */
			illegal_bop,	/* BOP 9D */
#endif	/* SOFTWIN_API or SWIN_HFX */
#line 441
#if	defined(SOFTWIN_API)
#error DIVERGENCE MVDM-HOST-DIV-528: SOFTWIN_API platform branch is not selected; see mirror register.
#else
#line 444
			illegal_bop,	/* BOP 9E */
#endif	/* SOFTWIN_API */
#line 446
#ifdef SWIN_HAW
#error DIVERGENCE MVDM-HOST-DIV-528: SWIN_HAW platform branch is not selected; see mirror register.
#else
#line 449
			illegal_bop,	/* BOP 9F */
#endif /* SWIN_HAW */
#line 451

#ifdef	NOVELL_IPX
			IPXResInit,	/* BOP A0 */
			IPXResEntry,	/* BOP A1 */
			IPXResInterrupt,/* BOP A2 */
			illegal_bop,	/* BOP A3 */
#else	/* NOVELL_IPX */
#line 458
			illegal_bop,	/* BOP A0 */
			illegal_bop,	/* BOP A1 */
			illegal_bop,	/* BOP A2 */
			illegal_bop,	/* BOP A3 */
#endif	/* NOVELL_IPX */
#line 463

#ifdef	NOVELL_TCPIP
			TCPResInit,	/* BOP A4 */
			TCPResEntry,	/* BOP A5 */
#else	/* NOVELL_TCPIP */
#line 468

			illegal_bop,	/* BOP A4 */
			illegal_bop,	/* BOP A5 */

#endif	/* NOVELL_TCPIP */
#line 473

#ifdef WINSOCK
                        ISWSEntry,      /* BOP A6 */
                        illegal_bop,    /* BOP A7 */
#else /* WINSOCK */
#line 478
                        illegal_bop,    /* BOP A6 */
                        illegal_bop,    /* BOP A7 */
#endif /* WINSOCK */ 
#line 481
			illegal_bop,	/* BOP A8 */ 
			illegal_bop,	/* BOP A9 */
#ifdef SWIN_HFX
#error DIVERGENCE MVDM-HOST-DIV-528: SWIN_HFX platform branch is not selected; see mirror register.
#else
#line 486
			illegal_bop,	/* BOP AA */
			illegal_bop,	/* BOP AB */
			illegal_bop,	/* BOP AC */
#endif
#line 490

#ifdef	MSWDVR
			msw_copyInit,	/* BOP AD */
			illegal_bop,	/* BOP AE */
			illegal_bop,	/* BOP AF */
			illegal_bop,	/* BOP B0 */
#else
#line 497
			illegal_bop,	/* BOP AD */
			illegal_bop,	/* BOP AE */
			illegal_bop,	/* BOP AF */
			illegal_bop,	/* BOP B0 */
#endif
#line 502

#ifdef CPU_40_STYLE
			virtual_device_trap,	/* BOP B1 */
#else
#line 506
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#endif
#line 508
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
#line 534
#ifdef GISP_SVGA 
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#else /* GISP_SVGA */
			illegal_bop,	/* BOP C8 */
			illegal_bop,	/* BOP C9 */
#endif /* GISP_SVGA */
#endif /* defined(XWINDOW) || defined(NTVDM) */
#line 541
			illegal_bop,	/* BOP CA */
			illegal_bop,	/* BOP CB */
			illegal_bop,	/* BOP CC */
			illegal_bop,	/* BOP CD */
			illegal_bop,	/* BOP CE */
			illegal_bop,	/* BOP CF */
#ifdef PROFILE
#error DIVERGENCE MVDM-HOST-DIV-531: PROFILE branch is not selected; see mirror register.
#else
#line 550
			illegal_bop,	/* BOP D0 */
			illegal_bop,	/* BOP D1 */
#endif
#line 553
			illegal_bop,	/* BOP D2 */
			illegal_bop,	/* BOP D3 */
#ifdef SIGNAL_PROFILING
#error DIVERGENCE MVDM-HOST-DIV-529: SIGNAL_PROFILING platform branch is not selected; see mirror register.
#else
#line 558
			illegal_bop,	/* BOP D4 */
			illegal_bop,	/* BOP D5 */
			illegal_bop,	/* BOP D6 */
#endif
#line 562
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
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#else
#line 599
			illegal_bop,	/* BOP F8 */
			illegal_bop,	/* BOP F9 */
#endif
#line 602
#ifndef GISP_CPU
			illegal_bop,	/* BOP FA */
			illegal_bop,	/* BOP FB */
			illegal_bop,	/* BOP FC */
#if defined(NTVDM) && defined(MONITOR)
                        switch_to_real_mode,	/* BOP FD */
#else
#error DIVERGENCE MVDM-HOST-DIV-541: unselected compound host branch; see mirror register.
#endif	/* NTVDM && MONITOR */
#if !defined(LDBIOS) && !defined(CPU_30_STYLE)
                        host_unsimulate,/* BOP FE */
#else
#error Excluded unsupported LDBIOS or CPU30 BIOS continuation




#endif	/* !LDBIOS && !CPU_30_STYLE */

#else /* ndef GISP_CPU */
#line 622
#error DIVERGENCE MVDM-HOST-DIV-523: GISP_CPU alternative is not selected; see mirror register.
#endif	/* GISP_CPU */
#line 624

                        control_bop     /* BOP FF */
			/* Don't put anymore entries after FF because
			   we only have a byte quantity */
		};
#endif /* MAC_LIKE */
#line 630



#ifndef PROD
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif	/* PROD */
#line 636
