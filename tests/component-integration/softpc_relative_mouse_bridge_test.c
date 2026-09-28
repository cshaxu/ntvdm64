#include "ntvdm-exe/softpc/mvdm_softpc_mouse_bridge.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

/* Boundary test only: IRQ notification/lock are observed, not emulated CPU
 * execution. Production keeps original VirtualX/Y and DoMouseInterrupt. */
unsigned short VirtualX=640,VirtualY=200;
static unsigned locked,notifications;
void host_ica_lock(void) { assert(!locked);locked=1; }
void host_ica_unlock(void) { assert(locked);locked=0; }
void DoMouseInterrupt(void) { assert(locked);++notifications; }

static void submit(mvdm_mouse_bridge *s,unsigned action,int dx,int dy,unsigned buttons,
    unsigned width,unsigned height,DWORD expected)
{
    console_mouse_input input={dx,dy,(uint16_t)width,(uint16_t)height,
        (uint16_t)buttons,(uint16_t)action};
    mvdm_mouse_bridge saved=*s;
    unsigned before=notifications;
    assert(mvdm_softpc_mouse_submit(s,&input)==expected);
    assert(!locked && notifications==before+(expected==ERROR_SUCCESS));
    if(expected!=ERROR_SUCCESS)assert(!memcmp(s,&saved,sizeof(saved)));
}
static mvdm_mouse_input_sample take(mvdm_mouse_bridge *s,unsigned action,int dx,int dy,unsigned buttons)
{
    mvdm_mouse_input_sample out;
    assert(mvdm_softpc_mouse_next(s,&out));
    assert(out.action==action && out.dx==dx && out.dy==dy && out.buttons==buttons);
    return out;
}
int main(void)
{
    mvdm_mouse_bridge s={0},other={0};
    mvdm_mouse_input_sample out;
    unsigned i;
    assert(!mvdm_softpc_mouse_pending(&s) && !mvdm_softpc_mouse_active(&s));
    assert(!mvdm_softpc_mouse_next(&s,&out));
    submit(&s,CONSOLE_MOUSE_MOVE,1,0,0,640,400,ERROR_INVALID_STATE);
    submit(&s,CONSOLE_MOUSE_ENTER,0,0,0,640,400,ERROR_SUCCESS);
    submit(&s,CONSOLE_MOUSE_ENTER,0,0,0,640,400,ERROR_INVALID_STATE);
    take(&s,CONSOLE_MOUSE_ENTER,0,0,0);
    assert(mvdm_softpc_mouse_active(&s));
    /* Two half-unit movements retain their remainder, with no fake press. */
    submit(&s,CONSOLE_MOUSE_MOVE,0,1,0,640,400,ERROR_SUCCESS);
    take(&s,CONSOLE_MOUSE_MOVE,0,0,0);
    submit(&s,CONSOLE_MOUSE_MOVE,0,1,1,640,400,ERROR_SUCCESS);
    take(&s,CONSOLE_MOUSE_MOVE,0,1,1);
    take(&s,CONSOLE_MOUSE_MOVE,0,0,1); /* Idle poll preserves held state. */
    submit(&s,CONSOLE_MOUSE_MOVE,0,-1,0,640,400,ERROR_SUCCESS);
    take(&s,CONSOLE_MOUSE_MOVE,0,0,0);
    submit(&s,CONSOLE_MOUSE_MOVE,0,-1,0,640,400,ERROR_SUCCESS);
    take(&s,CONSOLE_MOUSE_MOVE,0,-1,0);
    /* Mode-13 raster width 320 maps to original virtual width 640. */
    submit(&s,CONSOLE_MOUSE_MOVE,3,2,2,320,200,ERROR_SUCCESS);
    take(&s,CONSOLE_MOUSE_MOVE,6,2,2);
    submit(&s,CONSOLE_MOUSE_MOVE,INT_MAX,0,0,1,200,ERROR_ARITHMETIC_OVERFLOW);
    submit(&s,CONSOLE_MOUSE_MOVE,0,0,4,320,200,ERROR_INVALID_DATA);
    VirtualY=0;
    submit(&s,CONSOLE_MOUSE_MOVE,0,0,0,320,200,ERROR_NOT_READY);
    VirtualY=200;
    /* Retiring an old route must not cancel an already accepted new route. */
    submit(&s,CONSOLE_MOUSE_LEAVE,0,0,0,0,0,ERROR_SUCCESS);
    submit(&s,CONSOLE_MOUSE_ENTER,0,0,0,640,400,ERROR_SUCCESS);
    submit(&s,CONSOLE_MOUSE_MOVE,7,2,1,640,400,ERROR_SUCCESS);
    take(&s,CONSOLE_MOUSE_LEAVE,0,0,0);
    assert(mvdm_softpc_mouse_active(&s));
    mvdm_softpc_mouse_leave(&s);
    assert(!mvdm_softpc_mouse_active(&s) && mvdm_softpc_mouse_pending(&s));
    take(&s,CONSOLE_MOUSE_ENTER,0,0,0);
    take(&s,CONSOLE_MOUSE_MOVE,7,1,1);
    assert(mvdm_softpc_mouse_active(&s) && !mvdm_softpc_mouse_pending(&s));
    assert(!mvdm_softpc_mouse_active(&other) && !mvdm_softpc_mouse_pending(&other));
    /* Full-queue failure cannot change producer ownership or remainder. */
    for(i=0;i<MVDM_MOUSE_INPUT_CAPACITY;++i) {
        assert(mvdm_softpc_mouse_capacity(&s)==MVDM_MOUSE_INPUT_CAPACITY-i && !locked);
        submit(&s,CONSOLE_MOUSE_MOVE,0,1,0,640,400,ERROR_SUCCESS);
    }
    assert(!mvdm_softpc_mouse_capacity(&s) && !locked);
    submit(&s,CONSOLE_MOUSE_LEAVE,0,0,0,0,0,ERROR_BUFFER_OVERFLOW);
    for(i=0;i<MVDM_MOUSE_INPUT_CAPACITY;++i)
        take(&s,CONSOLE_MOUSE_MOVE,0,(int)(i&1),0);
    submit(&s,CONSOLE_MOUSE_LEAVE,0,0,0,0,0,ERROR_SUCCESS);
    take(&s,CONSOLE_MOUSE_LEAVE,0,0,0);mvdm_softpc_mouse_leave(&s);
    assert(!mvdm_softpc_mouse_next(&s,&out));
    submit(&s,CONSOLE_MOUSE_ENTER,0,0,0,640,400,ERROR_SUCCESS);
    mvdm_softpc_mouse_cancel(&s);
    assert(s.submitted && s.active && !s.queue.count);
    submit(&s,CONSOLE_MOUSE_MOVE,2,0,0,640,400,ERROR_SUCCESS);
    take(&s,CONSOLE_MOUSE_MOVE,2,0,0);
    submit(&s,CONSOLE_MOUSE_LEAVE,0,0,0,0,0,ERROR_SUCCESS);
    mvdm_softpc_mouse_cancel(&s);
    assert(!memcmp(&s,&other,sizeof(s)));
    puts("PASS relative bridge: scale/remainders, buttons, rapid re-entry, atomic rejection, independent state, cancellation; mocked ICA/IRQ only");
    return 0;
}
