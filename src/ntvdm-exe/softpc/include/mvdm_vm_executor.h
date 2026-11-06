#ifndef MVDM_VM_EXECUTOR_H
#define MVDM_VM_EXECUTOR_H
#include "mvdm_vm_execute.h"
#include "mvdm_vm_io.h"

typedef struct mvdm_vm_executor mvdm_vm_executor;
/* Worker owns the executor and borrowed host. Create before CPU entry; close
 * off CPU/ICA before closing/destroying the host. Closing cancels continuations
 * and joins admitted jobs; NTSRV still owns all child/chain termination. */
DWORD mvdm_vm_executor_create(ntvdm_vm_host *, mvdm_vm_executor **);
void mvdm_vm_executor_close(mvdm_vm_executor *);
void mvdm_vm_executor_destroy(mvdm_vm_executor *);
/* CPU admission: independently prepared launch/text transfer only on success.
 * No RPC or native wait occurs here. Output token is published only after a
 * real executor is queued. The caller retains both objects on any failure. */
DWORD mvdm_vm_executor_submit(mvdm_vm_executor *, uint32_t vm,
    ntvdm_command_launch *, ntvdm_vm_text *, uint64_t *token);
/* A translated guest event, scoped to this exact live request token. */
DWORD mvdm_vm_executor_key(mvdm_vm_executor *, uint64_t token,
    const KEY_EVENT_RECORD *);
#endif
