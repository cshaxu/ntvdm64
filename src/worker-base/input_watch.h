#ifndef WORKER_BASE_INPUT_WATCH_H
#define WORKER_BASE_INPUT_WATCH_H
#include <windows.h>

typedef struct worker_base_input_watch worker_base_input_watch;
typedef DWORD (*worker_base_input_ready_fn)(void *context);
typedef void (*worker_base_input_shutdown_fn)(void *context);
typedef void (*worker_base_input_failure_fn)(void *context,DWORD error);

/* Borrow cancellation/context until destroy joins. NULL ready callback exposes
 * the stable wake to an existing consumer; otherwise callback consumes input.
 * Both consumers acknowledge after their actual read, never merely on wake. */
worker_base_input_watch *worker_base_input_watch_create(HANDLE shutdown,HANDLE stop,
    worker_base_input_ready_fn ready,worker_base_input_shutdown_fn close,
    worker_base_input_failure_fn failed,void *context);
/* Duplicate a wait-only source; NULL disables it. No owner lock is acquired. */
DWORD worker_base_input_watch_bind(worker_base_input_watch *,HANDLE source);
DWORD worker_base_input_watch_ack(worker_base_input_watch *);
HANDLE worker_base_input_watch_event(worker_base_input_watch *);
/* Owner cancels callback transport first; never join under its callback lock. */
void worker_base_input_watch_destroy(worker_base_input_watch *);
#endif
