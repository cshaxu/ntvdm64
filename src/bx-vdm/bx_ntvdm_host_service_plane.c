#include "bx_ntvdm_host_service_plane.h"

#include <string.h>

static int bx_ntvdm_payload_matches_resource(const uint8_t *bytes,
    uint64_t byte_count, const bx_ntvdm_session_resource_v1 *resource)
{
    return bytes != 0 && resource != 0 && byte_count != 0u &&
        byte_count == resource->byte_count;
}

bx_ntvdm_host_service_plane_result_v1 bx_ntvdm_host_service_plane_v1_prepare(
    const bx_ntvdm_host_service_request_v1 *request,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_session_resources_v1 *resources,
    const bx_ntvdm_host_service_payloads_v1 *payloads,
    uint64_t aperture_bytes,
    bx_ntvdm_host_transaction_v1 *transaction)
{
    bx_ntvdm_guest_write_v1 write;
    uint64_t destination;

    if (transaction == 0) return BX_NTVDM_HOST_SERVICE_PLANE_REJECTED;
    memset(transaction, 0, sizeof(*transaction));
    if (request == 0 || cpu_before == 0 || resources == 0 || payloads == 0 ||
        !bx_ntvdm_session_resources_v1_valid(resources) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before))
        return BX_NTVDM_HOST_SERVICE_PLANE_REJECTED;
    /* demLoadDos consumes a 16-bit DI:0000 destination.  Do not silently
     * reinterpret that historical contract under protected/v8086 addressing. */
    if (cpu_before->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL)
        return BX_NTVDM_HOST_SERVICE_PLANE_UNHANDLED;
    if (request->family != BX_NTVDM_HOST_SERVICE_FAMILY_DEM ||
        request->selector != 0x50u || request->service != 0x11u)
        return BX_NTVDM_HOST_SERVICE_PLANE_UNHANDLED;
    if (!bx_ntvdm_payload_matches_resource(payloads->ntdos_bytes,
        payloads->ntdos_byte_count, &resources->resources[1]))
        return BX_NTVDM_HOST_SERVICE_PLANE_REJECTED;
    destination = ((uint64_t)(cpu_before->edi & 0xffffu)) << 4;
    bx_ntvdm_guest_write_v1_initialize(&write, destination,
        payloads->ntdos_byte_count, 0u);
    bx_ntvdm_host_transaction_v1_initialize(transaction, request, cpu_before,
        &write);
    if (!bx_ntvdm_host_transaction_v1_preflight(transaction, aperture_bytes,
        payloads->ntdos_byte_count)) {
        memset(transaction, 0, sizeof(*transaction));
        return BX_NTVDM_HOST_SERVICE_PLANE_REJECTED;
    }
    return BX_NTVDM_HOST_SERVICE_PLANE_READY;
}
