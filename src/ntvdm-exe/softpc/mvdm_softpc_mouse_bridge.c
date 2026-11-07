#include "mvdm_softpc_mouse_bridge.h"
#include <limits.h>

/* Original declarations: nt_mouse.c, nt_eoi.h. Their scheduling and guest
 * virtual extent remain authoritative; this file only binds relative input. */
extern void host_ica_lock(void);
extern void host_ica_unlock(void);
extern void DoMouseInterrupt(void);

DWORD mvdm_softpc_mouse_capacity(mvdm_mouse_bridge *state)
{
    DWORD capacity=MVDM_MOUSE_INPUT_CAPACITY;
    if(state) {
        host_ica_lock();
        capacity-=state->queue.count;
        /* Input watch has already latched readiness until the next read ack.
         * Clear it under the same ICA lock as dequeue, so a freed slot cannot
         * race this reset. Original nt_event_loop still waits for suspend/APC. */
        if(!capacity && state->input_ready)ResetEvent(state->input_ready);
        host_ica_unlock();
    }
    return capacity;
}

DWORD mvdm_softpc_mouse_submit(mvdm_mouse_bridge *state,const console_mouse_input *input)
{
    mvdm_mouse_input_sample sample={0};DWORD error=ERROR_SUCCESS;
    if(!state || !console_mouse_input_valid(input))return ERROR_INVALID_DATA;
    host_ica_lock();
    if((input->action==CONSOLE_MOUSE_ENTER && state->submitted) ||
        (input->action!=CONSOLE_MOUSE_ENTER && !state->submitted)) { error=ERROR_INVALID_STATE;goto done; }
    sample.buttons=input->buttons;sample.action=input->action;
    sample.dx=input->dx;sample.dy=input->dy;
    sample.width=input->width;sample.height=input->height;
    if(!mvdm_mouse_input_push(&state->queue,&sample)) { error=ERROR_BUFFER_OVERFLOW;goto done; }
    /* Commit producer state only after the sample has a durable queue slot. */
    state->submitted=input->action!=CONSOLE_MOUSE_LEAVE;
    DoMouseInterrupt();
done:
    host_ica_unlock();return error;
}
/* CPU owner calls after selecting the original mouse instance. Scale each
 * queued sample once, then retain the existing short-counter fragmentation.
 * Producer code never reads CPU/VM-owned geometry. ICA lock is already held. */
DWORD mvdm_softpc_mouse_prepare(mvdm_mouse_bridge *state, unsigned width, unsigned height)
{
    mvdm_mouse_input_sample *sample;
    int64_t x=0,y=0,rx=0,ry=0;
    if(!state || !state->queue.count)return ERROR_SUCCESS;
    sample=&state->queue.samples[state->queue.head];
    if(!sample->width)return ERROR_SUCCESS; /* already scaled, or LEAVE */
    if(!width || !height || width>UINT16_MAX || height>UINT16_MAX)return ERROR_INVALID_DATA;
    if(sample->action==CONSOLE_MOUSE_MOVE) {
        if(state->width==sample->width && state->height==sample->height &&
            state->virtual_width==width && state->virtual_height==height) {
            rx=state->remainder_x;ry=state->remainder_y;
        }
        x=(int64_t)sample->dx*width+rx;y=(int64_t)sample->dy*height+ry;
        rx=x%sample->width;ry=y%sample->height;
        x/=sample->width;y/=sample->height;
        if(x<INT32_MIN || x>INT32_MAX || y<INT32_MIN || y>INT32_MAX)
            return ERROR_ARITHMETIC_OVERFLOW;
    }
    state->width=sample->width;state->height=sample->height;
    state->virtual_width=(uint16_t)width;state->virtual_height=(uint16_t)height;
    state->remainder_x=rx;state->remainder_y=ry;
    sample->dx=(int32_t)x;sample->dy=(int32_t)y;
    sample->width=sample->height=0;
    return ERROR_SUCCESS;
}
BOOL mvdm_softpc_mouse_pending(const mvdm_mouse_bridge *state)
{ return state && state->queue.count!=0; }
BOOL mvdm_softpc_mouse_active(const mvdm_mouse_bridge *state)
{ return state && state->active; }
BOOL mvdm_softpc_mouse_next(mvdm_mouse_bridge *state,mvdm_mouse_input_sample *sample)
{
    BOOL full;
    if(!state || !sample)return FALSE;
    full=state->queue.count==MVDM_MOUSE_INPUT_CAPACITY;
    if(!mvdm_mouse_input_take(&state->queue,sample)) {
        if(!state->active)return FALSE;
        ZeroMemory(sample,sizeof(*sample));
        sample->buttons=state->queue.delivered_buttons;sample->action=CONSOLE_MOUSE_MOVE;
    }
    if(full && state->queue.count<MVDM_MOUSE_INPUT_CAPACITY && state->input_ready)
        SetEvent(state->input_ready);
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
        BOOL full=state->queue.count==MVDM_MOUSE_INPUT_CAPACITY;
        /* Original IRQ cancellation discards pending input, not the frontend
         * connection. Its already accepted ENTER/LEAVE still determines the
         * route of records that remain upstream. Clear deltas/buttons without
         * requiring the frontend to send a second ENTER after a guest reset. */
        mvdm_softpc_mouse_reset(state);
        state->submitted=state->active=submitted;
        if(full && state->input_ready)SetEvent(state->input_ready);
    }
}
