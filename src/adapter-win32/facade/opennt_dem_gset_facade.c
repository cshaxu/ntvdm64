/* DIVERGENCE(WIN32-DIV-007): OpenNT DEM received OEM host queries through
 * its NTVDM product composition.  Preserve the reached names and result/
 * last-error shape with their direct public Win32 counterparts. */
#include "adapter-win32/include/legacy-api/opennt_dem_gset_facade.h"

BOOL GetComputerNameOem(LPSTR name, LPDWORD chars)
{ return GetComputerNameA(name, chars); }

BOOL GetDiskFreeSpaceOem(LPCSTR root, LPDWORD sectors_per_cluster,
    LPDWORD bytes_per_sector, LPDWORD free_clusters, LPDWORD total_clusters)
{ return GetDiskFreeSpaceA(root, sectors_per_cluster, bytes_per_sector,
      free_clusters, total_clusters); }
