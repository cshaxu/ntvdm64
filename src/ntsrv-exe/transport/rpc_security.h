/* Private native transport state, never a broker command/wire record. */
#ifndef BROKER_RPC_SECURITY_H
#define BROKER_RPC_SECURITY_H
#include <windows.h>
#include <rpc.h>
/* Shared endpoint grammar for the service, clients, and monitor. */
#define BROKER_RPC_ENDPOINT_FORMAT L"ntvdm-basesrv-%lu-%08lx-%08lx-%ls"
#define BROKER_PACKAGE_ID_HEX_CHARS 32u
#define BROKER_PACKAGE_RESOURCE L"NTVDM_PACKAGE_ID"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct broker_rpc_scope {
    LUID logon;
    DWORD session;
    BOOL valid;
    WCHAR package_id[BROKER_PACKAGE_ID_HEX_CHARS+1u];
} broker_rpc_scope;
BOOL broker_rpc_capture_scope(broker_rpc_scope *scope);
RPC_STATUS broker_rpc_authorize(const broker_rpc_scope *scope, RPC_BINDING_HANDLE binding);
/* Validate a process attachment against the live local RPC peer. The caller
 * must duplicate a retained reference before the RPC-owned input expires. */
RPC_STATUS broker_rpc_peer_process(const broker_rpc_scope *, RPC_BINDING_HANDLE, HANDLE, DWORD *);
#ifdef __cplusplus
}
#endif
#endif
