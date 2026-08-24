#include "opennt_command_composition.h"

#include <limits.h>
#include <stdlib.h>

static const CHAR bx_ntvdm_empty_multisz[2] = { '\0', '\0' };

PWCHAR bx_ntvdm_command_environment_snapshot_session(
    const bx_ntvdm_command_misc_session *session)
{
    const CHAR *source = bx_ntvdm_empty_multisz;
    uint32_t bytes = sizeof(bx_ntvdm_empty_multisz);
    int characters;
    PWCHAR snapshot;
    if (session != NULL && bx_ntvdm_command_misc_session_valid(session) &&
        session->command_source_environment != NULL &&
        session->command_source_environment_bytes >= 2u) {
        source = session->command_source_environment;
        bytes = session->command_source_environment_bytes;
    }
    if (bytes > (uint32_t)INT_MAX || source[bytes - 2u] != '\0' ||
        source[bytes - 1u] != '\0') return NULL;
    characters = MultiByteToWideChar(CP_ACP, 0, source, (int)bytes, NULL, 0);
    if (characters <= 0) return NULL;
    snapshot = (PWCHAR)malloc((size_t)characters * sizeof(WCHAR));
    if (snapshot == NULL) return NULL;
    if (MultiByteToWideChar(CP_ACP, 0, source, (int)bytes, snapshot,
            characters) != characters) { free(snapshot); return NULL; }
    return snapshot;
}

PWCHAR bx_ntvdm_command_environment_snapshot(void)
{
    return bx_ntvdm_command_environment_snapshot_session(
        bx_ntvdm_command_misc_active_session());
}
void bx_ntvdm_command_environment_free_snapshot(PWCHAR snapshot) { free(snapshot); }

BOOL fSeparateWow;
CHAR comspec[] = "COMSPEC=";
CHAR *lpszzInitEnvironment;
WORD cchInitEnvironment;
CHAR *lpszzVDMEnv32;
DWORD cchVDMEnv32;

int RcMessageBox(UINT error, PVOID first, PVOID second, UINT flags)
{
    return bx_ntvdm_opennt_rc_message_box(error, (CHAR *)first,
        (CHAR *)second, (ULONG)flags);
}
