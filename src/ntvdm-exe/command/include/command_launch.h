#ifndef NTVDM_COMMAND_LAUNCH_H
#define NTVDM_COMMAND_LAUNCH_H
#include <windows.h>
#include "compat/include/mvdm_command_launch.h"

BOOL opennt_command_create_process_a(
    LPCSTR application_name,
    LPSTR command_line,
    LPSECURITY_ATTRIBUTES process_attributes,
    LPSECURITY_ATTRIBUTES thread_attributes,
    BOOL inherit_handles,
    DWORD creation_flags,
    LPVOID environment,
    LPCSTR current_directory,
    LPSTARTUPINFOA startup_info,
    LPPROCESS_INFORMATION process_information);


BOOL ntvdm_command_initialize(void);

/* Single use, including failed starts. Returned process/thread handles belong
 * to the caller, which coordinates completion through NTSRV. This object does
 * not wait, terminate, retry, or modify the worker's upstream binding.
 * Both capabilities come from the authenticated VM launch context. Borrow
 * them only during start; obtaining them need not delay source capture.
 * Destroy only after the start caller has returned. */
DWORD ntvdm_command_launch_start(ntvdm_command_launch *, HANDLE frontend,
    HANDLE execution, DWORD flags,
    PROCESS_INFORMATION *out);
#endif
