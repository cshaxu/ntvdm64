/* Private native transport state, never a broker command/wire record. */
#ifndef BROKER_RPC_SECURITY_H
#define BROKER_RPC_SECURITY_H
#include <windows.h>
#include <rpc.h>
typedef struct broker_rpc_scope {
    LUID logon;
    DWORD session;
    BOOL valid;
} broker_rpc_scope;
BOOL broker_rpc_capture_scope(broker_rpc_scope *scope);
RPC_STATUS broker_rpc_authorize(const broker_rpc_scope *scope, RPC_BINDING_HANDLE binding);
#endif
