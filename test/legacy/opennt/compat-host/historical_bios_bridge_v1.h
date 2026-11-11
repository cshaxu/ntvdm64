#ifndef RUNNER_HISTORICAL_BIOS_BRIDGE_V1_H
#define RUNNER_HISTORICAL_BIOS_BRIDGE_V1_H

#include <stdint.h>

#define RUNNER_HISTORICAL_BIOS_BRIDGE_V1_ABI 1u

typedef enum runner_historical_bios_bridge_v1_disposition {
    RUNNER_HISTORICAL_BIOS_BRIDGE_V1_COMPLETED = 0u,
    RUNNER_HISTORICAL_BIOS_BRIDGE_V1_INVALID_RECORD = 1u,
    RUNNER_HISTORICAL_BIOS_BRIDGE_V1_NOT_INITIALIZED = 2u,
    RUNNER_HISTORICAL_BIOS_BRIDGE_V1_UNSUPPORTED = 3u
} runner_historical_bios_bridge_v1_disposition;

typedef struct runner_historical_bios_bridge_v1_request {
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint8_t selector;
    uint8_t service_byte;
    uint16_t reserved;
    uint16_t ax;
    uint16_t cx;
    uint16_t dx;
    uint16_t cs;
    uint16_t ip;
    uint32_t reserved2;
} runner_historical_bios_bridge_v1_request;

typedef struct runner_historical_bios_bridge_v1_response {
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t disposition;
    uint32_t event_flags;
    uint16_t ax;
    uint16_t cx;
    uint16_t dx;
    uint16_t cs;
    uint16_t ip;
    uint16_t reserved;
} runner_historical_bios_bridge_v1_response;

_Static_assert(sizeof(runner_historical_bios_bridge_v1_request) == 28u,
    "v1 request layout must remain cross-transport stable");
_Static_assert(sizeof(runner_historical_bios_bridge_v1_response) == 28u,
    "v1 response layout must remain cross-transport stable");

int __cdecl runner_historical_bios_bridge_v1_initialize(void);
void __cdecl runner_historical_bios_bridge_v1_terminate(void);
int __cdecl runner_historical_bios_bridge_v1_invoke(
    const runner_historical_bios_bridge_v1_request *request,
    runner_historical_bios_bridge_v1_response *response);

#endif
