#include "insignia.h"
#include "host_def.h"
/*
 * SoftPC Revision 3.0
 *
 * Title	: Main program 
 *
 * Description	: Call initialisation functions then call simulate to 
 *	 	  do the work.
 *
 * Author	: Rod Macgregor
 *
 * Notes	: The flag -v tells SoftPC to work silently unless
 *		  an error occurs.
 *
 */

/*
 * static char SccsID[]="@(#)main.c	1.49 06/23/95 Copyright Insignia Solutions Ltd.";
 */


/*
 * O/S includes
 */

#include <stdlib.h>
#include <stdio.h>
#include TypesH

/*
 * SoftPC includes
 */

#include "xt.h"
#include "sas.h"
#include CpuH
#include "error.h"
#include "config.h"
#include "gvi.h"
#include "host.h"
#include "trace.h"
#ifdef SECURE
#line 44
#error DIVERGENCE MVDM-HOST-DIV-530: SECURE platform branch is not selected; see mirror register.
#endif
#line 46
#include "gmi.h"
#include "gfx_upd.h"
#include "cmos.h"
#include "gfi.h"
/* DIVERGENCE(MVDM-HOST-DIV-162): selected CCPU40/C-VID sources retain their
 * original tables but not the historical generated pre-config binder. */
#include "mvdm_cvidc_vector_binding.h"
#line 50
#include "timer.h"
#include "yoda.h"
//#include "host_env.h"

/* DIVERGENCE(MVDM-HOST-DIV-083): CPU_40_STYLE selects the same original
 * host startup provider but hid its declaration, while the NT DOS-emulation
 * and mouse providers have no selected public declaration carrier. Publish
 * their exact existing contracts without changing startup order. */
#line 54
#ifndef CPU_40_STYLE
#line 55
#error Excluded unsupported non-CPU40 host startup profile
#else
#line 61
extern void host_start_cpu(void);
#endif
#line 57
extern int InitialiseDosEmulation(int argc, char **argv);
extern void mouse_driver_initialisation(void);

#line 58
void init_virtual_drivers IPT0();

IMPORT void host_set_yoda_ints IPT0();
IMPORT void host_applClose IPT0();
IMPORT void setup_vga_globals IPT0();
#ifdef ANSI
extern void host_applInit(int argc, char *argv[]);
#else
#line 66
#error Unsupported excluded original configuration
#endif	/* ANSI */
#line 68

#line 69
#ifdef REAL_VGA
#line 70
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#endif
#line 72

/* Have global variables defined here to indicate what product to run as. */

#ifdef SOFTWINDOWS
#line 76
#error DIVERGENCE MVDM-HOST-DIV-529: SOFTWINDOWS platform branch is not selected; see mirror register.
#else
GLOBAL IBOOL Running_SoftWindows = FALSE;
GLOBAL CHAR *SPC_Product_Name = "SoftPC";
#endif /* SOFTWINDOWS */
#line 82

GLOBAL char **pargv;	/* Pointer to argv		*/
GLOBAL int *pargc;	/* Pointer to argc		*/

#ifndef NTVDM
#line 87
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif /* NTVDM */
#line 91

/* Does this host need to have a different entry point ? */

#if defined(NTVDM) || defined(host_main)
INT host_main IFN2(INT, argc, CHAR **, argv)
#else   /* host_main */
#line 97
#error Unsupported excluded original configuration
#endif  /* host_main */
#line 99
{
  IMPORT ULONG setup_global_data_ptr();

#ifndef	CPU_40_STYLE
#line 103
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#endif	/* CPU_40_STYLE */
#line 105

#ifdef SECURE
#line 107
#error DIVERGENCE MVDM-HOST-DIV-530: SECURE platform branch is not selected; see mirror register.
#endif
#line 110

#ifdef	SETUID_ROOT
#line 112
#error DIVERGENCE MVDM-HOST-DIV-529: SETUID_ROOT platform branch is not selected; see mirror register.
#endif	/* SETUID_ROOT */
#line 117

#if !defined(PROD) || defined(HUNTER)
#line 119
#error Unsupported excluded original configuration
#endif /* !PROD || HUNTER */
#line 123

/***********************************************************************
 *								       *
 * Set up the global pointers to argc and argv for lower functions.    *
 * These must be saved as soon as possible as they are required for    *
 * displaying the error panel for the HP port.  Giving a null pointer  *
 * as the address of argc crashed the X Toolkit.		       *
 *								       *
 ***********************************************************************/

  pargc = &argc;
  pargv = argv;

#if !defined(NTVDM) && !defined(macintosh) && !defined(VMS)
#line 137
#error Unsupported excluded original configuration
#endif	/* NTVDM, macintosh, VMS */
#line 139

#line 140
#ifndef PROD
#line 141
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif /* !PROD */
#line 143

#ifdef SECURE
#line 145
#error DIVERGENCE MVDM-HOST-DIV-530: SECURE platform branch is not selected; see mirror register.
#endif
#line 147

#ifndef NTVDM
#line 149
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif /* NTVDM */
#line 151
  host_applInit(argc,argv);	/* recommended home is host/xxxx_reset.c */

#ifdef SECURE
#line 154
#error Unsupported excluded original configuration
#endif
#line 167
#if defined(CPU_40_STYLE) && !defined(CCPU)
#line 168
#error DIVERGENCE MVDM-HOST-DIV-515: compound branch is not selected for this source; see mirror register.
#endif /* assembler 4.0 cpu */
#line 174

  verbose = FALSE;

#ifndef PROD
#line 178
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 180

  /*
   * Pre-Config Base code initilisation.
   *
   * Setup the initial gfi funtion pointers before going into config
   */
  gfi_init();

  /* DIVERGENCE(MVDM-HOST-DIV-162): config() can initialise EGA before the
   * original later setup_vga_globals() hook. Bind the selected original
   * C-VID vector table before that first controller call. */
  mvdm_cvidc_bind_video_vector();

  /*
   * Initialise any Windows 3.x compliant DOS Drivers.
   * We do it here as config (and who knows who else) believe they can
   * access certain driver data at any time. Logically it ought to be done
   * when the driver is loaded under DOS, however historically it used to
   * be done as Static Data initialisation, so we mirror this old method
   * as closely as possible.
   *
   * The mac doesn't use the base config system and so calls init_virtual_drivers()
   * as part of it's host_applInit().
   */

#ifndef	macintosh
#line 201
  init_virtual_drivers();
#endif

/*
 * Find our configuration
 *------------------------*/

  config();

#line 210
#if defined(PROFILE) && !defined(CPU_40_STYLE)
#line 211
#error Excluded unsupported non-CPU40 profiling startup
#endif	/*PROFILE*/
#line 217

#if defined(NTVDM) || defined(macintosh)
/* Read the cmos from file to emulate data not being
 * lost between invocations of SoftPC
 *-----------------------------------------------------*/

  cmos_pickup();
#endif	/* defined(NTVDM) || defined(macintosh) */

#if !defined(PROD) || defined(HUNTER)
#line 227
#error Unsupported excluded original configuration
#endif
#line 241

/*
 * initialise the cpu
 *----------------------*/

  cpu_init();



#line 248
#ifndef PROD
#line 249
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif /* PROD */
#line 263

#line 264
#ifdef GISP_SVGA
#line 265
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#endif /* GISP_SVGA */
#line 269

	/*
	 * Set up the VGA globals before host_init_screen() in
	 * case of graphics activity.
	 *-------------------------------------------------------*/

#ifndef A3CPU
	(VOID) setup_global_data_ptr();
#endif	/* not A3CPU */

#ifndef GISP_SVGA
	setup_vga_globals();
#else /* GISP_SVGA */
#line 282
#error DIVERGENCE MVDM-HOST-DIV-523: GISP_SVGA alternative is not selected; see mirror register.
#endif /* GISP_SVGA */
#line 284

#ifdef REAL_VGA
#line 286
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#endif /* REAL_VGA */
#line 289

  host_init_screen();

#ifdef REAL_VGA
#line 293
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#endif /* REAL_VGA */
#line 295

#line 296
#ifdef IPC
#line 297
#error Unsupported excluded original configuration
#endif
#line 299

#ifdef NTVDM
/*
 * If you've got Dos Emulation - flaunt it!!
 * Initialise VDDs, Read in the Dos ntio.sys file and arrange for the cpu
 * to start execution at it's initialisation entry point.
 */
    InitialiseDosEmulation(argc, argv);
#endif	/* NTVDM */

/*
 * simulate the Intel 8088/iAPX286 cpu
 *-------------------------------------*/
/*
	Start off the cpu emulation. This will either be software
	emulation of protected mode 286/287 or possibly hardware
	eg 486 on Sparc platform
*/

  host_start_cpu();
  host_applClose();    /* recommended home is host/xxxx_reset.c */

/*
 * We should never get here so return an error status.
 */

  return(-1);

}


/**/


GLOBAL void init_virtual_drivers IFN0()
{
#line 335
#ifdef HFX
#line 336
#error Unsupported excluded original configuration
#endif
#line 338
	mouse_driver_initialisation();
}
