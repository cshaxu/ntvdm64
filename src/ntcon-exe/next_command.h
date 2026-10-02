#ifndef NTCON_NEXT_COMMAND_H
#define NTCON_NEXT_COMMAND_H

#include <windows.h>

/* Project-added native analogue of the worker-side GetNextVDMCommand shape.
 * Attachments are broker-authenticated, borrowed only until dispose/transfer;
 * this is neither a DOS/WOW record nor a frontend transport implementation. */
typedef struct ntcon_next_command {
    HANDLE channel;
    HANDLE sender;
    HANDLE execution;
    HANDLE frontend;
    DWORD request;
} ntcon_next_command;

/* Native worker equivalent of the original worker's blocking get-next step.
 * The underlying transport may wait on a channel, but that is not NTCON's
 * execution contract. */
DWORD ntcon_get_next_command(ntcon_next_command *);
DWORD ntcon_complete_next_command(DWORD request,DWORD exit_code);
void ntcon_dispose_next_command(ntcon_next_command *);

#endif
