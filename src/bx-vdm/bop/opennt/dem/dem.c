/* OpenNT source mirror: base/mvdm/dos/dem/dem.c. */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "dem_shared.h"

char *pszDefaultDOSDirectory;
char demDebugBuffer[256];

int DemInit(int argc, char *argv[])
{
    char *psz;
    DWORD dw;

    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
    free(pszDefaultDOSDirectory);
    pszDefaultDOSDirectory = (char *)malloc(MAX_PATH + 14u);
    if (pszDefaultDOSDirectory == 0 ||
        (dw = GetSystemDirectoryA(pszDefaultDOSDirectory, MAX_PATH)) == 0u ||
        dw >= MAX_PATH) return 0;

    /* Divergence: VDMForWOW is a historical global host-composition choice.
     * A CLI Direct DEM mirror has no implicit WOW short-circuit here. */
    while (--argc > 0) {
        psz = *++argv;
        if (*psz == '-' || *psz == '/') {
            ++psz;
            if (tolower((unsigned char)*psz) == 'd') {
                fShowSVCMsg = 1u;
                break;
            }
        }
    }
    return 1;
}
