#ifndef COMPAT_MVDM_COMMAND_LAUNCH_H
#define COMPAT_MVDM_COMMAND_LAUNCH_H

#include <windows.h>
#include <stddef.h>

typedef struct ntvdm_command_launch ntvdm_command_launch;
struct mvdm_command_capture_view;

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
DWORD ntvdm_command_launch_prepare_guest(const struct mvdm_command_capture_view *,
    const HANDLE defaults[3], ntvdm_command_launch **out);
void ntvdm_command_launch_destroy(ntvdm_command_launch *);

#endif
