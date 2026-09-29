#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "window_mouse.h"
#ifdef NDEBUG
#error This fixture requires assertions.
#endif

typedef struct capture { INPUT_RECORD events[64];DWORD count,error; } capture;
static DWORD sink(void *context,const INPUT_RECORD *events,DWORD count)
{
    capture *out=context;
    if(out->error)return out->error;
    assert(out->count+count<=ARRAYSIZE(out->events));
    memcpy(out->events+out->count,events,count*sizeof(*events));out->count+=count;
    return ERROR_SUCCESS;
}
static console_mouse_input payload(const capture *out,unsigned index)
{
    console_mouse_input value;
    assert(index<out->count && out->events[index].EventType==CONSOLE_INPUT_RELATIVE_MOUSE);
    memcpy(&value,&out->events[index].Event,sizeof(value));
    assert(console_mouse_input_valid(&value));return value;
}
static void dos_contract(void)
{
    {
        frontend_dos_mouse route={0};capture output={0};
        assert(frontend_dos_mouse_enter(&route,sink,&output)==ERROR_NOT_READY);
        assert(!frontend_dos_mouse_geometry(&route,640,400));
        output.error=ERROR_BROKEN_PIPE;
        assert(frontend_dos_mouse_enter(&route,sink,&output)==ERROR_BROKEN_PIPE);
        assert(!route.active && !output.count);
        output.error=0;
        assert(!frontend_dos_mouse_enter(&route,sink,&output));
        assert(route.active && !route.source && output.count==1);
        assert(payload(&output,0).action==CONSOLE_MOUSE_ENTER);
        assert(!frontend_dos_mouse_enter(&route,sink,&output) && output.count==1);
        assert(!frontend_dos_mouse_leave(&route,sink,&output));
        assert(!route.active && output.count==2);
        assert(payload(&output,1).action==CONSOLE_MOUSE_LEAVE);
    }
    frontend_dos_mouse state={0},saved;
    frontend_window_input input={0};capture out={0};console_mouse_input p;
    input.event.type=KVM_EVENT_MOUSE;input.event.source_identity=17;
    input.event.data.mouse.relative=TRUE;
    assert(frontend_dos_mouse_dispatch(&state,&input,sink,&out)==ERROR_NOT_READY);
    assert(frontend_dos_mouse_geometry(&state,65536,400)==ERROR_INVALID_PARAMETER);
    assert(!frontend_dos_mouse_geometry(&state,640,400));
    input.event.data.mouse.delta_x=INT_MIN;input.event.data.mouse.delta_y=INT_MAX;
    saved=state;out.error=ERROR_BROKEN_PIPE;
    assert(frontend_dos_mouse_dispatch(&state,&input,sink,&out)==ERROR_BROKEN_PIPE);
    assert(!out.count && !memcmp(&state,&saved,sizeof(state)));
    out.error=0;assert(!frontend_dos_mouse_dispatch(&state,&input,sink,&out));
    assert(out.count==2 && payload(&out,0).action==CONSOLE_MOUSE_ENTER);
    p=payload(&out,1);assert(p.dx==INT_MIN && p.dy==INT_MAX && !p.buttons);
    assert(p.width==640 && p.height==400);
    input.event.data.mouse.delta_x=input.event.data.mouse.delta_y=0;
    input.event.data.mouse.buttons=KVM_MOUSE_BUTTON_RIGHT;
    assert(!frontend_dos_mouse_dispatch(&state,&input,sink,&out));
    p=payload(&out,2);assert(p.action==CONSOLE_MOUSE_MOVE && p.buttons==2 && !p.dx && !p.dy);
    input.event.type=KVM_EVENT_INPUT_RESET;saved=state;out.error=ERROR_BROKEN_PIPE;
    assert(frontend_dos_mouse_dispatch(&state,&input,sink,&out)==ERROR_BROKEN_PIPE);
    assert(!memcmp(&state,&saved,sizeof(state)));
    out.error=0;assert(!frontend_dos_mouse_dispatch(&state,&input,sink,&out));
    assert(state.active && state.source==17 && !state.buttons && !payload(&out,3).buttons);
    assert(!frontend_dos_mouse_dispatch(&state,&input,sink,&out) && out.count==4);
    input.event.source_identity=18;
    assert(frontend_dos_mouse_dispatch(&state,&input,sink,&out)==ERROR_INVALID_STATE);
    input.event.source_identity=17;input.event.type=KVM_EVENT_MOUSE;
    input.event.data.mouse.wheel_y=1;
    assert(frontend_dos_mouse_dispatch(&state,&input,sink,&out)==ERROR_NOT_SUPPORTED);
    input.event.type=KVM_EVENT_SOURCE_RETIRED;
    assert(!frontend_dos_mouse_dispatch(&state,&input,sink,&out));
    assert(!state.active && !state.source && payload(&out,4).action==CONSOLE_MOUSE_LEAVE);
    assert(!frontend_dos_mouse_leave(&state,sink,&out) && out.count==5);
    input.event.type=KVM_EVENT_MOUSE;input.event.source_identity=18;
    input.event.data.mouse.wheel_y=0;input.event.data.mouse.buttons=0;
    assert(!frontend_dos_mouse_geometry(&state,320,200));
    assert(!frontend_dos_mouse_dispatch(&state,&input,sink,&out));
    assert(payload(&out,5).action==CONSOLE_MOUSE_ENTER && payload(&out,6).width==320);
    puts("PASS DOS mouse converter: copied relative records, atomic enter/move, reset/release, retire/re-entry, source and shape rejection; no guest execution");
}
int main(void)
{
    dos_contract();
    return 0;
}
