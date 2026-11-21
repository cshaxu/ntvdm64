#ifndef MVDM_SOFTPC_MOUSE_BRIDGE_H
#define MVDM_SOFTPC_MOUSE_BRIDGE_H
#include <windows.h>
#include "compat/include/mvdm_mouse_record.h"
#include "mvdm_softpc_mouse_input.h"

/* Borrowed immutable functions; provider code outlives the bridge and its
 * producer/consumer threads. Bound before input activation, never replaced. */
typedef struct mvdm_mouse_services {
    void (*lock)(void);
    void (*unlock)(void);
    void (*interrupt)(void);
} mvdm_mouse_services;

typedef struct mvdm_mouse_bridge {
    const mvdm_mouse_services *services;
    mvdm_mouse_input queue;
    BOOL submitted,active;
    uint16_t width,height,virtual_width,virtual_height;
    int64_t remainder_x,remainder_y;
    HANDLE input_ready; /* Borrowed from the owning input watch; never closed here. */
} mvdm_mouse_bridge;

/* B owns the input state contract; the product embeds and binds storage.
 * Original threads use their existing host context. NULL means no endpoint. */
mvdm_mouse_bridge *mvdm_softpc_mouse_current(void);

/* Zero-initialized, worker-owned state with services bound before use.
 * Producer acquires original ICA lock;
 * consumer/EOI calls already hold it. No new task or interrupt scheduler. */
DWORD mvdm_softpc_mouse_submit(mvdm_mouse_bridge *,const console_mouse_input *);
/* Single event producer reserves at most this many raw input records before
 * removal from the frontend. Consumer can only increase available space. */
DWORD mvdm_softpc_mouse_capacity(mvdm_mouse_bridge *);
BOOL mvdm_softpc_mouse_pending(const mvdm_mouse_bridge *);
BOOL mvdm_softpc_mouse_active(const mvdm_mouse_bridge *);
/* Original CCPU painter reads a synchronized route snapshot, never UI state. */
BOOL mvdm_softpc_mouse_route_active(void);
/* CPU-side conversion, with the selected original instance extent. */
DWORD mvdm_softpc_mouse_prepare(mvdm_mouse_bridge *,unsigned,unsigned);
BOOL mvdm_softpc_mouse_next(mvdm_mouse_bridge *,mvdm_mouse_input_sample *);
/* After the CPU cursor owner restores the LEAVE background, retire only that
 * consumed route. Later ENTER/MOVE records and producer state survive. */
void mvdm_softpc_mouse_leave(mvdm_mouse_bridge *);
void mvdm_softpc_mouse_cancel(mvdm_mouse_bridge *);
/* Quiesced ownership handoff: clear route state, preserve the borrowed wake. */
static __inline void mvdm_softpc_mouse_reset(mvdm_mouse_bridge *state)
{
    if(state) {
        HANDLE ready=state->input_ready;
        const mvdm_mouse_services *services=state->services;
        ZeroMemory(state,sizeof(*state));state->input_ready=ready;
        state->services=services;
    }
}
#endif
