#ifndef BX_NTVDM_BOP_SELECTOR_PROBE_H
#define BX_NTVDM_BOP_SELECTOR_PROBE_H

#include "bx_ntvdm_bios_selector_map.h"
#include "bx_ntvdm_instruction_window_abi.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Identifies an admitted historical selector boundary. This does not execute
 * an owner, read another byte, alter CPU state, or select a resume address. */
int bx_ntvdm_bop_selector_probe_v1(
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_bios_owner_v1 *owner);

#ifdef __cplusplus
}
#endif

#endif
