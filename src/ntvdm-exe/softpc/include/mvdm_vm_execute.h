#ifndef MVDM_VM_EXECUTE_H
#define MVDM_VM_EXECUTE_H
#include "mvdm_vm_continuation.h"
#include "ntvdm-exe/command/include/command_launch.h"
#include "ntvdm-exe/frontend/vm_host.h"

/* Off-CPU executor only. All inputs remain owned by the caller until return;
 * frontend providers must remain alive throughout, including retirement.
 * Run16 reports NTSRV's terminal result. VM invalidation requests frontend
 * retirement through NTSRV; this helper never terminates a native process.
 * The caller completes/releases the continuation and destroys launch data. */
DWORD mvdm_vm_command_execute(mvdm_vm_continuation *, ntvdm_command_launch *,
    ntvdm_vm_host *, const common_frontend_location *, DWORD *exit_code);
#endif
