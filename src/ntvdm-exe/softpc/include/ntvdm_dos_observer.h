#ifndef NTVDM_DOS_OBSERVER_H
#define NTVDM_DOS_OBSERVER_H
#include "worker-base/task_observation.h"
#include "ntvdm-exe/session/session.h"
#define DOS_OBSERVER_SLOTS 256u
typedef struct ntvdm_dos_occurrence {
    uint64_t occurrence,direct;
    uint16_t psp;
    BOOL active;
} ntvdm_dos_occurrence;
typedef struct ntvdm_dos_observer {
    worker_task_observer outbox;
    session *owner;
    DWORD thread;
    HANDLE registration;
    uint64_t sequence;
    ntvdm_dos_occurrence slots[DOS_OBSERVER_SLOTS];
    unsigned char environment[8192];
} ntvdm_dos_observer;
DWORD ntvdm_dos_observer_start(ntvdm_dos_observer *,session *);
void ntvdm_dos_observer_stop(ntvdm_dos_observer *);
#endif
