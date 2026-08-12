#ifndef BX_NTVDM_CPU_RESULT_BRIDGE_V1_H
#define BX_NTVDM_CPU_RESULT_BRIDGE_V1_H

#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_guest_read_action_v1.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Narrow, typed-result bridge.  It owns source-derived BOP selection; its
 * Bochs caller receives only a validated generic CPU result or pass-through. */
int bx_ntvdm_cpu_result_bridge_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);

/* A separately source-admitted two-phase input path. Its caller sees only
 * the generic action/range and a checked final CPU result. */
int bx_ntvdm_cpu_result_bridge_v1_dispatch_guest_read(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_guest_read_action_v1 *action);
int bx_ntvdm_cpu_result_bridge_v1_complete_guest_read(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_read_action_v1 *action,
    const uint8_t *bytes, uint64_t byte_count,
    bx_ntvdm_cpu_result_v2 *result);

#ifdef __cplusplus
}
#endif

#endif
