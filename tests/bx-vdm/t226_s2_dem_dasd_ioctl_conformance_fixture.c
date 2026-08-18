#include <stdio.h>
#include <string.h>

#include "bx_ntvdm_bop_ingress_v1.h"
#include "bx_ntvdm_bop_provider_registry_v1.h"
#include "bx_ntvdm_dem_ioctl_metadata_provider_v1.h"
#include "bx_ntvdm_dem_package_facade_v1.h"
#include "bx_ntvdm_dem_raw_media_provider_v1.h"

static int prepare(uint8_t service, uint16_t ax, uint8_t drive,
    bx_ntvdm_exception_event_v1 *event, bx_ntvdm_cpu_state_v1 *cpu,
    bx_ntvdm_instruction_window_v1 *window, bx_ntvdm_bop_ingress_v1 *ingress,
    bx_ntvdm_bop_provider_selection_v1 *selection,
    bx_ntvdm_dem_plane_record_v1 *plane, bx_ntvdm_dem_package_route_v1 *route)
{
    const uint8_t bytes[4] = {0xc4u, 0xc4u, 0x50u, service};
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event->abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event->struct_bytes = sizeof(*event);
    event->kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event->cpu_id = 1u;
    event->vector = 6u;
    event->fault_rip = UINT64_C(0x5a90);
    bx_ntvdm_cpu_state_v1_initialize(cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu->eax = ax;
    cpu->ebx = drive;
    bx_ntvdm_instruction_window_v1_capture(window, bytes, sizeof(bytes));
    return bx_ntvdm_bop_ingress_v1_classify(window, ingress) &&
        bx_ntvdm_bop_provider_registry_v1_select(ingress, selection) &&
        bx_ntvdm_dem_plane_v1_classify(ingress, selection, plane) &&
        bx_ntvdm_dem_package_facade_v1_classify(ingress, selection, route);
}

static int error_result(const bx_ntvdm_cpu_result_v2 *result, uint16_t ax)
{
    return bx_ntvdm_cpu_result_v2_valid(result) &&
        result->disposition == BX_NTVDM_CPU_RESULT_V2_RESUME &&
        result->resume_rip == UINT64_C(0x5a94) &&
        result->cpu_delta.gpr16_write_mask == 1u &&
        result->cpu_delta.gpr16_values[0] == ax &&
        result->eflags_write_mask == BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF &&
        result->eflags_values == BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF;
}

int main(void)
{
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    bx_ntvdm_dem_plane_record_v1 plane;
    bx_ntvdm_dem_package_route_v1 route;
    bx_ntvdm_host_drive_snapshot_v1 snapshot;
    bx_ntvdm_cpu_result_v2 result;
    uint8_t types[26] = {0};

    /* `demIoctlChangeable` reads only the admitted immutable drive view.
     * A fixed C: remains non-removable, while the same source owner exposes
     * remote/CD-ROM through AL=09's DX form. */
    types[2] = 3u;
    if (!bx_ntvdm_host_drive_snapshot_v1_apply(UINT32_C(1) << 2u, types,
            0u, 0u, &snapshot) ||
        !prepare(0x21u, 0x4408u, 2u, &event, &cpu, &window, &ingress,
            &selection, &plane, &route)) return 1;
    bx_ntvdm_cpu_result_v2_pass_through(&result);
    if (!bx_ntvdm_dem_ioctl_metadata_provider_v1_dispatch(&ingress, &selection,
            &plane, &snapshot, &event, &cpu, &result) ||
        result.resume_rip != UINT64_C(0x5a94) ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 1u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != 0u) return 2;

    types[2] = 5u;
    if (!bx_ntvdm_host_drive_snapshot_v1_apply(UINT32_C(1) << 2u, types,
            0u, 0u, &snapshot) ||
        !prepare(0x21u, 0x4409u, 2u, &event, &cpu, &window, &ingress,
            &selection, &plane, &route)) return 3;
    bx_ntvdm_cpu_result_v2_pass_through(&result);
    if (!bx_ntvdm_dem_ioctl_metadata_provider_v1_dispatch(&ingress, &selection,
            &plane, &snapshot, &event, &cpu, &result) ||
        result.cpu_delta.gpr16_write_mask != ((1u << 0) | (1u << 3)) ||
        result.cpu_delta.gpr16_values[0] != 0x4400u ||
        result.cpu_delta.gpr16_values[3] != 0x1000u ||
        result.eflags_values != 0u) return 4;

    if (!prepare(0x21u, 0x4408u, 3u, &event, &cpu, &window, &ingress,
            &selection, &plane, &route)) return 5;
    bx_ntvdm_cpu_result_v2_pass_through(&result);
    if (!bx_ntvdm_dem_ioctl_metadata_provider_v1_dispatch(&ingress, &selection,
            &plane, &snapshot, &event, &cpu, &result) || !error_result(&result, 15u)) return 6;

    /* Generic and block-query IOCTL need BDS/raw-device composition.  The
     * metadata provider declines them; the raw-media owner preserves the
     * original invalid-function terminal instead of claiming a capability. */
    if (!prepare(0x21u, 0x440du, 2u, &event, &cpu, &window, &ingress,
            &selection, &plane, &route)) return 7;
    bx_ntvdm_cpu_result_v2_pass_through(&result);
    if (bx_ntvdm_dem_ioctl_metadata_provider_v1_dispatch(&ingress, &selection,
            &plane, &snapshot, &event, &cpu, &result) ||
        !bx_ntvdm_dem_raw_media_provider_v1_dispatch(&ingress, &selection,
            &route, &event, &cpu, &window, &result) || !error_result(&result, 1u)) return 8;
    if (!prepare(0x21u, 0x4411u, 2u, &event, &cpu, &window, &ingress,
            &selection, &plane, &route)) return 9;
    bx_ntvdm_cpu_result_v2_pass_through(&result);
    if (!bx_ntvdm_dem_raw_media_provider_v1_dispatch(&ingress, &selection,
            &route, &event, &cpu, &window, &result) || !error_result(&result, 1u)) return 10;

    /* No BDS, BIOS disk, INT13 or raw host-device handle is admitted for
     * absolute read/write in either enabled mutation profile. */
    if (!prepare(0x29u, 0u, 0u, &event, &cpu, &window, &ingress,
            &selection, &plane, &route)) return 11;
    bx_ntvdm_cpu_result_v2_pass_through(&result);
    if (!bx_ntvdm_dem_raw_media_provider_v1_dispatch(&ingress, &selection,
            &route, &event, &cpu, &window, &result) || !error_result(&result, 21u)) return 12;
    if (!prepare(0x2au, 0u, 0u, &event, &cpu, &window, &ingress,
            &selection, &plane, &route)) return 13;
    bx_ntvdm_cpu_result_v2_pass_through(&result);
    if (!bx_ntvdm_dem_raw_media_provider_v1_dispatch(&ingress, &selection,
            &route, &event, &cpu, &window, &result) || !error_result(&result, 21u)) return 14;

    puts("t226-s2 DEM DASD/IOCTL source-derived conformance verified");
    return 0;
}