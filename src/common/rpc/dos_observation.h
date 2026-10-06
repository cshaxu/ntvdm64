#ifndef COMMON_RPC_DOS_OBSERVATION_H
#define COMMON_RPC_DOS_OBSERVATION_H
#include "connection.h"
#include "common/protocol/dos_observation.h"
/* Inputs/context/stop are borrowed until actual RPC completion is drained. */
DWORD common_rpc_observe_dos_event(const common_rpc_connection *,
    const common_dos_observation *,BOOL gap,HANDLE stop);
#endif
