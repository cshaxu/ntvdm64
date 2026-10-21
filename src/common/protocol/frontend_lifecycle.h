/* Project-private state returned by the existing RetireWorkerlessFrontend
 * scalar RPC.  It describes a frontend lifecycle action, never a worker or
 * guest task state. */
#ifndef OPENNT_FRONTEND_LIFECYCLE_H
#define OPENNT_FRONTEND_LIFECYCLE_H

/* One bounded wait governs frontend admission, management Console return,
 * ordered worker close and local frontend-thread teardown.  These are all
 * parts of one authenticated frontend lifecycle, so they must not quietly
 * drift into different timeout policies in NTSRV and NTCON. */
#define OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS 10000u

enum opennt_frontend_retire_action {
    OPENNT_FRONTEND_RETIRE_NONE=0,
    OPENNT_FRONTEND_RETIRE_FINAL_CLOSE=1,
    OPENNT_FRONTEND_RETIRE_RETURN_CONSOLE=2,
    OPENNT_FRONTEND_RETIRE_PARK=3
};

#endif
