/*[
 *
 *	File		:	hwvga.h
 *
 *	Derived from:	(original)
 *
 *	Purpose		:	interface definition for hardware vga routines
 *
 *	Author		:	Rog
 *	Date		:	23 Feb 1993
 *
 *    SCCS Garbage	:	@(#)hwvga.h	1.5 11/22/93
 *	
 *	(c) Copyright Insignia Solutions Ltd., 1992 All rights reserved
 *
 *	Modifications	:
 *				Prototypes that don't depend on types
 *				defined in HostHwVgaH should go in gispsvga.h
 *				not here.  This file does contain some 
 *				prototypes from before this split was made.
 *
]*/


#ifdef GISP_SVGA
/* DIVERGENCE MVDM-HOST-DIV-534: unreferenced GISP hardware VGA interface is not selected; see mirror register. */
#endif /* GISP_SVGA */
