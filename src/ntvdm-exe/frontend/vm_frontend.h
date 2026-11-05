#ifndef NTVDM_VM_FRONTEND_H
#define NTVDM_VM_FRONTEND_H

#include "common/frontend-base/channel.h"
#include "common/protocol/frontend_instance.h"

typedef struct ntvdm_vm_frontends ntvdm_vm_frontends;

/* Registry control is serialized by the payload owner. I/O callbacks run on
 * the instance's channel thread and must not call registry control or retain
 * protocol buffer pointers. Context and input_ready remain borrowed until
 * retirement succeeds. No guest address may be used as a callback context. */
typedef struct ntvdm_vm_io {
    DWORD (*dispatch)(void *,const console_io_request *,console_io_reply *,frontend_base_result *);
    DWORD (*input_pending)(void *,BOOL *);
    void (*finish)(void *);
} ntvdm_vm_io;

DWORD ntvdm_vm_frontends_create(uint64_t payload,ntvdm_vm_frontends **);
/* The returned identity must be retained even on a post-registration error:
 * the registry owns rollback and retire remains required before context free. */
DWORD ntvdm_vm_frontend_register(ntvdm_vm_frontends *,uint64_t vm,HANDLE input_ready,
    const ntvdm_vm_io *,void *context,common_frontend_location *);
/* Returns an owned, wait-only capability for the existing launch/context RPC.
 * This does not bind or change the host worker's upstream execution context. */
DWORD ntvdm_vm_frontend_origin(ntvdm_vm_frontends *,const common_frontend_location *,HANDLE *);
DWORD ntvdm_vm_frontend_retire(ntvdm_vm_frontends *,const common_frontend_location *);
/* Failure retains the registry and any contexts still borrowed by callbacks. */
DWORD ntvdm_vm_frontends_destroy(ntvdm_vm_frontends *);

#endif
