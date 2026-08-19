/* Neutral host-composition seam for the directly imported OpenNT demfile.c
 * / demlabel.c package.  Historical OEM helpers and CCPU/SAS call plumbing
 * are unavailable to the standalone CLI process; each wrapper below maps
 * only that named dependency to ordinary Win32 APIs and leaves the imported
 * DEM ordering and failure handling in its original translation unit. */

#include "demfile_shim.h"

void demChMod(void);
void demCreate(void);
void demCreateNew(void);
void demDelete(void);
void demOpen(void);
void demRename(void);
void demCheckPath(void);

static BOOL oem_to_ansi(LPCSTR oem, LPSTR ansi, DWORD bytes)
{
    if (oem == NULL || ansi == NULL || bytes == 0u) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if (!OemToCharBuffA(oem, ansi, (DWORD)strlen(oem) + 1u)) return FALSE;
    return TRUE;
}

static LPSTR oem_copy(LPCSTR oem)
{
    size_t bytes;
    LPSTR ansi;
    if (oem == NULL) { SetLastError(ERROR_INVALID_PARAMETER); return NULL; }
    bytes = strlen(oem) + 1u;
    if (bytes > MAX_PATH + 1u) { SetLastError(ERROR_BUFFER_OVERFLOW); return NULL; }
    ansi = (LPSTR)malloc(bytes);
    if (ansi == NULL) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
    if (!oem_to_ansi(oem, ansi, (DWORD)bytes)) { free(ansi); return NULL; }
    return ansi;
}

HANDLE CreateFileOem(LPSTR name, DWORD access, DWORD share,
    LPSECURITY_ATTRIBUTES security, DWORD creation, DWORD flags, HANDLE template_file)
{
    LPSTR ansi = oem_copy(name);
    HANDLE result;
    if (ansi == NULL) return INVALID_HANDLE_VALUE;
    result = CreateFileA(ansi, access, share, security, creation, flags, template_file);
    free(ansi);
    return result;
}

BOOL DeleteFileOem(LPSTR name)
{ LPSTR ansi = oem_copy(name); BOOL result; if (ansi == NULL) return FALSE; result = DeleteFileA(ansi); free(ansi); return result; }
DWORD GetFileAttributesOem(LPSTR name)
{ LPSTR ansi = oem_copy(name); DWORD result; if (ansi == NULL) return INVALID_FILE_ATTRIBUTES; result = GetFileAttributesA(ansi); free(ansi); return result; }
BOOL SetFileAttributesOem(LPSTR name, DWORD attributes)
{ LPSTR ansi = oem_copy(name); BOOL result; if (ansi == NULL) return FALSE; result = SetFileAttributesA(ansi, attributes); free(ansi); return result; }
BOOL MoveFileOem(LPSTR source, LPSTR destination)
{ LPSTR src = oem_copy(source); LPSTR dst = oem_copy(destination); BOOL result; if (src == NULL || dst == NULL) { free(src); free(dst); return FALSE; } result = MoveFileA(src, dst); free(src); free(dst); return result; }
BOOL GetVolumeInformationOem(LPSTR root, LPSTR volume, DWORD volume_bytes,
    DWORD *serial, DWORD *maximum_component_length, DWORD *flags,
    LPSTR filesystem, DWORD filesystem_bytes)
{ LPSTR ansi = oem_copy(root); BOOL result; if (ansi == NULL) return FALSE; result = GetVolumeInformationA(ansi, volume, volume_bytes, serial, maximum_component_length, flags, filesystem, filesystem_bytes); free(ansi); return result; }
BOOL RemoveFontResourceOem(LPSTR name)
{ LPSTR ansi = oem_copy(name); BOOL result; if (ansi == NULL) return FALSE; result = RemoveFontResourceA(ansi); free(ansi); return result; }
void OutputDebugStringOem(LPSTR text)
{ LPSTR ansi = oem_copy(text); if (ansi != NULL) { OutputDebugStringA(ansi); free(ansi); } }

BOOL LoadVdmRedir(void)
{
    /* VDMREDIR.DLL is a separate historical Redirector composition package.
     * Do not make demfile fabricate it; the imported demOpen error path owns
     * the resulting failure. */
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
LPSTR VrConvertLocalNtPipeName(LPSTR existing, LPSTR name)
{ (void)existing; (void)name; SetLastError(ERROR_CALL_NOT_IMPLEMENTED); return NULL; }
void VrAddOpenNamedPipeInfo(HANDLE file, LPSTR name)
{ (void)file; (void)name; }

void *Sim32GetVDMPointer(ULONG address, ULONG bytes, int protect)
{
    (void)address; (void)bytes; (void)protect;
    /* VDD JFT/SFT helpers remain imported source but require the original
     * process/VDM table composition; no S3 BOP invokes this fallback. */
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return NULL;
}

BOOL bx_ntvdm_demfile_publish_handle(HANDLE file)
{ return bx_ntvdm_demhndl_publish_handle(file); }

int bx_ntvdm_demfile_invoke(bx_ntvdm_demhndl_call *call)
{
    void (*body)(void) = NULL;
    if (call == NULL) return 0;
    switch (call->service) {
    case BX_NTVDM_DEMFILE_CHMOD: body = demChMod; break;
    case BX_NTVDM_DEMFILE_CREATE: body = demCreate; break;
    case BX_NTVDM_DEMFILE_DELETE: body = demDelete; break;
    case BX_NTVDM_DEMFILE_OPEN: body = demOpen; break;
    case BX_NTVDM_DEMFILE_RENAME: body = demRename; break;
    case BX_NTVDM_DEMFILE_CREATE_NEW: body = demCreateNew; break;
    case BX_NTVDM_DEMFILE_CHECK_PATH: body = demCheckPath; break;
    default: return 0;
    }
    return bx_ntvdm_demhndl_invoke_body(call, body);
}
