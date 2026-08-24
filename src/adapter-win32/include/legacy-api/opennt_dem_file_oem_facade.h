#ifndef NTDOS64_OPENNT_DEM_FILE_OEM_FACADE_H
#define NTDOS64_OPENNT_DEM_FILE_OEM_FACADE_H

/* Same-shaped public-Win32 replacement for the OEM file calls reached by
 * the directly imported OpenNT DEM provider bodies.  DOS service ordering
 * and error selection remain in those bodies. */
#include <windows.h>

HANDLE CreateFileOem(LPSTR name, DWORD access, DWORD share,
    LPSECURITY_ATTRIBUTES security, DWORD creation, DWORD flags,
    HANDLE template_file);
BOOL DeleteFileOem(LPSTR name);
DWORD GetFileAttributesOem(LPSTR name);
BOOL SetFileAttributesOem(LPSTR name, DWORD attributes);
BOOL MoveFileOem(LPSTR source, LPSTR destination);
BOOL GetVolumeInformationOem(LPSTR root, LPSTR volume, DWORD volume_bytes,
    DWORD *serial, DWORD *maximum_component_length, DWORD *flags,
    LPSTR filesystem, DWORD filesystem_bytes);
BOOL RemoveFontResourceOem(LPSTR name);
void OutputDebugStringOem(LPSTR text);

#endif
