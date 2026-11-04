#ifndef LIB_WIN32_RPC_PEER_H
#define LIB_WIN32_RPC_PEER_H
#include <windows.h>
#include <rpc.h>
/* Match an already authenticated local RPC caller to a live process handle.
 * Caller owns authentication and authorization; process and binding are borrowed. */
RPC_STATUS win32_rpc_local_peer_process(RPC_BINDING_HANDLE binding, HANDLE process, DWORD *pid);
#endif
