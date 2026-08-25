#include <stdint.h>
#include <string.h>

#include "historical_bios_bridge_v1.h"
#include "reconstructed_monitor.h"

static int historical_bridge_transport(
    void *context,
    const reconstructed_monitor_bridge_request *request,
    reconstructed_monitor_bridge_response *response)
{
    runner_historical_bios_bridge_v1_request historical_request;
    runner_historical_bios_bridge_v1_response historical_response;

    (void)context;
    memset(&historical_request, 0, sizeof(historical_request));
    historical_request.abi_version = RUNNER_HISTORICAL_BIOS_BRIDGE_V1_ABI;
    historical_request.struct_bytes = (uint32_t)sizeof(historical_request);
    historical_request.selector = request->selector;
    historical_request.service_byte = request->next_byte;
    historical_request.ax = (uint16_t)request->state.eax;
    historical_request.cx = (uint16_t)request->state.ecx;
    historical_request.dx = (uint16_t)request->state.edx;
    historical_request.cs = request->state.cs;
    historical_request.ip = request->state.ip;

    if (!runner_historical_bios_bridge_v1_invoke(&historical_request, &historical_response) ||
        historical_response.disposition != RUNNER_HISTORICAL_BIOS_BRIDGE_V1_COMPLETED) {
        return 0;
    }
    response->write_mask = RECONSTRUCTED_MONITOR_BRIDGE_WRITE_AX |
        RECONSTRUCTED_MONITOR_BRIDGE_WRITE_CX | RECONSTRUCTED_MONITOR_BRIDGE_WRITE_DX |
        RECONSTRUCTED_MONITOR_BRIDGE_WRITE_CS | RECONSTRUCTED_MONITOR_BRIDGE_WRITE_IP;
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
    reconstructed_monitor_monitor monitor;
    reconstructed_monitor_state state;
    int result = 0;

    if (!runner_historical_bios_bridge_v1_initialize()) {
        return 1;
    }
    if (reconstructed_monitor_monitor_initialize(&monitor, ram, sizeof(ram),
            historical_bridge_transport, NULL) != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED) {
        result = 2;
        goto cleanup;
    }
    monitor.state.eax = 0xc0de5a00u;
    monitor.state.ecx = 0xbeef1111u;
    monitor.state.edx = 0xabcd2222u;
    if (reconstructed_monitor_monitor_run(&monitor, 2u) != RECONSTRUCTED_MONITOR_BOP_EXIT) {
        result |= 4;
    }
    if (reconstructed_monitor_monitor_get_state(&monitor, &state) != RECONSTRUCTED_MONITOR_BUDGET_EXHAUSTED ||
        state.ip != 6u || (state.eax & 0xffff0000u) != 0xc0de0000u ||
        (state.eax & 0xffu) > 6u || (state.edx & 0xffff0000u) != 0xabcd0000u ||
        ((state.edx >> 8) & 0xffu) == 0u || ((state.edx >> 8) & 0xffu) > 12u ||
        (state.edx & 0xffu) == 0u || (state.edx & 0xffu) > 31u ||
        (state.ecx & 0xffffu) < 1980u) {
        result |= 8;
    }

cleanup:
    runner_historical_bios_bridge_v1_terminate();
    return result;
}
