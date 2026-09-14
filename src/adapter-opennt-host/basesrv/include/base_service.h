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
OPENNT_BASE_SERVICE *OpenNtBaseServiceStart(void);
BOOL OpenNtBaseServiceStop(OPENNT_BASE_SERVICE *);
DWORD OpenNtBaseServiceConnect(OPENNT_BASE_SERVICE *,HANDLE,OPENNT_BASE_CONNECTION **,DWORD *);
DWORD OpenNtBaseServiceDisconnect(OPENNT_BASE_CONNECTION *);
BOOL OpenNtBaseServicePeer(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation);
/* For authenticated in-flight calls only. Caller closes the returned handle;
 * retaining it does not extend the RPC context or registration lifetime. */
DWORD OpenNtBaseServiceRetainPeer(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation,HANDLE *);
DWORD OpenNtBaseServiceFirst(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation,DWORD *);
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
    void *input,uint32_t bytes,void *output,uint32_t capacity,uint32_t *required);
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
void OpenNtBaseServiceReleaseCommandReply(void *);
DWORD OpenNtBaseServiceAttachStream(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation,DWORD role,HANDLE,DWORD *);
DWORD OpenNtBaseServiceRevokeStream(OPENNT_BASE_CONNECTION *,DWORD pid,DWORD generation,DWORD receipt);
#endif
