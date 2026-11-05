#ifndef NTVDM_VM_HOST_H
#define NTVDM_VM_HOST_H
#include "vm_frontend.h"

typedef struct ntvdm_vm_host ntvdm_vm_host;

/* The worker owns this object until all callers have stopped. Registry control
 * is serialized here; I/O callbacks must not reenter these control functions. */
DWORD ntvdm_vm_host_create(ntvdm_vm_host **);
DWORD ntvdm_vm_payload_begin(ntvdm_vm_host *,uint64_t *payload);
DWORD ntvdm_vm_payload_end(ntvdm_vm_host *,uint64_t payload);
DWORD ntvdm_vm_host_register(ntvdm_vm_host *,uint64_t payload,uint64_t vm,HANDLE input_ready,
    const ntvdm_vm_io *,void *context,common_frontend_location *);
DWORD ntvdm_vm_host_origin(ntvdm_vm_host *,const common_frontend_location *,HANDLE *);
DWORD ntvdm_vm_host_retire(ntvdm_vm_host *,const common_frontend_location *);
/* Terminal: forbids new admission, joins all callbacks, retains the object so
 * racing control callers receive cancellation rather than a freed pointer. */
DWORD ntvdm_vm_host_close(ntvdm_vm_host *);
/* Only after control callers have stopped. A failure retains the object. */
DWORD ntvdm_vm_host_destroy(ntvdm_vm_host *);
#endif
