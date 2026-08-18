#include <stdio.h>
#include <string.h>

#include "bx_ntvdm_bop_ingress_v1.h"
#include "bx_ntvdm_bop_provider_registry_v1.h"
#include "bx_ntvdm_dem_error_lock_plane_v1.h"

static int prepare(uint8_t service, bx_ntvdm_exception_event_v1 *event,
    bx_ntvdm_cpu_state_v1 *cpu, bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_bop_ingress_v1 *ingress,
    bx_ntvdm_bop_provider_selection_v1 *selection)
{
    const uint8_t bytes[4] = { 0xc4u, 0xc4u, 0x50u, service };
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event->abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event->struct_bytes = sizeof(*event);
    event->kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event->cpu_id = 1u;
    event->vector = 6u;
    event->fault_rip = UINT64_C(0x7490);
    bx_ntvdm_cpu_state_v1_initialize(cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    bx_ntvdm_instruction_window_v1_capture(window, bytes, sizeof(bytes));
    return bx_ntvdm_bop_ingress_v1_classify(window, ingress) &&
        bx_ntvdm_bop_provider_registry_v1_select(ingress, selection);
}

static int result_is_error(const bx_ntvdm_cpu_result_v2 *result,
    uint16_t expected_ax)
{
    return bx_ntvdm_cpu_result_v2_valid(result) &&
        result->disposition == BX_NTVDM_CPU_RESULT_V2_RESUME &&
        result->resume_rip == UINT64_C(0x7494) &&
        result->cpu_delta.gpr16_write_mask == 1u &&
        result->cpu_delta.gpr16_values[0] == expected_ax &&
        result->eflags_write_mask == BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF &&
        result->eflags_values == BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF;
}

int main(void)
{
    bx_ntvdm_dem_error_lock_plane_v1 plane;
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    bx_ntvdm_cpu_result_v2 result;

    bx_ntvdm_dem_error_lock_plane_v1_clear(&plane);
    if (!prepare(0x32u, &event, &cpu, &window, &ingress, &selection)) return 1;
    cpu.ds = 0x00a7u; cpu.edx = 0x0332u; cpu.ebx = 0x0070u;
    bx_ntvdm_cpu_result_v2_pass_through(&result);
    if (!bx_ntvdm_dem_error_lock_plane_v1_dispatch(&plane, &ingress, &selection,
            &event, &cpu, &window, &result) || !plane.has_registration ||
        plane.registration.hard_error_packet != UINT64_C(0x0da2) ||
        plane.registration.device_chain != UINT64_C(0x0ae0) ||
        !bx_ntvdm_cpu_result_v2_valid(&result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != UINT64_C(0x7494) ||
        result.cpu_delta.gpr16_write_mask != 0u ||
        result.eflags_write_mask != 0u) return 2;

    bx_ntvdm_dem_error_lock_plane_v1_clear(&plane);
    if (!prepare(0x33u, &event, &cpu, &window, &ingress, &selection)) return 3;
    bx_ntvdm_cpu_result_v2_pass_through(&result);
    if (!bx_ntvdm_dem_error_lock_plane_v1_dispatch(&plane, &ingress, &selection,
            &event, &cpu, &window, &result) || !result_is_error(&result, 0xffu) ||
        plane.has_registration != 0u) return 4;

    if (!prepare(0x3fu, &event, &cpu, &window, &ingress, &selection)) return 5;
    bx_ntvdm_cpu_result_v2_pass_through(&result);
    if (!bx_ntvdm_dem_error_lock_plane_v1_dispatch(&plane, &ingress, &selection,
            &event, &cpu, &window, &result) || !result_is_error(&result, 5u)) return 6;

    if (!prepare(0x32u, &event, &cpu, &window, &ingress, &selection)) return 7;
    cpu.ds = 0xffffu; cpu.edx = 0x0010u; cpu.ebx = 0x0070u;
    bx_ntvdm_cpu_result_v2_pass_through(&result);
    if (bx_ntvdm_dem_error_lock_plane_v1_dispatch(&plane, &ingress, &selection,
            &event, &cpu, &window, &result)) return 8;

    puts("t226-s2 DEM error/lock source-derived conformance verified");
    return 0;
}