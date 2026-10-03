#ifndef COMMON_RPC_CONNECTION_H
#define COMMON_RPC_CONNECTION_H
#include <windows.h>
#include <rpc.h>
#include "service.h" /* Generated from common/protocol/service.idl. */

/* Borrowed authenticated connection view. The owner keeps all references
 * alive across each call and serializes disconnect against in-flight calls.
 * No registration, execution policy or connection lifetime is owned here. */
typedef struct common_rpc_connection {
    RPC_BINDING_HANDLE binding;
    VDM_CONNECTION connection;
    HANDLE process;
    DWORD generation;
} common_rpc_connection;
#endif
