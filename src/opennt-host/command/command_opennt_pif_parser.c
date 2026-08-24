/* Compile the original OpenNT PIF parser as source.  The historical source
 * brings a private PIF_DATA typedef and legacy host declarations with it;
 * rename those translation-unit names only, then bridge its layout-identical
 * result to the COMMAND-facing record.  This keeps both original owner
 * bodies intact without letting NT4's global header universe leak into the
 * modern CLI composition. */
#include "command_pif_shim.h"

#define WINNT 1
#define _INSIGNIA_H 1
#undef SUBSYS_DOS
#undef EG_MALLOC_FAILURE
#undef EG_PIF_STARTDIR_ERR
#undef EG_PIF_STARTFILE_ERR
#undef EG_PIF_CMDLINE_ERR
#undef EG_PIF_ASK_CMDLINE
#undef EG_DOS_PROG_EXTENSION
#undef RMB_ABORT
#undef RMB_RETRY
#undef RMB_IGNORE
#undef RMB_ICON_INFO
#undef RMB_ICON_BANG
#undef RMB_ICON_STOP
#undef RMB_EDIT
#define PIF_DATA BX_NTVDM_OPENNT_PARSER_PIF_DATA
#define pfdata bx_ntvdm_command_pif_parser_global
#define GetPIFData bx_ntvdm_command_pif_parse_original
#define GetPIFConfigFiles bx_ntvdm_command_pif_original_get_config_files
#define RcMessageBox bx_ntvdm_command_pif_parser_message_box
#define ch_malloc bx_ntvdm_command_pif_parser_malloc
#pragma warning(push)
#pragma warning(disable:4005 4028 4142 4244)
/* DIVERGENCE(HOST-DIV-011): the original parser remains compiled verbatim,
 * but its directly composed source now lives in this owner-family production
 * closure rather than the removed NT4 global source tree. */
#include "nt_pif.c"
#pragma warning(pop)
#undef ch_malloc
#undef RcMessageBox
#undef GetPIFConfigFiles
#undef GetPIFData
#undef pfdata
#undef PIF_DATA

DWORD dwWNTPifFlags;
UCHAR WNTPifFgPr;
UCHAR WNTPifBgPr;
BOOL IdleDisabledFromPIF;
BX_NTVDM_OPENNT_PARSER_PIF_DATA bx_ntvdm_command_pif_parser_global;

int bx_ntvdm_command_pif_parser_message_box(UINT error, CHAR *first,
    CHAR *second, ULONG options)
{
    /* Preserve nt_pif.c's source-visible allocation/retry decision through
     * the single shared RcMessageBox facade.  The public Win32 dialog maps
     * the historical Abort/Retry/Ignore result set; the explicitly deferred
     * editable PIF branch remains a deterministic Abort in that facade. */
    return bx_ntvdm_opennt_rc_message_box(error, first, second, options);
}

BOOL GetPIFData(PIF_DATA *data, char *pif_name)
{
    /* nt_pif.c historically consults its process-global pfdata in addition
     * to the argument.  Preserve that coupling within this translated unit
     * without exposing a second, incompatible PIF_DATA declaration. */
    memcpy(&bx_ntvdm_command_pif_parser_global, data,
        sizeof(bx_ntvdm_command_pif_parser_global));
    return bx_ntvdm_command_pif_parse_original(
        (BX_NTVDM_OPENNT_PARSER_PIF_DATA *)data, pif_name);
}

BOOL bx_ntvdm_command_pif_select_config_files(char *pif_name,
    char *config_path, char *autoexec_path)
{
    PIF_DATA data;
    if (pif_name == NULL || config_path == NULL || autoexec_path == NULL) return FALSE;
    memset(&data, 0, sizeof(data));
    /* This is deliberately before cmdGetNextCmd/cmdCheckForPIF, whose
     * historical IgnoreConfigAutoexec setting prevents a second selection. */
    if (!GetPIFData(&data, pif_name)) return FALSE;
    bx_ntvdm_command_pif_original_get_config_files(TRUE, config_path);
    bx_ntvdm_command_pif_original_get_config_files(FALSE, autoexec_path);
    free(data.CmdLine);
    free(data.StartDir);
    free(data.StartFile);
    free(data.WinTitle);
    return TRUE;
}
