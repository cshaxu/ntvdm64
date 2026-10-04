#ifndef COMMON_RPC_MANAGEMENT_H
#define COMMON_RPC_MANAGEMENT_H
#include <windows.h>
#include <rpc.h>
#include "service.h"

/* Connectionless management RPCs authenticate the borrowed process capability.
 * The caller owns the binding/process and serializes their release with calls.
 * No BaseClient registration or monitor selection state is created here. */
typedef struct common_rpc_management {
    RPC_BINDING_HANDLE binding;
    HANDLE process;
} common_rpc_management;

/* Success transfers the MIDL allocation to the caller (MIDL_user_free).
 * Failure frees partial results and leaves count=0/items=NULL. */
DWORD common_rpc_task_snapshot(const common_rpc_management *state,
    ULONG *count,DTASKMGR_WORKER **items);
DWORD common_rpc_close_management_node(const common_rpc_management *state,const DTASKMGR_KEY *key);
#endif
