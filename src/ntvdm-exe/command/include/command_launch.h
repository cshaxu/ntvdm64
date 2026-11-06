#ifndef NTVDM_COMMAND_LAUNCH_H
#define NTVDM_COMMAND_LAUNCH_H
#include <windows.h>
#include <stddef.h>

typedef struct ntvdm_command_launch ntvdm_command_launch;

/* Inputs have already crossed the original command/encoding boundary. The
 * caller supplies the selected service branch, not an inferred program name.
 * Environment includes its two terminating NULs; directory is absolute so a
 * later process CWD change cannot redirect this request. Both capabilities
 * come from the authenticated VM launch context. Streams are resolved local
 * handles, never guest addresses or transported numeric handle values.
 * Prepare runs while the source handles and bound session are still valid.
 * On success all inputs are copied and retained independently of the caller. */
DWORD ntvdm_command_launch_prepare(unsigned int service, const char *command,
    const char *environment, size_t environment_bytes, const char *directory,
    const HANDLE streams[3], HANDLE frontend, HANDLE execution,
    ntvdm_command_launch **out);
/* Single use, including failed starts. Returned process/thread handles belong
 * to the caller, which coordinates completion through NTSRV. This object does
 * not wait, terminate, retry, or modify the worker's upstream binding.
 * Destroy only after the start caller has returned. */
DWORD ntvdm_command_launch_start(ntvdm_command_launch *, DWORD flags,
    PROCESS_INFORMATION *out);
void ntvdm_command_launch_destroy(ntvdm_command_launch *);
#endif
