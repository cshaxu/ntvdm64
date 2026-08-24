#ifndef APP_OPENNT_DEM_GSET_FACADE_H
#define APP_OPENNT_DEM_GSET_FACADE_H

/* Same-shaped public-Win32 replacement for reached OpenNT DEM global-setting
 * host calls.  Drive admission and BOP service meaning are outside this ABI. */
#include <windows.h>

BOOL GetComputerNameOem(LPSTR name, LPDWORD chars);
BOOL GetDiskFreeSpaceOem(LPCSTR root, LPDWORD sectors_per_cluster,
    LPDWORD bytes_per_sector, LPDWORD free_clusters, LPDWORD total_clusters);

#endif
