#ifndef MVDM_SOFTPC_EVENT_THREAD_H
#define MVDM_SOFTPC_EVENT_THREAD_H

#include <windows.h>

/* Preserve the original SoftPC event-worker cancellation direction, while
 * making its terminal lifetime explicit for the one-process session model.
 * The HANDLE remains a host-local worker resource; it never enters guest
 * state or the mapping-manager identity space. */
int mvdm_softpc_event_thread_alert_and_join(HANDLE event_thread);

#endif
