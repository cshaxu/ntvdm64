#ifndef BX_NTVDM_MACHINE_COMPOSITION_SEAM_H
#define BX_NTVDM_MACHINE_COMPOSITION_SEAM_H

#include "bx_ntvdm_machine_bop_v1.h"
#include "bx_ntvdm_cpu_state_abi.h"
#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_instruction_window_abi.h"

typedef bx_ntvdm_machine_bop_v1_mechanics bx_ntvdm_machine_composition_mechanics;

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_machine_composition_initialize_mechanics(
    bx_ntvdm_machine_composition_mechanics *mechanics);

/* The caller supplies copied generic facts and a transient native mechanics
 * context. Only HANDLED_RESUME authorizes the pre-recorded generic resume. */
bx_ntvdm_machine_bop_v1_result bx_ntvdm_machine_composition_v1_probe(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_state,
    const bx_ntvdm_instruction_window_v1 *instruction_window,
    const bx_ntvdm_machine_composition_mechanics *mechanics);

#ifdef __cplusplus
}
#endif

#endif
