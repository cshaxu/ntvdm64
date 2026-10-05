#ifndef WORKER_BASE_PUBLICATION_H
#define WORKER_BASE_PUBLICATION_H
#include <windows.h>

/* Copied opaque complete state, no VGA/Console pointers or worker-kind policy.
 * Offer/active belong to one serialized producer. Callback owns its transport.
 * Never hold that transport lock while disabling/draining or joining.
 * Shutdown is borrowed; owner cancels callback I/O before destroy/join. */
typedef struct worker_base_publication worker_base_publication;
typedef DWORD (*worker_base_publication_send_fn)(void *,const void *,SIZE_T);
worker_base_publication *worker_base_publication_create(worker_base_publication_send_fn,void *,HANDLE);
DWORD worker_base_publication_offer(worker_base_publication *,const void *,SIZE_T,BOOL *);
DWORD worker_base_publication_active(worker_base_publication *,BOOL);
/* Quiesced owner's final/current state. Requires inactive admission and no
 * concurrent callback; uses the same last-successful comparison. */
DWORD worker_base_publication_commit(worker_base_publication *,const void *,SIZE_T);
void worker_base_publication_destroy(worker_base_publication *);
#endif
