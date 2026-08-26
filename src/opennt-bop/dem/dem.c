/*
 *  dem.c - Main Module of DOS Emulation DLL.
 *
 *  Sudeepb 09-Apr-1991 Craeted
 */

/* DIVERGENCE(BOP-DIV-092): direct import from src/opennt/base/mvdm/dos/dem/dem.c.
 * The historical io.h/dem.h closure is unavailable outside the
 * NTVDM product shell; opennt_dem_common_compat.h supplies only its declarations and
 * exact diagnostic constants. */
#include "opennt-bop-overlay/dem/opennt_dem_common_compat.h"

/* DemInit - DEM Initialiazation routine. (This name may change when DEM is
 *           converted to DLL). */

PSZ pszDefaultDOSDirectory;

extern VOID TerminateVDM(VOID);

CHAR demDebugBuffer [256];

#if DBG
BOOL ToDebugOnF11 = FALSE;
#endif

BOOL DemInit (int argc, char *argv[])
{
    PSZ psz;
    DWORD dw;

    SetErrorMode (SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);

    pszDefaultDOSDirectory =  (PCHAR) malloc(MAX_PATH+14);
    if (!pszDefaultDOSDirectory ||
        !(dw = GetSystemDirectory(pszDefaultDOSDirectory, MAX_PATH)) ||
        dw >= MAX_PATH )
      {
        return FALSE;
        }

    if (VDMForWOW)
        return TRUE;

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
