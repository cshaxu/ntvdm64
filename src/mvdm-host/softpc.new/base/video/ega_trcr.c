#include "insignia.h"
#include "host_def.h"

#if !defined(NTVDM) || (defined(NTVDM) && !defined(X86GFX) )

/*
 * SccsID @(#)ega_tracer.c	1.7 8/25/93 Copyright Insignia Solutions
 *    Set of routines to output all accesses to the EGA, in a form that
 *    can be read in & executed by the stand-alone EGA.
 *
 */


#ifdef EGA_DUMP
#error DIVERGENCE MVDM-HOST-DIV-527: EGA_DUMP branch is not selected; see mirror register.
#endif

#endif	/* !NTVDM | (NTVDM & !X86GFX) */
