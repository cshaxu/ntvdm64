/* DIVERGENCE(WIN32-DIV-008): OpenNT DEM used OEM find exports and the
 * invasive NtVdmControl product interface.  Preserve source-facing spelling:
 * OEM enumeration maps to public Win32, and removed NtVdmControl returns its
 * explicit native unavailable status so the original fallback remains live. */
#include "adapter-win32/include/legacy-api/opennt_dem_search_facade.h"
#include <stdlib.h>
#include <string.h>

static LPSTR ansi_copy(LPCSTR oem)
{
    size_t bytes;
    LPSTR ansi;
    if (oem == NULL) { SetLastError(ERROR_INVALID_PARAMETER); return NULL; }
    bytes = strlen(oem) + 1u;
    if (bytes > MAX_PATH + 1u) { SetLastError(ERROR_BUFFER_OVERFLOW); return NULL; }
    ansi = (LPSTR)malloc(bytes);
    if (ansi == NULL) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
    if (!OemToCharBuffA(oem, ansi, (DWORD)bytes)) { free(ansi); return NULL; }
    return ansi;
}
HANDLE FindFirstFileOem(LPSTR name, LPWIN32_FIND_DATAA data)
{ LPSTR ansi = ansi_copy(name); HANDLE find; if (ansi == NULL) return INVALID_HANDLE_VALUE; find = FindFirstFileA(ansi, data); free(ansi); return find; }
BOOL FindNextFileOem(HANDLE find, LPWIN32_FIND_DATAA data)
{ return FindNextFileA(find, data); }
PVOID NTAPI RtlProcessHeap(VOID) { return GetProcessHeap(); }
NTSTATUS NtVdmControl(VDMSERVICECLASS service, PVOID data)
{ (void)service; (void)data; return STATUS_NOT_IMPLEMENTED; }
