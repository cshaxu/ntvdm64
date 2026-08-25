#ifndef RUNNER_HISTORICAL_BIOS_BRIDGE_V2_H
#define RUNNER_HISTORICAL_BIOS_BRIDGE_V2_H

#include "historical_transport_v2.h"

int __cdecl runner_historical_bios_bridge_v2_initialize(void);
void __cdecl runner_historical_bios_bridge_v2_terminate(void);
int __cdecl runner_historical_bios_bridge_v2_invoke(
    const runner_historical_transport_v2_request *request,
    runner_historical_transport_v2_response *response,
    uint32_t expected_session_token);

#endif
