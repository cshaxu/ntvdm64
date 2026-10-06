#ifndef COMMON_RPC_LOCAL_BINDING_H
#define COMMON_RPC_LOCAL_BINDING_H
#include <windows.h>
#include <rpc.h>
#ifdef __cplusplus
extern "C" {
#endif

/* Project-added ncalrpc client mechanism shared by BaseClient and NTMON.
 * Endpoint selection and peer/application authentication remain caller-owned.
 * On success the caller owns *binding and frees it with RpcBindingFree.
 * On failure no string or binding is retained. No shared/global state. */
RPC_STATUS common_rpc_bind_local(const WCHAR *endpoint,RPC_BINDING_HANDLE *binding);
#ifdef __cplusplus
}
#endif
#endif
