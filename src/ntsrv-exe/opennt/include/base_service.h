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
/* Copied management projection. PID selects the currently authenticated,
 * registered worker. BaseSrv sequence remains private routing state. No
 * HANDLE, original record pointer or guest address crosses this boundary. */
#define OPENNT_BASE_WORKER_IMAGE_CHARS 260u
typedef struct OPENNT_BASE_WORKER_INFO {
    uint32_t sequence;
    uint32_t kind;
    uint32_t state;
    uint64_t started_filetime;
    uint32_t task;
    /* Management-only depth: original DOS/WOW records or admitted native
     * Direct records. Zero is a resident worker without an active task. */
    uint32_t stack_depth;
    uint32_t process_id;
    WCHAR image[OPENNT_BASE_WORKER_IMAGE_CHARS];
} OPENNT_BASE_WORKER_INFO;
OPENNT_BASE_SERVICE *OpenNtBaseServiceStart(void);
BOOL OpenNtBaseServiceStop(OPENNT_BASE_SERVICE *);
BOOL OpenNtBaseServiceConfigureEmptyNotify(OPENNT_BASE_SERVICE *,OPENNT_BASE_EMPTY_NOTIFY,void *);
/* True only when all authenticated client connections and all finite launch
 * reservations are gone.  It deliberately says nothing about a quiet but
 * connected interactive VDM. */
BOOL OpenNtBaseServiceIsEmpty(OPENNT_BASE_SERVICE *);
/* Management callers are authenticated by the transport before reaching
 * these methods. The service resolves a PID while holding its registration
 * lock, so a removed/reused prior process cannot be selected. */
DWORD OpenNtBaseServiceSnapshot(OPENNT_BASE_SERVICE *,uint64_t *epoch,
    OPENNT_BASE_WORKER_INFO *entries,uint32_t capacity,uint32_t *count);
DWORD OpenNtBaseServiceTerminateWorker(OPENNT_BASE_SERVICE *,uint32_t process_id);
DWORD OpenNtBaseServiceConnect(OPENNT_BASE_SERVICE *,HANDLE,OPENNT_BASE_CONNECTION **,DWORD *);
/* Local Console membership is sampled only by the authenticated launcher
 * which is actually attached to that Console.  It is a bounded selection
 * hint for original ConsoleRecord association, never a task/worker claim. */
DWORD OpenNtBaseServiceReportConsoleMembers(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,DWORD count,const DWORD *members);
/* Independent native-worker admission through the existing reservation path.
 * Execution Console binding is authenticated separately from frontend I/O. */
DWORD OpenNtBaseServiceCreateNativeReservation(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,uint64_t *reservation);
/* Explicit launcher selection within its authenticated execution Console.
 * Returns a query/synchronize-only reference and pins that registered worker
 * generation for subsequent command/frontend attachments. No DOS record. */
DWORD OpenNtBaseServiceSelectNativeWorker(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,HANDLE *worker);
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
DWORD OpenNtBaseServiceAcquireFrontendRoot(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,uint64_t console_window,DWORD *create_root,HANDLE *root,
    HANDLE *capability,HANDLE *retire,HANDLE *restored);
DWORD OpenNtBaseServiceCancelFrontendRootReservation(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation);
DWORD OpenNtBaseServiceRegisterFrontendLease(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,uint64_t console_window,DWORD creator_pid,BOOL borrowed,
    HANDLE retire,HANDLE restored);
DWORD OpenNtBaseServiceFrontendJoinCandidate(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,DWORD *nonce,DWORD *candidate_pid);
DWORD OpenNtBaseServiceFrontendJoinDecision(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,DWORD nonce,BOOL same_console);
DWORD OpenNtBaseServiceFrontendLeaseReady(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation);
DWORD OpenNtBaseServiceRetainFrontendRoot(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,HANDLE capability,HANDLE *root,DWORD *root_generation);
/* Native backend registration is separate from original DOS/WOW records.
 * The backend registers itself using its authenticated connection and root
 * capability. Stop/closed are typed session-control attachments, not task IDs. */
DWORD OpenNtBaseServiceRegisterNativeBackend(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,HANDLE frontend,HANDLE stop,HANDLE closed);
DWORD OpenNtBaseServiceCompleteWorkerChannel(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation,DWORD request);
DWORD OpenNtBaseServiceBindNativeTarget(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation,
    DWORD request,HANDLE target);
/* Preserve the caller's verified execution Console across a hidden backend.
 * The returned unnamed event is a separate, wait-only capability, not the
 * frontend event or a caller-selected Console/worker identity. The root owns
 * the association even when the acquiring helper disconnects. Bind is allowed
 * only before task/reservation admission and never mutates original DOS records. */
DWORD OpenNtBaseServiceAcquireConsoleContext(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,HANDLE frontend,HANDLE *capability);
DWORD OpenNtBaseServiceBindConsoleContext(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,HANDLE capability);
DWORD OpenNtBaseServiceWorkerFrontendCapability(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,HANDLE *capability);
/* One pending launcher channel per authenticated connection, delivered only
 * to its admitted native worker. No launch payload or target-result policy here.
 * Receiver owns all four references; sender rundown drops pending ones only. */
DWORD OpenNtBaseServiceSubmitWorkerChannel(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,HANDLE capability,HANDLE channel,const WCHAR image[OPENNT_BASE_WORKER_IMAGE_CHARS]);
DWORD OpenNtBaseServiceTakeWorkerChannel(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,HANDLE *channel,HANDLE *caller_process,HANDLE *execution,HANDLE *frontend,DWORD *request);
DWORD OpenNtBaseServiceGetNextNativeCommand(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,HANDLE *channel,HANDLE *caller_process,HANDLE *execution,HANDLE *frontend,DWORD *request);
/* Request identifies an authenticated connection's still-pending original
 * DOS command, not a caller-nominated worker. The root's event wakes it to
 * acquire that selected worker and publish a direct route. No I/O payloads. */
DWORD OpenNtBaseServiceRequestFrontend(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,HANDLE capability);
DWORD OpenNtBaseServiceFrontendRequest(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,DWORD *request,HANDLE *worker);
/* Read-only original DOS occupancy for this authenticated frontend only.
 * Pending attachment is distinct from active original DOS records. */
DWORD OpenNtBaseServiceFrontendUsage(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,DWORD *pending,DWORD *tasks);
DWORD OpenNtBaseServiceRetireWorkerlessFrontend(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,DWORD *retired);
DWORD OpenNtBaseServiceFrontendStateChanged(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,HANDLE *state_changed);
DWORD OpenNtBaseServiceWorkerStateChanged(OPENNT_BASE_CONNECTION *,DWORD pid,
    DWORD generation,HANDLE *state_changed);
/* Atomically reject retirement while original work/admission remains, else
 * revoke future joins. Frontend first drains its own native/local requests. */
DWORD OpenNtBaseServiceRetireFrontend(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation);
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
/* Successful original InitTask notification, distinct from task completion.
 * The registered WOW worker may report only a dispatched original record.
 * Query is confined to the submitting connection's parent receipt and returns
 * an owned, synchronize-only event duplicate plus a latched success flag. */
DWORD OpenNtBaseServiceWowStarted(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation,ULONG task);
DWORD OpenNtBaseServiceWowStartup(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation,
    DWORD parent_receipt,HANDLE *event,BOOL *started);
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
