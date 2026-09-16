/*
 *  dem.c - Main Module of DOS Emulation DLL.
 *
 *  Sudeepb 09-Apr-1991 Craeted
 */

#include "io.h"
#include "dem.h"
#include "mvdm_softpc_firmware.h"

/* DemInit - DEM Initialiazation routine. (This name may change when DEM is
 *	     converted to DLL).
 *
 * Entry
 *	argc,argv - from softpc as it is.
 *
 *
 * Exit
 *	None
 */

PSZ pszDefaultDOSDirectory;

extern VOID	TerminateVDM(VOID);

CHAR demDebugBuffer [256];

#if DBG
BOOL ToDebugOnF11 = FALSE;
#endif

BOOL DemInit (int argc, char *argv[])
{
    PSZ psz;

    // Modify default hard error handling
    // - turn off all file io related popups
    // - keep GP fault popups from system
    //
    SetErrorMode (SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);

    pszDefaultDOSDirectory =  (PCHAR) malloc(MAX_PATH+14);
    /* DIVERGENCE(MVDM-HOST-DIV-154): the original NT4 system directory is
     * not this product's immutable MVDM system root.  Preserve
     * the original caller-owned directory buffer and every later DEM path
     * operation; obtain only its initial directory through the established
     * session media binding. */
    if (!pszDefaultDOSDirectory ||
        !mvdm_softpc_system_copy_root(pszDefaultDOSDirectory, MAX_PATH+14))
      {
        return FALSE;
        }

    if (VDMForWOW)
        return TRUE;

    // Check the debugging level
    while (--argc > 0) {
	psz = *++argv;
	if (*psz == '-' || *psz == '/') {
	    psz++;
	    if(tolower(*psz) == 'd'){
		fShowSVCMsg = DEMDOSDISP | DEMFILIO;
		break;
	    }
	}
    }


#if DBG
#ifndef i386
    if( getenv( "YODA" ) != 0 )
#else
    if( getenv( "DEBUGDOS" ) != 0 )
#endif
	ToDebugOnF11 = TRUE;
#endif

    return TRUE;
}
