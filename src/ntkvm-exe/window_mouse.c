#include "window_mouse.h"
#include <limits.h>
#include <string.h>

static LONG bounded(LONGLONG value,LONG limit)
{ return value<0 ? 0 : value>=limit ? limit-1 : (LONG)value; }

DWORD frontend_native_mouse_geometry(frontend_native_mouse *state,SMALL_RECT viewport,
    unsigned width,unsigned height)
{
    LONGLONG w,h;
    if(!state || !width || !height || viewport.Left<0 || viewport.Top<0 ||
        viewport.Right<viewport.Left || viewport.Bottom<viewport.Top)return ERROR_INVALID_PARAMETER;
    w=(LONGLONG)(viewport.Right-viewport.Left+1)*width;
    h=(LONGLONG)(viewport.Bottom-viewport.Top+1)*height;
    if(w>LONG_MAX || h>LONG_MAX)return ERROR_ARITHMETIC_OVERFLOW;
    /* Preserve the position within the viewport, not the native text caret.
     * Subcell pixels retain small relative movements until a cell is crossed. */
    state->x=state->ready ? bounded(state->x,(LONG)w) : (LONG)(w/2);
    state->y=state->ready ? bounded(state->y,(LONG)h) : (LONG)(h/2);
    state->viewport=viewport;state->cell_width=width;state->cell_height=height;
    state->ready=TRUE;return ERROR_SUCCESS;
}
static INPUT_RECORD record(const frontend_native_mouse *state,DWORD buttons,DWORD flags,DWORD control)
{
    INPUT_RECORD result={0};
    result.EventType=MOUSE_EVENT;
    result.Event.MouseEvent.dwMousePosition.X=(SHORT)(state->viewport.Left+state->x/(LONG)state->cell_width);
    result.Event.MouseEvent.dwMousePosition.Y=(SHORT)(state->viewport.Top+state->y/(LONG)state->cell_height);
    result.Event.MouseEvent.dwButtonState=buttons;
    result.Event.MouseEvent.dwEventFlags=flags;
    result.Event.MouseEvent.dwControlKeyState=control;
    return result;
}
DWORD frontend_native_mouse_release(frontend_native_mouse *state,BOOL retire,
    frontend_mouse_sink sink,void *context)
{
    DWORD error;
    if(!state || !sink)return ERROR_INVALID_PARAMETER;
    if(state->buttons) {
        INPUT_RECORD release=record(state,0,0,0);
        error=sink(context,&release,1);if(error)return error;
        state->buttons=0;
    }
    if(retire)state->source=0;
    return ERROR_SUCCESS;
}
DWORD frontend_native_mouse_dispatch(frontend_native_mouse *state,const frontend_window_input *input,
    frontend_mouse_sink sink,void *context)
{
    frontend_native_mouse next;INPUT_RECORD records[2];DWORD count=0,buttons,error;
    const kvm_input_event *event;
    if(!state || !input || !sink)return ERROR_INVALID_PARAMETER;
    event=&input->event;
    if(event->type!=KVM_EVENT_MOUSE && event->type!=KVM_EVENT_INPUT_RESET &&
        event->type!=KVM_EVENT_SOURCE_RETIRED)return ERROR_SUCCESS;
    if(!event->source_identity || (state->source && state->source!=event->source_identity))
        return ERROR_INVALID_STATE;
    if(event->type!=KVM_EVENT_MOUSE)
        return frontend_native_mouse_release(state,event->type==KVM_EVENT_SOURCE_RETIRED,sink,context);
    if(!state->ready)return ERROR_NOT_READY;
    /* The approved provider emits relative, content-scaled left/right events.
     * Reject unprovided shapes; never reinterpret wheel data as motion. */
    if(!event->data.mouse.relative || event->data.mouse.wheel_x || event->data.mouse.wheel_y ||
        (event->data.mouse.buttons & ~(KVM_MOUSE_BUTTON_LEFT|KVM_MOUSE_BUTTON_RIGHT)))
        return ERROR_NOT_SUPPORTED;
    next=*state;next.source=event->source_identity;
    next.x=bounded((LONGLONG)next.x+event->data.mouse.delta_x,
        (LONG)((next.viewport.Right-next.viewport.Left+1)*next.cell_width));
    next.y=bounded((LONGLONG)next.y+event->data.mouse.delta_y,
        (LONG)((next.viewport.Bottom-next.viewport.Top+1)*next.cell_height));
    buttons=((event->data.mouse.buttons&KVM_MOUSE_BUTTON_LEFT) ? FROM_LEFT_1ST_BUTTON_PRESSED : 0) |
        ((event->data.mouse.buttons&KVM_MOUSE_BUTTON_RIGHT) ? RIGHTMOST_BUTTON_PRESSED : 0);
    /* Movement precedes the button transition, matching the library's flush.
     * It must not invent a press while merely moving over a menu. */
    if(next.x!=state->x || next.y!=state->y)
        records[count++]=record(&next,state->buttons,MOUSE_MOVED,input->control_state);
    if(buttons!=state->buttons)records[count++]=record(&next,buttons,0,input->control_state);
    error=count ? sink(context,records,count) : ERROR_SUCCESS;
    if(error)return error;
    next.buttons=buttons;*state=next;return ERROR_SUCCESS;
}

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
