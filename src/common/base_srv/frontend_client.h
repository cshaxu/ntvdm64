/* Frontend-role control over the process's existing authenticated BaseSrv
 * connection. Explicit keys are borrowed values, never ambient selectors.
 * The owner keeps each key with its frontend-base adapter context; worker-base
 * uses the backend/process facade and does not compose frontend-base. */
#ifndef OPENNT_BASE_FRONTEND_CLIENT_H
#define OPENNT_BASE_FRONTEND_CLIENT_H
#include <windows.h>
#include "common/protocol/frontend_instance.h"
/* Returned events are owned handles; callers close them after endpoint users
 * have joined. Instance close/unregister never means terminate its host. */
DWORD OpenNtBaseClientFrontendIoDisconnected(const common_frontend_key *);
DWORD OpenNtBaseClientFrontendConsoleRestored(const common_frontend_key *);
DWORD OpenNtBaseClientFrontendCloseQuiesced(const common_frontend_key *);
DWORD OpenNtBaseClientFrontendStartupResult(HANDLE capability,DWORD status);
DWORD OpenNtBaseClientRetireWorkerlessFrontend(const common_frontend_key *,DWORD *retired);
DWORD OpenNtBaseClientFrontendStateChanged(const common_frontend_key *,HANDLE *state_changed);
DWORD OpenNtBaseClientRegisterFrontendRoot(HANDLE capability,common_frontend_key *);
DWORD OpenNtBaseClientRegisterFrontendLease(const common_frontend_key *,uint64_t console_window,DWORD creator_pid,
    BOOL borrowed,HANDLE retire,HANDLE restored);
DWORD OpenNtBaseClientFrontendJoinCandidate(const common_frontend_key *,DWORD *nonce,DWORD *candidate_pid);
DWORD OpenNtBaseClientFrontendJoinDecision(const common_frontend_key *,DWORD nonce,BOOL same_console);
DWORD OpenNtBaseClientFrontendLeaseReady(const common_frontend_key *);
DWORD OpenNtBaseClientFrontendRequest(const common_frontend_key *,DWORD *request,HANDLE *worker);
DWORD OpenNtBaseClientAttachFrontendRequest(const common_frontend_key *,DWORD request,HANDLE pipe,HANDLE ready,DWORD *generation);
DWORD OpenNtBaseClientRegisterEmbeddedFrontend(const common_frontend_location *,common_frontend_key *,HANDLE *,HANDLE *);
DWORD OpenNtBaseClientCloseFrontendInstance(const common_frontend_key *);
DWORD OpenNtBaseClientUnregisterFrontendInstance(const common_frontend_key *);
#endif
