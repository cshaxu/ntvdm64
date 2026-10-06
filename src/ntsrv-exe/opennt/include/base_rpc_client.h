/* Process-local client binding for the admitted BaseSrv VDM RPC facade.
 * The original BaseClient callers retain CsrClientCallServer's signature;
 * this header is app composition only and is never a command wire ABI. */
#ifndef OPENNT_BASE_RPC_CLIENT_H
#define OPENNT_BASE_RPC_CLIENT_H
#include <windows.h>
#include <stdint.h>
#include "common/protocol/dos_observation.h"
uint64_t OpenNtBaseClientObservationDirect(void);
DWORD OpenNtBaseClientObserveDosEvent(const common_dos_observation *,BOOL,HANDLE);
DWORD OpenNtBaseClientWorkerIoTransition(DWORD);
DWORD OpenNtBaseClientWorkerIoCheckpoint(DWORD,DWORD,DWORD *);
DWORD OpenNtBaseClientFrontendIoDisconnected(void);
DWORD OpenNtBaseClientSubmitNativeRequest(HANDLE,DWORD,BYTE *,HANDLE *,HANDLE *,DWORD *);
DWORD OpenNtBaseClientFinishNativeRequest(DWORD,DWORD *,DWORD *);
DWORD OpenNtBaseClientConnectCurrent(void);
DWORD OpenNtBaseClientBrokerProcess(HANDLE *server);
DWORD OpenNtBaseClientStartFrontend(uint64_t console_window,BOOL borrowed,
    HANDLE *root,HANDLE *capability,HANDLE *restored);
DWORD OpenNtBaseClientReturnFrontendConsole(void);
DWORD OpenNtBaseClientStartNativeWorker(HANDLE *);
DWORD OpenNtBaseClientStartVdmWorker(PCWSTR environment,DWORD characters,
    DWORD show,HANDLE frontend,HANDLE *worker,HANDLE *parent);
DWORD OpenNtBaseClientWaitFrontendConsoleRestored(void);
DWORD OpenNtBaseClientFrontendConsoleRestored(void);
DWORD OpenNtBaseClientFrontendStartupResult(HANDLE capability,DWORD status);
DWORD OpenNtBaseClientReportCurrentConsoleMembers(void);
/* Arm only after launcher creation rollback is no longer required. Workers
 * arm immediately after Connect, before entering guest code. */
DWORD OpenNtBaseClientWatchBroker(void);
DWORD WINAPI OpenNtBaseClientWowStarted(ULONG task);
/* Resolve only this client's original parent completion handle; no caller
 * guesses transport receipts. Returned startup event is owned/wait-only. */
DWORD OpenNtBaseClientWowStartup(HANDLE parent,HANDLE *event,BOOL *started);
DWORD OpenNtBaseClientFrontendUsage(DWORD *pending,DWORD *tasks);
DWORD OpenNtBaseClientRetireWorkerlessFrontend(DWORD *retired);
DWORD OpenNtBaseClientFrontendStateChanged(HANDLE *state_changed);
DWORD OpenNtBaseClientWorkerShutdownEvent(HANDLE *shutdown);
DWORD OpenNtBaseClientWorkerIoReleaseEvent(HANDLE *release);
DWORD OpenNtBaseClientWorkerStateChanged(HANDLE *state_changed);
DWORD OpenNtBaseClientRetireFrontend(void);
DWORD OpenNtBaseClientRegisterFrontendRoot(HANDLE capability);
DWORD OpenNtBaseClientAcquireFrontendRoot(uint64_t console_window,DWORD *create_root,
    HANDLE *root,HANDLE *capability,HANDLE *retire,HANDLE *restored);
DWORD OpenNtBaseClientCancelFrontendRootReservation(void);
DWORD OpenNtBaseClientRegisterFrontendLease(uint64_t console_window,DWORD creator_pid,
    BOOL borrowed,HANDLE retire,HANDLE restored);
DWORD OpenNtBaseClientFrontendJoinCandidate(DWORD *nonce,DWORD *candidate_pid);
DWORD OpenNtBaseClientFrontendJoinDecision(DWORD nonce,BOOL same_console);
DWORD OpenNtBaseClientFrontendLeaseReady(void);
DWORD OpenNtBaseClientRegisterNativeBackend(HANDLE frontend,HANDLE stop,HANDLE closed);
DWORD OpenNtBaseClientBindNativeTarget(DWORD request,HANDLE target,HANDLE receipt);
DWORD OpenNtBaseClientCompleteWorkerChannel(DWORD request,DWORD exit_code);
DWORD OpenNtBaseClientCompleteNativeRequest(DWORD request,DWORD exit_code,DWORD io_error,DWORD io_flags);
DWORD OpenNtBaseClientNativeExitCode(DWORD request,DWORD *exit_code);
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
/* Caller closes the query/synchronize-only handle. Valid only while its
 * original DOS command is pending; not a worker-selection request. */
DWORD OpenNtBaseClientCommandWorker(HANDLE *worker);
DWORD OpenNtBaseClientSelectNativeWorker(HANDLE *worker);
DWORD OpenNtBaseClientAttachFrontend(HANDLE pipe,DWORD *generation,HANDLE ready);
DWORD OpenNtBaseClientTakeFrontend(HANDLE *pipe,HANDLE *frontend,DWORD *frontend_generation,HANDLE *ready);
DWORD OpenNtBaseClientWaitFrontend(HANDLE *pipe,HANDLE *frontend,DWORD *frontend_generation,HANDLE *ready);
/* Worker-owned I/O binding, invoked only before delivery of a real command. */
void OpenNtBaseClientSetCommandBinding(DWORD (*ready)(void *),void *context);
void OpenNtBaseClientDisconnectCurrent(void);
/* App-only launch composition over the authenticated connection.  These do
 * not carry Console or command data: original CheckVDM owns both locally. */
DWORD OpenNtBaseClientReserveWorker(ULONG task,uint64_t *reservation);
DWORD OpenNtBaseClientReserveNativeWorker(uint64_t *reservation);
DWORD OpenNtBaseClientPrepareWorker(uint64_t reservation,HANDLE worker);
DWORD OpenNtBaseClientReleaseWorker(uint64_t reservation);
#endif
