#include "command_pif_shim.h"

#include <stdlib.h>
#include <string.h>

static BOOL oem_to_ansi(LPCSTR source, LPSTR destination, DWORD destination_bytes)
{
    size_t bytes;
    if (source == NULL || destination == NULL) return FALSE;
    bytes = strlen(source) + 1u;
    return bytes <= destination_bytes &&
        OemToCharBuffA(source, destination, (DWORD)bytes);
}

static BOOL ansi_to_oem(LPCSTR source, LPSTR destination, DWORD destination_bytes)
{
    size_t bytes;
    if (source == NULL || destination == NULL) return FALSE;
    bytes = strlen(source) + 1u;
    return bytes <= destination_bytes &&
        CharToOemBuffA(source, destination, (DWORD)bytes);
}

DWORD bx_ntvdm_command_pif_expand_environment_strings_oem(LPCSTR source,
    LPSTR destination, DWORD destination_bytes)
{
    CHAR ansi_source[MAX_PATH + 1u];
    CHAR ansi_destination[MAX_PATH + 1u];
    DWORD result;
    if (!oem_to_ansi(source, ansi_source, (DWORD)sizeof(ansi_source))) return 0u;
    result = ExpandEnvironmentStringsA(ansi_source, ansi_destination,
        (DWORD)sizeof(ansi_destination));
    if (result == 0u || result > sizeof(ansi_destination) ||
        !ansi_to_oem(ansi_destination, destination, destination_bytes)) return result;
    return result;
}

DWORD bx_ntvdm_command_pif_get_full_path_name_oem(LPCSTR source,
    DWORD destination_bytes, LPSTR destination, LPSTR *file_part)
{
    CHAR ansi_source[MAX_PATH + 1u];
    CHAR ansi_destination[MAX_PATH + 1u];
    LPSTR ansi_file_part = NULL;
    DWORD result;
    if (!oem_to_ansi(source, ansi_source, (DWORD)sizeof(ansi_source))) return 0u;
    result = GetFullPathNameA(ansi_source, (DWORD)sizeof(ansi_destination),
        ansi_destination, &ansi_file_part);
    if (result == 0u || result >= sizeof(ansi_destination) ||
        !ansi_to_oem(ansi_destination, destination, destination_bytes)) return result;
    if (file_part != NULL)
        *file_part = ansi_file_part == NULL ? NULL : destination + (ansi_file_part - ansi_destination);
    return result;
}

DWORD bx_ntvdm_command_pif_get_short_path_name_oem(LPCSTR source,
    LPSTR destination, DWORD destination_bytes)
{
    CHAR ansi_source[MAX_PATH + 1u];
    CHAR ansi_destination[MAX_PATH + 1u];
    DWORD result;
    if (!oem_to_ansi(source, ansi_source, (DWORD)sizeof(ansi_source))) return 0u;
    result = GetShortPathNameA(ansi_source, ansi_destination,
        (DWORD)sizeof(ansi_destination));
    if (result == 0u || result >= sizeof(ansi_destination) ||
        !ansi_to_oem(ansi_destination, destination, destination_bytes)) return result;
    return result;
}

DWORD bx_ntvdm_command_pif_search_path_oem(LPCSTR path, LPCSTR file_name,
    LPCSTR extension, DWORD destination_bytes, LPSTR destination,
    LPSTR *file_part)
{
    CHAR ansi_path[MAX_PATH + 1u];
    CHAR ansi_file[MAX_PATH + 1u];
    CHAR ansi_extension[MAX_PATH + 1u];
    CHAR ansi_destination[MAX_PATH + 1u];
    LPSTR ansi_file_part = NULL;
    DWORD result;
    if ((path != NULL && !oem_to_ansi(path, ansi_path, (DWORD)sizeof(ansi_path))) ||
        !oem_to_ansi(file_name, ansi_file, (DWORD)sizeof(ansi_file)) ||
        (extension != NULL && !oem_to_ansi(extension, ansi_extension,
            (DWORD)sizeof(ansi_extension)))) return 0u;
    result = SearchPathA(path == NULL ? NULL : ansi_path, ansi_file,
        extension == NULL ? NULL : ansi_extension, (DWORD)sizeof(ansi_destination),
        ansi_destination, &ansi_file_part);
    if (result == 0u || result >= sizeof(ansi_destination) ||
        !ansi_to_oem(ansi_destination, destination, destination_bytes)) return result;
    if (file_part != NULL)
        *file_part = ansi_file_part == NULL ? NULL : destination + (ansi_file_part - ansi_destination);
    return result;
}

BOOL bx_ntvdm_command_pif_set_console_title_oem(LPCSTR title)
{
    CHAR ansi_title[MAX_PATH + 1u];
    return oem_to_ansi(title, ansi_title, (DWORD)sizeof(ansi_title)) &&
        SetConsoleTitleA(ansi_title);
}

void *ch_malloc(unsigned int bytes)
{
    return malloc((size_t)bytes);
}
