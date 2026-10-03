/* NTSRV-private project adaptation state. Not a wire/public ABI.
 * One instance owns all lists and the existing recursive service lock.
 * Public service entries retain their original locking; service_* helpers
 * retain caller-held lock contracts. No new lock, registry or scheduler.
 * srvvdm owns DOS/WOW lists and locks; acquire service lock before those locks.
 * Process watches hold their own process copies. Connection rundown joins
 * connection-owned watches outside the lock before freeing the connection.
 * Service Stop requires RPC calls/rundowns and worker watches drained. */
#ifndef NTSRV_SERVICE_INTERNAL_H
#define NTSRV_SERVICE_INTERNAL_H
#include "basesrv.h"
#include <base_service.h>
#include <base_process.h>
#include <base_dispatch.h>
#include <base_reservation.h>
#include <base_command.h>
#include <base_wait.h>
#include <base_stream.h>
#include <base_interactive.h>
#include <base_event.h>
#include <base_config.h>
#include "ntsrv-exe/transport/vdm_receipt.h"
#include "ntsrv-exe/transport/worker_spawn.h"
#include "ntsrv-exe/transport/frontend_admission.h"
#include "run16-exe/frontend_bootstrap.h"
#include "common/codec/native_launch.h"
#include <stdio.h>
#define FRONTEND_STARTUP_DEADLINE_MS 10000u
typedef NTSTATUS (*OPENNT_USER_TEST_TOKEN_FOR_INTERACTIVE)(HANDLE,PLUID);
extern OPENNT_USER_TEST_TOKEN_FOR_INTERACTIVE UserTestTokenForInteractive;
/* srvvdm.c owns these source-shaped lists and their lock objects.  They are
 * intentionally not re-declared in a public product ABI: this binding only
 * reads them while constructing a bounded management projection. */
extern PWOWHEAD WOWHead;
extern PCONSOLERECORD DOSHead;
extern RTL_CRITICAL_SECTION BaseSrvWOWCriticalSection;
extern RTL_CRITICAL_SECTION BaseSrvDOSCriticalSection;

struct OPENNT_BASE_SERVICE {
    OPENNT_BASE_PROCESS_REGISTRY registry;
    OPENNT_BASE_RESERVATIONS *reservations;
    CRITICAL_SECTION lock;
    CONDITION_VARIABLE frontend_changed;
    ULONG next_console;
    ULONG next_wait_receipt;
    ULONG next_frontend_join;
    OPENNT_BASE_INTERACTIVE_SCOPE interactive;
    LIST_ENTRY connections;
    LIST_ENTRY retired_connections;
    LIST_ENTRY worker_watches;
    LIST_ENTRY frontend_routes;
    LIST_ENTRY console_contexts;
    HANDLE frontend_lifetime_changed;
    OPENNT_BASE_EMPTY_NOTIFY empty_notify;
    void *empty_notify_context;
    uint64_t management_epoch;
};
struct OPENNT_BASE_CONNECTION {
    OPENNT_BASE_SERVICE *service;
    CSR_PROCESS process;
    broker_vdm_receipts streams;
    uint64_t reservation;
    ULONG task;
    OPENNT_BASE_WORKER_KIND reservation_kind;
    HANDLE console;
    DWORD *console_members;
    HANDLE *console_member_processes; /* Pinned process identities, not task state. */
    DWORD console_member_count;
    /* A resident worker outlives its direct launcher. Retain the
     * authenticated frontend root's visible-Console identity separately
     * from the worker's own hidden Console membership. This is only a local
     * Console discriminator, never task or process authority. */
    DWORD *execution_console_members;
    HANDLE *execution_console_processes;
    DWORD execution_console_member_count;
    BOOL wow;
    BOOL retired;
    BOOL pending_creation;
    ULONG pending_vdm_binary; /* Original Check result, not caller-selected creation. */
    BOOL vdm_starting;
    BOOL registered_worker;
    BOOL native_worker;
    DWORD selected_native_generation; /* Explicit launcher grant, not membership alone. */
    /* The original BaseSrvDupStandardHandles copies these caller streams
     * into the suspended worker during UpdateDOSEntry.  The standalone
     * broker temporarily owns receipt-backed duplicates until that point;
     * it must release them afterwards so a pipe reader can observe EOF. */
    uint32_t pending_standard_streams[3];
    HANDLE parent_wait; /* Borrowed from this connection's receipt table. */
    uint32_t parent_receipt,completed_receipt;
    DWORD completed_exit_code;
    BOOL dos_completion_read;
    BOOL worker_failed;
    /* Only the original nonzero-DosSesId branch completes on real worker exit.
     * The broker transports that existing result; it does not synthesize a DOS
     * task event or add another task registry. Other branches keep srvvdm waits. */
    BOOL parent_is_worker_exit,vdm_exit_cancelled;
    HANDLE vdm_exit_watch,vdm_exit_process;
    HANDLE wow_start_event;
    BOOL wow_started;
    HANDLE frontend_capability; /* Root lease, independent of command lifetime. */
    uint64_t frontend_console_window,frontend_reserved_window;
    HANDLE frontend_retire,frontend_restored;
    HANDLE frontend_start_process; /* Borrowed only during the active Start RPC. */
    HANDLE frontend_start_capability,frontend_start_retire,frontend_start_restored;
    HANDLE frontend_start_result; /* Start RPC owns event; never exported. */
    DWORD frontend_start_status;
    BOOL frontend_start_reported;
    BOOL frontend_starting;
    BOOL frontend_return_pending,frontend_return_complete;
    BOOL frontend_borrowed,frontend_idle;
    ULONGLONG frontend_admission_deadline;
    ULONGLONG frontend_workerless_deadline; /* NTSRV-owned cancellable grace. */
    DWORD frontend_creator_generation;
    HANDLE frontend_exit_watch,frontend_exit_process;
    OPENNT_BASE_CONNECTION *frontend_join_caller;
    DWORD frontend_join_nonce,frontend_join_pid;
    int frontend_join_decision;
    DWORD retained_frontend_root; /* Authenticated launcher association, not a task. */
    HANDLE frontend_state_changed; /* Root-private auto-reset retirement wake. */
    HANDLE worker_state_changed; /* Per-launcher auto-reset worker transition wake. */
    BOOL frontend_closing; /* Admission barrier, never execution/task state. */
    DWORD frontend_request_root; /* Original pending command asks this root for I/O. */
    DWORD frontend_channel_root; /* One pending direct launcher-to-root attachment. */
    HANDLE frontend_execution;
    BYTE *native_command_payload;
    DWORD native_command_bytes;
    BOOL native_command_pending;
    DWORD channel_worker_generation; /* Zero selects the retained frontend route. */
    HANDLE channel_frontend; /* Authenticated I/O association for worker delivery. */
    /* One Submit RPC owns the event until its scope is cleared under lock.
     * Result resources are owned here until moved to its typed RPC outputs. */
    HANDLE native_start_event,native_start_target,native_start_receipt;
    DWORD native_start_worker,native_start_request,native_start_status;
    BOOL native_start_reported;
    DWORD native_root;
    DWORD native_inflight,native_activity_root;
    /* Project-owned native-text counterpart to BaseSrv's original DOSRECORD
     * projection.  The broker, not the native Console process list, owns
     * logical request depth and the active task label exposed to NTMON. */
    LIST_ENTRY win32records;
    struct OPENNT_BASE_WIN32RECORD *pending_win32record;
    DWORD next_win32record;
    HANDLE native_stop,native_closed;
    LIST_ENTRY service_link;
    LIST_ENTRY retired_link;
};
typedef struct OPENNT_BASE_WIN32RECORD {
    LIST_ENTRY link;
    DWORD request;
    /* Bind only the admitted direct target's actual CreateProcess identity. */
    DWORD process_id;
    DWORD launcher_generation,exit_code,completion_error;
    BOOL completed;
    HANDLE receipt; /* Signalled after NTVWM reports exit and I/O release. */
    DWORD io_error,io_flags; /* Final I/O precedes receipt under the service lock. */
    BOOL startup_delivered;
    WCHAR image[OPENNT_BASE_WORKER_IMAGE_CHARS];
} OPENNT_BASE_WIN32RECORD;
typedef struct OPENNT_FRONTEND_ROUTE {
    LIST_ENTRY link;
    OPENNT_BASE_CONNECTION *root; /* Removed before this connection is freed. */
    HANDLE worker;
    HANDLE pipe;
    HANDLE ready;
    BOOL delivered;
    BOOL native_worker; /* I/O route survives its native launcher, not its root. */
    DWORD request;
} OPENNT_FRONTEND_ROUTE;
typedef struct OPENNT_BASE_WORKER_WATCH {
    LIST_ENTRY link;
    OPENNT_BASE_SERVICE *service;
    CSR_PROCESS process;
    HANDLE wait;
    HANDLE console;
    BOOL wow;
    uint64_t reservation;
    OPENNT_BASE_WORKER_KIND kind;
    FILETIME started;
    /* The original GetNextVDMCommand body frees VDMINFO immediately after
     * copying it to the worker.  These are labels keyed to surviving original
     * DOSRECORD identities.  They never model execution lifetime: srvvdm.c's
     * DOSRecord chain remains the sole source of task depth and state. */
    LIST_ENTRY management_labels;
    BOOL termination_requested;
    HANDLE shutdown;
    BOOL frontend_associated;
} OPENNT_BASE_WORKER_WATCH;
typedef struct OPENNT_BASE_MANAGEMENT_LABEL {
    LIST_ENTRY link;
    PDOSRECORD record;
    HANDLE parent_wait;
    ULONG task;
    WCHAR image[OPENNT_BASE_WORKER_IMAGE_CHARS];
} OPENNT_BASE_MANAGEMENT_LABEL;
typedef struct OPENNT_BASE_CONSOLE_CONTEXT {
    LIST_ENTRY link;
    OPENNT_BASE_CONNECTION *root; /* Removed under service lock before root free. */
    HANDLE capability,console;
} OPENNT_BASE_CONSOLE_CONTEXT;
typedef struct OPENNT_BASE_SERVICE_RESOURCES {
    OPENNT_BASE_CONNECTION *connection;
    DWORD role;
    HANDLE worker;
    OPENNT_BASE_WAIT_BINDING wait;
    OPENNT_BASE_RESOURCE_BINDING binding;
} OPENNT_BASE_SERVICE_RESOURCES;

typedef BOOL (WINAPI *SERVICE_COMPARE_HANDLES)(HANDLE,HANDLE);

typedef NTSTATUS (NTAPI *SERVICE_QUERY_OBJECT)(HANDLE,ULONG,PVOID,ULONG,PULONG);

/* Finite internal linkage only; resource init/release transport original calls,
 * not DOS/WOW policy. Cross-module calls never acquire a second service state. */
void service_signal_frontend_states(OPENNT_BASE_SERVICE *service);
void service_signal_worker_states(OPENNT_BASE_SERVICE *service);
/* Process-watch callback is cross-module even without a direct call. */
VOID CALLBACK service_worker_terminated(PVOID context,BOOLEAN fired);
void service_delete_console_context(OPENNT_BASE_CONSOLE_CONTEXT *context);
BOOL service_root_console_matches(OPENNT_BASE_CONNECTION *root,HANDLE console);
void service_release_console_identities(OPENNT_BASE_CONNECTION *connection);
DWORD service_copy_execution_console_members(OPENNT_BASE_CONNECTION *destination,
    const OPENNT_BASE_CONNECTION *source);
void service_clear_pending_win32record(OPENNT_BASE_CONNECTION *connection);
void service_clear_win32records(OPENNT_BASE_CONNECTION *connection);
void service_release_launcher_results(OPENNT_BASE_SERVICE *service,DWORD generation);
void service_delete_frontend(OPENNT_FRONTEND_ROUTE *route);
void service_clear_frontend_channel(OPENNT_BASE_CONNECTION *connection);
void service_clear_frontend(OPENNT_BASE_CONNECTION *connection);
void service_prune_cancelled_frontends(OPENNT_BASE_SERVICE *service);
void service_preserve_parent_results(OPENNT_BASE_SERVICE *service,
    HANDLE console,BOOL wow);
NTSTATUS service_wait_deliver(void *context,HANDLE event,uint32_t *receipt);
void service_resources_init(OPENNT_BASE_SERVICE_RESOURCES *scope,
    OPENNT_BASE_CONNECTION *connection,DWORD role);
void service_abandon_launch(OPENNT_BASE_CONNECTION *connection);
void service_clear_management_labels(OPENNT_BASE_WORKER_WATCH *watch);
void service_capture_initial_management_labels(OPENNT_BASE_WORKER_WATCH *watch);
void service_capture_checked_management_label(OPENNT_BASE_SERVICE *service,
    HANDLE console,const BASE_CHECKVDM_MSG *command);
BOOL service_root_has_worker(OPENNT_BASE_CONNECTION *root);
DWORD service_queue_native_command(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,HANDLE capability,const WCHAR image[OPENNT_BASE_WORKER_IMAGE_CHARS],
    DWORD bytes,const BYTE *payload);
DWORD service_take_native_command(OPENNT_BASE_CONNECTION *root,DWORD pid,
    DWORD generation,DWORD capacity,BYTE *payload,DWORD *bytes,
    HANDLE *caller_process,HANDLE *execution,HANDLE *frontend,DWORD *request,DWORD *caller_generation);
void service_retire_completed_root(OPENNT_BASE_CONNECTION *parent,DWORD idle_worker);
DWORD service_bind_existing_console(OPENNT_BASE_CONNECTION *connection);
#endif
