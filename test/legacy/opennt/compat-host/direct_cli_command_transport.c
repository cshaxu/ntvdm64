/*
 * Default-disabled direct-CLI replacement for the historical CSR/BaseSrv
 * command transport. It preserves config.c's first PIF query only; it is not
 * a command queue, launcher, PIF parser, or DOS service implementation.
 */
#include <windows.h>
#include <stddef.h>
#include <string.h>

#include "vdmapi.h"

static char direct_cli_pif_path[MAX_PATH];
static BOOL direct_cli_configured;
static BOOL direct_cli_first_vdm;
static BOOL direct_cli_pif_request_seen;

void runner_direct_cli_transport_end(void) {
    memset(direct_cli_pif_path, 0, sizeof(direct_cli_pif_path));
    direct_cli_configured = FALSE;
    direct_cli_first_vdm = FALSE;
    direct_cli_pif_request_seen = FALSE;
}

BOOL runner_direct_cli_transport_begin(const char *pif_path, BOOL first_vdm) {
    size_t path_length;

    runner_direct_cli_transport_end();
    if (pif_path == NULL) {
        return FALSE;
    }

    path_length = strlen(pif_path);
    if (path_length == 0 || path_length >= sizeof(direct_cli_pif_path)) {
        return FALSE;
    }

    memcpy(direct_cli_pif_path, pif_path, path_length + 1);
    direct_cli_configured = TRUE;
    direct_cli_first_vdm = first_vdm ? TRUE : FALSE;
    return TRUE;
}

BOOL APIENTRY GetNextVDMCommand(PVDMINFO info) {
    size_t path_length;
    const USHORT expected_state = ASKING_FOR_PIF | ASKING_FOR_DOS_BINARY;

    if (!direct_cli_configured) {
        return FALSE;
    }

    if (info == NULL) {
        return direct_cli_first_vdm;
    }

    if (info->VDMState != expected_state || direct_cli_pif_request_seen ||
        info->PifFile == NULL) {
        return FALSE;
    }

    path_length = strlen(direct_cli_pif_path) + 1;
    if (path_length > info->PifLen) {
        return FALSE;
    }

    memcpy(info->PifFile, direct_cli_pif_path, path_length);
    direct_cli_pif_request_seen = TRUE;
    return TRUE;
}
