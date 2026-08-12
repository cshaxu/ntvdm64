#include <stdint.h>
#include <string.h>

#include "historical_bios_bridge_v1.h"

extern unsigned long ntdos64_ccpu_sm0_unexpected_calls(void);
extern uint16_t getAX(void);
extern void setAX(uint16_t value);

static void initialize_request(ntdos64_historical_bios_bridge_v1_request *request)
{
    memset(request, 0, sizeof(*request));
    request->abi_version = NTDOS64_HISTORICAL_BIOS_BRIDGE_V1_ABI;
    request->struct_bytes = (uint32_t)sizeof(*request);
    request->selector = 0x50u;
    request->service_byte = 0x14u;
    request->ax = 0x5a00u;
    request->cx = 0x1111u;
    request->dx = 0x2222u;
    request->cs = 0u;
    request->ip = 3u;
}

int main(void)
{
    ntdos64_historical_bios_bridge_v1_request request;
    ntdos64_historical_bios_bridge_v1_response response;
    int result = 0;

    initialize_request(&request);
    if (!ntdos64_historical_bios_bridge_v1_invoke(&request, &response) ||
        response.disposition != NTDOS64_HISTORICAL_BIOS_BRIDGE_V1_NOT_INITIALIZED) {
        result |= 1;
    }
    if (!ntdos64_historical_bios_bridge_v1_initialize()) {
        return result | 2;
    }

    setAX(0x4a4au);
    request.abi_version = 2u;
    if (!ntdos64_historical_bios_bridge_v1_invoke(&request, &response) ||
        response.disposition != NTDOS64_HISTORICAL_BIOS_BRIDGE_V1_INVALID_RECORD ||
        getAX() != 0x4a4au) {
        result |= 4;
    }
    request.abi_version = NTDOS64_HISTORICAL_BIOS_BRIDGE_V1_ABI;
    request.selector = 0x51u;
    if (!ntdos64_historical_bios_bridge_v1_invoke(&request, &response) ||
        response.disposition != NTDOS64_HISTORICAL_BIOS_BRIDGE_V1_UNSUPPORTED ||
        getAX() != 0x4a4au) {
        result |= 8;
    }

    initialize_request(&request);
    if (!ntdos64_historical_bios_bridge_v1_invoke(&request, &response) ||
        response.disposition != NTDOS64_HISTORICAL_BIOS_BRIDGE_V1_COMPLETED ||
        response.ip != 4u || response.cx < 1980u ||
        (response.ax & 0xffu) > 6u || (response.dx >> 8) == 0u ||
        (response.dx >> 8) > 12u || (response.dx & 0xffu) == 0u ||
        (response.dx & 0xffu) > 31u) {
        result |= 16;
    }
    if (ntdos64_ccpu_sm0_unexpected_calls() != 0u) {
        result |= 32;
    }

    ntdos64_historical_bios_bridge_v1_terminate();
    return result;
}
