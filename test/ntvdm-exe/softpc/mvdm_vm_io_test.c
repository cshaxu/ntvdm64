/* Compile the real provider with one failure seam, confined to this fixture. */
#include "mvdm_vm_io.h"
extern HANDLE vm_io_reset_failure;
static BOOL reset_ready(HANDLE ready)
{
    if (ready == vm_io_reset_failure) {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    return ResetEvent(ready);
}
#define ResetEvent reset_ready
#include "ntvdm-exe/softpc/mvdm_vm_io.c"
