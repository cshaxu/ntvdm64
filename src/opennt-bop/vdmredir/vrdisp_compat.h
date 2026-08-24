#ifndef NTDOS64_OPENNT_BOP_VDMREDIR_VRDISP_COMPAT_H
#define NTDOS64_OPENNT_BOP_VDMREDIR_VRDISP_COMPAT_H
#include "adapter-bop/bx_ntvdm_generic_ud_bridge.h"
typedef int (*bx_ntvdm_vr_dispatch_body_fn)(uint8_t service, const struct bx_ntvdm_generic_ud_event_v1 *event, struct bx_ntvdm_generic_ud_outcome_v1 *outcome);
int bx_ntvdm_vr_dispatch_with_frame(uint8_t service, const struct bx_ntvdm_generic_ud_event_v1 *event, struct bx_ntvdm_generic_ud_outcome_v1 *outcome, bx_ntvdm_vr_dispatch_body_fn body);
#endif
