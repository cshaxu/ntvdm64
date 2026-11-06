#ifndef NTVDM_COMMAND_TEXT_H
#define NTVDM_COMMAND_TEXT_H

#include "compat/include/nt.h"

/* Original cmdCreateProcess preparation, shared with saved-VM requests.
 * Inputs remain borrowed and unchanged. A NULL environment preserves native
 * inheritance. On success the caller releases both strings with RtlFreeAnsiString;
 * on failure both outputs are empty. No process, guest or scheduling state changes. */
DWORD ntvdm_command_prepare_text(const char *command, const char *environment,
    PANSI_STRING command_out, PANSI_STRING environment_out);

#endif
