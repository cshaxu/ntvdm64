#include <stdint.h>
#include <string.h>

#include "historical_bios_bridge_v1.h"
#include "reconstructed_monitor_s4.h"

static int historical_bridge_transport(
    void *context,
    const ntdos64_s4_bridge_request *request,
    ntdos64_s4_bridge_response *response)
{
    ntdos64_historical_bios_bridge_v1_request historical_request;
    ntdos64_historical_bios_bridge_v1_response historical_response;

    (void)context;
    memset(&historical_request, 0, sizeof(historical_request));
    historical_request.abi_version = NTDOS64_HISTORICAL_BIOS_BRIDGE_V1_ABI;
    historical_request.struct_bytes = (uint32_t)sizeof(historical_request);
    historical_request.selector = request->selector;
    historical_request.service_byte = request->next_byte;
    historical_request.ax = (uint16_t)request->state.eax;
    historical_request.cx = (uint16_t)request->state.ecx;
    historical_request.dx = (uint16_t)request->state.edx;
    historical_request.cs = request->state.cs;
    historical_request.ip = request->state.ip;

    if (!ntdos64_historical_bios_bridge_v1_invoke(&historical_request, &historical_response) ||
        historical_response.disposition != NTDOS64_HISTORICAL_BIOS_BRIDGE_V1_COMPLETED) {
        return 0;
    }
    response->write_mask = NTDOS64_S4_BRIDGE_WRITE_AX |
        NTDOS64_S4_BRIDGE_WRITE_CX | NTDOS64_S4_BRIDGE_WRITE_DX |
        NTDOS64_S4_BRIDGE_WRITE_CS | NTDOS64_S4_BRIDGE_WRITE_IP;
    response->ax = historical_response.ax;
    response->cx = historical_response.cx;
    response->dx = historical_response.dx;
    response->cs = historical_response.cs;
    response->ip = historical_response.ip;
    return 1;
}

int main(void)
{
    uint8_t ram[] = {0xc4u, 0xc4u, 0x50u, 0x14u, 0xd6u, 0xfeu};
    ntdos64_s4_monitor monitor;
    ntdos64_s4_state state;
    int result = 0;

    if (!ntdos64_historical_bios_bridge_v1_initialize()) {
        return 1;
    }
    if (ntdos64_s4_monitor_initialize(&monitor, ram, sizeof(ram),
            historical_bridge_transport, NULL) != NTDOS64_S4_BUDGET_EXHAUSTED) {
        result = 2;
        goto cleanup;
    }
    monitor.state.eax = 0xc0de5a00u;
    monitor.state.ecx = 0xbeef1111u;
    monitor.state.edx = 0xabcd2222u;
    if (ntdos64_s4_monitor_run(&monitor, 2u) != NTDOS64_S4_BOP_EXIT) {
        result |= 4;
    }
    if (ntdos64_s4_monitor_get_state(&monitor, &state) != NTDOS64_S4_BUDGET_EXHAUSTED ||
        state.ip != 6u || (state.eax & 0xffff0000u) != 0xc0de0000u ||
        (state.eax & 0xffu) > 6u || (state.edx & 0xffff0000u) != 0xabcd0000u ||
        ((state.edx >> 8) & 0xffu) == 0u || ((state.edx >> 8) & 0xffu) > 12u ||
        (state.edx & 0xffu) == 0u || (state.edx & 0xffu) > 31u ||
        (state.ecx & 0xffffu) < 1980u) {
        result |= 8;
    }

cleanup:
    ntdos64_historical_bios_bridge_v1_terminate();
    return result;
}
