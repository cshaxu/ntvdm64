#include "window_mouse.h"
#include <limits.h>
#include <string.h>

static INPUT_RECORD dos_record(const frontend_dos_mouse *state,unsigned action,int32_t dx,int32_t dy,unsigned buttons)
{
    INPUT_RECORD result={0};
    console_mouse_input payload={dx,dy,state->width,state->height,(uint16_t)buttons,(uint16_t)action};
    if(action==CONSOLE_MOUSE_LEAVE)payload.width=payload.height=0;
    result.EventType=CONSOLE_INPUT_RELATIVE_MOUSE;
    memcpy(&result.Event,&payload,sizeof(payload));
    return result;
}
DWORD frontend_dos_mouse_geometry(frontend_dos_mouse *state,unsigned width,unsigned height)
{
    if(!state || !width || !height || width>UINT16_MAX || height>UINT16_MAX)
        return ERROR_INVALID_PARAMETER;
    state->width=(uint16_t)width;state->height=(uint16_t)height;
    return ERROR_SUCCESS;
}
DWORD frontend_dos_mouse_leave(frontend_dos_mouse *state,frontend_mouse_sink sink,void *context)
{
    DWORD error;
    if(!state || !sink)return ERROR_INVALID_PARAMETER;
    if(state->active) {
        INPUT_RECORD event=dos_record(state,CONSOLE_MOUSE_LEAVE,0,0,0);
        error=sink(context,&event,1);if(error)return error;
    }
    state->active=FALSE;state->buttons=0;state->source=0;
    return ERROR_SUCCESS;
}
DWORD frontend_dos_mouse_enter(frontend_dos_mouse *state,frontend_mouse_sink sink,void *context)
{
    INPUT_RECORD event;DWORD error;
    if(!state || !sink)return ERROR_INVALID_PARAMETER;
    if(state->active)return ERROR_SUCCESS;
    if(!state->width || !state->height)return ERROR_NOT_READY;
    event=dos_record(state,CONSOLE_MOUSE_ENTER,0,0,0);
    error=sink(context,&event,1);if(error)return error;
    state->active=TRUE;return ERROR_SUCCESS;
}
DWORD frontend_dos_mouse_dispatch(frontend_dos_mouse *state,const frontend_window_input *input,
    frontend_mouse_sink sink,void *context)
{
    const kvm_input_event *event;INPUT_RECORD records[2];DWORD count=0,error;
    uint16_t buttons;
    if(!state || !input || !sink)return ERROR_INVALID_PARAMETER;
    event=&input->event;
    if(event->type!=KVM_EVENT_MOUSE && event->type!=KVM_EVENT_INPUT_RESET &&
        event->type!=KVM_EVENT_SOURCE_RETIRED)return ERROR_SUCCESS;
    if(!event->source_identity || (state->source && state->source!=event->source_identity))
        return ERROR_INVALID_STATE;
    if(event->type==KVM_EVENT_SOURCE_RETIRED)return frontend_dos_mouse_leave(state,sink,context);
    if(event->type==KVM_EVENT_INPUT_RESET) {
        if(!state->active || !state->buttons)return ERROR_SUCCESS;
        records[0]=dos_record(state,CONSOLE_MOUSE_MOVE,0,0,0);
        error=sink(context,records,1);if(error)return error;
        state->buttons=0;return ERROR_SUCCESS;
    }
    if(!state->width || !state->height)return ERROR_NOT_READY;
    if(!event->data.mouse.relative || event->data.mouse.wheel_x || event->data.mouse.wheel_y ||
        (event->data.mouse.buttons&~(KVM_MOUSE_BUTTON_LEFT|KVM_MOUSE_BUTTON_RIGHT)))
        return ERROR_NOT_SUPPORTED;
    buttons=(uint16_t)(((event->data.mouse.buttons&KVM_MOUSE_BUTTON_LEFT) ? 1 : 0) |
        ((event->data.mouse.buttons&KVM_MOUSE_BUTTON_RIGHT) ? 2 : 0));
    /* One atomic queue write preserves ENTER before its first sample. KVM
     * already scales client deltas to content pixels; do not scale twice. */
    if(!state->active)records[count++]=dos_record(state,CONSOLE_MOUSE_ENTER,0,0,0);
    records[count++]=dos_record(state,CONSOLE_MOUSE_MOVE,event->data.mouse.delta_x,
        event->data.mouse.delta_y,buttons);
    error=sink(context,records,count);if(error)return error;
    state->active=TRUE;state->source=event->source_identity;state->buttons=buttons;
    return ERROR_SUCCESS;
}
