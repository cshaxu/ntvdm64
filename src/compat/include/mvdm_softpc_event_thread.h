#ifndef MVDM_SOFTPC_EVENT_THREAD_H
#define MVDM_SOFTPC_EVENT_THREAD_H

#include <windows.h>

/* The worker installs borrowed signal handles before starting the original
 * event thread and clears them after it has stopped. Authorization stays
 * with the composing worker. */
typedef struct mvdm_softpc_terminal_events {
    HANDLE server, worker_close, console_close;
} mvdm_softpc_terminal_events;
extern mvdm_softpc_terminal_events mvdm_softpc_input_terminal_events;
BOOL ntvdm_worker_lifecycle_input_terminal(void);

/* Preserve the original SoftPC event-worker cancellation direction, while
 * making its terminal lifetime explicit for the one-process session model.
 * The HANDLE remains a host-local worker resource; it never enters guest
 * state or the mapping-manager identity space. */
int mvdm_softpc_event_thread_alert_and_join(HANDLE event_thread, BOOL started);

#endif
