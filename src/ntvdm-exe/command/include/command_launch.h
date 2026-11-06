#ifndef NTVDM_COMMAND_LAUNCH_H
#define NTVDM_COMMAND_LAUNCH_H
#include <windows.h>
#include <stddef.h>
#include "mvdm_command_native_child.h"

typedef struct ntvdm_command_launch ntvdm_command_launch;

/* Inputs have already crossed the original command/encoding boundary. The
 * caller supplies the selected service branch, not an inferred program name.
 * Environment includes its two terminating NULs; directory is absolute so a
 * later process CWD change cannot redirect this request. Streams are resolved local
 * handles, never guest addresses or transported numeric handle values.
 * Prepare runs while the source handles and bound session are still valid.
 * On success all inputs are copied and retained independently of the caller. */
DWORD ntvdm_command_launch_prepare(unsigned int service, const char *command,
    const char *environment, size_t environment_bytes, const char *directory,
    const HANDLE streams[3], ntvdm_command_launch **out);
/* CPU admission: reuse original OEM/environment conversion and interpret only
 * -1 as the original unchanged/default standard handle. Inputs stay borrowed
 * until this call returns; success owns the converted data and duplicates. */
DWORD ntvdm_command_launch_prepare_guest(const mvdm_command_capture_view *,
    const HANDLE defaults[3], ntvdm_command_launch **out);
/* Single use, including failed starts. Returned process/thread handles belong
 * to the caller, which coordinates completion through NTSRV. This object does
 * not wait, terminate, retry, or modify the worker's upstream binding.
 * Both capabilities come from the authenticated VM launch context. Borrow
 * them only during start; obtaining them need not delay source capture.
 * Destroy only after the start caller has returned. */
DWORD ntvdm_command_launch_start(ntvdm_command_launch *, HANDLE frontend,
    HANDLE execution, DWORD flags,
    PROCESS_INFORMATION *out);
void ntvdm_command_launch_destroy(ntvdm_command_launch *);
#endif
