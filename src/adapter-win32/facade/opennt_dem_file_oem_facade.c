/* DIVERGENCE(WIN32-DIV-006): OpenNT DEM received these OEM exports from the
 * NTVDM product host.  Preserve their names, parameters, conversion and
 * last-error contract using only public Win32 APIs and call-local storage. */
#include "adapter-win32/include/opennt-api/opennt_dem_file_oem_facade.h"
#include "opennt-bop/observation/demfile_create_observation.h"

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

static LPWSTR oem_to_wide_copy(LPCSTR oem)
{
    int chars;
    LPWSTR wide;
    if (oem == NULL) { SetLastError(ERROR_INVALID_PARAMETER); return NULL; }
    chars = MultiByteToWideChar(CP_OEMCP, 0, oem, -1, NULL, 0);
    if (chars <= 0) return NULL;
    wide = (LPWSTR)malloc((size_t)chars * sizeof(*wide));
    if (wide == NULL) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
    if (MultiByteToWideChar(CP_OEMCP, 0, oem, -1, wide, chars) != chars) {
        free(wide); return NULL;
    }
    return wide;
}

HANDLE CreateFileOem(LPSTR name, DWORD access, DWORD share,
    LPSECURITY_ATTRIBUTES security, DWORD creation, DWORD flags, HANDLE template_file)
{
    LPWSTR wide = oem_to_wide_copy(name);
    HANDLE result;
    DWORD conversion_error = wide == NULL ? GetLastError() : ERROR_SUCCESS;
    DWORD first_error = ERROR_SUCCESS, retry_error = ERROR_SUCCESS;
    if (wide == NULL) {
        runtime_demfile_create_observation_record(NULL, access, share,
            creation, flags, conversion_error, first_error, retry_error);
        return INVALID_HANDLE_VALUE;
    }
    /* OpenNT oemuni/file.c uses its TEB static Unicode buffer then CreateFileW.
     * Public Win32 has no supported equivalent buffer, so use call-local OEM
     * conversion while retaining the historical read-open retry ordering. */
    result = CreateFileW(wide, access == GENERIC_READ ? access | FILE_WRITE_ATTRIBUTES : access,
        share, security, creation, flags, template_file);
    if (result == INVALID_HANDLE_VALUE) first_error = GetLastError();
    if (result == INVALID_HANDLE_VALUE && access == GENERIC_READ) {
        result = CreateFileW(wide, access, share, security, creation, flags, template_file);
        if (result == INVALID_HANDLE_VALUE) retry_error = GetLastError();
    }
    runtime_demfile_create_observation_record(wide, access, share,
        creation, flags, conversion_error, first_error, retry_error);
    free(wide);
    return result;
}

BOOL DeleteFileOem(LPSTR name)
{ LPSTR ansi = ansi_copy(name); BOOL result; if (ansi == NULL) return FALSE; result = DeleteFileA(ansi); free(ansi); return result; }
DWORD GetFileAttributesOem(LPSTR name)
{ LPSTR ansi = ansi_copy(name); DWORD result; if (ansi == NULL) return INVALID_FILE_ATTRIBUTES; result = GetFileAttributesA(ansi); free(ansi); return result; }
BOOL SetFileAttributesOem(LPSTR name, DWORD attributes)
{ LPSTR ansi = ansi_copy(name); BOOL result; if (ansi == NULL) return FALSE; result = SetFileAttributesA(ansi, attributes); free(ansi); return result; }
BOOL MoveFileOem(LPSTR source, LPSTR destination)
{ LPSTR src = ansi_copy(source); LPSTR dst = ansi_copy(destination); BOOL result; if (src == NULL || dst == NULL) { free(src); free(dst); return FALSE; } result = MoveFileA(src, dst); free(src); free(dst); return result; }
BOOL GetVolumeInformationOem(LPSTR root, LPSTR volume, DWORD volume_bytes, DWORD *serial,
    DWORD *maximum_component_length, DWORD *flags, LPSTR filesystem, DWORD filesystem_bytes)
{ LPSTR ansi = ansi_copy(root); BOOL result; if (ansi == NULL) return FALSE; result = GetVolumeInformationA(ansi, volume, volume_bytes, serial, maximum_component_length, flags, filesystem, filesystem_bytes); free(ansi); return result; }
BOOL RemoveFontResourceOem(LPSTR name)
{ LPSTR ansi = ansi_copy(name); BOOL result; if (ansi == NULL) return FALSE; result = RemoveFontResourceA(ansi); free(ansi); return result; }
void OutputDebugStringOem(LPSTR text)
{ LPSTR ansi = ansi_copy(text); if (ansi != NULL) { OutputDebugStringA(ansi); free(ansi); } }
