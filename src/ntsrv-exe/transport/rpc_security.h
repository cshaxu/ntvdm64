/* Service-side RPC authorization; not a wire record. */
#ifndef BROKER_RPC_SECURITY_H
#define BROKER_RPC_SECURITY_H
#include "common/base_srv/scope.h"
#ifdef __cplusplus
extern "C" {
#endif
RPC_STATUS broker_rpc_authorize(const broker_rpc_scope *scope, RPC_BINDING_HANDLE binding);
RPC_STATUS broker_rpc_peer_process(const broker_rpc_scope *, RPC_BINDING_HANDLE, HANDLE, DWORD *);
#ifdef __cplusplus
}
#endif
#endif
