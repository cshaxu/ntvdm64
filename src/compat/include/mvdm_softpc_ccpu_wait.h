#ifndef MVDM_SOFTPC_CCPU_WAIT_H
#define MVDM_SOFTPC_CCPU_WAIT_H

#include <windows.h>

/*
 * Process-local wake carrier for the selected original CCPU executor.
 * CCPU's own interrupt map remains the sole source of pending-work truth;
 * this event only lets its HLT path yield the host CPU until that map changes.
 */
BOOL mvdm_softpc_ccpu_wait_begin(void);
void mvdm_softpc_ccpu_wait_end(void);
void mvdm_softpc_ccpu_wait_signal(void);
void mvdm_softpc_ccpu_wait_for_event(void);

#endif
