#ifndef MVDM_STANDALONE_WORKER_H
#define MVDM_STANDALONE_WORKER_H

#include <setjmp.h>
#include <windows.h>
#include "ntvdm-exe/frontend/vm_host.h"

/* The standalone process binding is deliberately finite: it owns only data
 * that NT4 process creation/CSRSS supplied to this one worker address space.
 * It does not classify images, own command records or schedule VDM work. */
DWORD mvdm_standalone_worker_begin(void);
jmp_buf *mvdm_standalone_worker_termination_escape(void);
DWORD mvdm_standalone_worker_completion_code(void);
int mvdm_standalone_worker_finish(int result);
/* Host bridge control only. Never infer VM identity from the current CPU or
 * borrow a guest pointer. The owner outlives original worker shutdown. */
ntvdm_vm_host *mvdm_standalone_worker_frontends(void);
DWORD mvdm_standalone_worker_close_frontends(void);
struct mvdm_vm_executor;
struct mvdm_vm_executor *mvdm_standalone_worker_executor(void);

#endif
