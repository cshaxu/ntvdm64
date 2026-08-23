/* demmsg.c - debug messages handling for DEM
 *
 * Modification History
 *
 * Sudeepb 31-Mar-1991 Created
 */
/* Divergence: this original conditional precedes its historical headers.
 * The standalone graph must project the original DEM debug compilation
 * condition here so the source-owned demPrintMsg body is emitted. */
#ifndef DBG
#define DBG 1
#endif
#if DBG

/* Direct import from src/opennt/base/mvdm/dos/dem/demmsg.c.
 * Divergence: the historical demmsg.h/dem.h include closure is supplied by
 * dem_common_shim.h; no message selection or debug-output logic is replaced. */
#include <stdio.h>
#include "../../../opennt-host/capability/dem_common_shim.h"

PCHAR aMsg [] = {
    "DOS Location Not Found. Using Default.\n",
    "Read On NTDOS.SYS Failed.\n",
    "Open On NTDOS.SYS Failed.\n",
    "EAs Not Supported\n",
    "Letter mismatch in Set_Default_Drive\n",
    "Volume ID support is missing\n",
    "Invalid Date Time Format for NT\n",
    "DTA has an Invalid Find Handle for FINDNEXT\n",
    "Unexpected failure to get file information\n",
    "File Size is too big for DOS\n"
};

VOID demPrintMsg (ULONG iMsg)
{

    if (fShowSVCMsg){
       sprintf(demDebugBuffer,aMsg[iMsg]);
       OutputDebugStringOem(demDebugBuffer);
    }

    iMsg;

    return;
}

#endif
