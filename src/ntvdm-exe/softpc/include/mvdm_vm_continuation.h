#ifndef MVDM_VM_CONTINUATION_H
#define MVDM_VM_CONTINUATION_H
#include "compat/include/mvdm_vm_continuation_store.h"

/* CPU-side bounded descriptor transport, reached only after bridge signature
 * and ring checks. Uses the same continuation owner as native publication. */
DWORD mvdm_vm_application_bop(uint32_t operation, uint32_t vm,
    uint32_t descriptor_address, uint32_t descriptor_extent);
/* Channel/executor thread only, never CCPU/ICA. Borrow up to four lifecycle
 * handles (channel stop, worker, VM retirement, broker); NULL entries omitted.
 * The executor reference remains owned by the caller throughout. A guest ack
 * that already won remains authoritative; cancellation is never an ack. */
DWORD mvdm_vm_application_publish(mvdm_vm_continuation *, const void *, uint32_t,
    const HANDLE cancellation[4]);
/* Channel-thread publication boundary. Commit the private model only after
 * the exact copied frame has been applied by the calling VM. Any failure
 * discards the candidate; unsupported dispatch operations remain errors. */
struct ntvdm_vm_text;
DWORD mvdm_vm_application_commit(mvdm_vm_continuation *, struct ntvdm_vm_text *,
    const HANDLE cancellation[4]);
struct console_io_request;
struct console_io_reply;
/* Serving-thread output dispatch. Transport errors are returned; an operation
 * failure is reported in reply.error/result. PUBLICATION_END acknowledges
 * only after the exact frame is guest-applied. Other unsupported operations
 * remain errors; this is not a substitute input or graphics provider. */
DWORD mvdm_vm_application_dispatch(mvdm_vm_continuation *, struct ntvdm_vm_text *,
    const HANDLE cancellation[4], const struct console_io_request *,
    struct console_io_reply *);
#endif
