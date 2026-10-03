#ifndef COMMON_RPC_WORKER_CONTROL_H
#define COMMON_RPC_WORKER_CONTROL_H
#include "connection.h"

/* Pure service exchanges. State and input handles are borrowed; successfully
 * returned handles belong to the caller. Workers retain wait sets, execution
 * and failure/retirement policy. The existing Take/Wait choice is explicit. */
DWORD common_rpc_worker_frontend_capability(const common_rpc_connection *state,HANDLE *capability);
DWORD common_rpc_acquire_console_context(const common_rpc_connection *state,HANDLE frontend,HANDLE *capability);
DWORD common_rpc_bind_console_context(const common_rpc_connection *state,HANDLE capability);
DWORD common_rpc_register_native_backend(const common_rpc_connection *state,HANDLE frontend,HANDLE stop,HANDLE closed);
DWORD common_rpc_worker_shutdown_event(const common_rpc_connection *state,HANDLE *shutdown);
DWORD common_rpc_worker_state_changed(const common_rpc_connection *state,HANDLE *state_changed);
DWORD common_rpc_take_frontend(const common_rpc_connection *state,HANDLE *pipe,HANDLE *frontend,DWORD *frontend_generation,HANDLE *ready,BOOL wait);
#endif
