#ifndef BX_NTVDM_BOP_SHIM_DEM_COMMON_SHIM_H
#define BX_NTVDM_BOP_SHIM_DEM_COMMON_SHIM_H

/* Historical include surface for directly imported dem.c.  It contains
 * declarations/constants only; lifecycle policy remains in the imported
 * source and host binding remains in demmisc_shim. */
#include <ctype.h>
#include <stdlib.h>
#include <windows.h>

#include "demmisc_shim.h"

/* OpenNT's DEM sources were compiled with their DBG surface available: other
 * directly imported owners call demPrintMsg unconditionally.  The current
 * standalone config projection does not define DBG, so project that original
 * compilation condition here rather than supplying a source-derived message
 * replacement. */
#ifndef DBG
#define DBG 1
#endif

typedef CHAR *PSZ;

extern CHAR demDebugBuffer[256];
#define DEMDOSDISP 0x40000000u
#define DEMFILIO   0x20000000u

#endif
