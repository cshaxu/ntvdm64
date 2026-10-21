#ifndef COMMON_RPC_INTERFACE_QUERY_H
#define COMMON_RPC_INTERFACE_QUERY_H
#include <windows.h>
#include <rpc.h>
/* Diagnostic only: same authenticated endpoint, caller's existing deadline.
 * A missing current interface does not alone prove a revision mismatch. */
DWORD common_rpc_classify_interface(RPC_BINDING_HANDLE binding,
    const RPC_IF_ID *expected,ULONGLONG deadline);
#endif
