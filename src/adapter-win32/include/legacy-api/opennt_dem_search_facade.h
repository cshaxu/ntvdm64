#ifndef NTDOS64_OPENNT_DEM_SEARCH_FACADE_H
#define NTDOS64_OPENNT_DEM_SEARCH_FACADE_H

/* Same-shaped replacement for reached DEM search host APIs.  Search/FCB
 * algorithm and service selection remain in the OpenNT provider body. */
#include <windows.h>
#include <winternl.h>
#include "adapter-win32/include/compiler/opennt_dem_ntdecl_compat.h"

HANDLE FindFirstFileOem(LPSTR name, LPWIN32_FIND_DATAA data);
BOOL FindNextFileOem(HANDLE find, LPWIN32_FIND_DATAA data);
PVOID NTAPI RtlProcessHeap(VOID);
NTSTATUS NtVdmControl(VDMSERVICECLASS service, PVOID data);

#endif
