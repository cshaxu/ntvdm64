#ifndef MVDM_VM_CONTINUATION_H
#define MVDM_VM_CONTINUATION_H
#include "mvdm_vm_wake.h"
#include <windows.h>

typedef struct mvdm_vm_continuation mvdm_vm_continuation;
typedef struct {
    uint32_t error, exit_code, reentry;
} mvdm_vm_completion;

/* Begin only inside the actual VMM callback, binding its registered VM and
 * generation before publishing a token. VMM owns all client/return state.
 * Begin returns one executor reference, released even after retirement.
 * Complete records an already authoritative terminal result, then publishes
 * the existing counted wake. It does not decide task outcome or final I/O.
 * A publication failure is returned to the lifecycle owner, not retried or
 * converted into successful delivery. The owner must handle that failure.
 * Only one executor may complete/release its reference; callers stop before
 * process teardown. Reset invalidates pending calls without waiting for them. */
uint32_t mvdm_vm_continuation_begin(uint32_t vm, uint64_t *token,
    mvdm_vm_continuation **executor);
uint32_t mvdm_vm_continuation_complete(mvdm_vm_continuation *,
    const mvdm_vm_completion *);
void mvdm_vm_continuation_release(mvdm_vm_continuation *);
/* Admission failed before returning the token to guest. Remove only this
 * unexposed request; the caller still releases its executor reference. */
void mvdm_vm_continuation_abandon(mvdm_vm_continuation *);
/* Borrowed while the executor reference remains owned. Signaled on VM
 * retirement/reset or final guest consumption, not merely host completion.
 * An off-CPU executor includes it in its wait set and asks the existing
 * lifecycle owner to cancel its task; this event itself terminates nothing. */
HANDLE mvdm_vm_continuation_invalidated(mvdm_vm_continuation *);
/* Immutable identity while the caller owns its executor reference. */
uint64_t mvdm_vm_continuation_generation(const mvdm_vm_continuation *);
uint32_t mvdm_vm_continuation_consume(uint64_t token, uint32_t vm,
    mvdm_vm_completion *);
void mvdm_vm_continuation_retire(uint32_t vm, uint64_t generation);
void mvdm_vm_continuation_reset(void);

/* One owned guest-application job per continuation. The existing VM wake
 * delivers it; ready is borrowed until finish_io and must be waited on only
 * off the CPU/ICA thread, together with the frontend's cancellation handles.
 * Acknowledgement means guest application, not merely copying the bytes.
 * The executor reference must outlive the job and its wait. */
uint32_t mvdm_vm_continuation_post_io(mvdm_vm_continuation *,
    const void *bytes, uint32_t size, uint64_t *ticket, HANDLE *ready);
/* CPU thread: ticket=0 queries identity/extent; otherwise copy that exact job.
 * No pointer into the retained job is returned. */
uint32_t mvdm_vm_continuation_read_io(uint64_t token, uint32_t vm,
    uint64_t *ticket, void *bytes, uint32_t capacity, uint32_t *size);
uint32_t mvdm_vm_continuation_ack_io(uint64_t token, uint32_t vm,
    uint64_t ticket, uint32_t error);
/* Returns the acknowledged result, or explicitly cancels an unacknowledged
 * job with cancellation_error. Zero cannot manufacture an acknowledgement. */
uint32_t mvdm_vm_continuation_finish_io(mvdm_vm_continuation *,
    uint64_t ticket, uint32_t cancellation_error);
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
