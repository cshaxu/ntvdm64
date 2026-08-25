#include "opennt_command_composition.h"

#include <limits.h>
#include <stdlib.h>

static const CHAR runtime_empty_multisz[2] = { '\0', '\0' };

PWCHAR runtime_command_environment_snapshot_session(
    const runtime_command_misc_session *session)
{
    const CHAR *source = runtime_empty_multisz;
    uint32_t bytes = sizeof(runtime_empty_multisz);
    int characters;
    PWCHAR snapshot;
    if (session != NULL && runtime_command_misc_session_valid(session) &&
        session->input.environment != NULL &&
        session->input.environment_bytes >= 2u) {
        source = session->input.environment;
        bytes = session->input.environment_bytes;
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

PWCHAR runtime_command_environment_snapshot(void)
{
    return runtime_command_environment_snapshot_session(
        runtime_command_misc_active_session());
}
void runtime_command_environment_free_snapshot(PWCHAR snapshot) { free(snapshot); }

BOOL fSeparateWow;

int RcMessageBox(UINT error, PVOID first, PVOID second, UINT flags)
{
    return runtime_opennt_rc_message_box(error, (CHAR *)first,
        (CHAR *)second, (ULONG)flags);
}
