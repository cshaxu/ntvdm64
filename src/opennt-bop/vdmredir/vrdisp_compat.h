#ifndef APP_OPENNT_BOP_VDMREDIR_VRDISP_COMPAT_H
#define APP_OPENNT_BOP_VDMREDIR_VRDISP_COMPAT_H
#include "adapter-bop/generic_ud_bridge.h"
typedef int (*runtime_vr_dispatch_body_fn)(uint8_t service, const struct runtime_generic_ud_event_v1 *event, struct runtime_generic_ud_outcome_v1 *outcome);
int runtime_vr_dispatch_with_frame(uint8_t service, const struct runtime_generic_ud_event_v1 *event, struct runtime_generic_ud_outcome_v1 *outcome, runtime_vr_dispatch_body_fn body);
#endif
