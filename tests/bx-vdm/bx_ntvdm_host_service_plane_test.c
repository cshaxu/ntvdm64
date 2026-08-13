#include <stdint.h>

#include "bx_ntvdm_host_service_plane.h"

int main(void)
{
    static const uint8_t ntio[] = { 1u };
    static const uint8_t ntdos[] = { 0xe9u, 0xa9u, 0x51u };
    static const uint8_t command[] = { 2u };
    bx_ntvdm_host_service_request_v1 request;
    bx_ntvdm_cpu_state_v1 state;
    bx_ntvdm_session_resources_v1 resources;
    bx_ntvdm_host_service_payloads_v1 payloads;
    bx_ntvdm_host_transaction_v1 transaction;

    bx_ntvdm_session_resources_v1_initialize(&resources);
    resources.resources[0].kind = BX_NTVDM_SESSION_RESOURCE_NTIO;
    resources.resources[0].byte_count = sizeof(ntio);
    resources.resources[1].kind = BX_NTVDM_SESSION_RESOURCE_NTDOS;
    resources.resources[1].byte_count = sizeof(ntdos);
    resources.resources[2].kind = BX_NTVDM_SESSION_RESOURCE_COMMAND;
    resources.resources[2].byte_count = sizeof(command);
    payloads.ntio_bytes = ntio;
    payloads.ntio_byte_count = sizeof(ntio);
    payloads.ntdos_bytes = ntdos;
    payloads.ntdos_byte_count = sizeof(ntdos);
    payloads.command_bytes = command;
    payloads.command_byte_count = sizeof(command);
    request.selector = 0x50u;
    request.service = 0x11u;
    request.family = BX_NTVDM_HOST_SERVICE_FAMILY_DEM;
    bx_ntvdm_cpu_state_v1_initialize(&state, BX_NTVDM_CPU_EXECUTION_REAL);
    state.edi = 0x08b0u;
    if (bx_ntvdm_host_service_plane_v1_prepare(&request, &state, &resources,
        &payloads, 0x100000u, &transaction) !=
        BX_NTVDM_HOST_SERVICE_PLANE_READY ||
        transaction.guest_write.guest_physical_address != 0x8b00u ||
        transaction.guest_write.byte_count != sizeof(ntdos)) return 1;
    request.service = 0x12u;
    if (bx_ntvdm_host_service_plane_v1_prepare(&request, &state, &resources,
        &payloads, 0x100000u, &transaction) !=
        BX_NTVDM_HOST_SERVICE_PLANE_UNHANDLED) return 2;
    request.service = 0x11u;
    payloads.ntdos_byte_count = 2u;
    if (bx_ntvdm_host_service_plane_v1_prepare(&request, &state, &resources,
        &payloads, 0x100000u, &transaction) !=
        BX_NTVDM_HOST_SERVICE_PLANE_REJECTED) return 3;
    payloads.ntdos_byte_count = sizeof(ntdos);
    state.edi = 0xffffu;
    if (bx_ntvdm_host_service_plane_v1_prepare(&request, &state, &resources,
        &payloads, 0xffff2u, &transaction) !=
        BX_NTVDM_HOST_SERVICE_PLANE_REJECTED) return 4;
    state.edi = 0x08b0u;
    state.execution_mode = BX_NTVDM_CPU_EXECUTION_PROTECTED;
    if (bx_ntvdm_host_service_plane_v1_prepare(&request, &state, &resources,
        &payloads, 0x100000u, &transaction) !=
        BX_NTVDM_HOST_SERVICE_PLANE_UNHANDLED) return 5;
    state.execution_mode = BX_NTVDM_CPU_EXECUTION_REAL;
    return bx_ntvdm_host_service_plane_v1_prepare(&request, &state, &resources,
        &payloads, 0x100000u, 0) == BX_NTVDM_HOST_SERVICE_PLANE_REJECTED ? 0 : 6;
}
