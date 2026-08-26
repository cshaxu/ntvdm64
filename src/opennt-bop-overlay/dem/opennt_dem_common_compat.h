#ifndef RUNTIME_BOP_SHIM_DEM_COMMON_SHIM_H
#define RUNTIME_BOP_SHIM_DEM_COMMON_SHIM_H

/* Historical include surface for directly imported dem.c.  It contains
 * declarations/constants only; lifecycle policy remains in the imported
 * source and host binding remains in opennt_demmisc_compat. */
#include <ctype.h>
#include <stdlib.h>
#include <windows.h>

#include "adapter-win32/facade/opennt_command_oem_facade.h"
#include "opennt-bop-overlay/dem/opennt_demmisc_compat.h"

/* DIVERGENCE(BOP-DIV-092): retain DemInit's original spelling while binding
 * its historical installed-system directory through the same-shaped public
 * Win32 facade. */
#undef GetSystemDirectory
#define GetSystemDirectory(buffer, bytes) \
    runtime_opennt_system_directory((buffer), (bytes))

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
