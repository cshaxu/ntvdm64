#ifndef WORKER_BASE_PUBLICATION_H
#define WORKER_BASE_PUBLICATION_H
#include <windows.h>

/* Copied opaque complete state, no VGA/Console pointers or worker-kind policy.
 * Offer/active belong to one serialized producer. Callback owns its transport.
 * Never hold that transport lock while disabling/draining or joining.
 * Shutdown is borrowed; owner cancels callback I/O before destroy/join. */
typedef struct worker_base_publication worker_base_publication;
typedef DWORD (*worker_base_publication_send_fn)(void *,const void *,SIZE_T);
/* A source callback creates one immutable complete frame only when the
 * publisher has accepted a dirty notification. The returned allocation is
 * owned by the process heap and becomes the publisher's responsibility. */
typedef DWORD (*worker_base_publication_capture_fn)(void *,void **,SIZE_T *);
worker_base_publication *worker_base_publication_create(worker_base_publication_send_fn,void *,HANDLE);
/* Configure an inactive publisher for a source-owned deferred capture. This
 * preserves the common cadence/teardown owner while keeping source memory and
 * source-specific dirty policy out of worker-base. */
DWORD worker_base_publication_set_capture(worker_base_publication *,worker_base_publication_capture_fn,void *);
/* Records one source dirty notification. It never reads source memory or
 * constructs a frame on the caller's execution context. */
DWORD worker_base_publication_signal(worker_base_publication *);
DWORD worker_base_publication_offer(worker_base_publication *,const void *,SIZE_T,BOOL *);
DWORD worker_base_publication_active(worker_base_publication *,BOOL);
/* Quiesced owner's final/current state. Requires inactive admission and no
 * concurrent callback; always delivers the owner-supplied complete frame. */
DWORD worker_base_publication_commit(worker_base_publication *,const void *,SIZE_T);
void worker_base_publication_destroy(worker_base_publication *);
#endif
