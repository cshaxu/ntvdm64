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

/* Existing original-host and worker shutdown meet through native signals.
 * The worker observes the operation and owns the product-specific response. */
typedef enum mvdm_softpc_exit_operation {
    MVDM_EXIT_CONSOLE_GROUP,
    MVDM_EXIT_PARTICIPANT,
    MVDM_EXIT_STATUS,
    MVDM_EXIT_STATUS_ON_LOSS,
    MVDM_EXIT_CLOSE_FRONTENDS,
    MVDM_EXIT_CONSOLE_CONTROL,
    MVDM_WOW_STARTED,
    MVDM_WOW_BIND,
    MVDM_COMMAND_CHILD
} mvdm_softpc_exit_operation;
struct compat_command_child_request;

typedef struct mvdm_softpc_exit_handoff {
    HANDLE request, completion;
    mvdm_softpc_exit_operation operation;
    DWORD status;
    ULONG wow_request;
    USHORT wow_task;
    struct compat_command_child_request *command_child;
    SRWLOCK lock;
    BOOL console_issued;
    DWORD console_status;
} mvdm_softpc_exit_handoff;
extern mvdm_softpc_exit_handoff mvdm_softpc_exit_request;
extern volatile LONG mvdm_softpc_exit_owner;
extern BOOL mvdm_softpc_exit_original_ready;
DWORD mvdm_softpc_request_console_exit(void);
typedef enum mvdm_softpc_exit_permission {
    MVDM_EXIT_UNMANAGED,
    MVDM_EXIT_WORKER_GRANTED,
    MVDM_EXIT_SERVICE_LOST,
    MVDM_EXIT_CONSOLE_GRANTED
} mvdm_softpc_exit_permission;
mvdm_softpc_exit_permission mvdm_softpc_wait_for_exit(void);
DWORD mvdm_softpc_exchange_exit(mvdm_softpc_exit_operation, DWORD *reply);
DWORD WINAPI NtvdmAuthorizeExit(DWORD exit_code);

/* Preserve the original SoftPC event-worker cancellation direction, while
 * making its terminal lifetime explicit for the one-process session model.
 * The HANDLE remains a host-local worker resource; it never enters guest
 * state or the mapping-manager identity space. */
int mvdm_softpc_event_thread_alert_and_join(HANDLE event_thread, BOOL started);

#endif
