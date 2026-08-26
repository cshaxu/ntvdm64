/* DIVERGENCE(BOP-DIV-056): the retained source path exports the original
 * dispatcher names only.  The copied-frame provider implementation is a
 * source-derived rehost and belongs in the matching private overlay. */
#define VrDispatch opennt_bop_overlay_VrDispatch
#define VrDispatchTable opennt_bop_overlay_VrDispatchTable
#define runtime_vr_dispatch_with_frame opennt_bop_overlay_vr_dispatch_with_frame
#include "opennt-bop-overlay/vdmredir/vrdisp.c"
#undef VrDispatch
#undef VrDispatchTable
#undef runtime_vr_dispatch_with_frame

BOOL VrDispatch(ULONG service)
{
    return opennt_bop_overlay_VrDispatch(service);
}

int runtime_vr_dispatch_with_frame(uint8_t service,
    const struct runtime_generic_ud_event *event,
    struct runtime_generic_ud_outcome *outcome,
    runtime_vr_dispatch_body_fn body)
{
    return opennt_bop_overlay_vr_dispatch_with_frame(service, event, outcome,
        body);
}
