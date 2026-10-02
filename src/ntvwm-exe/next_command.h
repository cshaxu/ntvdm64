#ifndef NTVWM_NEXT_COMMAND_H
#define NTVWM_NEXT_COMMAND_H

#include <windows.h>

/* Project-added native analogue of the worker-side GetNextVDMCommand shape.
 * Attachments are broker-authenticated, borrowed only until dispose/transfer;
 * this is neither a DOS/WOW record nor a frontend transport implementation. */
typedef struct ntvwm_next_command {
    HANDLE channel;
    HANDLE sender;
    HANDLE execution;
    HANDLE frontend;
    DWORD request;
} ntvwm_next_command;

/* Native worker equivalent of the original worker's blocking get-next step.
 * The underlying transport may wait on a channel, but that is not NTVWM's
 * execution contract. */
DWORD ntvwm_get_next_command(ntvwm_next_command *);
DWORD ntvwm_complete_next_command(DWORD request,DWORD exit_code);
void ntvwm_dispose_next_command(ntvwm_next_command *);

#endif
