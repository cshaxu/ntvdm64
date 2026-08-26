#ifndef APP_OPENNT_BOP_VDMREDIR_VRDISP_COMPAT_H
#define APP_OPENNT_BOP_VDMREDIR_VRDISP_COMPAT_H
#include "adapter-bop/generic_ud_bridge.h"
typedef int (*runtime_vr_dispatch_body_fn)(uint8_t service, const struct runtime_generic_ud_event *event, struct runtime_generic_ud_outcome *outcome);
int runtime_vr_dispatch_with_frame(uint8_t service, const struct runtime_generic_ud_event *event, struct runtime_generic_ud_outcome *outcome, runtime_vr_dispatch_body_fn body);
#endif
