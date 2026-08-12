#ifndef BX_NTVDM_LEGACY_PLANE_GATE_V1_H
#define BX_NTVDM_LEGACY_PLANE_GATE_V1_H

#include <stdint.h>
#include "bx_ntvdm_bop_ingress_v1.h"

#ifdef __cplusplus
extern "C" {
#endif
/* Validates only source-plane ownership of an existing legacy endpoint.
 * It neither dispatches a service nor changes guest/CPU state. */
int bx_ntvdm_legacy_plane_gate_v1_dem(const bx_ntvdm_instruction_window_v1 *window,
    uint8_t expected_service);
int bx_ntvdm_legacy_plane_gate_v1_command(const bx_ntvdm_instruction_window_v1 *window,
    uint8_t expected_service);
#ifdef __cplusplus
}
#endif
#endif
