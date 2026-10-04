#include "mvdm_softpc_mouse_input.h"
#include "common/protocol/console_mouse.h"
#include <limits.h>

int mvdm_mouse_input_push(mvdm_mouse_input *queue,const mvdm_mouse_input_sample *sample)
{
    mvdm_mouse_input_sample *tail;
    int64_t x,y;
    if(!queue || !sample || (sample->buttons&~3u))
        return 0;
    /* Original nt_event.c::nt_process_mouse stops extending a movement
     * backlog above half its 32-entry ring, retaining every button edge.
     * Relative samples must accumulate displacement rather than overwrite
     * absolute coordinates. Keep the same 16-event pressure boundary; route
     * transitions and changed buttons remain ordered, separate records. */
    if(queue->count>16 && sample->action==CONSOLE_MOUSE_MOVE) {
        tail=&queue->samples[(queue->head+queue->count-1)%MVDM_MOUSE_INPUT_CAPACITY];
        if(tail->action==CONSOLE_MOUSE_MOVE && tail->buttons==sample->buttons) {
            x=(int64_t)tail->dx+sample->dx;y=(int64_t)tail->dy+sample->dy;
            if(x>=INT32_MIN && x<=INT32_MAX && y>=INT32_MIN && y<=INT32_MAX) {
                tail->dx=(int32_t)x;tail->dy=(int32_t)y;return 1;
            }
        }
    }
    if(queue->count==MVDM_MOUSE_INPUT_CAPACITY)return 0;
    queue->samples[(queue->head+queue->count)%MVDM_MOUSE_INPUT_CAPACITY]=*sample;
    ++queue->count;return 1;
}
int mvdm_mouse_input_take(mvdm_mouse_input *queue,mvdm_mouse_input_sample *sample)
{
    mvdm_mouse_input_sample *pending;
    if(!queue || !sample || !queue->count)return 0;
    pending=&queue->samples[queue->head];*sample=*pending;
    if(sample->dx>SHRT_MAX)sample->dx=SHRT_MAX;
    if(sample->dx<SHRT_MIN)sample->dx=SHRT_MIN;
    if(sample->dy>SHRT_MAX)sample->dy=SHRT_MAX;
    if(sample->dy<SHRT_MIN)sample->dy=SHRT_MIN;
    pending->dx-=sample->dx;pending->dy-=sample->dy;
    if(pending->dx || pending->dy)sample->buttons=queue->delivered_buttons;
    else {
        queue->delivered_buttons=sample->buttons;
        queue->head=(queue->head+1)%MVDM_MOUSE_INPUT_CAPACITY;--queue->count;
    }
    return 1;
}
void mvdm_mouse_input_clear(mvdm_mouse_input *queue)
{
    if(queue)queue->head=queue->count=0;
}
