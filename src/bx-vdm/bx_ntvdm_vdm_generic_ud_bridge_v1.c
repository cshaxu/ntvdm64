#include "bx_ntvdm_vdm_generic_ud_bridge_v1.h"
#include "bx_ntvdm_boot_namespace_composition_v1.h"
#include "bx_ntvdm_native_bop_composition_v1.h"

static int bx_ntvdm_vdm_generic_ud_event_valid(
    const struct bx_ntvdm_generic_ud_event_v1 *event)
{
    return event != 0 && event->magic == BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC &&
        event->abi_version == BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION &&
        event->struct_bytes == sizeof(*event) &&
        event->window_bytes <= BX_NTVDM_GENERIC_UD_WINDOW_BYTES;
}

int bx_ntvdm_vdm_generic_ud_classify_v1(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    bx_ntvdm_bop_ingress_v1 *ingress,
    bx_ntvdm_bop_provider_selection_v1 *selection)
{
    bx_ntvdm_exception_event_v1 boundary;
    bx_ntvdm_cpu_state_v1 state;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_cpu_result_v2 result;

    if (!bx_ntvdm_vdm_generic_ud_event_valid(event) || ingress == 0 ||
        selection == 0) return 0;
    boundary.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    boundary.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    boundary.struct_bytes = (uint32_t)sizeof(boundary);
    boundary.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    boundary.cpu_id = event->cpu_id;
    boundary.vector = event->vector;
    boundary.error_code = event->error_code;
    boundary.reserved0 = 0u;
    boundary.fault_rip = event->fault_rip;
    bx_ntvdm_cpu_state_v1_initialize(&state, event->execution_mode);
    state.eax = event->eax; state.ebx = event->ebx; state.ecx = event->ecx;
    state.edx = event->edx; state.esi = event->esi; state.edi = event->edi;
    state.ebp = event->ebp; state.esp = event->esp; state.eip = event->eip;
    state.eflags = event->eflags;
    state.cs = event->cs; state.ds = event->ds; state.es = event->es;
    state.ss = event->ss; state.fs = event->fs; state.gs = event->gs;
    bx_ntvdm_instruction_window_v1_capture(&window, event->window,
        event->window_bytes);
    if (!bx_ntvdm_bop_ingress_v1_dispatch(&boundary, &state, &window, ingress,
            &result) || !bx_ntvdm_cpu_result_v2_valid(&result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH)
        return 0;
    return bx_ntvdm_bop_provider_registry_v1_select(ingress, selection);
}

/* This alternative link implementation remains a decline until the adapter
 * explicitly binds its one composition session. */
int bx_ntvdm_mantle_generic_ud_bridge_v1(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    if (bx_ntvdm_boot_namespace_composition_v1_handle(event, outcome)) return 1;
    return bx_ntvdm_native_bop_composition_v1_handle(event, outcome);
}
