#ifndef NTVWM_NEXT_COMMAND_H
#define NTVWM_NEXT_COMMAND_H

#include <windows.h>

/* Project-added native analogue of the worker-side GetNextVDMCommand shape.
 * Payload and attachments are recipient-owned until dispose/transfer;
 * this is neither a DOS/WOW record nor a frontend transport implementation. */
typedef struct ntvwm_next_command {
    BYTE *payload;
    DWORD bytes;
    HANDLE sender;
    HANDLE execution;
    HANDLE frontend;
    DWORD request;
    DWORD caller_generation;
} ntvwm_next_command;

/* Native worker equivalent of the original worker's blocking get-next step.
 * Commands are copied through authenticated NTSRV RPC, not a control pipe. */
DWORD ntvwm_get_next_command(ntvwm_next_command *);
DWORD ntvwm_complete_next_command(DWORD request,DWORD exit_code);
DWORD ntvwm_complete_native_request(DWORD request,DWORD exit_code,DWORD io_error,DWORD io_flags);
void ntvwm_dispose_next_command(ntvwm_next_command *);

#endif
