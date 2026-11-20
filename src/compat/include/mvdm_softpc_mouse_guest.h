#ifndef MVDM_SOFTPC_MOUSE_GUEST_H
#define MVDM_SOFTPC_MOUSE_GUEST_H
#include "mvdm_softpc_mouse_bridge.h"

/* Called only by original nt_mouse.c with its original mouse_io.h types,
 * ICA lock and CPU-thread ownership. Original statics remain in that owner. */
BOOL mvdm_softpc_mouse_apply(MOUSE_CURSOR_STATUS *,MOUSE_VECTOR *,int *,int *,BOOL *,
    BOOL,BOOL *,IS16 *,IS16 *);
void mvdm_softpc_mouse_receive(const INPUT_RECORD *);
#endif
