#include "mvdm_softpc_mouse_bridge.h"
#include <limits.h>

/* Original declarations: nt_mouse.c, nt_eoi.h. Their scheduling and guest
 * virtual extent remain authoritative; this file only binds relative input. */
extern void host_ica_lock(void);
extern void host_ica_unlock(void);
extern void DoMouseInterrupt(void);
extern unsigned short VirtualX,VirtualY;

DWORD mvdm_softpc_mouse_capacity(mvdm_mouse_bridge *state)
{
    DWORD capacity=MVDM_MOUSE_INPUT_CAPACITY;
    if(state) {
        host_ica_lock();
        capacity-=state->queue.count;
        host_ica_unlock();
    }
    return capacity;
}

DWORD mvdm_softpc_mouse_submit(mvdm_mouse_bridge *state,const console_mouse_input *input)
{
    mvdm_mouse_input_sample sample={0};DWORD error=ERROR_SUCCESS;
    int64_t x=0,y=0,rx=0,ry=0;
    if(!state || !console_mouse_input_valid(input))return ERROR_INVALID_DATA;
    host_ica_lock();
    if((input->action==CONSOLE_MOUSE_ENTER && state->submitted) ||
        (input->action!=CONSOLE_MOUSE_ENTER && !state->submitted)) { error=ERROR_INVALID_STATE;goto done; }
    sample.buttons=input->buttons;sample.action=input->action;
    if(input->action==CONSOLE_MOUSE_MOVE) {
        if(!VirtualX || !VirtualY) { error=ERROR_NOT_READY;goto done; }
        if(state->width==input->width && state->height==input->height &&
            state->virtual_width==VirtualX && state->virtual_height==VirtualY) {
            rx=state->remainder_x;ry=state->remainder_y;
        }
        x=(int64_t)input->dx*VirtualX+rx;y=(int64_t)input->dy*VirtualY+ry;
        rx=x%input->width;ry=y%input->height;
        x/=input->width;y/=input->height;
        if(x<INT32_MIN || x>INT32_MAX || y<INT32_MIN || y>INT32_MAX) {
            error=ERROR_ARITHMETIC_OVERFLOW;goto done;
        }
        sample.dx=(int32_t)x;sample.dy=(int32_t)y;
    }
    if(!mvdm_mouse_input_push(&state->queue,&sample)) { error=ERROR_BUFFER_OVERFLOW;goto done; }
    /* Commit producer state only after the sample has a durable queue slot. */
    state->submitted=input->action!=CONSOLE_MOUSE_LEAVE;
    state->width=input->width;state->height=input->height;
    state->virtual_width=VirtualX;state->virtual_height=VirtualY;
    state->remainder_x=rx;state->remainder_y=ry;
    DoMouseInterrupt();
done:
    host_ica_unlock();return error;
}
BOOL mvdm_softpc_mouse_pending(const mvdm_mouse_bridge *state)
{ return state && state->queue.count!=0; }
BOOL mvdm_softpc_mouse_active(const mvdm_mouse_bridge *state)
{ return state && state->active; }
BOOL mvdm_softpc_mouse_next(mvdm_mouse_bridge *state,mvdm_mouse_input_sample *sample)
{
    if(!state || !sample)return FALSE;
    if(!mvdm_mouse_input_take(&state->queue,sample)) {
        if(!state->active)return FALSE;
        ZeroMemory(sample,sizeof(*sample));
        sample->buttons=state->queue.delivered_buttons;sample->action=CONSOLE_MOUSE_MOVE;
    }
    if(sample->action==CONSOLE_MOUSE_ENTER)state->active=TRUE;
    /* LEAVE is returned while active, allowing the original CPU-thread cursor
     * owner to restore its saved background before retiring this route. */
    return TRUE;
}
void mvdm_softpc_mouse_leave(mvdm_mouse_bridge *state)
{ if(state)state->active=FALSE; }
void mvdm_softpc_mouse_cancel(mvdm_mouse_bridge *state)
{
    if(state) {
        BOOL submitted=state->submitted;
        /* Original IRQ cancellation discards pending input, not the frontend
         * connection. Its already accepted ENTER/LEAVE still determines the
         * route of records that remain upstream. Clear deltas/buttons without
         * requiring the frontend to send a second ENTER after a guest reset. */
        ZeroMemory(state,sizeof(*state));
        state->submitted=state->active=submitted;
    }
}
