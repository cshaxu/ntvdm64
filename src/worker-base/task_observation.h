#ifndef WORKER_BASE_TASK_OBSERVATION_H
#define WORKER_BASE_TASK_OBSERVATION_H
#include <windows.h>
#include "common/protocol/dos_observation.h"
#define WORKER_OBSERVATION_CAPACITY 128u
/* Distinct ordered facts cannot use the latest-frame publication queue.
 * Producer callbacks are serialized by their original execution owner.
 * Publish runs on the owned consumer; cancel must unblock that owner's
 * current transport call without freeing its pending input/state. */
typedef DWORD (*worker_observation_publish)(void *,const common_dos_observation *,BOOL);
typedef void (*worker_observation_cancel)(void *);
typedef struct worker_task_observer {
    CRITICAL_SECTION lock;
    HANDLE thread,wake,stop,drained;
    common_dos_observation queue[WORKER_OBSERVATION_CAPACITY];
    uint32_t head,count;
    LONG lost;
    worker_observation_publish publish;
    worker_observation_cancel cancel;
    void *context;
} worker_task_observer;
/* Start before producer registration. Context outlives stop/join. */
DWORD worker_task_observer_start(worker_task_observer *,worker_observation_publish,
    worker_observation_cancel,void *);
/* No wait/allocation/RPC; FALSE records a gap, never an execution failure. */
BOOL worker_task_observer_offer(worker_task_observer *,const common_dos_observation *);
/* After producer quiescence, normal teardown may wait finitely for accepted
 * facts to finish publication. This does not certify missing/failed reports. */
DWORD worker_task_observer_drain(worker_task_observer *,DWORD timeout);
/* Quiesce/unregister producers first; join before releasing transport/session. */
void worker_task_observer_stop(worker_task_observer *);
#endif
