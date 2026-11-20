#ifndef COMPAT_MVDM_SOFTPC_WORKER_EXIT_H
#define COMPAT_MVDM_SOFTPC_WORKER_EXIT_H

struct mvdm_vm_rundown;
/* Borrowed before CPU entry; clear after original shutdown, before freeing. */
extern struct mvdm_vm_rundown *mvdm_softpc_exit_rundown;
void ntvdm_worker_lifecycle_authorize_exit(void);

#endif
