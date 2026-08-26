/* DIVERGENCE(WIN32-DIV-005): OpenNT DEM called OEM host exports supplied by
 * the NTVDM product host.  Retain their original names, parameters and Win32
 * last-error behavior through public OEM/ANSI APIs. */
#include "adapter-win32/include/opennt-api/opennt_dem_oem_facade.h"

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

BOOL CreateDirectoryOem(LPSTR name, LPSECURITY_ATTRIBUTES security)
{
    LPSTR ansi = ansi_copy(name);
    BOOL result;
    if (ansi == NULL) return FALSE;
    result = CreateDirectoryA(ansi, security);
    free(ansi);
    return result;
}

BOOL RemoveDirectoryOem(LPSTR name)
{
    LPSTR ansi = ansi_copy(name);
    BOOL result;
    if (ansi == NULL) return FALSE;
    result = RemoveDirectoryA(ansi);
    free(ansi);
    return result;
}

BOOL SetCurrentDirectoryOem(LPSTR name)
{
    LPSTR ansi = ansi_copy(name);
    BOOL result;
    if (ansi == NULL) return FALSE;
    result = SetCurrentDirectoryA(ansi);
    free(ansi);
    return result;
}
