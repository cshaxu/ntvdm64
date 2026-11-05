#include "common/protocol/frontend_instance.h"
#ifndef COMMON_RPC_FRONTEND_CONTROL_H
#define COMMON_RPC_FRONTEND_CONTROL_H
#include "connection.h"
#include <stdint.h>

/* Borrowed authenticated connection and input capabilities. Successful returned
 * attachments are caller-owned. Scalar outputs preserve the facade contracts;
 * no frontend lifetime, wait policy or registry is implemented here. */
DWORD common_rpc_retire_workerless_frontend(const common_rpc_connection *state,const common_frontend_key *key,DWORD *retired);
DWORD common_rpc_frontend_state_changed(const common_rpc_connection *state,const common_frontend_key *key,HANDLE *state_changed);
DWORD common_rpc_start_frontend(const common_rpc_connection *state,uint64_t window,BOOL borrowed,
    HANDLE *root,HANDLE *capability,HANDLE *restored);
DWORD common_rpc_frontend_console_restored(const common_rpc_connection *state,const common_frontend_key *key);
DWORD common_rpc_frontend_startup_result(const common_rpc_connection *state,HANDLE capability,DWORD status);
DWORD common_rpc_wait_frontend_console_restored(const common_rpc_connection *state);
DWORD common_rpc_frontend_io_disconnected(const common_rpc_connection *state,const common_frontend_key *key);
#endif
