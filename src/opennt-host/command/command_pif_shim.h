#ifndef BX_NTVDM_BOP_SHIM_COMMAND_PIF_SHIM_H
#define BX_NTVDM_BOP_SHIM_COMMAND_PIF_SHIM_H

/* Smallest modern host-capability seam for the directly imported COMMAND PIF
 * owner and the directly included OpenNT nt_pif.c parser.  It deliberately
 * supplies no PIF policy or parser: those remain original OpenNT code. */
#include "command_misc_shim.h"
#include "adapter-win32/facade/opennt_error_dialog_facade.h"

#ifndef SUBSYS_DOS
#define SUBSYS_DOS 1u
#endif
#ifndef EG_PIF_STARTDIR_ERR
#define EG_PIF_STARTDIR_ERR 19u
#define EG_PIF_STARTFILE_ERR 20u
#define EG_PIF_CMDLINE_ERR 21u
#define EG_PIF_ASK_CMDLINE 22u
#define EG_DOS_PROG_EXTENSION 28u
#endif
#ifndef RMB_ABORT
#define RMB_ABORT 1u
#endif
#ifndef RMB_RETRY
#define RMB_RETRY 2u
#endif
#ifndef RMB_IGNORE
#define RMB_IGNORE 4u
#endif
#ifndef RMB_ICON_INFO
#define RMB_ICON_INFO 8u
#endif
#ifndef RMB_ICON_BANG
#define RMB_ICON_BANG 16u
#endif
#ifndef RMB_ICON_STOP
#define RMB_ICON_STOP 32u
#endif
#ifndef RMB_EDIT
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
/* `nt_pif.c`'s original allocation/error loop calls this exact adapter
 * spelling. It delegates to the shared OpenNT-shaped dialog facade rather
 * than making parser-local retry policy. */
int bx_ntvdm_command_pif_parser_message_box(UINT error, CHAR *first,
    CHAR *second, ULONG options);
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
