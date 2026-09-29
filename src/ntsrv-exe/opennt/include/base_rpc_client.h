/* Process-local client binding for the admitted BaseSrv VDM RPC facade.
 * The original BaseClient callers retain CsrClientCallServer's signature;
 * this header is app composition only and is never a command wire ABI. */
#ifndef OPENNT_BASE_RPC_CLIENT_H
#define OPENNT_BASE_RPC_CLIENT_H
#include <windows.h>
#include <stdint.h>
DWORD OpenNtBaseClientConnectCurrent(void);
/* Arm only after launcher creation rollback is no longer required. Workers
 * arm immediately after Connect, before entering guest code. */
DWORD OpenNtBaseClientWatchBroker(void);
DWORD WINAPI OpenNtBaseClientWowStarted(ULONG task);
/* Resolve only this client's original parent completion handle; no caller
 * guesses transport receipts. Returned startup event is owned/wait-only. */
DWORD OpenNtBaseClientWowStartup(HANDLE parent,HANDLE *event,BOOL *started);
DWORD OpenNtBaseClientFrontendUsage(DWORD *pending,DWORD *tasks);
DWORD OpenNtBaseClientRetireFrontend(void);
DWORD OpenNtBaseClientRegisterFrontendRoot(HANDLE capability);
DWORD OpenNtBaseClientRegisterNativeBackend(HANDLE frontend,HANDLE stop,HANDLE closed);
DWORD OpenNtBaseClientNativeSampleEpoch(uint64_t *epoch);
DWORD OpenNtBaseClientReportNativeBackend(uint64_t epoch,DWORD members);
DWORD OpenNtBaseClientCompleteWorkerChannel(void);
DWORD OpenNtBaseClientWorkerFrontendCapability(HANDLE *capability);
DWORD OpenNtBaseClientRetainFrontendRoot(HANDLE capability,HANDLE *root,DWORD *generation);
/* Separate execution association; caller owns/closes the wait-only event. */
DWORD OpenNtBaseClientAcquireConsoleContext(HANDLE frontend,HANDLE *capability);
DWORD OpenNtBaseClientBindConsoleContext(HANDLE capability);
DWORD OpenNtBaseClientSubmitWorkerChannel(HANDLE capability,HANDLE channel);
DWORD OpenNtBaseClientTakeWorkerChannel(HANDLE *channel,HANDLE *caller_process,HANDLE *execution,HANDLE *frontend);
DWORD OpenNtBaseClientWaitWorkerChannel(HANDLE *channel,HANDLE *caller_process,HANDLE *execution,HANDLE *frontend);
DWORD OpenNtBaseClientRequestFrontend(HANDLE capability);
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
