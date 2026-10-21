/* Project-private state returned by the existing RetireWorkerlessFrontend
 * scalar RPC.  It describes a frontend lifecycle action, never a worker or
 * guest task state. */
#ifndef OPENNT_FRONTEND_LIFECYCLE_H
#define OPENNT_FRONTEND_LIFECYCLE_H

/* Only service bootstrap, empty-service retirement and Console-close grace
 * use this duration. Ordinary RPC and resource joins have no deadline. */
#define OPENNT_LIFECYCLE_GRACE_MS 5000u

enum opennt_frontend_retire_action {
    OPENNT_FRONTEND_RETIRE_NONE=0,
    OPENNT_FRONTEND_RETIRE_FINAL_CLOSE=1,
    OPENNT_FRONTEND_RETIRE_RETURN_CONSOLE=2,
    OPENNT_FRONTEND_RETIRE_PARK=3
};

#endif
