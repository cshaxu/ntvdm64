#ifndef NTDOS64_OPENNT_PIF_OEM_FACADE_H
#define NTDOS64_OPENNT_PIF_OEM_FACADE_H

/* Same-shaped public-Win32 replacement for the OEM string/path operations
 * used by OpenNT softpc.new/host/src/nt_pif.c.  PIF parsing and COMMAND
 * policy remain in their OpenNT owners. */
#include <windows.h>

DWORD bx_ntvdm_command_pif_expand_environment_strings_oem(LPCSTR source,
    LPSTR destination, DWORD destination_bytes);
DWORD bx_ntvdm_command_pif_get_full_path_name_oem(LPCSTR source,
    DWORD destination_bytes, LPSTR destination, LPSTR *file_part);
DWORD bx_ntvdm_command_pif_get_short_path_name_oem(LPCSTR source,
    LPSTR destination, DWORD destination_bytes);
DWORD bx_ntvdm_command_pif_search_path_oem(LPCSTR path, LPCSTR file_name,
    LPCSTR extension, DWORD destination_bytes, LPSTR destination,
    LPSTR *file_part);
BOOL bx_ntvdm_command_pif_set_console_title_oem(LPCSTR title);
void *ch_malloc(unsigned int bytes);

#endif
