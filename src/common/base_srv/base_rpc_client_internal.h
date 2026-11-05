/* Private composition seam, never a wire declaration or copied resource. */
#ifndef OPENNT_BASE_RPC_CLIENT_INTERNAL_H
#define OPENNT_BASE_RPC_CLIENT_INTERNAL_H
#include "common/rpc/connection.h"
/* One statically linked execution extension per client. The callback only
 * releases its local state during core disconnect, after RPCs have stopped.
 * It cannot reconnect or replace connection authority. */
common_rpc_connection OpenNtBaseClientExecutionConnection(void (*cleanup)(void));
/* Borrowed process identity and optional server wait handle; no ownership
 * transfer and no role/instance state is stored by this accessor. */
common_rpc_connection OpenNtBaseClientControlConnection(HANDLE *server);
#endif
