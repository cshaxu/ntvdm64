#ifndef BX_NTVDM_BOP_SHIM_COMMAND_PIF_SHIM_H
#define BX_NTVDM_BOP_SHIM_COMMAND_PIF_SHIM_H

/* Smallest modern host-capability seam for the directly imported COMMAND PIF
 * owner and the directly included OpenNT nt_pif.c parser.  It deliberately
 * supplies no PIF policy or parser: those remain original OpenNT code. */
#include "command_misc_shim.h"

#ifndef SUBSYS_DOS
#define SUBSYS_DOS 1u
#endif
#ifndef EG_PIF_STARTDIR_ERR
#define EG_PIF_STARTDIR_ERR 19u
#define EG_PIF_STARTFILE_ERR 20u
#define EG_PIF_CMDLINE_ERR 21u
#define EG_PIF_ASK_CMDLINE 22u
#define EG_DOS_PROG_EXTENSION 28u
#define RMB_ICON_INFO 8u
#define RMB_EDIT 128u
#endif

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
BOOL GetPIFData(PIF_DATA *data, char *pif_name);
/* Execute the unmodified nt_pif.c CONFIG/AUTOEXEC selection while the
 * startup PIF is still the initial host input. */
BOOL bx_ntvdm_command_pif_select_config_files(char *pif_name,
    char *config_path, char *autoexec_path);
DWORD GetFileAttributesOem(LPSTR name);
BOOL SetCurrentDirectoryOem(LPSTR name);

#define ExpandEnvironmentStringsOem bx_ntvdm_command_pif_expand_environment_strings_oem
#define GetFullPathNameOem bx_ntvdm_command_pif_get_full_path_name_oem
#define GetShortPathNameOem bx_ntvdm_command_pif_get_short_path_name_oem
#define SearchPathOem bx_ntvdm_command_pif_search_path_oem
#ifdef SetConsoleTitle
#undef SetConsoleTitle
#endif
#define SetConsoleTitle bx_ntvdm_command_pif_set_console_title_oem

#endif
