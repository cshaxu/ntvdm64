#include "ntvdm-exe/softpc/mvdm_softpc_mouse_input.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    mvdm_mouse_input queue={0},saved;
    mvdm_mouse_input_sample sample={70000,-70000,1,0},out;
    int64_t x=0,y=0;unsigned count=0,i;
    assert(!mvdm_mouse_input_take(&queue,&out));
    assert(mvdm_mouse_input_push(&queue,&sample));
    while(mvdm_mouse_input_take(&queue,&out)) {
        assert(out.dx>=SHRT_MIN && out.dx<=SHRT_MAX && out.dy>=SHRT_MIN && out.dy<=SHRT_MAX);
        x+=out.dx;y+=out.dy;++count;
        assert(out.buttons==(queue.count ? 0u : 1u));
    }
    assert(x==70000 && y==-70000 && count==3 && queue.delivered_buttons==1);
    sample.dx=sample.dy=0;sample.buttons=0;
    assert(mvdm_mouse_input_push(&queue,&sample));
    assert(mvdm_mouse_input_take(&queue,&out) && !out.dx && !out.dy && !out.buttons);
    sample.dx=INT_MIN;sample.dy=INT_MAX;
    assert(mvdm_mouse_input_push(&queue,&sample));x=y=0;
    while(mvdm_mouse_input_take(&queue,&out)) { x+=out.dx;y+=out.dy; }
    assert(x==INT_MIN && y==INT_MAX);
    sample.dx=1;sample.dy=-1;
    for(i=0;i<MVDM_MOUSE_INPUT_CAPACITY;++i) {
        sample.buttons=i&1;assert(mvdm_mouse_input_push(&queue,&sample));
    }
    saved=queue;assert(!mvdm_mouse_input_push(&queue,&sample));
    assert(!memcmp(&queue,&saved,sizeof(queue)));
    for(i=0;i<MVDM_MOUSE_INPUT_CAPACITY;++i) {
        assert(mvdm_mouse_input_take(&queue,&out));
        assert(out.dx==1 && out.dy==-1 && out.buttons==(i&1));
    }
    sample.buttons=4;assert(!mvdm_mouse_input_push(&queue,&sample));
    sample.buttons=0;assert(mvdm_mouse_input_push(&queue,&sample));
    mvdm_mouse_input_clear(&queue);assert(!mvdm_mouse_input_take(&queue,&out));
    puts("PASS relative carrier: exact signed motion, split/button ordering, release, FIFO wrap, bounded rejection without mutation");
    return 0;
}
