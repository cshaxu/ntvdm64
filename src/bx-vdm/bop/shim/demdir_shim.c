/* Neutral OEM-directory capability seam for the imported OpenNT demdir.c. */
#include "demdir_shim.h"

void demCreateDir(void);
void demDeleteDir(void);
void demQueryCurrentDir(void);
void demSetCurrentDir(void);

static LPSTR ansi_copy(LPCSTR oem)
{
    size_t bytes; LPSTR ansi;
    if (oem == NULL) { SetLastError(ERROR_INVALID_PARAMETER); return NULL; }
    bytes = strlen(oem) + 1u;
    if (bytes > MAX_PATH + 1u) { SetLastError(ERROR_BUFFER_OVERFLOW); return NULL; }
    ansi = (LPSTR)malloc(bytes);
    if (ansi == NULL) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
    if (!OemToCharBuffA(oem, ansi, (DWORD)bytes)) { free(ansi); return NULL; }
    return ansi;
}

BOOL CreateDirectoryOem(LPSTR name, LPSECURITY_ATTRIBUTES security)
{ LPSTR ansi = ansi_copy(name); BOOL result; if (!ansi) return FALSE; result = CreateDirectoryA(ansi, security); free(ansi); return result; }
BOOL RemoveDirectoryOem(LPSTR name)
{ LPSTR ansi = ansi_copy(name); BOOL result; if (!ansi) return FALSE; result = RemoveDirectoryA(ansi); free(ansi); return result; }
BOOL SetCurrentDirectoryOem(LPSTR name)
{ LPSTR ansi = ansi_copy(name); BOOL result; if (!ansi) return FALSE; result = SetCurrentDirectoryA(ansi); free(ansi); return result; }
int bx_ntvdm_demdir_invoke(bx_ntvdm_demhndl_call *call)
{
    void (*body)(void) = NULL;
    if (call == NULL) return 0;
    switch (call->service) {
    case BX_NTVDM_DEMDIR_CREATE: body = demCreateDir; break;
    case BX_NTVDM_DEMDIR_DELETE: body = demDeleteDir; break;
    case BX_NTVDM_DEMDIR_QUERY_CURRENT: body = demQueryCurrentDir; break;
    case BX_NTVDM_DEMDIR_SET_CURRENT: body = demSetCurrentDir; break;
    default: return 0;
    }
    return bx_ntvdm_demhndl_invoke_body(call, body);
}
