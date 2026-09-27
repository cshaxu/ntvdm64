#ifndef MVDM_SOFTPC_MOUSE_BRIDGE_H
#define MVDM_SOFTPC_MOUSE_BRIDGE_H
#include <windows.h>
#include "product-abi/console_mouse.h"
#include "mvdm_softpc_mouse_input.h"

typedef struct mvdm_mouse_bridge {
    mvdm_mouse_input queue;
    BOOL submitted,active;
    uint16_t width,height,virtual_width,virtual_height;
    int64_t remainder_x,remainder_y;
} mvdm_mouse_bridge;

/* Worker Console client owns storage; original event/CPU threads access only
 * their already-bound session. NULL means no character frontend endpoint. */
mvdm_mouse_bridge *mvdm_softpc_mouse_current(void);

/* Zero-initialized, worker-owned state. Producer acquires original ICA lock;
 * consumer/EOI calls already hold it. No new task or interrupt scheduler. */
DWORD mvdm_softpc_mouse_submit(mvdm_mouse_bridge *,const console_mouse_input *);
BOOL mvdm_softpc_mouse_pending(const mvdm_mouse_bridge *);
BOOL mvdm_softpc_mouse_active(const mvdm_mouse_bridge *);
/* Original CCPU painter reads a synchronized route snapshot, never UI state. */
BOOL mvdm_softpc_mouse_route_active(void);
BOOL mvdm_softpc_mouse_next(mvdm_mouse_bridge *,mvdm_mouse_input_sample *);
/* After the CPU cursor owner restores the LEAVE background, retire only that
 * consumed route. Later ENTER/MOVE records and producer state survive. */
void mvdm_softpc_mouse_leave(mvdm_mouse_bridge *);
void mvdm_softpc_mouse_cancel(mvdm_mouse_bridge *);
#endif
