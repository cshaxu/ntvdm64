#ifndef NTW32_NEXT_COMMAND_H
#define NTW32_NEXT_COMMAND_H

#include <windows.h>

/* Project-added native analogue of the worker-side GetNextVDMCommand shape.
 * Attachments are broker-authenticated, borrowed only until dispose/transfer;
 * this is neither a DOS/WOW record nor a frontend transport implementation. */
typedef struct ntw32_next_command {
    HANDLE channel;
    HANDLE sender;
    HANDLE execution;
    HANDLE frontend;
    DWORD request;
} ntw32_next_command;

/* Native worker equivalent of the original worker's blocking get-next step.
 * The underlying transport may wait on a channel, but that is not NTW32's
 * execution contract. */
DWORD ntw32_get_next_command(ntw32_next_command *);
DWORD ntw32_complete_next_command(DWORD request,DWORD exit_code);
void ntw32_dispose_next_command(ntw32_next_command *);

#endif
