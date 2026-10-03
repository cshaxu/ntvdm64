#ifndef COMMON_RPC_NATIVE_COMMAND_H
#define COMMON_RPC_NATIVE_COMMAND_H
#include "connection.h"

/* Copied command bytes remain in caller storage. Successful attachments are
 * caller-owned; a failed call releases partial attachments and clears outputs. */
DWORD common_rpc_next_native_command(const common_rpc_connection *,DWORD capacity,
    BYTE *payload,DWORD *bytes,HANDLE *caller_process,HANDLE *execution,
    HANDLE *frontend,DWORD *request,DWORD *caller_generation);
DWORD common_rpc_native_startup_result(const common_rpc_connection *,
    DWORD caller_generation,DWORD request,DWORD status,HANDLE target,HANDLE receipt);
DWORD common_rpc_complete_native_command(const common_rpc_connection *,
    DWORD request,DWORD exit_code,DWORD io_error,DWORD io_flags);
DWORD common_rpc_submit_native_request(const common_rpc_connection *,HANDLE frontend,
    DWORD bytes,BYTE *payload,HANDLE *target,HANDLE *receipt,DWORD *request);
DWORD common_rpc_finish_native_request(const common_rpc_connection *,DWORD request,
    DWORD *exit_code,DWORD *target_completed);
#endif
