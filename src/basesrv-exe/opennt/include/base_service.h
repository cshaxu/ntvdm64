#ifndef OPENNT_BASE_SERVICE_H
#define OPENNT_BASE_SERVICE_H
#include <windows.h>
#include <stdint.h>
/* Native composition boundary; opaque pointers never enter command records.
 * One service instance per process, matching original BaseSrv globals.
 * Call only after RPC identity authentication. All operations serialized by
 * the service; transport guarantees connection rundown follows active calls. */
typedef struct OPENNT_BASE_SERVICE OPENNT_BASE_SERVICE;
typedef struct OPENNT_BASE_CONNECTION OPENNT_BASE_CONNECTION;
typedef void (WINAPI *OPENNT_BASE_EMPTY_NOTIFY)(void *);
/* Copied management projection.  It deliberately contains no process ID,
 * HANDLE, original record pointer, or guest address. */
#define OPENNT_BASE_WORKER_IMAGE_CHARS 260u
typedef struct OPENNT_BASE_WORKER_INFO {
    uint32_t sequence;
    uint32_t kind;
    uint32_t state;
    /* Count of original source records currently executing or queued for this
     * worker. This is management-only metadata, not a guest-visible task ID. */
    uint32_t reserved;
    uint64_t started_filetime;
    uint32_t task;
    /* Management-only depth: 0 is the resident PermCom, 1 its COMMAND,
     * and each child command increases the visible call depth. */
    uint32_t stack_depth;
    WCHAR image[OPENNT_BASE_WORKER_IMAGE_CHARS];
} OPENNT_BASE_WORKER_INFO;
/* The original service owns ConsoleRecord selection.  This callback only
 * answers membership for already authenticated, live process handles; it
 * never accepts a caller-supplied Console identity or selects a command. */
typedef DWORD (WINAPI *OPENNT_BASE_CONSOLE_QUERY)(void *,HANDLE,const HANDLE *,DWORD,HANDLE,DWORD,BYTE *);
OPENNT_BASE_SERVICE *OpenNtBaseServiceStart(void);
BOOL OpenNtBaseServiceStop(OPENNT_BASE_SERVICE *);
/* Configure before the first authenticated connection. */
BOOL OpenNtBaseServiceConfigureConsoleQuery(OPENNT_BASE_SERVICE *,OPENNT_BASE_CONSOLE_QUERY,void *);
BOOL OpenNtBaseServiceConfigureEmptyNotify(OPENNT_BASE_SERVICE *,OPENNT_BASE_EMPTY_NOTIFY,void *);
/* True only when all authenticated client connections and all finite launch
 * reservations are gone.  It deliberately says nothing about a quiet but
 * connected interactive VDM. */
BOOL OpenNtBaseServiceIsEmpty(OPENNT_BASE_SERVICE *);
/* Management callers are authenticated by the transport before reaching
 * these methods.  The epoch rejects selections copied before a broker restart. */
DWORD OpenNtBaseServiceSnapshot(OPENNT_BASE_SERVICE *,uint64_t *epoch,
    OPENNT_BASE_WORKER_INFO *entries,uint32_t capacity,uint32_t *count);
DWORD OpenNtBaseServiceTerminateWorker(OPENNT_BASE_SERVICE *,uint64_t epoch,uint32_t sequence);
DWORD OpenNtBaseServiceConnect(OPENNT_BASE_SERVICE *,HANDLE,OPENNT_BASE_CONNECTION **,DWORD *);
DWORD OpenNtBaseServiceDisconnect(OPENNT_BASE_CONNECTION *);
BOOL OpenNtBaseServicePeer(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation);
/* For authenticated in-flight calls only. Caller closes the returned handle;
 * retaining it does not extend the RPC context or registration lifetime. */
DWORD OpenNtBaseServiceRetainPeer(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation,HANDLE *);
/* Independent of DOS command receipts: a root registers an unnamed event
 * received through an authenticated typed attachment. Descendants present
 * a restricted duplicate of that same object, never a trusted handle number.
 * Retain returns a query/synchronize-only root process and its generation;
 * caller closes the process. No task or worker selection occurs here. */
DWORD OpenNtBaseServiceRegisterFrontendRoot(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,HANDLE capability);
DWORD OpenNtBaseServiceRetainFrontendRoot(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,HANDLE capability,HANDLE *root,DWORD *root_generation);
DWORD OpenNtBaseServiceWorkerFrontendCapability(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,HANDLE *capability);
/* Request identifies an authenticated connection's still-pending original
 * DOS command, not a caller-nominated worker. The root's event wakes it to
 * acquire that selected worker and publish a direct route. No I/O payloads. */
DWORD OpenNtBaseServiceRequestFrontend(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,HANDLE capability);
DWORD OpenNtBaseServiceFrontendRequest(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,DWORD *request,HANDLE *worker);
DWORD OpenNtBaseServiceAttachFrontendRequest(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,DWORD request,HANDLE pipe,HANDLE ready);
/* Frontend transport authentication only: the original command's waiting
 * launcher may retain its selected DOS worker with query/synchronize rights.
 * No caller-selected PID, task selection, Console handle or channel payload. */
DWORD OpenNtBaseServiceRetainCommandWorker(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,HANDLE *worker);
/* One root frontend per selected DOS worker. Attach carries a pipe and a
 * manual-reset readiness event, never input/frame payloads. Take transfers
 * pipe, frontend process and synchronize-only event once; caller owns all
 * three returned handles. */
DWORD OpenNtBaseServiceAttachFrontend(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,HANDLE pipe,HANDLE ready);
DWORD OpenNtBaseServiceTakeFrontend(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,HANDLE *pipe,HANDLE *frontend,DWORD *frontend_generation,HANDLE *ready);
DWORD OpenNtBaseServiceWaitFrontend(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,HANDLE *pipe,HANDLE *frontend,DWORD *frontend_generation,HANDLE *ready);
DWORD OpenNtBaseServiceFirst(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation,DWORD *);
DWORD OpenNtBaseServiceRegisterWowExec(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation,DWORD window);
/* These are authenticated service bindings around the original CheckVDM
 * no-worker result.  They never implement task selection or command payloads. */
DWORD OpenNtBaseServiceCreateReservation(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation,
    ULONG task,uint64_t *reservation);
DWORD OpenNtBaseServicePrepareWorker(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation,
    uint64_t reservation,HANDLE worker);
DWORD OpenNtBaseServiceReleaseReservation(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation,
    uint64_t reservation);
BOOL OpenNtBaseServiceWorkerReservation(OPENNT_BASE_CONNECTION *,uint64_t *reservation,
    ULONG *task,HANDLE *console);
DWORD OpenNtBaseServiceCheck(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation,
    void *input,uint32_t bytes,void *output,uint32_t capacity,uint32_t *required,
    HANDLE *parent_event,uint32_t *parent_receipt);
/* Update carries scalar entry input only.  For PROCESS_HANDLE the broker
 * resolves the source-shaped pseudo-handle to the worker retained under the
 * authenticated launch reservation; its parent event is a typed RPC attachment. */
DWORD OpenNtBaseServiceUpdate(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation,
    const void *input,uint32_t bytes,void *output,uint32_t capacity,uint32_t *required,
    HANDLE *parent_event,uint32_t *parent_receipt);
DWORD OpenNtBaseServiceExitCode(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation,
    uint32_t parent_receipt,DWORD *exit_code);
/* Source-shaped BasepSetReenterCount binding.  The client cannot supply a
 * Console HANDLE across RPC; the authenticated connection already owns it. */
DWORD OpenNtBaseServiceReenter(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation,
    uint32_t increment);
/* GetNextVDMCommand's Console identity remains connection-local. The copied
 * request/reply contains no native handle. The original wait event is a
 * separate, typed RPC attachment; reply ownership transfers to the caller and
 * must be released by OpenNtBaseServiceReleaseCommandReply. */
DWORD OpenNtBaseServiceGet(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation,
    const void *input,uint32_t bytes,void **output,uint32_t *output_bytes,HANDLE *wait_event,
    HANDLE standard[3],ULONG *standard_count);
/* Source-shaped BasepExitVDM binding.  The original Console/WOW selection
 * stays service-local; only the original WOW discriminator/task are copied.
 * A true close_worker_wait tells the client to close the already-delivered
 * local worker wait event. */
DWORD OpenNtBaseServiceExit(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation,
    BOOL is_wow,ULONG wow_task,BOOL *close_worker_wait);
void OpenNtBaseServiceReleaseCommandReply(void *);
DWORD OpenNtBaseServiceAttachStream(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation,DWORD role,HANDLE,DWORD *);
DWORD OpenNtBaseServiceRevokeStream(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation,DWORD receipt);
#endif
