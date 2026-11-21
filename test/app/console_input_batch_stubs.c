/* Controlled queue fixture has no live worker or text-video renderer. */
#include <windows.h>
#include "worker-base/connection.h"
#include "ntvdm-exe/win32/worker_lifecycle.h"
#include "compat/include/mvdm_softpc_text_video.h"

int mvdm_softpc_text_video_local(void) { return 0; }
DWORD ntvdm_worker_lifecycle_fault_event(HANDLE *event)
{
    if (event) *event = NULL;
    return ERROR_NOT_SUPPORTED;
}
DWORD worker_base_shutdown_event(HANDLE *event)
{ if (event) *event = NULL; return ERROR_NOT_SUPPORTED; }
DWORD worker_base_io_checkpoint(DWORD reason, DWORD request, DWORD *decision)
{ (void)reason; (void)request; (void)decision; return ERROR_NOT_SUPPORTED; }
DWORD worker_base_io_open(HANDLE *pipe, HANDLE *peer, HANDLE *ready, DWORD *generation)
{ (void)pipe; (void)peer; (void)ready; (void)generation; return ERROR_NOT_SUPPORTED; }
DWORD worker_base_io_close(HANDLE *pipe, HANDLE *peer, HANDLE *ready)
{ (void)pipe; (void)peer; (void)ready; return ERROR_NOT_SUPPORTED; }

/* This controlled queue never enters CCPU/ICA. Keep the production composition
 * linkable, but fail if a future test path accidentally crosses that boundary. */
void host_ica_lock(void) { RaiseFailFastException(NULL, NULL, 0); }
void host_ica_unlock(void) { RaiseFailFastException(NULL, NULL, 0); }
void DoMouseInterrupt(void) { RaiseFailFastException(NULL, NULL, 0); }
