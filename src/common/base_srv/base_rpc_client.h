/* Process-local client binding for the admitted BaseSrv VDM RPC facade.
 * The original BaseClient callers retain CsrClientCallServer's signature;
 * this header is app composition only and is never a command wire ABI. */
#ifndef OPENNT_BASE_RPC_CLIENT_H
#define OPENNT_BASE_RPC_CLIENT_H
#include <windows.h>
#include <stdint.h>
#include "common/protocol/dos_observation.h"
#include "common/protocol/native_launch_type.h"
uint64_t OpenNtBaseClientObservationDirect(void);
DWORD OpenNtBaseClientObserveDosEvent(const common_dos_observation *,BOOL,HANDLE);
DWORD OpenNtBaseClientWorkerIoTransition(DWORD);
DWORD OpenNtBaseClientWorkerIoCheckpoint(DWORD,DWORD,DWORD *);
DWORD OpenNtBaseClientFrontendIoDisconnected(void);
DWORD OpenNtBaseClientSubmitNativeRequest(DWORD,HANDLE,DWORD,BYTE *,HANDLE *,HANDLE *,DWORD *);
DWORD OpenNtBaseClientFinishNativeRequest(DWORD,DWORD *,DWORD *);
/* Ordinary connection waits for the RPC result, not a product deadline. */
DWORD OpenNtBaseClientConnectCurrent(void);
/* Run16 service-startup exception: phases share the supplied remaining budget. */
DWORD OpenNtBaseClientConnectCurrentTimeout(DWORD milliseconds);
/* Preserve ordinary failures; confirmed admitted service loss maps to the
 * original disconnected-port Win32 error. No peer observation is involved. */
DWORD OpenNtBaseClientFailure(DWORD error);
DWORD OpenNtBaseClientBrokerProcess(HANDLE *server);
DWORD OpenNtBaseClientStartFrontend(uint64_t console_window,BOOL borrowed,
    HANDLE *root,HANDLE *capability,HANDLE *restored);
DWORD OpenNtBaseClientStartNativeWorker(DWORD native_type,HANDLE *);
DWORD OpenNtBaseClientStartVdmWorker(PCWSTR environment,DWORD characters,
    DWORD show,HANDLE frontend,HANDLE *worker,HANDLE *parent);
/* Finalize an admitted launcher request. NTSRV returns immediately for a
 * non-owning/nested request; an outer lease includes NTCON's final restoration.
 * Invoke for failed requests too, before releasing the client connection. */
DWORD OpenNtBaseClientWaitFrontendConsoleRestored(void);
DWORD OpenNtBaseClientFrontendConsoleRestored(void);
DWORD OpenNtBaseClientFrontendCloseQuiesced(void);
DWORD OpenNtBaseClientFrontendStartupResult(HANDLE capability,DWORD status);
DWORD OpenNtBaseClientReportCurrentConsoleMembers(void);
/* Mechanism only: the executable supplies loss cleanup after its resources
 * exist. Callback runs off-thread, outside client locks; it must not disconnect
 * or destroy this watcher. Owner stops/joins before releasing callback state. */
DWORD OpenNtBaseClientWatchBroker(void (*lost)(void *),void *context);
void OpenNtBaseClientStopBrokerWatch(void);
DWORD WINAPI OpenNtBaseClientWowStarted(ULONG task);
/* Resolve only this client's original parent completion handle; no caller
 * guesses transport receipts. Returned startup event is owned/wait-only. */
DWORD OpenNtBaseClientWowStartup(HANDLE parent,HANDLE *event,BOOL *started);
DWORD OpenNtBaseClientRetireWorkerlessFrontend(DWORD *retired);
DWORD OpenNtBaseClientFrontendStateChanged(HANDLE *state_changed);
/* Borrowed until Disconnect; no allocation or RPC after successful Connect. */
DWORD OpenNtBaseClientWorkerControls(HANDLE *server,HANDLE *shutdown,HANDLE *console_close,HANDLE *wow_task_close);
DWORD OpenNtBaseClientTakeWowTaskClose(uint64_t *identity);
DWORD WINAPI OpenNtBaseClientFailWowTaskClose(uint64_t identity);
DWORD WINAPI OpenNtBaseClientBindWowTask(ULONG task,USHORT htask,uint64_t *identity);
DWORD WINAPI OpenNtBaseClientRetireWowTask(uint64_t identity);
DWORD OpenNtBaseClientWorkerShutdownEvent(HANDLE *shutdown);
/* Report inability to continue. NTSRV selects the authenticated participant's
 * close transaction; successful submission is not permission to exit. */
DWORD OpenNtBaseClientRequestParticipantExit(void);
/* Close only this authenticated participant's associated Console. */
DWORD OpenNtBaseClientRequestConsoleExit(void);
DWORD OpenNtBaseClientWorkerConsoleCloseEvent(HANDLE *close);
DWORD OpenNtBaseClientWorkerIoReleaseEvent(HANDLE *release);
DWORD OpenNtBaseClientRegisterFrontendRoot(HANDLE capability);
DWORD OpenNtBaseClientRegisterFrontendLease(uint64_t console_window,DWORD creator_pid,
    BOOL borrowed,HANDLE retire,HANDLE restored);
DWORD OpenNtBaseClientFrontendJoinCandidate(DWORD *nonce,DWORD *candidate_pid);
DWORD OpenNtBaseClientFrontendJoinDecision(DWORD nonce,BOOL same_console);
DWORD OpenNtBaseClientFrontendLeaseReady(void);
DWORD OpenNtBaseClientRegisterNativeBackend(HANDLE frontend);
DWORD OpenNtBaseClientBindNativeTarget(DWORD request,HANDLE target,HANDLE receipt);
DWORD OpenNtBaseClientCompleteWorkerChannel(DWORD request,DWORD exit_code);
DWORD OpenNtBaseClientCompleteNativeRequest(DWORD request,DWORD exit_code,DWORD io_error,DWORD io_flags);
DWORD OpenNtBaseClientWorkerFrontendCapability(HANDLE *capability);
DWORD OpenNtBaseClientRetainFrontendRoot(HANDLE capability,HANDLE *root,DWORD *generation);
/* Separate execution association; caller owns/closes the wait-only event. */
DWORD OpenNtBaseClientAcquireConsoleContext(HANDLE frontend,HANDLE *capability);
DWORD OpenNtBaseClientBindConsoleContext(HANDLE capability);
DWORD OpenNtBaseClientGetNextNativeCommand(DWORD capacity,BYTE *payload,DWORD *bytes,HANDLE *caller_process,HANDLE *execution,HANDLE *frontend,DWORD *request,DWORD *caller_generation);
DWORD OpenNtBaseClientNativeStartupResult(DWORD caller_generation,DWORD request,DWORD status,HANDLE target,HANDLE receipt);
DWORD OpenNtBaseClientRequestFrontend(HANDLE capability);
/* Success with zero request/NULL worker is an explicit disconnect command. */
DWORD OpenNtBaseClientFrontendRequest(DWORD *request,HANDLE *worker);
DWORD OpenNtBaseClientAttachFrontendRequest(DWORD request,HANDLE pipe,HANDLE ready,DWORD *generation);
DWORD OpenNtBaseClientTakeFrontend(HANDLE *pipe,HANDLE *frontend,DWORD *frontend_generation,HANDLE *ready);
DWORD OpenNtBaseClientWaitFrontend(HANDLE *pipe,HANDLE *frontend,DWORD *frontend_generation,HANDLE *ready);
/* Worker-owned I/O binding, invoked only before delivery of a real command. */
void OpenNtBaseClientSetCommandBinding(DWORD (*ready)(void *),void *context);
void OpenNtBaseClientDisconnectCurrent(void);
#endif
