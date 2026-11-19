#ifndef MVDM_STANDALONE_WORKER_H
#define MVDM_STANDALONE_WORKER_H

#include <windows.h>
#include "ntvdm-exe/frontend/vm_host.h"

/* Host bridge control only. Never infer VM identity from the current CPU or
 * borrow a guest pointer. The owner outlives original worker shutdown. */
ntvdm_vm_host *mvdm_standalone_worker_frontends(void);
DWORD mvdm_standalone_worker_close_frontends(void);
struct mvdm_vm_executor;
struct mvdm_vm_executor *mvdm_standalone_worker_executor(void);

#endif
