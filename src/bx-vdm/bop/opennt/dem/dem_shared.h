#ifndef BX_NTVDM_BOP_OPENNT_DEM_SHARED_H
#define BX_NTVDM_BOP_OPENNT_DEM_SHARED_H

/* OpenNT source mirror: src/opennt/base/mvdm/dos/dem/dem.h. */

#include <stdint.h>

extern uintptr_t *pulDTALocation;
extern uint16_t *pusCurrentPDB;
extern void *pSFTHead;
extern void *pExtendedError;
extern char *pszDefaultDOSDirectory;
extern char demDebugBuffer[256];
extern uint32_t fShowSVCMsg;
extern uint32_t CurrentISVC;

#endif
