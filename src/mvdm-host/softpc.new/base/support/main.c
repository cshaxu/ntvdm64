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
#error DIVERGENCE MVDM-HOST-DIV-530: SECURE platform branch is not selected; see mirror register.
#endif
#line 46
#include "gmi.h"
#include "gfx_upd.h"
#include "cmos.h"
#include "gfi.h"
#include "timer.h"
#include "yoda.h"
//#include "host_env.h"

extern	void	host_start_cpu();	/* Start up the Intel emulation */

GLOBAL VOID    mouse_driver_initialisation(void);
GLOBAL void    InitialiseDosEmulation(int, char **);
GLOBAL ULONG   setup_global_data_ptr();

void init_virtual_drivers IPT0();

IMPORT void host_set_yoda_ints IPT0();
IMPORT void host_applClose IPT0();
#if defined(NEC_98)
#error Unsupported NEC PC-98 configuration
#else    //NEC_98
#line 67
IMPORT void setup_vga_globals IPT0();
#endif   //NEC_98
#ifdef ANSI
extern void host_applInit(int argc, char *argv[]);
#else
#error Unsupported excluded original configuration
#endif	/* ANSI */
#line 74

#ifdef REAL_VGA
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#endif
#line 78

/* Have global variables defined here to indicate what product to run as. */

#ifdef SOFTWINDOWS
#error DIVERGENCE MVDM-HOST-DIV-529: SOFTWINDOWS platform branch is not selected; see mirror register.
#else
#line 85
GLOBAL IBOOL Running_SoftWindows = FALSE;
GLOBAL CHAR *SPC_Product_Name = "SoftPC";
#endif /* SOFTWINDOWS */

GLOBAL char **pargv;	/* Pointer to argv		*/
GLOBAL int *pargc;	/* Pointer to argc		*/

#ifndef NTVDM
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif /* NTVDM */
#line 97

/* Does this host need to have a different entry point ? */

#if defined(NTVDM) || defined(host_main)
INT host_main IFN2(INT *, argc, CHAR **, argv)
#else   /* host_main */
#error Unsupported excluded original configuration
#endif  /* host_main */
#line 105
{

#ifndef	CPU_40_STYLE
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#endif	/* CPU_40_STYLE */
#line 110

#ifdef SECURE
#error DIVERGENCE MVDM-HOST-DIV-530: SECURE platform branch is not selected; see mirror register.
#endif
#line 115

#ifdef	SETUID_ROOT
#error DIVERGENCE MVDM-HOST-DIV-529: SETUID_ROOT platform branch is not selected; see mirror register.
#endif	/* SETUID_ROOT */
#line 122

#if !defined(PROD) || defined(HUNTER)
#error Unsupported excluded original configuration
#endif /* !PROD || HUNTER */
#line 128

/***********************************************************************
 *								       *
 * Set up the global pointers to argc and argv for lower functions.    *
 * These must be saved as soon as possible as they are required for    *
 * displaying the error panel for the HP port.  Giving a null pointer  *
 * as the address of argc crashed the X Toolkit.		       *
 *								       *
 ***********************************************************************/

  pargc = argc;
  pargv = argv;

#if !defined(NTVDM) && !defined(macintosh) && !defined(VMS)
#error Unsupported excluded original configuration
#endif	/* NTVDM, macintosh, VMS */
#line 144

#ifndef PROD
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif /* !PROD */
#line 148

#ifdef SECURE
#error DIVERGENCE MVDM-HOST-DIV-530: SECURE platform branch is not selected; see mirror register.
#endif
#line 152

#ifndef NTVDM
#error DIVERGENCE MVDM-HOST-DIV-537: non-NTVDM branch is not selected; see mirror register.
#endif /* NTVDM */
#line 156
  host_applInit(*argc,argv);	/* recommended home is host/xxxx_reset.c */

#ifdef SECURE
#error Unsupported excluded original configuration
#endif
#line 172
#if defined(CPU_40_STYLE) && !defined(CCPU)
#error DIVERGENCE MVDM-HOST-DIV-515: compound branch is not selected for this source; see mirror register.
#endif /* assembler 4.0 cpu */
#line 179

  verbose = FALSE;

#ifndef PROD
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif
#line 185

  /*
   * Pre-Config Base code initilisation.
   *
   * Setup the initial gfi funtion pointers before going into config
   */
#ifndef NEC_98
  gfi_init();
#endif   //NEC_98

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
  init_virtual_drivers();
#endif

/*
 * Find our configuration
 *------------------------*/

  config();

#if defined(PROFILE) && !defined(CPU_40_STYLE)
#error Excluded unsupported non-CPU40 profiling startup
#endif	/*PROFILE*/
#line 224

#ifndef NEC_98
#if defined(NTVDM) || defined(macintosh)
/* Read the cmos from file to emulate data not being
 * lost between invocations of SoftPC
 *-----------------------------------------------------*/

  cmos_pickup();
#endif	/* defined(NTVDM) || defined(macintosh) */

#if !defined(PROD) || defined(HUNTER)
#error Unsupported excluded original configuration
#endif
#line 249
#endif   //NEC_98

/*
 * initialise the cpu
 *----------------------*/

  cpu_init();

#ifndef PROD
#error DIVERGENCE MVDM-HOST-DIV-500: non-PROD debug branch is not selected; see mirror register.
#endif /* PROD */
#line 272

#ifdef GISP_SVGA
#error DIVERGENCE MVDM-HOST-DIV-522: GISP_SVGA platform branch is not selected; see mirror register.
#endif /* GISP_SVGA */
#line 278

	/*
	 * Set up the VGA globals before host_init_screen() in
	 * case of graphics activity.
	 *-------------------------------------------------------*/

#ifndef A3CPU
	(VOID) setup_global_data_ptr();
#endif	/* not A3CPU */

#if defined(NEC_98)
#error Unsupported NEC PC-98 configuration
#else    //NEC_98
#line 291
#ifndef GISP_SVGA
	setup_vga_globals();
#else /* GISP_SVGA */
#error DIVERGENCE MVDM-HOST-DIV-523: GISP_SVGA alternative is not selected; see mirror register.
#endif /* GISP_SVGA */
#line 296
#endif   //NEC_98

#if defined(NEC_98)
#error Unsupported NEC PC-98 configuration
#else   //NEC_98
#line 301
#ifdef REAL_VGA
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#endif /* REAL_VGA */
#line 305

  host_init_screen();

#ifdef REAL_VGA
#error DIVERGENCE MVDM-HOST-DIV-525: REAL_VGA platform branch is not selected; see mirror register.
#endif /* REAL_VGA */
#line 311
#endif   //NEC_98

#ifdef IPC
#error Unsupported excluded original configuration
#endif
#line 316

#ifdef NTVDM
/*
 * If you've got Dos Emulation - flaunt it!!
 * Initialise VDDs, Read in the Dos ntio.sys file and arrange for the cpu
 * to start execution at it's initialisation entry point.
 */
    InitialiseDosEmulation(*argc, argv);
#endif	/* NTVDM */

  return 0;
}

INT softpc_run_initialized(void)
{
#line 326
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

GLOBAL void init_virtual_drivers (void)
{
#ifdef HFX
#error Unsupported excluded original configuration
#endif
#line 354
	mouse_driver_initialisation();
}
